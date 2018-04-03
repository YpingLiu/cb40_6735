/*
 * Copyright (C) 2006, 2007, 2008, 2011 Apple Inc. All rights reserved.
 *               2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/rendering/RenderListBox.h"

#include "core/HTMLNames.h"
#include "core/accessibility/AXObjectCache.h"
#include "core/css/CSSFontSelector.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/dom/Document.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/NodeRenderStyle.h"
#include "core/dom/StyleEngine.h"
#include "core/editing/FrameSelection.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLDivElement.h"
#include "core/html/HTMLOptGroupElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/HTMLSelectElement.h"
#include "core/page/EventHandler.h"
#include "core/page/FocusController.h"
#include "core/page/Page.h"
#include "core/page/SpatialNavigation.h"
#include "core/rendering/HitTestResult.h"
#include "core/rendering/PaintInfo.h"
#include "core/rendering/RenderScrollbar.h"
#include "core/rendering/RenderLayer.h"
#include "core/rendering/RenderText.h"
#include "core/rendering/RenderTheme.h"
#include "core/rendering/RenderView.h"
#include "core/rendering/TextRunConstructor.h"
#include "platform/fonts/FontCache.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/scroll/Scrollbar.h"
#include "platform/text/BidiTextRun.h"
#include <math.h>

#if USE_POPUP_LISTBOX
#include "core/frame/FrameHost.h"
#include "core/page/Chrome.h"
#endif

namespace blink {

using namespace HTMLNames;

// Default size when the multiple attribute is present but size attribute is absent.
const int defaultSize = 4;

const int defaultPaddingBottom = 1;

RenderListBox::RenderListBox(Element* element)
    : RenderBlockFlow(element)
{
    ASSERT(element);
    ASSERT(element->isHTMLElement());
    ASSERT(isHTMLSelectElement(element));
}

RenderListBox::~RenderListBox()
{
#if USE_POPUP_LISTBOX
    if (m_popup)
        m_popup->disconnectClient();
    m_popup = nullptr;
#endif
}

inline HTMLSelectElement* RenderListBox::selectElement() const
{
    return toHTMLSelectElement(node());
}

int RenderListBox::size() const
{
    int specifiedSize = selectElement()->size();
    if (specifiedSize >= 1)
        return specifiedSize;

    return defaultSize;
}

LayoutUnit RenderListBox::defaultItemHeight() const
{
    return style()->fontMetrics().height() + defaultPaddingBottom;
}

LayoutUnit RenderListBox::itemHeight() const
{
    HTMLSelectElement* select = selectElement();
    if (!select)
        return 0;
    Element* baseItem = ElementTraversal::firstChild(*select);
    if (!baseItem)
        return defaultItemHeight();
    if (isHTMLOptGroupElement(baseItem))
        baseItem = &toHTMLOptGroupElement(baseItem)->optGroupLabelElement();
    else if (!isHTMLOptionElement(baseItem))
        return defaultItemHeight();
    RenderObject* baseItemRenderer = baseItem->renderer();
    if (!baseItemRenderer)
        return defaultItemHeight();
    if (!baseItemRenderer || !baseItemRenderer->isBox())
        return defaultItemHeight();
    return toRenderBox(baseItemRenderer)->height();
}

void RenderListBox::computeLogicalHeight(LayoutUnit, LayoutUnit logicalTop, LogicalExtentComputedValues& computedValues) const
{
    LayoutUnit height = itemHeight() * size();
    // FIXME: The item height should have been added before updateLogicalHeight was called to avoid this hack.
    updateIntrinsicContentLogicalHeight(height);

    height += borderAndPaddingHeight();

    RenderBox::computeLogicalHeight(height, logicalTop, computedValues);
}

void RenderListBox::stopAutoscroll()
{
    HTMLSelectElement* select = selectElement();
    if (select->isDisabledFormControl())
        return;
    select->handleMouseRelease();
}

void RenderListBox::computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const
{
    RenderBlockFlow::computeIntrinsicLogicalWidths(minLogicalWidth, maxLogicalWidth);
    if (style()->width().isPercent())
        minLogicalWidth = 0;
}

void RenderListBox::scrollToRect(const LayoutRect& rect)
{
    if (hasOverflowClip()) {
        ASSERT(layer());
        ASSERT(layer()->scrollableArea());
        layer()->scrollableArea()->exposeRect(rect, ScrollAlignment::alignToEdgeIfNeeded, ScrollAlignment::alignToEdgeIfNeeded);
    }
}

#if USE_POPUP_LISTBOX
void RenderListBox::updateFromElement()
{
#if USE_POPUP_LISTBOX
    if (m_popupIsVisible)
        m_popup->updateFromElement();
#endif
}

void RenderListBox::showPopup()
{
    if (m_popupIsVisible)
        return;

    if (document().frameHost()->chrome().hasOpenedPopup())
        return;

    // Create m_innerBlock here so it ends up as the first child.
    // This is important because otherwise we might try to create m_innerBlock
    // inside the showPopup call and it would fail.
    //createInnerBlock();
    if (!m_popup)
        m_popup = document().frameHost()->chrome().createPopupMenu(*document().frame(), this);
    m_popupIsVisible = true;

    FloatQuad quad(localToAbsoluteQuad(FloatQuad(borderBoundingBox())));
    IntSize size = pixelSnappedIntRect(frameRect()).size();
    HTMLSelectElement* select = selectElement();
    m_popup->show(quad, size, select->optionToListIndex(select->selectedIndex()));
}

void RenderListBox::hidePopup()
{
    if (m_popup)
        m_popup->hide();
}

void RenderListBox::valueChanged(unsigned listIndex, bool fireOnChange)
{
    // Check to ensure a page navigation has not occurred while
    // the popup was up.
    Document& doc = toElement(node())->document();
    if (&doc != doc.frame()->document())
        return;

    HTMLSelectElement* select = selectElement();
    select->optionSelectedByUser(select->listToOptionIndex(listIndex), fireOnChange);
}

void RenderListBox::listBoxSelectItem(int listIndex, bool allowMultiplySelections, bool shift, bool fireOnChangeNow)
{
    selectElement()->listBoxSelectItem(listIndex, allowMultiplySelections, shift, fireOnChangeNow);
}

bool RenderListBox::multiple() const
{
    return selectElement()->multiple();
}

String RenderListBox::itemText(unsigned listIndex) const
{
    HTMLSelectElement* select = selectElement();
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement> >& listItems = select->listItems();
    if (listIndex >= listItems.size())
        return String();

    String itemString;
    Element* element = listItems[listIndex];
    if (isHTMLOptGroupElement(*element))
        itemString = toHTMLOptGroupElement(*element).groupLabelText();
    else if (isHTMLOptionElement(*element))
        itemString = toHTMLOptionElement(*element).textIndentedToRespectGroupLabel();

    applyTextTransform(style(), itemString, ' ');
    return itemString;
}

String RenderListBox::itemAccessibilityText(unsigned listIndex) const
{
    // Allow the accessible name be changed if necessary.
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement> >& listItems = selectElement()->listItems();
    if (listIndex >= listItems.size())
        return String();
    return listItems[listIndex]->fastGetAttribute(aria_labelAttr);
}

String RenderListBox::itemToolTip(unsigned listIndex) const
{
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement> >& listItems = selectElement()->listItems();
    if (listIndex >= listItems.size())
        return String();
    return listItems[listIndex]->title();
}

bool RenderListBox::itemIsEnabled(unsigned listIndex) const
{
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement> >& listItems = selectElement()->listItems();
    if (listIndex >= listItems.size())
        return false;
    HTMLElement* element = listItems[listIndex];
    if (!isHTMLOptionElement(*element))
        return false;

    bool groupEnabled = true;
    if (Element* parentElement = element->parentElement()) {
        if (isHTMLOptGroupElement(*parentElement))
            groupEnabled = !parentElement->isDisabledFormControl();
    }
    if (!groupEnabled)
        return false;

    return !element->isDisabledFormControl();
}

PopupMenuStyle RenderListBox::itemStyle(unsigned listIndex) const
{
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement> >& listItems = selectElement()->listItems();
    if (listIndex >= listItems.size()) {
        // If we are making an out of bounds access, then we want to use the style
        // of a different option element (index 0). However, if there isn't an option element
        // before at index 0, we fall back to the menu's style.
        if (!listIndex)
            return menuStyle();

        // Try to retrieve the style of an option element we know exists (index 0).
        listIndex = 0;
    }
    HTMLElement* element = listItems[listIndex];

    Color itemBackgroundColor = resolveColor(CSSPropertyBackgroundColor);

    RenderStyle* style = element->renderStyle() ? element->renderStyle() : element->computedStyle();
    return style ? PopupMenuStyle(resolveColor(style, CSSPropertyColor), itemBackgroundColor, style->font(), style->visibility() == VISIBLE,
        style->display() == NONE, style->textIndent(), style->direction(), isOverride(style->unicodeBidi()),
        PopupMenuStyle::DefaultBackgroundColor) : menuStyle();
}

PopupMenuStyle RenderListBox::menuStyle() const
{
    const RenderObject* o = this;
    const RenderStyle* s = o->style();
    return PopupMenuStyle(o->resolveColor(CSSPropertyColor), o->resolveColor(CSSPropertyBackgroundColor), s->font(), s->visibility() == VISIBLE,
        s->display() == NONE, s->textIndent(), style()->direction(), isOverride(style()->unicodeBidi()));
}

LayoutUnit RenderListBox::clientPaddingLeft() const
{
    return paddingLeft();
}

const int endOfLinePadding = 2;
LayoutUnit RenderListBox::clientPaddingRight() const
{
    if (style()->appearance() == MenulistPart || style()->appearance() == MenulistButtonPart) {
        // For these appearance values, the theme applies padding to leave room for the
        // drop-down button. But leaving room for the button inside the popup menu itself
        // looks strange, so we return a small default padding to avoid having a large empty
        // space appear on the side of the popup menu.
        return endOfLinePadding;
    }

    // If the appearance isn't MenulistPart, then the select is styled (non-native), so
    // we want to return the user specified padding.
    return paddingRight();
}

int RenderListBox::listSize() const
{
    return selectElement()->listItems().size();
}

int RenderListBox::selectedIndex() const
{
    HTMLSelectElement* select = selectElement();
    return select->optionToListIndex(select->selectedIndex());
}

void RenderListBox::popupDidHide()
{
    m_popupIsVisible = false;
}

bool RenderListBox::itemIsSeparator(unsigned listIndex) const
{
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement> >& listItems = selectElement()->listItems();
    return listIndex < listItems.size() && isHTMLHRElement(*listItems[listIndex]);
}

bool RenderListBox::itemIsLabel(unsigned listIndex) const
{
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement> >& listItems = selectElement()->listItems();
    return listIndex < listItems.size() && isHTMLOptGroupElement(*listItems[listIndex]);
}

bool RenderListBox::itemIsSelected(unsigned listIndex) const
{
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement> >& listItems = selectElement()->listItems();
    if (listIndex >= listItems.size())
        return false;
    HTMLElement* element = listItems[listIndex];
    return isHTMLOptionElement(*element) && toHTMLOptionElement(*element).selected();
}

void RenderListBox::setTextFromItem(unsigned listIndex)
{
}
#endif
} // namespace blink

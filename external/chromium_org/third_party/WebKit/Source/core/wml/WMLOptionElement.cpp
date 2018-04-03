/**
 * Copyright (C) 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"

#if ENABLE(WML)
#include "core/wml/WMLOptionElement.h"

#include "core/HTMLNames.h"
#include "core/WMLNames.h"
#include "core/dom/Attribute.h"
#include "core/dom/NodeRenderStyle.h"
#include "core/rendering/style/RenderStyle.h"
#include "core/wml/WMLOptGroupElement.h"
#include "core/wml/WMLSelectElement.h"

namespace blink {

using namespace WMLNames;

WMLOptionElement::WMLOptionElement(Document& document)
    : WMLFormControlElement(optionTag, document)
{
}

PassRefPtr<WMLOptionElement> WMLOptionElement::create(Document& document)
{
    return adoptRef(new WMLOptionElement(document));
}

WMLOptionElement::~WMLOptionElement()
{
}

const AtomicString& WMLOptionElement::formControlType() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, option, ("option"));
    return option;
}

static inline WMLSelectElement* ownerSelectElement(Element* element)
{
    ContainerNode* select = element->parentNode();
    while (select && !select->hasTagName(selectTag))
        select = select->parentNode();

    if (!select)
        return 0;

    return static_cast<WMLSelectElement*>(select);
}

void WMLOptionElement::accessKeyAction(bool)
{
    if (WMLSelectElement* select = ownerSelectElement(this))
        select->accessKeySetSelectedIndex(OptionElement::optionIndex(select, this));
}

void WMLOptionElement::childrenChanged(const ChildrenChange& change)
{
    if (WMLSelectElement* select = ownerSelectElement(this))
        select->childrenChanged(change);
    Element::childrenChanged(change);
}

void WMLOptionElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == HTMLNames::valueAttr)
        m_data.setValue(parseValueSubstitutingVariableReferences(value));
    else if (name == HTMLNames::titleAttr)
        m_data.setLabel(parseValueSubstitutingVariableReferences(value));
    else if (name == onpickAttr) {
        // Register intrinsic event in card
        RefPtr<WMLIntrinsicEvent> event = WMLIntrinsicEvent::create(document(), value);

        createEventHandlerIfNeeded();
        eventHandler()->registerIntrinsicEvent(WMLIntrinsicEventOnPick, event);
    } else
        WMLFormControlElement::parseAttribute(name, value);
}

void WMLOptionElement::attach(const AttachContext& context)
{
    if (parentNode()->renderStyle())
        setRenderStyle(styleForRenderer());
    WMLFormControlElement::attach(context);
}

void WMLOptionElement::detach(const AttachContext& context)
{
    m_style.clear();
    WMLFormControlElement::detach(context);
}

void WMLOptionElement::setRenderStyle(PassRefPtr<RenderStyle> style)
{
    m_style = style;
}

Node::InsertionNotificationRequest WMLOptionElement::insertedInto(ContainerNode* insertionPoint)
{
    WMLSelectElement* select;
    if (selected() && (select = ownerSelectElement(this)))
        select->scrollToSelection();

    return WMLFormControlElement::insertedInto(insertionPoint);
}

bool WMLOptionElement::selected() const
{
    return m_data.selected();
}

void WMLOptionElement::setSelectedState(bool selected)
{
    if (this->selected() == selected)
        return;

    OptionElement::setSelectedState(m_data, this, selected);

    if (WMLSelectElement* select = ownerSelectElement(this)) {
        if (select->multiple() || selected)
            handleIntrinsicEventIfNeeded();
    }
}

String WMLOptionElement::value() const
{
    return OptionElement::collectOptionValue(m_data, this);
}

String WMLOptionElement::text() const
{
    return OptionElement::collectOptionLabelOrText(m_data, this);
}

String WMLOptionElement::textIndentedToRespectGroupLabel() const
{
    return OptionElement::collectOptionTextRespectingGroupLabel(m_data, this);
}

RenderStyle* WMLOptionElement::nonRendererRenderStyle() const
{
    return m_style.get();
}

void WMLOptionElement::handleIntrinsicEventIfNeeded()
{
    WMLSelectElement* select = ownerSelectElement(this);
    if (!select || !select->initialized())
        return;

    WMLIntrinsicEventHandler* eventHandler = this->eventHandler();
    if (!eventHandler)
        return;

    if (eventHandler->hasIntrinsicEvent(WMLIntrinsicEventOnPick))
        eventHandler->triggerIntrinsicEvent(WMLIntrinsicEventOnPick);
}

bool WMLOptionElement::disabled() const
{
    /* Dummy implementation, as disabled() is pure virtual in OptionElement class */
    return false;
}

bool WMLOptionElement::isDisplayNone() const
{
    ContainerNode* parent = parentNode();
    // Check for parent optgroup having display NONE
    if (parent && isWMLOptGroupElement(parent)) {
        if (toWMLOptGroupElement(*parent).isDisplayNone())
            return true;
    }
    RenderStyle* style = m_style.get();
    return style && style->display() == NONE;
}

}

#endif

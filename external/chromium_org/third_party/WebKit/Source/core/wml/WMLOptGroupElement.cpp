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
#include "core/wml/WMLOptGroupElement.h"

#include "core/HTMLNames.h"
#include "core/WMLNames.h"
#include "core/dom/Attribute.h"
#include "core/dom/Document.h"
#include "core/dom/NodeRenderStyle.h"
#include "core/rendering/style/RenderStyle.h"
#include "core/wml/WMLSelectElement.h"

namespace blink {

using namespace WMLNames;

WMLOptGroupElement::WMLOptGroupElement(Document& document)
    : WMLFormControlElement(optgroupTag, document)
{
}

PassRefPtr<WMLOptGroupElement> WMLOptGroupElement::create(Document& document)
{
    return adoptRef(new WMLOptGroupElement(document));
}

WMLOptGroupElement::~WMLOptGroupElement()
{
}

const AtomicString& WMLOptGroupElement::formControlType() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, optgroup, ("optgroup"));
    return optgroup;
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

void WMLOptGroupElement::accessKeyAction(bool)
{
    WMLSelectElement* select = ownerSelectElement(this);
    if (!select || select->focused())
        return;

    // send to the parent to bring focus to the list box
    select->accessKeyAction(false);
}

void WMLOptGroupElement::childrenChanged(const ChildrenChange& change)
{
    recalcSelectOptions();
    Element::childrenChanged(change);
}

void WMLOptGroupElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    WMLFormControlElement::parseAttribute(name, value);
    recalcSelectOptions();
}

void WMLOptGroupElement::attach(const AttachContext& context)
{
    if (parentNode()->renderStyle())
        setRenderStyle(styleForRenderer());
    WMLFormControlElement::attach(context);
}

void WMLOptGroupElement::detach(const AttachContext& context)
{
    m_style.clear();
    WMLFormControlElement::detach(context);
}

void WMLOptGroupElement::setRenderStyle(PassRefPtr<RenderStyle> style)
{
    m_style = style;
}

RenderStyle* WMLOptGroupElement::nonRendererRenderStyle() const
{
    return m_style.get();
}

String WMLOptGroupElement::groupLabelText() const
{
    String itemText = title();

    // In WinIE, leading and trailing whitespace is ignored in options and optgroups. We match this behavior.
    itemText = itemText.stripWhiteSpace();
    // We want to collapse our whitespace too.  This will match other browsers.
    itemText = itemText.simplifyWhiteSpace();

    return itemText;
}

void WMLOptGroupElement::recalcSelectOptions()
{
    if (WMLSelectElement* select = ownerSelectElement(this))
        select->setRecalcListItems();
}

bool WMLOptGroupElement::isDisplayNone() const
{
    RenderStyle* style = m_style.get();
    return style && style->display() == NONE;
}

}

#endif

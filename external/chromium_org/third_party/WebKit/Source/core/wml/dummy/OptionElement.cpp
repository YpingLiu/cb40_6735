/*
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
#include "OptionElement.h"

#include "core/HTMLNames.h"
#include "core/dom/Element.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/ScriptLoader.h"
#include "core/WMLNames.h"
#include "core/wml/WMLOptionElement.h"
#include "core/wml/dummy/OptionGroupElement.h"
#include "core/wml/dummy/SelectElement.h"
#include "wtf/Assertions.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

void OptionElement::setSelectedState(OptionElementData& data, Element* element, bool selected)
{
    if (data.selected() == selected)
        return;

    data.setSelected(selected);
    element->setNeedsStyleRecalc(LocalStyleChange);
}

int OptionElement::optionIndex(SelectElement* selectElement, const Element* element)
{
    if (!selectElement)
        return 0;

    // Let's do this dynamically. Might be a bit slow, but we're sure
    // we won't forget to update a member variable in some cases...
    const Vector<Element*>& items = selectElement->listItems();
    int length = items.size();
    int optionIndex = 0;
    for (int i = 0; i < length; ++i) {
        if (!isOptionElement(items[i]))
            continue;
        if (items[i] == element)
            return optionIndex;
        ++optionIndex;
    }

    return 0;
}

String OptionElement::collectOptionLabelOrText(const OptionElementData& data, const Element* element)
{
    String text;
    // FIXME:
    // WinIE does not use the label attribute, so as a quirk, we ignore it.
    //if (element->document().inQuirksMode())
    //    text = data.label();
    if (text.isEmpty())
        text = collectOptionInnerText(element);
    return text.stripWhiteSpace().simplifyWhiteSpace();
}

String OptionElement::collectOptionInnerText(const Element* element)
{
    /// M: @{
    StringBuilder text;
    for (Node* node = element->firstChild(); node; ) {
        if (node->isTextNode() || node->nodeType() == Node::CDATA_SECTION_NODE)
            text.append(node->nodeValue());
        // Text nodes inside script elements are not part of the option text.
        if (node->isElementNode() && toScriptLoaderIfPossible(toElement(node)))
            node = NodeTraversal::nextSkippingChildren(*node, element);
        else
            node = NodeTraversal::next(*node, element);
    }
    return text.toString();
    /// @}
}

String OptionElement::collectOptionTextRespectingGroupLabel(const OptionElementData& data, const Element* element)
{
    Element* parentElement = static_cast<Element*>(element->parentNode());
    if (parentElement && toOptionGroupElement(parentElement))
        return "    " + collectOptionLabelOrText(data, element);

    return collectOptionLabelOrText(data, element);
}

String OptionElement::collectOptionValue(const OptionElementData& data, const Element* element)
{
    String value = data.value();
    if (!value.isNull())
        return value;

    // Use the text if the value wasn't set.
    return collectOptionInnerText(element).stripWhiteSpace();
}

// OptionElementData
OptionElementData::OptionElementData()
    : m_selected(false)
{
}

OptionElementData::~OptionElementData()
{
}

OptionElement* toOptionElement(Element* element)
{
    if (element->isWMLElement() && element->hasTagName(WMLNames::optionTag))
        return static_cast<WMLOptionElement*>(element);
    return 0;
}

bool isOptionElement(Element* element)
{
    return element->hasTagName(WMLNames::optionTag);
}

}


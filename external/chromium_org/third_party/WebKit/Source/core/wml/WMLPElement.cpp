/**
 * Copyright (C) 2008 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
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
#include "core/wml/WMLPElement.h"

#include "core/dom/Attribute.h"
#include "core/dom/Document.h"
#include "core/dom/TagCollection.h"
#include "core/CSSPropertyNames.h"
#include "core/CSSValueKeywords.h"
#include "core/HTMLNames.h"
#include "core/WMLNames.h"

namespace blink {

using namespace WMLNames;

WMLPElement::WMLPElement(Document& document)
    : WMLElement(pTag, document)
{
}

PassRefPtr<WMLPElement> WMLPElement::create(Document& document)
{
    return adoptRef(new WMLPElement(document));
}

bool WMLPElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name == HTMLNames::alignAttr || name == modeAttr)
        return true;
    return WMLElement::isPresentationAttribute(name);
}

void WMLPElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (name == HTMLNames::alignAttr) {
        if (equalIgnoringCase(value, "middle") || equalIgnoringCase(value, "center"))
            addPropertyToPresentationAttributeStyle(style, CSSPropertyTextAlign, CSSValueWebkitCenter);
        else if (equalIgnoringCase(value, "left"))
            addPropertyToPresentationAttributeStyle(style, CSSPropertyTextAlign, CSSValueWebkitLeft);
        else if (equalIgnoringCase(value, "right"))
            addPropertyToPresentationAttributeStyle(style, CSSPropertyTextAlign, CSSValueWebkitRight);
        else
            addPropertyToPresentationAttributeStyle(style, CSSPropertyTextAlign, value);
    } else if (name == modeAttr) {
        m_mode = value;
        if (m_mode == "wrap")
            addPropertyToPresentationAttributeStyle(style, CSSPropertyWordWrap, CSSValueBreakWord);
        else if (m_mode == "nowrap")
            addPropertyToPresentationAttributeStyle(style, CSSPropertyWhiteSpace, CSSValueNowrap);
    } else
        WMLElement::parseAttribute(name, value);
}

Node::InsertionNotificationRequest WMLPElement::insertedInto(ContainerNode* insertionPoint)
{
    WMLElement::insertedInto(insertionPoint);

    // If not explicitly specified, the linewrap mode is identical to 
    // the line-wrap mode of the previous paragraph in the text flow of
    // a card. The default mode for the first paragraph in a card is wrap.
    if (!m_mode.isEmpty())
        return InsertionDone;

    RefPtr<TagCollection> collection = document().getElementsByTagName("p");
    if (!collection)
        return InsertionDone;

    unsigned length = collection->length();
    if (length < 2)
        return InsertionDone;

    // Assure we're the last inserted paragraph element
    // This only works while parsing, otherwhise this statement is never true.
    if (collection->item(length - 1) != this)
        return InsertionDone;

    WMLPElement* lastParagraph = static_cast<WMLPElement*>(collection->item(length - 2));
    ASSERT(lastParagraph);

    AtomicString lastMode = lastParagraph->getAttribute(modeAttr);
    if (lastMode.isEmpty() || lastMode == "wrap") // Default value, do nothing.
        return InsertionDone;

    setAttribute(modeAttr, lastMode);

    return InsertionDone;
}

}

#endif

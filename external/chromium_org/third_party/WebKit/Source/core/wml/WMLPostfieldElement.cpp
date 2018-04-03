/**
 * Copyright (C) 2008 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
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
#include "core/wml/WMLPostfieldElement.h"

#include "core/HTMLNames.h"
#include "core/WMLNames.h"
#include "core/wml/WMLDocument.h"
#include "core/wml/WMLGoElement.h"
#include "wtf/text/CString.h"
#include "wtf/text/TextEncoding.h"


namespace blink {

using namespace WMLNames;

WMLPostfieldElement::WMLPostfieldElement(Document& document)
    : WMLElement(postfieldTag, document)
{
}

PassRefPtr<WMLPostfieldElement> WMLPostfieldElement::create(Document& document)
{
    return adoptRef(new WMLPostfieldElement(document));
}

Node::InsertionNotificationRequest WMLPostfieldElement::insertedInto(ContainerNode* insertionPoint)
{
    WMLElement::insertedInto(insertionPoint);

    if (insertionPoint && insertionPoint->hasTagName(goTag))
        static_cast<WMLGoElement*>(insertionPoint)->registerPostfieldElement(this);

    return InsertionDone;
}

void WMLPostfieldElement::removedFrom(ContainerNode* insertionPoint)
{
    if (insertionPoint && insertionPoint->hasTagName(goTag))
        static_cast<WMLGoElement*>(insertionPoint)->deregisterPostfieldElement(this);

    WMLElement::removedFrom(insertionPoint);
}

String WMLPostfieldElement::name() const
{
    return parseValueSubstitutingVariableReferences(getAttribute(HTMLNames::nameAttr));
}

String WMLPostfieldElement::value() const
{
    return parseValueSubstitutingVariableReferences(getAttribute(HTMLNames::valueAttr));
}

static inline CString encodedString(const WTF::TextEncoding& encoding, const String& data)
{
    return encoding.encode(data, WTF::EntitiesForUnencodables);
}

void WMLPostfieldElement::encodeData(const WTF::TextEncoding& encoding, CString& name, CString& value)
{
    name = encodedString(encoding, this->name());
    value = encodedString(encoding, this->value());
}

}

#endif

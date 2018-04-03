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
#include "core/wml/WMLMetaElement.h"

#include "core/HTMLNames.h"
#include "core/WMLNames.h"
#include "core/dom/Attribute.h"
#include "core/dom/Document.h"

namespace blink {

using namespace WMLNames;

WMLMetaElement::WMLMetaElement(Document& document)
    : WMLElement(metaTag, document)
{
}

PassRefPtr<WMLMetaElement> WMLMetaElement::create(Document& document)
{
    return adoptRef(new WMLMetaElement(document));
}

WMLMetaElement::~WMLMetaElement()
{
}

void WMLMetaElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == HTMLNames::http_equivAttr)
        m_equiv = parseValueForbiddingVariableReferences(value);
    else if (name == HTMLNames::contentAttr)
        m_content = parseValueForbiddingVariableReferences(value);
    else if (name == HTMLNames::nameAttr) {
        // FIXME: The user agent must ignore any meta-data named with this attribute.
    } else
        WMLElement::parseAttribute(name, value);
}

Node::InsertionNotificationRequest WMLMetaElement::insertedInto(ContainerNode* insertionPoint)
{
    WMLElement::insertedInto(insertionPoint);

    if (m_equiv.isNull() || m_content.isNull())
        return InsertionDone;

    document().processHttpEquiv(m_equiv, m_content, true);

    return InsertionDone;
}

}

#endif

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
#include "core/wml/WMLAccessElement.h"

#include "core/WMLNames.h"
#include "core/dom/Attribute.h"
#include "core/wml/WMLDocument.h"
#include "core/wml/WMLVariables.h"

namespace blink {

using namespace WMLNames;

WMLAccessElement::WMLAccessElement(Document& document)
    : WMLElement(accessTag, document)
{
}

PassRefPtr<WMLAccessElement> WMLAccessElement::create(Document& document)
{
    return adoptRef(new WMLAccessElement(document));
}

void WMLAccessElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == domainAttr) {
        String parsedValue = parseValueForbiddingVariableReferences(value);
        if (parsedValue.isEmpty())
            return;

        m_domain = parsedValue;
    } else if (name == pathAttr) {
        String parsedValue = parseValueForbiddingVariableReferences(value);
        if (parsedValue.isEmpty())
            return;

        m_path = parsedValue;
    } else
        WMLElement::parseAttribute(name, value);
}

Node::InsertionNotificationRequest WMLAccessElement::insertedInto(ContainerNode* insertionPoint)
{
    WMLElement::insertedInto(insertionPoint);

    WMLPageState* pageState = wmlPageStateForDocument(document());
    if (!pageState || pageState->processAccessControlData(m_domain, m_path))
        return InsertionDone;

    pageState->resetAccessControlData();
    reportWMLError(document(), WMLErrorMultipleAccessElements);
    return InsertionDone;
}

}

#endif

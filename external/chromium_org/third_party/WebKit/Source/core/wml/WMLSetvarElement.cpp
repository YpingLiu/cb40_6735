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
#include "core/wml/WMLSetvarElement.h"

#include "core/HTMLNames.h"
#include "core/WMLNames.h"
#include "core/dom/Attribute.h"
#include "core/wml/WMLErrorHandling.h"
#include "core/wml/WMLTaskElement.h"
#include "core/wml/WMLVariables.h"


namespace blink {

using namespace WMLNames;

WMLSetvarElement::WMLSetvarElement(Document& document)
    : WMLElement(setvarTag, document)
{
}

PassRefPtr<WMLSetvarElement> WMLSetvarElement::create(Document& document)
{
    return adoptRef(new WMLSetvarElement(document));
}

WMLSetvarElement::~WMLSetvarElement()
{
}

void WMLSetvarElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == HTMLNames::nameAttr) {
        if (!isValidVariableName(parseValueSubstitutingVariableReferences(value, WMLErrorInvalidVariableName))) {
            reportWMLError(document(), WMLErrorInvalidVariableName);
            return;
        }
    } else
        WMLElement::parseAttribute(name, value);
}

Node::InsertionNotificationRequest WMLSetvarElement::insertedInto(ContainerNode* insertionPoint)
{
    WMLElement::insertedInto(insertionPoint);
 
    if (!insertionPoint || !insertionPoint->isWMLElement())
        return InsertionDone;

    if (static_cast<WMLElement*>(insertionPoint)->isWMLTaskElement())
        static_cast<WMLTaskElement*>(insertionPoint)->registerVariableSetter(this);

    return InsertionDone;
}

void WMLSetvarElement::removedFrom(ContainerNode* insertionPoint)
{
    if (insertionPoint && insertionPoint->isWMLElement()) {
        if (static_cast<WMLElement*>(insertionPoint)->isWMLTaskElement())
            static_cast<WMLTaskElement*>(insertionPoint)->deregisterVariableSetter(this);
    }

    WMLElement::removedFrom(insertionPoint);
}

String WMLSetvarElement::name() const
{
    return parseValueSubstitutingVariableReferences(getAttribute(HTMLNames::nameAttr), WMLErrorInvalidVariableName);
}

String WMLSetvarElement::value() const
{
    return parseValueSubstitutingVariableReferences(getAttribute(HTMLNames::valueAttr));
}

}

#endif

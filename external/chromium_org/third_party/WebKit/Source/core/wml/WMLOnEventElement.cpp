/**
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
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
#include "core/wml/WMLOnEventElement.h"

#include "core/dom/Attribute.h"
#include "core/HTMLNames.h"
#include "core/wml/WMLErrorHandling.h"
#include "core/wml/WMLEventHandlingElement.h"
#include "core/wml/WMLIntrinsicEventHandler.h"
#include "core/WMLNames.h"
#include "core/wml/WMLVariables.h"

namespace blink {

using namespace WMLNames;

WMLOnEventElement::WMLOnEventElement(Document& document)
    : WMLElement(oneventTag, document)
    , m_type(WMLIntrinsicEventUnknown)
{
}

PassRefPtr<WMLOnEventElement> WMLOnEventElement::create(Document& document)
{
    return adoptRef(new WMLOnEventElement(document));
}

void WMLOnEventElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == HTMLNames::typeAttr) {
        String parsedValue = parseValueForbiddingVariableReferences(value);
        if (parsedValue.isEmpty())
            return;

        if (parsedValue == onenterforwardAttr.toString())
            m_type = WMLIntrinsicEventOnEnterForward;
        else if (parsedValue == onenterbackwardAttr.toString())
            m_type = WMLIntrinsicEventOnEnterBackward;
        else if (parsedValue == ontimerAttr.toString())
            m_type = WMLIntrinsicEventOnTimer;
        else if (parsedValue == onpickAttr.toString())
            m_type = WMLIntrinsicEventOnPick;
    } else
        WMLElement::parseAttribute(name, value);
}

static inline WMLEventHandlingElement* eventHandlingParent(Node* parent)
{
    if (!parent || !parent->isWMLElement())
        return 0;

    return toWMLEventHandlingElement(static_cast<WMLElement*>(parent));
}

void WMLOnEventElement::registerTask(WMLTaskElement* task)
{
    if (m_type == WMLIntrinsicEventUnknown)
        return;

    // Register intrinsic event to the event handler of the owner of onevent element 
    WMLEventHandlingElement* eventHandlingElement = eventHandlingParent(parentNode());
    if (!eventHandlingElement)
        return;

    eventHandlingElement->createEventHandlerIfNeeded();

    RefPtr<WMLIntrinsicEvent> event = WMLIntrinsicEvent::createWithTask(task);
    if (!eventHandlingElement->eventHandler()->registerIntrinsicEvent(m_type, event))
        reportWMLError(document(), WMLErrorConflictingEventBinding);
}

void WMLOnEventElement::deregisterTask(WMLTaskElement*)
{
    WMLEventHandlingElement* eventHandlingElement = eventHandlingParent(parentNode());
    if (!eventHandlingElement)
        return;

    eventHandlingElement->eventHandler()->deregisterIntrinsicEvent(m_type);
}

}

#endif

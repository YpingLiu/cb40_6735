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
#include "core/wml/WMLTimerElement.h"

#include "core/HTMLNames.h"
#include "core/WMLNames.h"
#include "core/dom/Attribute.h"
#include "core/wml/WMLCardElement.h"
#include "core/wml/WMLDocument.h"
#include "core/wml/WMLPageState.h"
#include "core/wml/WMLTemplateElement.h"
#include "core/wml/WMLVariables.h"

namespace blink {

using namespace WMLNames;

WMLTimerElement::WMLTimerElement(Document& document)
    : WMLElement(timerTag, document)
    , m_timer(this, &WMLTimerElement::timerFired)
{
}

PassRefPtr<WMLTimerElement> WMLTimerElement::create(Document& document)
{
    return adoptRef(new WMLTimerElement(document));
}

void WMLTimerElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == HTMLNames::nameAttr)
        m_name = parseValueForbiddingVariableReferences(value);
    else
        WMLElement::parseAttribute(name, value);
}

Node::InsertionNotificationRequest WMLTimerElement::insertedInto(ContainerNode* insertionPoint)
{
    WMLElement::insertedInto(insertionPoint);

    // If the value of timeout is not a positive integer, ignore it
    if (value().toInt() <= 0)
        return InsertionDone;

    if (!insertionPoint || !insertionPoint->isWMLElement())
        return InsertionDone;

    if (insertionPoint->hasTagName(cardTag)) {
        m_card = static_cast<WMLCardElement*>(insertionPoint);
        m_card->setIntrinsicEventTimer(this);
    }

    return InsertionDone;
}

void WMLTimerElement::removedFrom(ContainerNode* insertionPoint)
{
    if (insertionPoint && insertionPoint->isWMLElement() && insertionPoint->hasTagName(cardTag)) {
        m_card->setIntrinsicEventTimer(0);
        m_card = 0;
    }

    WMLElement::removedFrom(insertionPoint);
}

void WMLTimerElement::timerFired(Timer<WMLTimerElement>*)
{
    if (!m_card)
        return;

    WMLPageState* pageState = wmlPageStateForDocument(document());
    if (!pageState)
        return;

    String value = this->value();

    // When the timer expires, set the name varialbe of timer to '0'
    if (!m_name.isEmpty()) {
        value = "0";
        pageState->storeVariable(m_name, value);
    }

    WMLIntrinsicEventType eventType = WMLIntrinsicEventOnTimer;
    WMLIntrinsicEventHandler* eventHandler = m_card->eventHandler();

    bool hasIntrinsicEvent = false;
    if (eventHandler && eventHandler->hasIntrinsicEvent(eventType))
        hasIntrinsicEvent = true;
    else if (m_card->templateElement()) {
        eventHandler = m_card->templateElement()->eventHandler();
        if (eventHandler && eventHandler->hasIntrinsicEvent(eventType))
            hasIntrinsicEvent = true;
    }

    if (hasIntrinsicEvent)
        eventHandler->triggerIntrinsicEvent(eventType);
}

void WMLTimerElement::start(int interval)
{
    if (!m_card || m_timer.isActive())
        return;

    if (interval <= 0 && !m_name.isEmpty()) {
        if (WMLPageState* pageState = wmlPageStateForDocument(document()))
            interval = pageState->getVariable(m_name).toInt();
    }

    if (interval <= 0)
        interval = value().toInt();

    if (interval > 0)
        m_timer.startOneShot(interval / 10.0f, FROM_HERE);
}

void WMLTimerElement::stop()
{
    if (m_timer.isActive())
        m_timer.stop();
}

void WMLTimerElement::storeIntervalToPageState()
{
    if (!m_timer.isActive())
        return;

    int interval = static_cast<int>(m_timer.nextFireInterval()) * 10;

    if (WMLPageState* pageState = wmlPageStateForDocument(document()))
        pageState->storeVariable(m_name, String::number(interval));
}

String WMLTimerElement::value() const
{
    return parseValueSubstitutingVariableReferences(getAttribute(HTMLNames::valueAttr));
}

}

#endif

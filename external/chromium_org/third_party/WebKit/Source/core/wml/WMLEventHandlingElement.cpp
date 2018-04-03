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
#include "core/wml/WMLEventHandlingElement.h"

#include "core/wml/WMLCardElement.h"
#include "core/wml/WMLDoElement.h"
#include "core/wml/WMLIntrinsicEventHandler.h"
#include "core/wml/WMLOptionElement.h"
#include "core/wml/WMLTaskElement.h"
#include "core/wml/WMLTemplateElement.h"
#include "core/WMLNames.h"

namespace blink {

using namespace WMLNames;

WMLEventHandlingElement::WMLEventHandlingElement()
{
}

WMLEventHandlingElement::~WMLEventHandlingElement()
{
}

void WMLEventHandlingElement::createEventHandlerIfNeeded()
{
    if (!m_eventHandler)
        m_eventHandler = adoptPtr(new WMLIntrinsicEventHandler);
}

void WMLEventHandlingElement::registerDoElement(WMLDoElement* doElement, Document& document)
{
    Vector<WMLDoElement*>::iterator it = m_doElements.begin();
    Vector<WMLDoElement*>::iterator end = m_doElements.end();

    for (; it != end; ++it) {
        if ((*it)->name() == doElement->name()) {
            reportWMLError(document, WMLErrorDuplicatedDoElement);
            return;
        }
    }

    ASSERT(m_doElements.find(doElement) == kNotFound);
    m_doElements.append(doElement);
    doElement->setActive(true);
}

void WMLEventHandlingElement::deregisterDoElement(WMLDoElement* doElement)
{
    doElement->setActive(false);

    size_t position = m_doElements.find(doElement);
    if (position == kNotFound)
        return;

    m_doElements.remove(position);
}

WMLEventHandlingElement* toWMLEventHandlingElement(WMLElement* element)
{
    if (!element->isWMLElement())
        return 0;

    if (element->hasTagName(cardTag))
        return static_cast<WMLCardElement*>(element);
    else if (element->hasTagName(optionTag))
        return static_cast<WMLOptionElement*>(element);
    else if (element->hasTagName(templateTag))
        return static_cast<WMLTemplateElement*>(element);
        ///M: workground for op01 wml test case.
    else if (element->hasTagName(pTag)) {
        Node* parent = element->parentNode();
        if (!parent || !parent->isWMLElement())
            return 0;
        return toWMLEventHandlingElement(static_cast<WMLElement*>(parent));
    }

    return 0;
}

}

#endif

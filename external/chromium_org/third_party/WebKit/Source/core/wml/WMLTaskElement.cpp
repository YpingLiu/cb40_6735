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
#include "core/wml/WMLTaskElement.h"

#include "core/wml/WMLAnchorElement.h"
#include "core/wml/WMLDoElement.h"
#include "core/WMLNames.h"
#include "core/wml/WMLOnEventElement.h"
#include "core/wml/WMLPageState.h"
#include "core/wml/WMLSetvarElement.h"

namespace blink {

using namespace WMLNames;

WMLTaskElement::WMLTaskElement(const QualifiedName& tagName, Document& document)
    : WMLElement(tagName, document)
{
}

WMLTaskElement::~WMLTaskElement()
{
}

Node::InsertionNotificationRequest WMLTaskElement::insertedInto(ContainerNode* insertionPoint)
{
    WMLElement::insertedInto(insertionPoint);

    if (!insertionPoint || !insertionPoint->isWMLElement())
        return InsertionDone;

    if (insertionPoint->hasTagName(anchorTag))
        static_cast<WMLAnchorElement*>(insertionPoint)->registerTask(this);
    else if (insertionPoint->hasTagName(doTag))
        static_cast<WMLDoElement*>(insertionPoint)->registerTask(this);
    else if (insertionPoint->hasTagName(oneventTag))
        static_cast<WMLOnEventElement*>(insertionPoint)->registerTask(this);

    return InsertionDone;
}

void WMLTaskElement::removedFrom(ContainerNode* insertionPoint)
{
    if (insertionPoint && insertionPoint->isWMLElement()) {
        if (insertionPoint->hasTagName(anchorTag))
            static_cast<WMLAnchorElement*>(insertionPoint)->deregisterTask(this);
        else if (insertionPoint->hasTagName(doTag))
            static_cast<WMLDoElement*>(insertionPoint)->deregisterTask(this);
        else if (insertionPoint->hasTagName(oneventTag))
            static_cast<WMLOnEventElement*>(insertionPoint)->deregisterTask(this);
    }

    WMLElement::removedFrom(insertionPoint);
}

void WMLTaskElement::registerVariableSetter(WMLSetvarElement* element)
{
    ASSERT(m_variableSetterElements.find(element) == WTF::kNotFound);
    m_variableSetterElements.append(element);
}

void WMLTaskElement::deregisterVariableSetter(WMLSetvarElement* element)
{
    size_t position = m_variableSetterElements.find(element);
    ASSERT(position != WTF::kNotFound);
    m_variableSetterElements.remove(position);
}

void WMLTaskElement::storeVariableState(WMLPageState* pageState)
{
    if (!pageState || m_variableSetterElements.isEmpty())
        return;

    WMLVariableMap variables;
    Vector<WMLSetvarElement*>::iterator it = m_variableSetterElements.begin();
    Vector<WMLSetvarElement*>::iterator end = m_variableSetterElements.end();

    for (; it != end; ++it) {
        WMLSetvarElement* setterElement = (*it);

        String name = setterElement->name();
        if (name.isEmpty())
            continue;

        String value = setterElement->value();
        variables.set(name, value);

        // The next setvar element may depend on this variable value. It's safe to store the current
        // name value pair in the page state, as the whole page state is replaced soon by this new map
        pageState->storeVariable(name, value);
    }

    pageState->storeVariables(variables);
}

}

#endif

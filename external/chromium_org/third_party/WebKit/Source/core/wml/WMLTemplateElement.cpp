/*
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
#include "core/wml/WMLTemplateElement.h"

#include "core/dom/Attribute.h"
#include "core/dom/TagCollection.h"
#include "core/wml/WMLCardElement.h"
#include "core/wml/WMLDocument.h"
#include "core/wml/WMLIntrinsicEventHandler.h"
#include "core/WMLNames.h"

namespace blink {

using namespace WMLNames;

WMLTemplateElement::WMLTemplateElement(Document& document)
    : WMLElement(templateTag, document)
{
}

WMLTemplateElement::~WMLTemplateElement()
{
}

PassRefPtr<WMLTemplateElement> WMLTemplateElement::create(Document& document)
{
    return adoptRef(new WMLTemplateElement(document));
}

void WMLTemplateElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    WMLIntrinsicEventType eventType = WMLIntrinsicEventUnknown;

    if (name == onenterforwardAttr)
        eventType = WMLIntrinsicEventOnEnterForward;
    else if (name == onenterbackwardAttr)
        eventType = WMLIntrinsicEventOnEnterBackward;
    else if (name == ontimerAttr)
        eventType = WMLIntrinsicEventOnTimer;
    else {
        WMLElement::parseAttribute(name, value);
        return;
    }

    if (eventType == WMLIntrinsicEventUnknown)
        return;

    // Register intrinsic event in card
    RefPtr<WMLIntrinsicEvent> event = WMLIntrinsicEvent::create(document(), value);

    createEventHandlerIfNeeded();
    eventHandler()->registerIntrinsicEvent(eventType, event);
}

void WMLTemplateElement::registerTemplatesInDocument(Document* doc)
{
    ASSERT(doc);

    // Build list of cards in document
    RefPtr<TagCollection> collection = doc->getElementsByTagName("card");
    if (!collection)
        return;

    unsigned length = collection->length();
    if (length < 1)
        return;

    HashSet<WMLCardElement*> cards;
    for (unsigned i = 0; i < length; ++i)
        cards.add(static_cast<WMLCardElement*>(collection->item(i)));

    if (cards.isEmpty())
        return;

    // Register template element to all cards
    collection = doc->getElementsByTagName("template");
    if (!collection)
        return;

    length = collection->length();
    if (length < 1)
        return;

    // Only one template element should be allowed in a document
    // Calling setTemplateElement() twice on a WMLCardElement, will result in a parser error.
    for (unsigned i = 0; i < length; ++i) {
        WMLTemplateElement* temp = static_cast<WMLTemplateElement*>(collection->item(i));

        HashSet<WMLCardElement*>::iterator it = cards.begin();
        HashSet<WMLCardElement*>::iterator end = cards.end();

        for (; it != end; ++it)
            (*it)->setTemplateElement(temp);
    }
}

}

#endif

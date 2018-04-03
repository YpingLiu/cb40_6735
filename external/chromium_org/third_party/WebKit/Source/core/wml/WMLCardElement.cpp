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
#include "core/wml/WMLCardElement.h"

#include "core/HTMLNames.h"
#include "core/WMLNames.h"
#include "core/dom/Attribute.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/TagCollection.h"
#include "core/frame/LocalFrame.h"
#include "core/loader/FrameLoader.h"
#include "core/page/Page.h"
#include "core/rendering/style/RenderStyle.h"
#include "core/wml/WMLDoElement.h"
#include "core/wml/WMLDocument.h"
#include "core/wml/WMLInputElement.h"
#include "core/wml/WMLIntrinsicEventHandler.h"
#include "core/wml/WMLSelectElement.h"
#include "core/wml/WMLTemplateElement.h"
#include "core/wml/WMLTimerElement.h"
#include "core/wml/WMLVariables.h"

namespace blink {

using namespace WMLNames;

WMLCardElement::WMLCardElement(Document& document)
    : WMLElement(cardTag, document)
    , m_isNewContext(false)
    , m_isOrdered(false)
    , m_isVisible(false)
    , m_eventTimer(0)
    , m_template(0)
{
    ASSERT(hasTagName(cardTag));
}

PassRefPtr<WMLCardElement> WMLCardElement::create(Document& document)
{
    return adoptRef(new WMLCardElement(document));
}

WMLCardElement::~WMLCardElement()
{
}

void WMLCardElement::showCard()
{
    ASSERT(attached());

    if (m_isVisible) {
        ASSERT(renderer());
        return;
    }

    m_isVisible = true;
    ASSERT(!renderer());

    lazyReattachIfAttached();
}

void WMLCardElement::hideCard()
{
    ASSERT(attached());

    if (!m_isVisible) {
        ASSERT(!renderer());
        return;
    }

    m_isVisible = false;
    ASSERT(renderer());

    lazyReattachIfAttached();
}

void WMLCardElement::setTemplateElement(WMLTemplateElement* temp)
{
    // Only one template is allowed to be attached to a card
    if (m_template) {
        reportWMLError(document(), WMLErrorMultipleTemplateElements);
        return;
    }

    m_template = temp;
}

void WMLCardElement::setIntrinsicEventTimer(WMLTimerElement* timer)
{
    // Only one timer is allowed in a card 
    if (m_eventTimer) {
        reportWMLError(document(), WMLErrorMultipleTimerElements);
        return;
    }

    m_eventTimer = timer;
}

void WMLCardElement::handleIntrinsicEventIfNeeded()
{
    WMLPageState* pageState = wmlPageStateForDocument(document());
    if (!pageState)
        return;

    LocalFrame* frame = document().frame();
    if (!frame)
        return;

    // Calculate the entry method of current card 
    WMLIntrinsicEventType eventType = WMLIntrinsicEventUnknown;

    switch (frame->loader().loadTypeWML()) {
    case FrameLoadTypeReload:
        break;
    case FrameLoadTypeWMLBackward:
        eventType = WMLIntrinsicEventOnEnterBackward;
        break;
    case FrameLoadTypeWMLDeckNotAccessible:
        reportWMLError(document(), WMLErrorDeckNotAccessible);
        return;
    default:
        eventType = WMLIntrinsicEventOnEnterForward;
        break;
    }

    // Figure out target event handler
    WMLIntrinsicEventHandler* eventHandler = this->eventHandler();
    bool hasIntrinsicEvent = false;

    if (eventType != WMLIntrinsicEventUnknown) {
        if (eventHandler && eventHandler->hasIntrinsicEvent(eventType))
            hasIntrinsicEvent = true;
        else if (m_template) {
            eventHandler = m_template->eventHandler();
            if (eventHandler && eventHandler->hasIntrinsicEvent(eventType))
                hasIntrinsicEvent = true;
        }
    }
 
    if (hasIntrinsicEvent)
        eventHandler->triggerIntrinsicEvent(eventType);

    // Start the timer if it exists in current card
    if (m_eventTimer && eventType != WMLIntrinsicEventOnEnterBackward)
        m_eventTimer->start();

    for (Element* element = ElementTraversal::firstWithin(*this); element; element = ElementTraversal::next(*element, this)) {
        if (element->hasTagName(inputTag))
            static_cast<WMLInputElement*>(element)->initialize();
        else if (element->hasTagName(selectTag))
            static_cast<WMLSelectElement*>(element)->selectInitialOptions();
    }
}

void WMLCardElement::handleDeckLevelTaskOverridesIfNeeded()
{
    // Spec: The event-handling element may appear inside a template element and specify 
    // event-processing behaviour for all cards in the deck. A deck-level event-handling
    // element is equivalent to specifying the event-handling element in each card. 
    if (!m_template) 
        return;

    Vector<WMLDoElement*>& templateDoElements = m_template->doElements();
    if (templateDoElements.isEmpty())
        return;

    Vector<WMLDoElement*>& cardDoElements = doElements();
    Vector<WMLDoElement*>::iterator it = cardDoElements.begin();
    Vector<WMLDoElement*>::iterator end = cardDoElements.end();

    HashSet<String> cardDoElementNames;
    for (; it != end; ++it)
        cardDoElementNames.add((*it)->name());

    it = templateDoElements.begin();
    end = templateDoElements.end();

    for (; it != end; ++it)
        (*it)->setActive(!cardDoElementNames.contains((*it)->name()));
}

void WMLCardElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    WMLIntrinsicEventType eventType = WMLIntrinsicEventUnknown;

    if (name == onenterforwardAttr)
        eventType = WMLIntrinsicEventOnEnterForward;
    else if (name == onenterbackwardAttr)
        eventType = WMLIntrinsicEventOnEnterBackward;
    else if (name == ontimerAttr)
        eventType = WMLIntrinsicEventOnTimer;
    else if (name == newcontextAttr)
        m_isNewContext = (value == "true");
    else if (name == orderedAttr)
        m_isOrdered = (value == "true");
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

Node::InsertionNotificationRequest WMLCardElement::insertedInto(ContainerNode* insertionPoint)
{
    WMLElement::insertedInto(insertionPoint);
    Document& document = this->document();

    // The first card inserted into a document, is visible by default.
    if (!m_isVisible) {
        RefPtr<TagCollection> collection = document.getElementsByTagName("card");
        if (collection && collection->length() == 1 && collection->item(0) == this)
            m_isVisible = true;
    }

    // For the WML layout tests we embed WML content in a XHTML document. Navigating to different cards
    // within the same deck has a different behaviour in HTML than in WML. HTML wants to "scroll to anchor"
    // (see FrameLoader) but WML wants a reload. Notify the root document of the layout test that we want
    // to mimic WML behaviour. This is rather tricky, but has been tested extensively. Usually it's not possible
    // at all to embed WML in HTML, it's not designed that way, we're just "abusing" it for dynamically created layout tests.
    if (document.page() && document.page()->mainFrame()) {
        Document* rootDocument = document.page()->deprecatedLocalMainFrame()->document();
        if (rootDocument != &document)
            rootDocument->setContainsWMLContent(true);
    }

    return InsertionDone;
}

RenderObject* WMLCardElement::createRenderer(RenderStyle* style)
{
    if (!m_isVisible) {
        style->setUnique();
        style->setDisplay(NONE);
    }

    return WMLElement::createRenderer(style);
}

WMLCardElement* WMLCardElement::findNamedCardInDocument(Document* doc, const String& cardName)
{
    if (cardName.isEmpty())
        return 0;

    RefPtr<TagCollection> collection = doc->getElementsByTagName("card");
    if (!collection)
        return 0;

    unsigned length = collection->length();
    if (length < 1)
        return 0;

    for (unsigned i = 0; i < length; ++i) {
        WMLCardElement* card = static_cast<WMLCardElement*>(collection->item(i));
        if (card->getIdAttribute() != cardName)
            continue;

        return card;
    }

    return 0;
}

WMLCardElement* WMLCardElement::determineActiveCard(Document* doc)
{
    WMLPageState* pageState = wmlPageStateForDocument(*doc);
    if (!pageState)
        return 0;

    RefPtr<TagCollection> collection = doc->getElementsByTagName("card");
    if (!collection)
        return 0;

    unsigned length = collection->length();
    if (length < 1)
        return 0;

    // Figure out the new target card
    String cardName = doc->url().fragmentIdentifier();

    WMLCardElement* activeCard = findNamedCardInDocument(doc, cardName);
    if (activeCard) {
        // Hide all cards - except the destination card - in document
        for (unsigned i = 0; i < length; ++i) {
            WMLCardElement* card = static_cast<WMLCardElement*>(collection->item(i));

            if (card == activeCard)
                card->showCard();
            else
                card->hideCard();
        }
    } else {
        // If the target URL didn't contain a fragment identifier, activeCard
        // is 0, and has to be set to the first card element in the deck.
        activeCard = static_cast<WMLCardElement*>(collection->item(0));
        activeCard->showCard();
    }

    // Assure destination card is visible
    ASSERT(activeCard->isVisible());
    ASSERT(activeCard->attached());
    ASSERT(activeCard->renderer());

    // Update the document title
    doc->setTitle(activeCard->title());

    return activeCard;
}

}

#endif

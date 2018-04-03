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
#include "core/wml/WMLPrevElement.h"

#include "core/frame/Frame.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/page/Page.h"
#include "core/wml/WMLCardElement.h"
#include "core/wml/WMLDocument.h"
#include "core/wml/WMLPageState.h"
#include "core/wml/WMLTimerElement.h"
#include "core/WMLNames.h"

namespace blink {

using namespace WMLNames;

WMLPrevElement::WMLPrevElement(Document& document)
    : WMLTaskElement(prevTag, document)
{
}

PassRefPtr<WMLPrevElement> WMLPrevElement::create(Document& document)
{
    return adoptRef(new WMLPrevElement(document));
}

WMLPrevElement::~WMLPrevElement()
{
}

void WMLPrevElement::executeTask()
{
    ASSERT(document().isWMLDocument());
    WMLDocument& document = static_cast<WMLDocument&>(this->document());

    WMLPageState* pageState = wmlPageStateForDocument(document);
    if (!pageState)
        return;

    WMLCardElement* card = document.activeCard();
    if (!card)
        return;

    storeVariableState(pageState);

    // Stop the timer of the current card if it is active
    if (WMLTimerElement* eventTimer = card->eventTimer())
        eventTimer->stop();

    document.frame()->loader().client()->navigateBackForward(-1);
}

}

#endif

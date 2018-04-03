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
#include "core/wml/WMLGoElement.h"

#include "core/HTMLNames.h"
#include "core/WMLNames.h"
#include "core/dom/Attribute.h"
#include "core/loader/FormSubmission.h"
#include "core/frame/LocalFrame.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoadRequest.h"
#include "platform/network/FormData.h"
#include "platform/network/FormDataBuilder.h"
#include "platform/network/ResourceRequest.h"
#include "platform/weborigin/SecurityPolicy.h"
#include "core/wml/WMLCardElement.h"
#include "core/wml/WMLDocument.h"
#include "core/wml/WMLPageState.h"
#include "core/wml/WMLPostfieldElement.h"
#include "core/wml/WMLTimerElement.h"
#include "core/wml/WMLVariables.h"
#include "wtf/text/CString.h"
#include "wtf/text/TextEncoding.h"

namespace blink {

using namespace WMLNames;

WMLGoElement::WMLGoElement(Document& document)
    : WMLTaskElement(goTag, document)
{
}
 
PassRefPtr<WMLGoElement> WMLGoElement::create(Document& document)
{
    return adoptRef(new WMLGoElement(document));
}

void WMLGoElement::registerPostfieldElement(WMLPostfieldElement* postfield)
{
    ASSERT(m_postfieldElements.find(postfield) == WTF::kNotFound);
    m_postfieldElements.append(postfield);
}
 
void WMLGoElement::deregisterPostfieldElement(WMLPostfieldElement* postfield)
{
    size_t position = m_postfieldElements.find(postfield);
    ASSERT(position != kNotFound);
    m_postfieldElements.remove(position);
}

void WMLGoElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == HTMLNames::methodAttr)
        m_formAttributes.updateMethodType(value);
    else if (name == HTMLNames::enctypeAttr)
        m_formAttributes.updateEncodingType(parseValueSubstitutingVariableReferences(value));
    else if (name == HTMLNames::accept_charsetAttr)
        m_formAttributes.setAcceptCharset(parseValueForbiddingVariableReferences(value));
    else
        WMLTaskElement::parseAttribute(name, value);
}

void WMLGoElement::executeTask()
{
    ASSERT(document().isWMLDocument());
    WMLDocument& document = static_cast<WMLDocument&>(this->document());

    WMLPageState* pageState = wmlPageStateForDocument(document);
    if (!pageState)
        return;

    WMLCardElement* card = document.activeCard();
    if (!card)
        return;

    LocalFrame* frame = document.frame();
    if (!frame)
        return;

    FrameLoader& loader = frame->loader();

    String href = getAttribute(HTMLNames::hrefAttr);
    if (href.isEmpty())
        return;

    // Substitute variables within target url attribute value
    KURL url = document.completeURL(substituteVariableReferences(href, document, WMLVariableEscapingEscape));
    if (url.isEmpty())
        return;

    storeVariableState(pageState);

    // Stop the timer of the current card if it is active
    if (WMLTimerElement* eventTimer = card->eventTimer())
        eventTimer->stop();

    // FIXME: 'newcontext' handling not implemented for external cards
    bool inSameDeck = document.url().path() == url.path();
    if (inSameDeck && url.hasFragmentIdentifier()) {
        if (WMLCardElement* card = WMLCardElement::findNamedCardInDocument(&document, url.fragmentIdentifier())) {
            if (card->isNewContext())
                pageState->reset();
        }
    }

    // Prepare loading the destination url
    ResourceRequest request(url);

    if (getAttribute(sendrefererAttr) == "true") {
        String referrer = SecurityPolicy::generateReferrerHeader(document.referrerPolicy(), request.url(), document.outgoingReferrer());
        request.setHTTPReferrer(Referrer(referrer, document.referrerPolicy()));
    }

    AtomicString cacheControl = getAttribute(cache_controlAttr);

    if (m_formAttributes.method() == FormSubmission::PostMethod)
        preparePOSTRequest(request, inSameDeck, cacheControl);
    else
        prepareGETRequest(request, url);

    // Set HTTP cache-control header if needed
    if (!cacheControl.isEmpty()) {
        request.setHTTPHeaderField("cache-control", cacheControl);

        if (cacheControl == "no-cache")
            request.setCachePolicy(ReloadIgnoringCacheData);
    }

    FrameLoadRequest frameRequest(&document, request);
    loader.load(frameRequest);
}

void WMLGoElement::preparePOSTRequest(ResourceRequest& request, bool inSameDeck, const String& cacheControl)
{
    request.setHTTPMethod("POST");

    /// M+ workaround for OP01 PIM website
    if (inSameDeck && !cacheControl.isEmpty() && cacheControl != "no-cache") {
    /// M-
        request.setCachePolicy(ReturnCacheDataDontLoad);
        return;
    }

    RefPtr<FormData> data;

    if (m_formAttributes.isMultiPartForm()) { // multipart/form-data
        Vector<char> boundary = FormDataBuilder::generateUniqueBoundaryString();
        data = createFormData(boundary.data());
        request.setHTTPContentType(m_formAttributes.encodingType() + "; boundary=" + boundary.data());
    } else {
        // text/plain or application/x-www-form-urlencoded
        data = createFormData(CString());
        request.setHTTPContentType(m_formAttributes.encodingType());
    }

    request.setHTTPBody(data.get());
}

void WMLGoElement::prepareGETRequest(ResourceRequest& request, const KURL& url)
{
    request.setHTTPMethod("GET");

    // Eventually display error message?
    if (m_formAttributes.isMultiPartForm())
        return;

    RefPtr<FormData> data = createFormData(CString());

    KURL remoteURL(url);
    /// M: TODO WML_SUPPORT Check if we still need this workaround for ALPS00231562 @{
    String query = data->flattenToString();
    if (!query.isEmpty())
        remoteURL.setQuery(query);
    request.setURL(remoteURL);
}

PassRefPtr<FormData> WMLGoElement::createFormData(const CString& boundary)
{
    CString key;
    CString value;

    Vector<char> encodedData;
    WTF::TextEncoding encoding =
            FormDataBuilder::encodingFromAcceptCharset(m_formAttributes.acceptCharset(), document().inputEncoding(), document().defaultCharset());

    Vector<WMLPostfieldElement*>::iterator it = m_postfieldElements.begin();
    Vector<WMLPostfieldElement*>::iterator end = m_postfieldElements.end();

    RefPtr<FormData> result = FormData::create();
    for (; it != end; ++it) {
        (*it)->encodeData(encoding, key, value);

        if (m_formAttributes.isMultiPartForm()) {
            Vector<char> header;
            FormDataBuilder::beginMultiPartHeader(header, boundary, key);
            FormDataBuilder::finishMultiPartHeader(header);
            result->appendData(header.data(), header.size());

            if (size_t dataSize = value.length())
                result->appendData(value.data(), dataSize);

            result->appendData("\r\n", 2);
        } else
            FormDataBuilder::addKeyValuePairAsFormData(encodedData, key, value);
    }

    if (m_formAttributes.isMultiPartForm())
        FormDataBuilder::addBoundaryToMultiPartHeader(encodedData, boundary, true);

    result->appendData(encodedData.data(), encodedData.size());
    return result;
}


/// M: Add href for go element. @{
KURL WMLGoElement::href() const
{
    String href = substituteVariableReferences(getAttribute(HTMLNames::hrefAttr), document(), WMLVariableEscapingEscape);
    return document().completeURL(href);
}
/// @}

}

#endif

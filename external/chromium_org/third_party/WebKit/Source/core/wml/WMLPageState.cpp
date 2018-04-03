/*
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2004-2007 Apple Inc. All rights reserved.
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
#include "core/wml/WMLPageState.h"

#include "core/dom/Document.h"
#include "core/frame/LocalFrame.h"
#include "core/loader/HistoryItem.h"
#include "core/page/Page.h"
#include "platform/weborigin/KURL.h"
#include <wtf/text/CString.h>

namespace blink {

WMLPageState::WMLPageState(Page* page)
    : m_page(page)
    , m_hasAccessControlData(false)
    , m_pendingLoadType(FrameLoadTypeStandard)
    , m_previousURL(KURL())
{
}

WMLPageState::~WMLPageState()
{
    m_variables.clear();
}

#ifndef NDEBUG
// Debugging helper for use within gdb
void WMLPageState::dump()
{
    WMLVariableMap::iterator it = m_variables.begin();
    WMLVariableMap::iterator end = m_variables.end();

    fprintf(stderr, "Dumping WMLPageState (this=%p) associated with Page (page=%p)...\n", this, m_page);
    for (; it != end; ++it)
        fprintf(stderr, "\t-> name: '%s'\tvalue: '%s'\n", (*it).first.latin1().data(), (*it).second.latin1().data());
}
#endif

void WMLPageState::reset()
{
    // Remove all the variables
    m_variables.clear();
    m_previousURL = KURL();
    // Clear the navigation history state
    /* FIXME
    if (m_page) {
        HistoryItem* currentItem = m_page->backForward()->currentItem();
        m_page->backForward()->close();
        m_page->backForward()->setCurrentItem(currentItem);
    }*/
}

static inline String normalizedHostName(const String& passedHost)
{
    if (passedHost.contains("127.0.0.1")) {
        String host = passedHost;
        return host.replace("127.0.0.1", "localhost");
    }

    return passedHost;
}

static inline String hostFromURL(const KURL& url)
{
    // Default to "localhost"
    String host = normalizedHostName(url.host());
    return host.isEmpty() ? "localhost" : host;
}

static bool getCurrentURLs(Page* page, KURL& currentURL)
{
    if (page && page->deprecatedLocalMainFrame()) {
        HistoryItem* currentItem = page->deprecatedLocalMainFrame()->loader().currentItem();
        if (!currentItem)
            return false;
        currentURL = currentItem->url();
        return true;
    }
    return false;
}

bool WMLPageState::processAccessControlData(const String& domain, const String& path)
{
    if (m_hasAccessControlData)
        return false;

    m_hasAccessControlData = true;

    KURL currentURL;
    if (m_previousURL.isNull() || !getCurrentURLs(m_page, currentURL))
        return true;

    // Spec: The path attribute defaults to the value "/"
    m_accessPath = path.isEmpty() ? "/" : path;

    // Spec: The domain attribute defaults to the current decks domain.
    String previousHost = hostFromURL(m_previousURL);
    m_accessDomain = domain.isEmpty() ? previousHost : normalizedHostName(domain);

    // Spec: To simplify the development of applications that may not know the absolute path to the 
    // current deck, the path attribute accepts relative URIs. The user agent converts the relative 
    // path to an absolute path and then performs prefix matching against the PATH attribute.
    Document* document = m_page->mainFrame() ? m_page->deprecatedLocalMainFrame()->document() : 0;
    if (document && previousHost == m_accessDomain && !m_accessPath.startsWith("/")) {
        String currentPath = currentURL.path();

        size_t index = currentPath.reverseFind('/');
        if (index != WTF::kNotFound)
            m_accessPath = document->completeURL(currentPath.left(index + 1) + m_accessPath).path();
    }

    return true;
}

void WMLPageState::resetAccessControlData()
{
    m_hasAccessControlData = false;
    m_accessDomain = String();
    m_accessPath = String();
}

bool WMLPageState::canAccessDeck() const
{
    if (!m_hasAccessControlData)
        return true;

    KURL currentURL;
    if (m_previousURL.isNull() || !getCurrentURLs(m_page, currentURL))
        return true;

    if (equalIgnoringFragmentIdentifier(m_previousURL, currentURL))
       return true;

    return hostIsAllowedToAccess(hostFromURL(m_previousURL)) && pathIsAllowedToAccess(m_previousURL.path());
}

bool WMLPageState::hostIsAllowedToAccess(const String& host) const
{
    // Spec: The access domain is suffix-matched against the domain name portion of the referring URI
    Vector<String> subdomainsAllowed;
    if (m_accessDomain.contains('.'))
        m_accessDomain.split('.', subdomainsAllowed);
    else
        subdomainsAllowed.append(m_accessDomain);

    Vector<String> subdomainsCheck;
    if (host.contains('.'))
        host.split('.', subdomainsCheck);
    else
        subdomainsCheck.append(host);

    Vector<String>::iterator itAllowed = subdomainsAllowed.end() - 1;
    Vector<String>::iterator beginAllowed = subdomainsAllowed.begin();

    Vector<String>::iterator itCheck = subdomainsCheck.end() - 1;
    Vector<String>::iterator beginCheck = subdomainsCheck.begin();

    bool hostOk = true;
    for (; itAllowed >= beginAllowed && itCheck >= beginCheck; ) {
        if (*itAllowed != *itCheck) {
            hostOk = false;
            break;
        }

        --itAllowed;
        --itCheck;
    }

    return hostOk;
}

bool WMLPageState::pathIsAllowedToAccess(const String& path) const
{
    // Spec: The access path is prefix matched against the path portion of the referring URI
    Vector<String> subpathsAllowed;
    if (m_accessPath.contains('/'))
        m_accessPath.split('/', subpathsAllowed);
    else
        subpathsAllowed.append(m_accessPath);

    Vector<String> subpathsCheck;
    if (path.contains('/'))
        path.split('/', subpathsCheck);
    else
        subpathsCheck.append(path);

    Vector<String>::iterator itAllowed = subpathsAllowed.begin();
    Vector<String>::iterator endAllowed = subpathsAllowed.end();

    Vector<String>::iterator itCheck = subpathsCheck.begin();
    Vector<String>::iterator endCheck = subpathsCheck.end();

    bool pathOk = true;
    for (; itAllowed != endAllowed && itCheck != endCheck; ) {
        if (*itAllowed != *itCheck) {
            pathOk = false;
            break;
        }

        ++itAllowed;
        ++itCheck;
    }

    return pathOk;
}

void WMLPageState::setPendingLoadType(FrameLoadType loadType)
{
    if (m_pendingLoadType != FrameLoadTypeWMLDeckNotAccessible)
        m_pendingLoadType = loadType;
}

FrameLoadType WMLPageState::getPendingLoadType()
{
    FrameLoadType tmp = m_pendingLoadType;
    m_pendingLoadType = FrameLoadTypeStandard;
    return tmp;
}

}

#endif

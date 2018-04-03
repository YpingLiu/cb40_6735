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

#ifndef WMLMetaElement_h
#define WMLMetaElement_h

#if ENABLE(WML)
#include "core/wml/WMLElement.h"

namespace blink {

class WMLMetaElement : public WMLElement {
public:
    static PassRefPtr<WMLMetaElement> create(Document&);

    WMLMetaElement(Document&);
    virtual ~WMLMetaElement();

    virtual void parseAttribute(const QualifiedName&, const AtomicString&) OVERRIDE;
    virtual InsertionNotificationRequest insertedInto(ContainerNode*) OVERRIDE;

private:
    AtomicString m_equiv;
    AtomicString m_content;
};

}

#endif
#endif

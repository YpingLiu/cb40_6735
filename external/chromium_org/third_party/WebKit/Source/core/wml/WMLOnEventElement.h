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

#ifndef WMLOnEventElement_h
#define WMLOnEventElement_h

#if ENABLE(WML)
#include "core/wml/WMLElement.h"
#include "core/wml/WMLIntrinsicEventHandler.h"

namespace blink {

class WMLTaskElement;

class WMLOnEventElement : public WMLElement {
public:
    static PassRefPtr<WMLOnEventElement> create(Document&);

    WMLOnEventElement(Document&);

    virtual void parseAttribute(const QualifiedName&, const AtomicString&) OVERRIDE;

    void registerTask(WMLTaskElement*);
    void deregisterTask(WMLTaskElement*);

private:
    WMLIntrinsicEventType m_type;
};

}

#endif
#endif

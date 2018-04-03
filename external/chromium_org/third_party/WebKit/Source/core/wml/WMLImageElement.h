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

#ifndef WMLImageElement_h
#define WMLImageElement_h

#if ENABLE(WML)
#include "core/wml/WMLElement.h"
#include "core/wml/WMLImageLoader.h"

namespace blink {

class WMLImageElement : public WMLElement {
public:
    static PassRefPtr<WMLImageElement> create(Document&);

    WMLImageElement(Document&);
    virtual ~WMLImageElement();

    virtual void parseAttribute(const QualifiedName&, const AtomicString&) OVERRIDE;
    virtual bool isPresentationAttribute(const QualifiedName&) const OVERRIDE;
    virtual void collectStyleForPresentationAttribute(const QualifiedName&, const AtomicString&, MutableStylePropertySet*) OVERRIDE;

    virtual void attach(const AttachContext& = AttachContext()) OVERRIDE;
    virtual RenderObject* createRenderer(RenderStyle*);

    virtual InsertionNotificationRequest insertedInto(ContainerNode*) OVERRIDE;
    virtual bool isURLAttribute(Attribute*) const;
    virtual const QualifiedName& imageSourceAttributeName() const;

    String altText() const;

    bool useFallbackAttribute() { return m_useFallbackAttribute; }
    void setUseFallbackAttribute(bool value) { m_useFallbackAttribute = value; }

private:
    WMLImageLoader m_imageLoader;
    bool m_useFallbackAttribute;
};

}

#endif
#endif

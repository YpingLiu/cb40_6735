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

#ifndef WMLElement_h
#define WMLElement_h

#if ENABLE(WML)
#include "core/WMLNames.h"
#include "core/dom/Element.h"
#include "core/wml/WMLErrorHandling.h"

namespace blink {

class WMLElement : public Element {
public:
    static PassRefPtr<WMLElement> create(const QualifiedName& tagName, Document&);

    virtual bool isWMLElement() const { return true; }
    virtual bool isWMLTaskElement() const { return false; }

    bool hasTagName(const WMLQualifiedName& name) const { return hasLocalName(name.localName()); }

    void addCSSLengthToStyle(MutableStylePropertySet*, CSSPropertyID, const String& value);

    virtual void parseAttribute(const QualifiedName&, const AtomicString&) OVERRIDE;
    virtual bool isPresentationAttribute(const QualifiedName&) const OVERRIDE;
    virtual void collectStyleForPresentationAttribute(const QualifiedName&, const AtomicString&, MutableStylePropertySet*) OVERRIDE;

    virtual String title() const;

    virtual bool rendererIsNeeded(RenderStyle*);
    virtual RenderObject* createRenderer(RenderStyle*);

    virtual void insertedIntoTree(bool);

protected:
    WMLElement(const QualifiedName& tagName, Document&, ConstructionType = CreateWMLElement);

    void applyAlignmentAttributeToStyle(const AtomicString&, MutableStylePropertySet*);
    void applyBorderAttributeToStyle(const AtomicString&, MutableStylePropertySet*);
    unsigned parseBorderWidthAttribute(const AtomicString&) const;

    // Helper function for derived classes
    AtomicString parseValueSubstitutingVariableReferences(const AtomicString&, WMLErrorCode defaultErrorCode = WMLErrorInvalidVariableReference) const;
    AtomicString parseValueForbiddingVariableReferences(const AtomicString&) const;
};

DEFINE_ELEMENT_TYPE_CASTS(WMLElement, isWMLElement());

template <typename T> bool isElementOfType(const WMLElement&);
template <> inline bool isElementOfType<const WMLElement>(const WMLElement&) { return true; }

inline bool Node::hasTagName(const WMLQualifiedName& name) const
{
    return isWMLElement() && toWMLElement(*this).hasTagName(name);
}

// This requires isWML*Element(const WMLElement&).
#define DEFINE_WMLELEMENT_TYPE_CASTS_WITH_FUNCTION(thisType) \
    inline bool is##thisType(const thisType* element); \
    inline bool is##thisType(const thisType& element); \
    inline bool is##thisType(const WMLElement* element) { return element && is##thisType(*element); } \
    inline bool is##thisType(const Node& node) { return node.isWMLElement() ? is##thisType(toWMLElement(node)) : false; } \
    inline bool is##thisType(const Node* node) { return node && is##thisType(*node); } \
    template<typename T> inline bool is##thisType(const PassRefPtr<T>& node) { return is##thisType(node.get()); } \
    template<typename T> inline bool is##thisType(const RefPtr<T>& node) { return is##thisType(node.get()); } \
    template <> inline bool isElementOfType<const thisType>(const WMLElement& element) { return is##thisType(element); } \
    DEFINE_ELEMENT_TYPE_CASTS_WITH_FUNCTION(thisType)

} // namespace blink

#include "core/WMLElementTypeHelpers.h"

#endif // ENABLE(WML)
#endif // WMLElement_h

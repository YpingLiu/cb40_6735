/*
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
#include "core/wml/WMLElement.h"

#include "core/CSSPropertyNames.h"
#include "core/HTMLNames.h"
#include "core/WMLNames.h"
#include "core/dom/Attribute.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/rendering/RenderObject.h"
#include "core/wml/WMLErrorHandling.h"
#include "core/wml/WMLVariables.h"

using std::max;
using std::min;

namespace blink {

using namespace WMLNames;

WMLElement::WMLElement(const QualifiedName& tagName, Document& document, ConstructionType type)
: Element(tagName, &document, type)
{
}

PassRefPtr<WMLElement> WMLElement::create(const QualifiedName& tagName, Document& document)
{
    return adoptRef(new WMLElement(tagName, document));
}

bool WMLElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name == HTMLNames::alignAttr)
        return true;
    return Element::isPresentationAttribute(name);
}
    
void WMLElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (isIdAttributeName(name)
            || name == HTMLNames::classAttr
            || name == HTMLNames::styleAttr)
        return Element::parseAttribute(name, value);

    if (name == HTMLNames::tabindexAttr) {
        String indexstring = value;
        int tabindex = 0;
        if (parseHTMLInteger(indexstring, tabindex)) {
            // Clamp tabindex to the range of 'short' to match Firefox's behavior.
            setTabIndexExplicitly(max(static_cast<int>(std::numeric_limits<short>::min()), min(tabindex, static_cast<int>(std::numeric_limits<short>::max()))));
        }
    }
}

void WMLElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (name == HTMLNames::alignAttr) {
        if (equalIgnoringCase(value, "middle"))
            addPropertyToPresentationAttributeStyle(style, CSSPropertyTextAlign, CSSValueCenter);
        else
            addPropertyToPresentationAttributeStyle(style, CSSPropertyTextAlign, value);
    }
}

void WMLElement::addCSSLengthToStyle(MutableStylePropertySet* style, CSSPropertyID propertyID, const String& value)
{
    // FIXME: This function should not spin up the CSS parser, but should instead just figure out the correct
    // length unit and make the appropriate parsed value.

    // strip attribute garbage..
    StringImpl* v = value.impl();
    if (v) {
        unsigned int l = 0;

        while (l < v->length() && (*v)[l] <= ' ')
            l++;

        for (; l < v->length(); l++) {
            UChar cc = (*v)[l];
            if (cc > '9')
                break;
            if (cc < '0') {
                if (cc == '%' || cc == '*')
                    l++;
                if (cc != '.')
                    break;
            }
        }

        if (l != v->length()) {
            addPropertyToPresentationAttributeStyle(style, propertyID, v->substring(0, l));
            return;
        }
    }

    addPropertyToPresentationAttributeStyle(style, propertyID, value);
}

String WMLElement::title() const
{
    return parseValueSubstitutingVariableReferences(getAttribute(HTMLNames::titleAttr));
}

bool WMLElement::rendererIsNeeded(RenderStyle* style)
{
    return document().documentElement() == this || style->display() != NONE;
}

RenderObject* WMLElement::createRenderer(RenderStyle* style)
{
    return RenderObject::createObject(this, style);
}

void WMLElement::applyAlignmentAttributeToStyle(const AtomicString& alignment, MutableStylePropertySet* style)
{
    // Vertical alignment with respect to the current baseline of the text
    // right or left means floating images.
    CSSValueID floatValue = CSSValueInvalid;
    CSSValueID verticalAlignValue = CSSValueInvalid;

    if (equalIgnoringCase(alignment, "absmiddle"))
        verticalAlignValue = CSSValueMiddle;
    else if (equalIgnoringCase(alignment, "absbottom"))
        verticalAlignValue = CSSValueBottom;
    else if (equalIgnoringCase(alignment, "left")) {
        floatValue = CSSValueLeft;
        verticalAlignValue = CSSValueTop;
    } else if (equalIgnoringCase(alignment, "right")) {
        floatValue = CSSValueRight;
        verticalAlignValue = CSSValueTop;
    } else if (equalIgnoringCase(alignment, "top"))
        verticalAlignValue = CSSValueTop;
    else if (equalIgnoringCase(alignment, "middle"))
        verticalAlignValue = CSSValueWebkitBaselineMiddle;
    else if (equalIgnoringCase(alignment, "center"))
        verticalAlignValue = CSSValueMiddle;
    else if (equalIgnoringCase(alignment, "bottom"))
        verticalAlignValue = CSSValueBaseline;
    else if (equalIgnoringCase(alignment, "texttop"))
        verticalAlignValue = CSSValueTextTop;

    if (floatValue != CSSValueInvalid)
        addPropertyToPresentationAttributeStyle(style, CSSPropertyFloat, floatValue);

    if (verticalAlignValue != CSSValueInvalid)
        addPropertyToPresentationAttributeStyle(style, CSSPropertyVerticalAlign, verticalAlignValue);
}

unsigned WMLElement::parseBorderWidthAttribute(const AtomicString& value) const
{
    unsigned borderWidth = 0;
    if (value.isEmpty() || !parseHTMLNonNegativeInteger(value, borderWidth))
        return hasLocalName(tableTag.localName()) ? 1 : borderWidth;
    return borderWidth;
}

void WMLElement::applyBorderAttributeToStyle(const AtomicString& value, MutableStylePropertySet* style)
{
    addPropertyToPresentationAttributeStyle(style, CSSPropertyBorderWidth, parseBorderWidthAttribute(value), CSSPrimitiveValue::CSS_PX);
    addPropertyToPresentationAttributeStyle(style, CSSPropertyBorderStyle, CSSValueSolid);
}


AtomicString WMLElement::parseValueSubstitutingVariableReferences(const AtomicString& value, WMLErrorCode defaultErrorCode) const
{
    bool isValid = false;
    if (!containsVariableReference(value, isValid))
        return value;

    if (!isValid) {
        reportWMLError(document(), defaultErrorCode);
        return AtomicString();
    }

    return AtomicString(substituteVariableReferences(value, document()));
}

AtomicString WMLElement::parseValueForbiddingVariableReferences(const AtomicString& value) const
{
    bool isValid = false;
    if (containsVariableReference(value, isValid)) {
        reportWMLError(document(), WMLErrorInvalidVariableReferenceLocation);
        return AtomicString();
    }

    return value;
}

// FIXME: WML_SUPPORT
void WMLElement::insertedIntoTree(bool deep)
{
    //https://bugs.webkit.org/show_bug.cgi?id=82701
    //Element::insertedInto(rootParent);
}

}

#endif

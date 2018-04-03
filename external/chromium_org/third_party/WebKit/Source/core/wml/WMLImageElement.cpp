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
#include "core/wml/WMLImageElement.h"

#include "core/dom/Attribute.h"
#include "core/html/HTMLElement.h"
#include "core/rendering/RenderImage.h"
#include "core/CSSPropertyNames.h"
#include "core/CSSValueKeywords.h"
#include "core/HTMLNames.h"
#include "core/WMLNames.h"
#include "core/wml/WMLVariables.h"

namespace blink {

using namespace WMLNames;

WMLImageElement::WMLImageElement(Document& document)
    : WMLElement(imgTag, document)
    , m_imageLoader(this)
    , m_useFallbackAttribute(false)
{
}

PassRefPtr<WMLImageElement> WMLImageElement::create(Document& document)
{
    return adoptRef(new WMLImageElement(document));
}

WMLImageElement::~WMLImageElement()
{
}

void WMLImageElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == HTMLNames::altAttr) {
        if (renderer() && renderer()->isImage())
            toRenderImage(renderer())->updateAltText();
    } else if (name == HTMLNames::srcAttr || name == localsrcAttr)
        m_imageLoader.updateFromElement(ImageLoader::UpdateIgnorePreviousError);
    else
        WMLElement::parseAttribute(name, value);
}

bool WMLImageElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name == HTMLNames::widthAttr ||
            name == HTMLNames::heightAttr ||
            name == HTMLNames::borderAttr ||
            name == HTMLNames::vspaceAttr ||
            name == HTMLNames::hspaceAttr ||
            name == HTMLNames::alignAttr)
        return true;
    return WMLElement::isPresentationAttribute(name);
}


void WMLImageElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (name == HTMLNames::widthAttr)
        addCSSLengthToStyle(style, CSSPropertyWidth, value);
    else if (name == HTMLNames::heightAttr)
        addCSSLengthToStyle(style, CSSPropertyHeight, value);
    else if (name == HTMLNames::borderAttr)
        applyBorderAttributeToStyle(value, style);
    else if (name == HTMLNames::vspaceAttr) {
        addCSSLengthToStyle(style, CSSPropertyMarginTop, value);
        addCSSLengthToStyle(style, CSSPropertyMarginBottom, value);
    } else if (name == HTMLNames::hspaceAttr) {
        addCSSLengthToStyle(style, CSSPropertyMarginLeft, value);
        addCSSLengthToStyle(style, CSSPropertyMarginRight, value);
    } else if (name == HTMLNames::alignAttr)
        applyAlignmentAttributeToStyle(value, style);
    else
        WMLElement::collectStyleForPresentationAttribute(name, value, style);
}

void WMLImageElement::attach(const AttachContext& context)
{
    WMLElement::attach(context);

    if (renderer() && renderer()->isImage()) {
        RenderImage* renderImage = toRenderImage(renderer());
        RenderImageResource* renderImageResource = renderImage->imageResource();
        if (renderImageResource->hasImage())
            return;

        // If we have no image at all because we have no src attribute, set
        // image height and width for the alt text instead.
        if (!m_imageLoader.image() && !renderImageResource->cachedImage())
            renderImage->setImageSizeForAltText();
        else
            renderImageResource->setImageResource(m_imageLoader.image());
    }
}

RenderObject* WMLImageElement::createRenderer(RenderStyle* style)
{
    if (style->hasContent())
        return RenderObject::createObject(this, style);

    RenderImage* image = new RenderImage(this);
    image->setImageResource(RenderImageResource::create());
    return image;
}

Node::InsertionNotificationRequest WMLImageElement::insertedInto(ContainerNode* insertionPoint)
{
    // If we have been inserted from a renderer-less document,
    // our loader may have not fetched the image, so do it now.
    if (insertionPoint->inDocument() && !m_imageLoader.image())
        m_imageLoader.updateFromElement();

    return WMLElement::insertedInto(insertionPoint);
}

bool WMLImageElement::isURLAttribute(Attribute* attr) const
{
    return attr->name() == HTMLNames::srcAttr
           || attr->name() == localsrcAttr;
}

const QualifiedName& WMLImageElement::imageSourceAttributeName() const
{
    // Spec: Any 'localsrc' parameter specified takes precedence over the 
    // image specified in the src parameter.
    if (hasAttribute(localsrcAttr) && !m_useFallbackAttribute)
        return localsrcAttr;

    return HTMLNames::srcAttr;
}

String WMLImageElement::altText() const
{
    return substituteVariableReferences(getAttribute(HTMLNames::altAttr), document());
}

}

#endif

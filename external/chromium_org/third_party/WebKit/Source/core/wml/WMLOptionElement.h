/**
 * Copyright (C) 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
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

#ifndef WMLOptionElement_h
#define WMLOptionElement_h

#if ENABLE(WML)
#include "core/wml/dummy/OptionElement.h"
#include "core/wml/WMLFormControlElement.h"
#include "core/wml/WMLEventHandlingElement.h"

/// M: @{
#include "core/WMLNames.h"
/// @}

namespace blink {

class WMLOptionElement : public WMLFormControlElement, public WMLEventHandlingElement, public OptionElement {
public:
    static PassRefPtr<WMLOptionElement> create(Document&);

    WMLOptionElement(Document&);
    virtual ~WMLOptionElement();

    virtual const AtomicString& formControlType() const;

    virtual bool rendererIsNeeded(RenderStyle*) { return false; }

    virtual void accessKeyAction(bool sendToAnyElement);
    virtual void childrenChanged(const ChildrenChange&) OVERRIDE;
    virtual void parseAttribute(const QualifiedName&, const AtomicString&) OVERRIDE;

    virtual void attach(const AttachContext& = AttachContext()) OVERRIDE;
    virtual void detach(const AttachContext& = AttachContext()) OVERRIDE;
    virtual void setRenderStyle(PassRefPtr<RenderStyle>);

    virtual InsertionNotificationRequest insertedInto(ContainerNode*) OVERRIDE;

    virtual bool selected() const;
    virtual void setSelectedState(bool);

    virtual String text() const;
    virtual String textIndentedToRespectGroupLabel() const;
    virtual String value() const;

    virtual bool disabled() const;

    bool isDisplayNone() const;
private:
    virtual RenderStyle* nonRendererRenderStyle() const;
    void handleIntrinsicEventIfNeeded();

private:
    OptionElementData m_data;
    RefPtr<RenderStyle> m_style;
};

}

#endif
#endif

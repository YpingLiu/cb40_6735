/*
 * Copyright (C) 2006, 2007, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef RenderWMLTextControlSingleLine_h
#define RenderWMLTextControlSingleLine_h

#if ENABLE(WML)
#include "core/wml/WMLInputElement.h"
#include "core/rendering/wml/RenderWMLTextControl.h"

namespace blink {

class WMLInputElement;

class RenderWMLTextControlSingleLine : public RenderWMLTextControl {
public:
    RenderWMLTextControlSingleLine(WMLInputElement*);
    virtual ~RenderWMLTextControlSingleLine();
    // FIXME: Move createInnerEditorStyle() to TextControlInnerEditorElement.
    virtual PassRefPtr<RenderStyle> createInnerEditorStyle(const RenderStyle* startStyle) const OVERRIDE FINAL;

    void capsLockStateMayHaveChanged();

protected:
    virtual void centerContainerIfNeeded(RenderBox*) const { }
    virtual LayoutUnit computeLogicalHeightLimit() const;
    Element* containerElement() const;
    Element* editingViewPortElement() const;
    WMLInputElement* inputElement() const;

private:
    virtual bool hasControlClip() const OVERRIDE FINAL;
    virtual LayoutRect controlClipRect(const LayoutPoint&) const OVERRIDE FINAL;
    virtual bool isTextField() const OVERRIDE FINAL { return true; }

    virtual void paint(PaintInfo&, const LayoutPoint&) OVERRIDE;
    virtual void layout() OVERRIDE;

    virtual bool nodeAtPoint(const HitTestRequest&, HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) OVERRIDE FINAL;

    virtual void autoscroll(const IntPoint&) OVERRIDE FINAL;

    // Subclassed to forward to our inner div.
    virtual LayoutUnit scrollLeft() const OVERRIDE FINAL;
    virtual LayoutUnit scrollTop() const OVERRIDE FINAL;
    virtual LayoutUnit scrollWidth() const OVERRIDE FINAL;
    virtual LayoutUnit scrollHeight() const OVERRIDE FINAL;
    virtual void setScrollLeft(LayoutUnit) OVERRIDE FINAL;
    virtual void setScrollTop(LayoutUnit) OVERRIDE FINAL;

    int textBlockWidth() const;
    virtual float getAvgCharWidth(AtomicString family) OVERRIDE FINAL;
    virtual LayoutUnit preferredContentLogicalWidth(float charWidth) const OVERRIDE FINAL;
    virtual LayoutUnit computeControlLogicalHeight(LayoutUnit lineHeight, LayoutUnit nonContentHeight) const OVERRIDE;

    virtual void styleDidChange(StyleDifference, const RenderStyle* oldStyle) OVERRIDE FINAL;

    bool textShouldBeTruncated() const;

    Element* innerSpinButtonElement() const;

    bool m_shouldDrawCapsLockIndicator;
    LayoutUnit m_desiredInnerEditorLogicalHeight;
};

DEFINE_RENDER_OBJECT_TYPE_CASTS(RenderWMLTextControlSingleLine, isTextField());

// ----------------------------

class RenderWMLTextControlInnerBlock : public RenderBlockFlow {
public:
    RenderWMLTextControlInnerBlock(Element* element) : RenderBlockFlow(element) { }
    virtual int inlineBlockBaseline(LineDirectionMode direction) const OVERRIDE { return lastLineBoxBaseline(direction); }

private:
    virtual bool isIntristicallyScrollable(ScrollbarOrientation orientation) const OVERRIDE
    {
        return orientation == HorizontalScrollbar;
    }
    virtual bool scrollsOverflowX() const OVERRIDE { return hasOverflowClip(); }
    virtual bool scrollsOverflowY() const OVERRIDE { return false; }
    virtual bool hasLineIfEmpty() const OVERRIDE { return true; }
};

}

#endif

#endif // RenderWMLTextControlSingleLine_h

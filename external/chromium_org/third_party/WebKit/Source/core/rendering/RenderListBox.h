/*
 * This file is part of the select element renderer in WebCore.
 *
 * Copyright (C) 2006, 2007, 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef RenderListBox_h
#define RenderListBox_h

#include "core/rendering/RenderBlockFlow.h"

#define USE_POPUP_LISTBOX true

#if USE_POPUP_LISTBOX
#include "platform/PopupMenu.h"
#include "platform/PopupMenuClient.h"
#endif

namespace blink {

class HTMLSelectElement;

#if USE_POPUP_LISTBOX
class RenderListBox FINAL : public RenderBlockFlow, private PopupMenuClient {
#else
class RenderListBox FINAL : public RenderBlockFlow {
#endif
public:
    explicit RenderListBox(Element*);
    virtual ~RenderListBox();

#if USE_POPUP_LISTBOX
    bool popupIsVisible() const { return m_popupIsVisible; }
    void showPopup();
    void hidePopup();
    virtual void updateFromElement() OVERRIDE;
#endif

    int size() const;

    void paintInvalidationOfScrollbarIfNeeded();

    // Unlike scrollRectToVisible this will not scroll parent boxes.
    void scrollToRect(const LayoutRect&);
private:
    HTMLSelectElement* selectElement() const;

    virtual const char* renderName() const OVERRIDE { return "RenderListBox"; }

    virtual bool isListBox() const OVERRIDE { return true; }

    virtual void computeLogicalHeight(LayoutUnit logicalHeight, LayoutUnit logicalTop, LogicalExtentComputedValues&) const OVERRIDE;
    virtual void computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const OVERRIDE;

    virtual void stopAutoscroll() OVERRIDE;

    void setHasVerticalScrollbar(bool hasScrollbar);
    PassRefPtr<Scrollbar> createScrollbar();
    void destroyScrollbar();

    LayoutUnit defaultItemHeight() const;
    LayoutUnit itemHeight() const;

#if USE_POPUP_LISTBOX
    // PopupMenuClient methods
    virtual void valueChanged(unsigned listIndex, bool fireOnChange = true) OVERRIDE;
    virtual void selectionChanged(unsigned, bool) OVERRIDE { }
    virtual void selectionCleared() OVERRIDE { }
    virtual String itemText(unsigned listIndex) const OVERRIDE;
    virtual String itemToolTip(unsigned listIndex) const OVERRIDE;
    virtual String itemAccessibilityText(unsigned listIndex) const OVERRIDE;
    virtual bool itemIsEnabled(unsigned listIndex) const OVERRIDE;
    virtual PopupMenuStyle itemStyle(unsigned listIndex) const OVERRIDE;
    virtual PopupMenuStyle menuStyle() const OVERRIDE;
    virtual LayoutUnit clientPaddingLeft() const OVERRIDE;
    virtual LayoutUnit clientPaddingRight() const OVERRIDE;
    virtual int listSize() const OVERRIDE;
    virtual int selectedIndex() const OVERRIDE;
    virtual void popupDidHide() OVERRIDE;
    virtual bool itemIsSeparator(unsigned listIndex) const OVERRIDE;
    virtual bool itemIsLabel(unsigned listIndex) const OVERRIDE;
    virtual bool itemIsSelected(unsigned listIndex) const OVERRIDE;
    virtual void setTextFromItem(unsigned listIndex) OVERRIDE;
    virtual void listBoxSelectItem(int listIndex, bool allowMultiplySelections, bool shift, bool fireOnChangeNow = true) OVERRIDE;
    virtual bool multiple() const OVERRIDE;

    RefPtr<PopupMenu> m_popup;
    bool m_popupIsVisible;
#endif
};

DEFINE_RENDER_OBJECT_TYPE_CASTS(RenderListBox, isListBox());

} // namepace blink

#endif // RenderListBox_h

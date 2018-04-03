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

#ifndef WMLSelectElement_h
#define WMLSelectElement_h

#if ENABLE(WML)
#include "core/wml/WMLFormControlElement.h"
#include "core/wml/dummy/SelectElement.h"

namespace blink {

class WMLSelectElement : public WMLFormControlElement, public SelectElement {
public:
    static PassRefPtr<WMLSelectElement> create(Document&);

    WMLSelectElement(Document&);
    virtual ~WMLSelectElement();

    virtual const AtomicString& formControlName() const;
    virtual const AtomicString& formControlType() const;
 
    virtual bool isKeyboardFocusable(KeyboardEvent*) const;
    virtual bool isMouseFocusable() const;
    virtual bool canSelectAll() const { return !m_data.usesMenuList(); }
    virtual void selectAll();

    virtual void didRecalcStyle(StyleRecalcChange) OVERRIDE;

    virtual void dispatchFocusEvent(Element* oldFocusedElement, FocusType) OVERRIDE;
    virtual void dispatchBlurEvent(Element* newFocusedElement) OVERRIDE;

    virtual bool canStartSelection() const { return false; }

    virtual int selectedIndex() const;
    virtual void setSelectedIndex(int index, bool deselect = true);
    virtual void setSelectedIndexByUser(int index, bool deselect = true, bool fireOnChangeNow = false, bool allowMultipleSelection = false);

    virtual int size() const { return m_data.size(); }
    virtual bool multiple() const { return m_data.multiple(); }

    virtual bool saveFormControlState(String& value) const;
    virtual void restoreFormControlState(const String&);

    virtual void childrenChanged(const ChildrenChange&) OVERRIDE;

    virtual void parseAttribute(const QualifiedName&, const AtomicString&) OVERRIDE;

    virtual RenderObject* createRenderer(RenderStyle*);

    virtual bool appendFormData(FormDataList&, bool);
    virtual int optionToListIndex(int optionIndex) const;
    virtual int listToOptionIndex(int listIndex) const;

    virtual const Vector<Element*>& listItems() const { return m_data.listItems(this); }
    virtual void reset();

    virtual void defaultEventHandler(Event*);
    virtual void accessKeyAction(bool sendMouseEvents);
    virtual void setActiveSelectionAnchorIndex(int index);
    virtual void setActiveSelectionEndIndex(int index);
    virtual void updateListBoxSelection(bool deselectOtherOptions);
    virtual void listBoxOnChange();
    virtual void menuListOnChange();

    virtual int activeSelectionStartListIndex() const;
    virtual int activeSelectionEndListIndex() const;

    void accessKeySetSelectedIndex(int);
    void setRecalcListItems();
    void scrollToSelection();
    void selectInitialOptions();

    bool initialized() const { return m_initialized; }
    
    virtual void listBoxSelectItem(int listIndex, bool allowMultiplySelections, bool shift, bool fireOnChangeNow = true);
private:
    virtual void insertedIntoTree(bool);

    void calculateDefaultOptionIndices();
    void selectDefaultOptions();
    void initializeVariables();
    void updateVariables();

    Vector<unsigned> parseIndexValueString(const String&) const;
    Vector<unsigned> valueStringToOptionIndices(const String&) const;
    String optionIndicesToValueString() const;
    String optionIndicesToString() const;

    virtual void updateValidity() {}

    AtomicString name() const;
    AtomicString value() const;
    AtomicString iname() const;
    AtomicString ivalue() const;

    SelectElementData m_data;
    bool m_initialized;
    Vector<unsigned> m_defaultOptionIndices;
};

}

#endif
#endif

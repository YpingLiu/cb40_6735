/**
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

#ifndef WMLInputElement_h
#define WMLInputElement_h

#if ENABLE(WML)
#include "core/wml/WMLFormControlElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/wml/dummy/InputElement.h"
#include "core/html/shadow/TextControlInnerElements.h"
#include "core/page/FocusType.h"

namespace blink {

class FormDataList;
class Position;
class VisiblePosition;

class WMLInputElement : public WMLFormControlElement, public InputElement {
public:
    static PassRefPtr<WMLInputElement> create(Document&);

    WMLInputElement(Document&);
    virtual ~WMLInputElement();

    virtual InputElement* toInputElement() { return this; }

    virtual bool isKeyboardFocusable(KeyboardEvent*) const;
    virtual bool isMouseFocusable() const;
    virtual void dispatchFocusEvent(Element* oldFocusedElement, FocusType) OVERRIDE;
    virtual void dispatchBlurEvent(Element* newFocusedElement) OVERRIDE;
    virtual void updateFocusAppearance(bool restorePreviousSelection);
    virtual void aboutToUnload();

    virtual bool shouldUseInputMethod() const { return !m_isPasswordField; }
    virtual bool isChecked() const { return false; }
    virtual bool isAutofilled() const { return false; }
    virtual bool isIndeterminate() const { return false; }
    virtual bool isTextFormControl() const { return true; }
    virtual bool isRadioButton() const { return false; }
    virtual bool isCheckbox() const { return false; }
    virtual bool isTextField() const { return true; }
    virtual bool isSearchField() const { return false; }
    virtual bool isInputTypeHidden() const { return false; }
    virtual bool isPasswordField() const { return m_isPasswordField; }
    virtual bool searchEventsShouldBeDispatched() const { return false; }

    virtual int size() const;
    virtual const AtomicString& formControlType() const;
    virtual const AtomicString& formControlName() const;
    virtual const String& suggestedValue() const;
    virtual String value() const;
    virtual void setValue(const String&, bool sendChangeEvent = false);
    virtual void setValueForUser(const String&);
    virtual String visibleValue() const { return value(); }
    virtual String convertFromVisibleValue(const String& value) const { return value; }
    virtual void setValueFromRenderer(const String&);

    virtual bool wasChangedSinceLastFormControlChangeEvent() const;
    virtual void setChangedSinceLastFormControlChangeEvent(bool);

    virtual bool saveFormControlState(String& value) const;
    virtual void restoreFormControlState(const String&);

    virtual void select();
    virtual void accessKeyAction(bool sendToAnyElement);
    virtual void parseAttribute(const QualifiedName&, const AtomicString&) OVERRIDE;

    virtual void copyNonAttributePropertiesFromElement(const Element&);

    virtual RenderObject* createRenderer(RenderStyle*);
    virtual void detach(const AttachContext& = AttachContext()) OVERRIDE;
    virtual bool appendFormData(FormDataList&, bool);
    virtual void reset();

    virtual void defaultEventHandler(Event*);
    virtual void cacheSelection(int start, int end);

    virtual bool isAcceptableValue(const String&) const { return true; }
    virtual String sanitizeValue(const String& proposedValue) const;

    virtual void documentDidBecomeActive();

    virtual void willMoveToNewOwnerDocument();
    virtual void didMoveToNewOwnerDocument();

    /// M: this function remove any character that is not conformed to the input format @{
    String getTextConformedToInputMask(const String&);
    /// @}

    bool isConformedToInputMask(const String&);
    bool isConformedToInputMask(UChar, unsigned, bool isUserInput = true);
#if ENABLE(WCSS)
    virtual InputElementData data() const { return m_data; }
#endif
    /// M: Add WMLInputElement maxLength
    virtual int maxLength() const;

    //TODO WML_SUPPORT from HTMLTextFormControlElement
    String innerTextValue() const;
    void forwardEvent(Event*);
    virtual void subtreeHasChanged();

    // selection
    int selectionStart() const;
    int selectionEnd() const;
    void selectionChanged(bool userTriggered);
    bool hasCachedSelection() const { return m_data.cachedSelectionStart() >= 0; }

    /// for shadow root
    Element* placeholderElement() { return 0; }
    Element* innerSpinButtonElement() { return 0; }

    /// M: @{
    HTMLElement* innerTextElement() const;
    /// @}

    /// for userModify style
    bool isDisabledOrReadOnly() { return false; }

    /// M: @{
    void setInnerTextValue(const String&);
    /// @}

    bool hasInputMask();

    static const int maximumLength;
private:
    friend class WMLCardElement;
    void initialize();

    /// M: @{
    virtual void didAddUserAgentShadowRoot(ShadowRoot&) OVERRIDE;
    void createShadowSubtree();
    void destroyShadowSubtree();

    // selection
    int indexForVisiblePosition(const VisiblePosition& pos) const;
    int computeSelectionStart() const;
    int computeSelectionEnd() const;
    /// @}

    virtual bool supportsMaxLength() const { return true; }
    String validateInputMask(const String&);
    unsigned cursorPositionToMaskIndex(unsigned);

    void parseMaxLengthAttribute(const AtomicString&);
    void updateValueIfNeeded();

    /// M: @{
    RefPtr<HTMLElement> m_innerText;
    /// @}
    InputElementData m_data;
    bool m_isPasswordField;
    bool m_isEmptyOk;
    int m_size;
    unsigned m_maxLength;

    bool m_wasChangedSinceLastChangeEvent;
    String m_formatMask;
    unsigned m_numOfCharsAllowedByMask;
    /// M: @{
    int m_prelength;
    /// @}
};

WMLInputElement* enclosingWMLInputControl(const Position& position);

}

#endif
#endif

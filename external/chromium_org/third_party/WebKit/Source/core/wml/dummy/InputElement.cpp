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
#include "InputElement.h"

namespace blink {

// FIXME: According to HTML4, the length attribute's value can be arbitrarily
// large. However, due to https://bugs.webkit.org/show_bug.cgi?id=14536 things
// get rather sluggish when a text field has a larger number of characters than
// this, even when just clicking in the text field.
const int InputElement::s_maximumLength = 524288;
const int InputElement::s_defaultSize = 20;

void InputElement::dispatchFocusEvent(InputElement* inputElement, Element* element)
{
}

void InputElement::dispatchBlurEvent(InputElement* inputElement, Element* element)
{
}

void InputElement::setValueFromRenderer(InputElementData& data, InputElement* inputElement, Element* element, const String& value)
{
}

void InputElement::updateFocusAppearance(InputElementData& data, InputElement* inputElement, Element* element, bool restorePreviousSelection)
{
}

void InputElement::updateSelectionRange(InputElement* inputElement, Element* element, int start, int end)
{
}


void InputElement::aboutToUnload(InputElement* inputElement, Element* element)
{
}

String InputElement::sanitizeUserInputValue(const InputElement* inputElement, const String& proposedValue, int maxLength)
{
    return String();
}

void InputElement::handleBeforeTextInsertedEvent(InputElementData& data, InputElement* inputElement, Element* element, Event* event)
{
}

void InputElement::parseSizeAttribute(InputElementData& data, Element* element, const AtomicString& value)
{
}

void InputElement::parseMaxLengthAttribute(InputElementData& data, InputElement* inputElement, Element* element, const AtomicString& value)
{
}

void InputElement::notifyFormStateChanged(Element* element)
{
}


// InputElementData
InputElementData::InputElementData()
    : m_size(InputElement::s_defaultSize)
    , m_maxLength(InputElement::s_maximumLength)
    , m_cachedSelectionStart(-1)
    , m_cachedSelectionEnd(-1)
#if ENABLE(WCSS)
    , m_inputFormatMask("*m")
    , m_maxInputCharsAllowed(InputElement::s_maximumLength)
#endif
{
}

InputElementData::~InputElementData()
{
}

const AtomicString& InputElementData::name() const
{
    return m_name.isNull() ? emptyAtom : m_name;
}

}

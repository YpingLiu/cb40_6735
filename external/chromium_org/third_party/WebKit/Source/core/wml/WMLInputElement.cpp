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

#include "config.h"

#if ENABLE(WML)
#include "core/wml/WMLInputElement.h"

#include "core/accessibility/AXObjectCache.h"
#include "core/dom/Attribute.h"
#include "core/editing/InputMethodController.h"
#include "core/events/KeyboardEvent.h"
#include "core/events/TextEvent.h"
#include "core/frame/LocalFrame.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/Position.h"
#include "core/dom/Text.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/Editor.h"
#include "core/editing/VisiblePosition.h"
#include "core/html/FormDataList.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/html/shadow/TextControlInnerElements.h"
#include "core/rendering/wml/RenderWMLTextControlSingleLine.h"
#include "core/HTMLNames.h"
#include "core/WMLNames.h"
#include "core/wml/WMLDocument.h"
#include "core/wml/WMLPageState.h"
#include "wtf/text/StringBuilder.h"

using std::min;
namespace blink {

using namespace WMLNames;

const int WMLInputElement::maximumLength = 524288;
const int defaultSize = 20;

WMLInputElement::WMLInputElement(Document& document)
    : WMLFormControlElement(inputTag, document)
    , m_isPasswordField(false)
    , m_isEmptyOk(false)
    , m_size(defaultSize)
    , m_maxLength(maximumLength)
    , m_wasChangedSinceLastChangeEvent(false)
    , m_numOfCharsAllowedByMask(0)
    , m_prelength(0)
{
}

PassRefPtr<WMLInputElement> WMLInputElement::create(Document& document)
{
    RefPtr<WMLInputElement> inputElem = adoptRef(new WMLInputElement(document));
    inputElem->ensureUserAgentShadowRoot();
    return inputElem.release();
}

WMLInputElement::~WMLInputElement()
{
    // M: TODO WML_SUPPORT
    // Issue 229605:    Remove page cache code from Blink
    // https://code.google.com/p/chromium/issues/detail?id=229605
    //if (m_isPasswordField)
    //    document()->unregisterForDocumentActivationCallbacks(this);
}

void WMLInputElement::createShadowSubtree()
{
    Document& doc = document();
    m_innerText = TextControlInnerEditorElement::create(doc);

    userAgentShadowRoot()->appendChild(m_innerText.release(), IGNORE_EXCEPTION);
}

void WMLInputElement::destroyShadowSubtree()
{
    /// M: @{
    if (ShadowRoot* root = innerTextElement()->userAgentShadowRoot())
        root->removeChildren();
    /// @}
}


void WMLInputElement::didAddUserAgentShadowRoot(ShadowRoot&)
{
    createShadowSubtree();
}

static const AtomicString& formatCodes()
{
    DEFINE_STATIC_LOCAL(AtomicString, codes, ("AaNnXxMm"));
    return codes;
}

bool WMLInputElement::isKeyboardFocusable(KeyboardEvent*) const
{
    return WMLFormControlElement::isFocusable();
}

bool WMLInputElement::isMouseFocusable() const
{
    return WMLFormControlElement::isFocusable();
}

void WMLInputElement::dispatchFocusEvent(Element* oldFocusedElement, FocusType type)
{
    InputElement::dispatchFocusEvent(this, this);
    WMLElement::dispatchFocusEvent(0, type);
}

void WMLInputElement::dispatchBlurEvent(Element* newFocusedElement)
{
    // Firstly check if it is allowed to leave this input field
    String val = value();
    if ((!m_isEmptyOk && val.isEmpty()) || !isConformedToInputMask(val)) {
        updateFocusAppearance(true);
        return;
    }

    // update the name variable of WML input elmenet
    String nameVariable = formControlName();
    if (!nameVariable.isEmpty())
        wmlPageStateForDocument(document())->storeVariable(nameVariable, val);

    InputElement::dispatchBlurEvent(this, this);
    WMLElement::dispatchBlurEvent(0);
}

void WMLInputElement::updateFocusAppearance(bool restorePreviousSelection)
{
    InputElement::updateFocusAppearance(m_data, this, this, restorePreviousSelection);
}

void WMLInputElement::aboutToUnload()
{
    InputElement::aboutToUnload(this, this);
}

int WMLInputElement::size() const
{
    return m_data.size();
}

const AtomicString& WMLInputElement::formControlType() const
{
    // needs to be lowercase according to DOM spec
    if (m_isPasswordField) {
        DEFINE_STATIC_LOCAL(const AtomicString, password, ("password"));
        return password;
    }

    DEFINE_STATIC_LOCAL(const AtomicString, text, ("text"));
    return text;
}

const AtomicString& WMLInputElement::formControlName() const
{
    return m_data.name();
}

const String& WMLInputElement::suggestedValue() const
{
    return m_data.suggestedValue();
}

String WMLInputElement::value() const
{
    String value = m_data.value();
    if (value.isNull())
        value = sanitizeValue(getAttribute(HTMLNames::valueAttr));

    return value;
}

void WMLInputElement::setValue(const String& value, bool)
{
    setFormControlValueMatchesRenderer(false);
    m_data.setValue(sanitizeValue(value));
    /// M: @{
    setInnerTextValue(m_data.value());
    /// @}
    if (inDocument())
        document().updateRenderTreeIfNeeded();
    if (renderer())
        renderer()->updateFromElement();
    setNeedsStyleRecalc(SubtreeStyleChange);

    unsigned max = m_data.value().length();
    if (document().focusedElement() == this)
        InputElement::updateSelectionRange(this, this, max, max);
    else
        cacheSelection(max, max);

    InputElement::notifyFormStateChanged(this);
}

void WMLInputElement::setValueForUser(const String&)
{
    /* InputElement class defines pure virtual function 'setValueForUser', which 
       will be useful only in HTMLInputElement. Do nothing in 'WMLInputElement'.
     */
}

void WMLInputElement::setValueFromRenderer(const String& value)
{
    InputElement::setValueFromRenderer(m_data, this, this, value);
    m_data.setValue(value);
}

bool WMLInputElement::wasChangedSinceLastFormControlChangeEvent() const
{
    return m_wasChangedSinceLastChangeEvent;
}

void WMLInputElement::setChangedSinceLastFormControlChangeEvent(bool changed)
{
    m_wasChangedSinceLastChangeEvent = changed;
}

bool WMLInputElement::saveFormControlState(String& result) const
{
    if (m_isPasswordField)
        return false;

    result = value();
    return true;
}

void WMLInputElement::restoreFormControlState(const String& state)
{
    ASSERT(!m_isPasswordField); // should never save/restore password fields
    setValue(state);
}

void WMLInputElement::select()
{
    //FIXME: WML_SUPPORT
    //setSelectionRange(this, 0, innerTextValue().length());
}

void WMLInputElement::accessKeyAction(bool)
{
    // should never restore previous selection here
    focus(false);
}

void WMLInputElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == HTMLNames::nameAttr)
        m_data.setName(parseValueForbiddingVariableReferences(value));
    else if (name == HTMLNames::typeAttr) {
        String type = parseValueForbiddingVariableReferences(value);
        m_isPasswordField = (type == "password");
    } else if (name == HTMLNames::valueAttr) {
        // We only need to setChanged if the form is looking at the default value right now.
        if (m_data.value().isNull())
            setNeedsStyleRecalc(SubtreeStyleChange);
        setFormControlValueMatchesRenderer(false);
    } else if (name == HTMLNames::maxlengthAttr)
        parseMaxLengthAttribute(value);
    else if (name == HTMLNames::sizeAttr) {
        int oldSize = m_size;
        int valueAsInteger = value.toInt();
        m_size = valueAsInteger > 0 ? valueAsInteger : defaultSize;
        if (m_size != oldSize && renderer())
            renderer()->setNeedsLayoutAndPrefWidthsRecalc();
    } else if (name == WMLNames::formatAttr)
        m_formatMask = validateInputMask(parseValueForbiddingVariableReferences(value));
    else if (name == WMLNames::emptyokAttr)
        m_isEmptyOk = (value == "true");
    else
        WMLElement::parseAttribute(name, value);

    // FIXME: Handle 'accesskey' attribute
    // FIXME: Handle 'tabindex' attribute
    // FIXME: Handle 'title' attribute
}

void WMLInputElement::parseMaxLengthAttribute(const AtomicString& value)
{
    int maxLength;
    if (!parseHTMLInteger(value, maxLength))
        maxLength = maximumLength;
    if (maxLength < 0 || maxLength > maximumLength)
        maxLength = maximumLength;
    int oldMaxLength = m_maxLength;
    m_maxLength = maxLength;
    m_data.setMaxLength(maxLength);
    if (oldMaxLength != maxLength)
        updateValueIfNeeded();
    setNeedsStyleRecalc(SubtreeStyleChange);
    //setNeedsValidityCheck();
}

void WMLInputElement::updateValueIfNeeded()
{
    String newValue = sanitizeValue(m_data.value());
    if (newValue != m_data.value())
        setValue(newValue);
}

void WMLInputElement::copyNonAttributePropertiesFromElement(const Element& source)
{
    const WMLInputElement& sourceElement = static_cast<const WMLInputElement&>(source);
    m_data.setValue(sourceElement.m_data.value());
    WMLFormControlElement::copyNonAttributePropertiesFromElement(source);
}

RenderObject* WMLInputElement::createRenderer(RenderStyle* style)
{
    /// M: @{
    return new RenderWMLTextControlSingleLine(this);
    /// @}
}

void WMLInputElement::detach(const AttachContext& context)
{
    WMLElement::detach(context);
    setFormControlValueMatchesRenderer(false);
}
    
bool WMLInputElement::appendFormData(FormDataList& encoding, bool)
{
    if (formControlName().isEmpty())
        return false;

    encoding.appendData(formControlName(), value());
    return true;
}

void WMLInputElement::reset()
{
    setValue(String());
}

void WMLInputElement::defaultEventHandler(Event* evt)
{
    bool clickDefaultFormButton = false;

    if (evt->type() == EventTypeNames::textInput && evt->hasInterface(EventNames::TextEvent)) {
        TextEvent* textEvent = static_cast<TextEvent*>(evt);
        if (textEvent->data() == "\n")
            clickDefaultFormButton = true;
        else {
            // Reset m_prelength when textInput event
            m_prelength = 0;
            if (renderer() && !isConformedToInputMask(textEvent->data()[0], innerTextValue().length() + 1))
                // If the inputed char doesn't conform to the input mask, stop handling
                return;
        }
    }
    /* WMLInputElement doesn't support autofill
    if (evt->type() == EventTypeNames::keydown && evt->isKeyboardEvent() && focused() && document().frame()
        && document().frame()->editor()->doTextFieldCommandFromEvent(this, static_cast<KeyboardEvent*>(evt))) {
        evt->setDefaultHandled();
        return;
    }*/
    
    // Let the key handling done in EventTargetNode take precedence over the event handling here for editable text fields
    if (!clickDefaultFormButton) {
        WMLElement::defaultEventHandler(evt);
        if (evt->defaultHandled())
            return;
    }

    // Use key press event here since sending simulated mouse events
    // on key down blocks the proper sending of the key press event.
    if (evt->type() == EventTypeNames::keypress && evt->isKeyboardEvent()) {
        // Simulate mouse click on the default form button for enter for these types of elements.
        if (static_cast<KeyboardEvent*>(evt)->charCode() == '\r')
            clickDefaultFormButton = true;
    }

    if (clickDefaultFormButton) {
        // Fire onChange for text fields.
        if (wasChangedSinceLastFormControlChangeEvent()) {
            setChangedSinceLastFormControlChangeEvent(false);
            dispatchChangeEvent();
            //dispatchEvent(Event::create(EventTypeNames::change, true, false));
        }

        evt->setDefaultHandled();
        return;
    }

    if (evt->isBeforeTextInsertedEvent())
        InputElement::handleBeforeTextInsertedEvent(m_data, this, this, evt);

    if (renderer() && (evt->isMouseEvent() || evt->isDragEvent() || evt->type() == EventTypeNames::blur || evt->type() == EventTypeNames::focus))
        forwardEvent(evt);

    ///M: add. When has two input element, can't move second input element.
    if (evt->type() == EventTypeNames::webkitEditableContentChanged && renderer() && renderer()->isTextControl()) {
        subtreeHasChanged();
    }
}

void WMLInputElement::cacheSelection(int start, int end)
{
    m_data.setCachedSelectionStart(start);
    m_data.setCachedSelectionEnd(end);
}

static bool isASCIILineBreak(UChar c)
{
    return c == '\r' || c == '\n';
}

static String limitLength(const String& string, unsigned maxLength)
{
    unsigned newLength = std::min(maxLength, string.length());
    // FIXME: We should not truncate the string at a control character. It's not
    // compatible with IE and Firefox.
    for (unsigned i = 0; i < newLength; ++i) {
        const UChar current = string[i];
        if (current < ' ' && current != '\t') {
            newLength = i;
            break;
        }
    }
    if (newLength == string.length())
        return string;
    if (newLength > 0 && U16_IS_LEAD(string[newLength - 1]))
        --newLength;
    return string.left(newLength);
}

String WMLInputElement::sanitizeValue(const String& proposedValue) const
{
    return limitLength(proposedValue.removeCharacters(isASCIILineBreak), WMLInputElement::maximumLength);
}

void WMLInputElement::documentDidBecomeActive()
{
    ASSERT(m_isPasswordField);
    reset();
}

void WMLInputElement::willMoveToNewOwnerDocument()
{
    // M: TODO WML_SUPPORT
    // Issue 229605:    Remove page cache code from Blink
    // https://code.google.com/p/chromium/issues/detail?id=229605
    // Always unregister for cache callbacks when leaving a document, even if we would otherwise like to be registered
    //if (m_isPasswordField)
    //    document()->unregisterForDocumentActivationCallbacks(this);

    // M: FIXME WML_SUPPORT
    //WMLElement::willMoveToNewOwnerDocument();
}

void WMLInputElement::didMoveToNewOwnerDocument()
{
    // M: TODO WML_SUPPORT
    // Issue 229605:    Remove page cache code from Blink
    // https://code.google.com/p/chromium/issues/detail?id=229605
    //if (m_isPasswordField)
    //    document()->registerForDocumentActivationCallbacks(this);

    // M: TODO WML_SUPPORT
    //WMLElement::didMoveToNewOwnerDocument();
}

void WMLInputElement::initialize()
{
    String nameVariable = formControlName();
    String variableValue;
    WMLPageState* pageSate = wmlPageStateForDocument(document());
    ASSERT(pageSate);
    if (!nameVariable.isEmpty())
        variableValue = pageSate->getVariable(nameVariable);

    if (variableValue.isEmpty() || !isConformedToInputMask(variableValue)) {
        String val = value();
        if (isConformedToInputMask(val))
            variableValue = val;
        else
            variableValue = "";
 
        pageSate->storeVariable(nameVariable, variableValue);
    }
    setValue(variableValue);
 
    if (!hasAttribute(WMLNames::emptyokAttr)) {
        if (m_formatMask.isEmpty() || 
            // check if the format codes is just "*f"
           (m_formatMask.length() == 2 && m_formatMask[0] == '*' && formatCodes().find(m_formatMask[1]) != kNotFound))
            m_isEmptyOk = true;
    }
}

String WMLInputElement::validateInputMask(const String& inputMask)
{
    bool isValid = true;
    bool hasWildcard = false;
    unsigned escapeCharCount = 0;
    unsigned maskLength = inputMask.length();
    UChar formatCode;
 
    for (unsigned i = 0; i < maskLength; ++i) {
        formatCode = inputMask[i];

        if (formatCodes().find(formatCode) == kNotFound) {
            if (formatCode == '*' || (WTF::isASCIIDigit(formatCode) && formatCode != '0')) {
                // validate codes which ends with '*f' or 'nf'
                formatCode = inputMask[++i];
                if ((i + 1 != maskLength) || formatCodes().find(formatCode) == kNotFound) {
                    isValid = false;
                    break;
                }
                hasWildcard = true;
            } else if (formatCode == '\\') {
                //skip over the next mask character
                ++i;
                ++escapeCharCount;
            } else {
                isValid = false;
                break;
            }
        }
    }

    if (!isValid)
        return String();
 
    // calculate the number of characters allowed to be entered by input mask
    m_numOfCharsAllowedByMask = maskLength;

    if (escapeCharCount)
        m_numOfCharsAllowedByMask -= escapeCharCount;

    if (hasWildcard) {
        formatCode = inputMask[maskLength - 2];
        if (formatCode == '*')
            m_numOfCharsAllowedByMask = m_maxLength;
        else {
            unsigned leftLen = String(&formatCode).toInt();
            m_numOfCharsAllowedByMask = leftLen + m_numOfCharsAllowedByMask - 2;
        }
    }

    return inputMask;
}

/// M: this function remove any character that is not conformed to the input format @{
String WMLInputElement::getTextConformedToInputMask(const String& inputChars)
{
    String newChars = inputChars;

    LocalFrame* frame = document().frame();
    if (!frame)
        return newChars;
    // Set m_prelength when composition just start
    if (!frame->inputMethodController().hasComposition())
        m_prelength = innerTextValue().length();
    for (unsigned i = 0; i < inputChars.length(); ++i) {
        if (!isConformedToInputMask(inputChars[i], i + 1, false)) {
            // if this char is not conformed to the input mask, remove the char and everything after it.
            newChars = inputChars.left(i);
            break;
        }
    }

    return newChars;
}
/// @}

bool WMLInputElement::isConformedToInputMask(const String& inputChars)
{
    // Reset m_prelength before conformation check
    m_prelength = 0;
    for (unsigned i = 0; i < inputChars.length(); ++i)
        if (!isConformedToInputMask(inputChars[i], i + 1, false))
            return false;

    return true;
}
 
bool WMLInputElement::isConformedToInputMask(UChar inChar, unsigned inputCharCount, bool isUserInput)
{
    /// M: @{
    unsigned maxLen = m_formatMask.isEmpty() ? m_maxLength : min(m_numOfCharsAllowedByMask, m_maxLength);
    if ((isUserInput && inputCharCount > maxLen) ||
            (!isUserInput && inputCharCount + m_prelength > maxLen))
        return false;
    /// @}

    if (m_formatMask.isEmpty())
        return true;

    unsigned maskIndex = 0;
    if (isUserInput) {
        unsigned cursorPosition = 0;
        if (renderer())
            cursorPosition = selectionStart();
        else
            cursorPosition = m_data.cachedSelectionStart();

        maskIndex = cursorPositionToMaskIndex(cursorPosition);
    } else
        maskIndex = cursorPositionToMaskIndex(m_prelength + inputCharCount - 1);

    bool ok = true;
    UChar mask = m_formatMask[maskIndex];
    // match the inputed character with input mask
    switch (mask) {
    case 'A':
        ok = !WTF::isASCIIDigit(inChar) && !WTF::isASCIILower(inChar) && WTF::isASCIIPrintable(inChar);
        break;
    case 'a':
        ok = !WTF::isASCIIDigit(inChar) && !WTF::isASCIIUpper(inChar) && WTF::isASCIIPrintable(inChar);
        break;
    case 'N':
        ok = WTF::isASCIIDigit(inChar);
        break;
    case 'n':
        ok = !WTF::isASCIIAlpha(inChar) && WTF::isASCIIPrintable(inChar);
        break;
    case 'X':
        ok = !WTF::isASCIILower(inChar) && WTF::isASCIIPrintable(inChar);
        break;
    case 'x':
        ok = !WTF::isASCIIUpper(inChar) && WTF::isASCIIPrintable(inChar);
        break;
    case 'M':
        ok = WTF::isASCII(inChar) ? WTF::isASCIIPrintable(inChar) : true;
        break;
    case 'm':
        ok = WTF::isASCII(inChar) ? WTF::isASCIIPrintable(inChar) : true;
        break;
    default:
        ok = (mask == inChar);
        break;
    }

    return ok;
}

unsigned WMLInputElement::cursorPositionToMaskIndex(unsigned cursorPosition)
{
    UChar mask;
    int index = -1;
    do {
        mask = m_formatMask[++index];
        if (mask == '\\')
            ++index;
        else if (mask == '*' || (WTF::isASCIIDigit(mask) && mask != '0')) {
            index = m_formatMask.length() - 1;
            break;
        }
    } while (cursorPosition--);
 
    return index;
}

/// M: Add WMLInputElement maxLength
int WMLInputElement::maxLength() const
{
    return m_maxLength;
}

static String finishText(StringBuilder& result)
{
    // Remove one trailing newline; there's always one that's collapsed out by rendering.
    size_t size = result.length();
    if (size && result[size - 1] == '\n')
        result.resize(--size);
    return result.toString();
}

// TODO WML_SUPPORT Need check
// Moved from HTMLTextFormControlElement
String WMLInputElement::innerTextValue() const
{
    Element* innerText = innerTextElement();
    if (!innerText || !isTextFormControl())
        return emptyString();

    StringBuilder result;
    for (Node* node = innerText; node; node = NodeTraversal::next(*node, innerText)) {
        if (node->hasTagName(WMLNames::brTag))
            result.append(newlineCharacter);
        else if (node->isTextNode())
            result.append(toText(node)->data());
    }
    return finishText(result);
}

void WMLInputElement::forwardEvent(Event* event)
{
    if (event->type() == EventTypeNames::blur || event->type() == EventTypeNames::focus)
        return;
    //FIXME: WML_SUPPORT forward to where?
    innerTextElement()->defaultEventHandler(event);
}

//FIXME: WML_SUPPORT
void WMLInputElement::subtreeHasChanged()
{
    String innerValue = innerTextValue();

    setValueFromRenderer(sanitizeValue(innerTextValue()));
    // Recalc for :invalid change.
    setNeedsStyleRecalc(SubtreeStyleChange);

    //didSetValueByUserEdit(wasChanged ? ValueChangeStateChanged : ValueChangeStateNone);
}

WMLInputElement* enclosingWMLInputControl(const Position& position)
{
    ASSERT(position.isNull() || position.anchorType() == Position::PositionIsOffsetInAnchor
        || position.containerNode() || !position.anchorNode()->shadowHost()
        || (position.anchorNode()->parentNode() && position.anchorNode()->parentNode()->isShadowRoot()));

    Node* container = position.containerNode();
    if (!container)
        return 0;
    Element* ancestor = container->shadowHost();
    return ancestor && isWMLInputElement(ancestor) ? toWMLInputElement(ancestor) : 0;
}

int WMLInputElement::indexForVisiblePosition(const VisiblePosition& pos) const
{
    Position indexPosition = pos.deepEquivalent().parentAnchoredEquivalent();
    if (enclosingWMLInputControl(indexPosition) != this)
        return 0;
    RefPtr<Range> range = Range::create(*indexPosition.document());
    range->setStart(innerTextElement(), 0, ASSERT_NO_EXCEPTION);
    range->setEnd(indexPosition.containerNode(), indexPosition.offsetInContainerNode(), ASSERT_NO_EXCEPTION);
    return TextIterator::rangeLength(range.get());
}

int WMLInputElement::computeSelectionStart() const
{
    ASSERT(isTextFormControl());
    LocalFrame* frame = document().frame();
    if (!frame)
        return 0;

    return indexForVisiblePosition(VisiblePosition(frame->selection().start()));
}

int WMLInputElement::computeSelectionEnd() const
{
    ASSERT(isTextFormControl());
    LocalFrame* frame = document().frame();
    if (!frame)
        return 0;

    return indexForVisiblePosition(VisiblePosition(frame->selection().end()));
}

#if 0
TextFieldSelectionDirection WMLInputElement::computeSelectionDirection() const
{
    ASSERT(isTextFormControl());
    Frame* frame = document().frame();
    if (!frame)
        return SelectionHasNoDirection;

    const VisibleSelection& selection = frame->selection()->selection();
    return selection.isDirectional() ? (selection.isBaseFirst() ? SelectionHasForwardDirection : SelectionHasBackwardDirection) : SelectionHasNoDirection;
}
#endif

int WMLInputElement::selectionEnd() const
{
    if (!isTextFormControl())
        return 0;
    if (document().focusedElement() != this && hasCachedSelection())
        return m_data.cachedSelectionEnd();
    return computeSelectionEnd();
}

//FIXME: WML_SUPPORT
int WMLInputElement::selectionStart() const
{
    if (!isTextFormControl())
        return 0;
    if (document().focusedElement() != this && hasCachedSelection())
        return m_data.cachedSelectionStart();

    return computeSelectionStart();
}

void WMLInputElement::selectionChanged(bool userTriggered)
{
    if (!renderer() || !isTextFormControl())
        return;

    // selectionStart() or selectionEnd() will return cached selection when this node doesn't have focus
    cacheSelection(computeSelectionStart(), computeSelectionEnd()/*, computeSelectionDirection()*/);

    if (LocalFrame* frame = document().frame()) {
        if (frame->selection().isRange() && userTriggered)
            dispatchEvent(Event::createBubble(EventTypeNames::select));
    }
}

/// M: @{
void WMLInputElement::setInnerTextValue(const String& value)
{
    if(!innerTextElement())
        return;

    bool textIsChanged = value != innerTextValue();
    if (textIsChanged || !innerTextElement()->hasChildren()) {
        if (textIsChanged && renderer()) {
            if (AXObjectCache* cache = document().existingAXObjectCache())
                cache->postNotification(this, AXObjectCache::AXValueChanged, false);
        }

        innerTextElement()->setInnerText(value, ASSERT_NO_EXCEPTION);
    }

    setFormControlValueMatchesRenderer(true);
}

HTMLElement* WMLInputElement::innerTextElement() const
{
    return toHTMLElement(userAgentShadowRoot()->getElementById(ShadowElementNames::innerEditor()));
}
/// @}

bool WMLInputElement::hasInputMask()
{
    return m_maxLength < maximumLength || !m_formatMask.isEmpty();
}

}

#endif

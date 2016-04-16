
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Text.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/AdapterInputConnection.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/ImeAdapter.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::IString;
using Elastos::Core::ICharSequence;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::EIID_ISpannable;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::CSelection;
using Elastos::Droid::View::InputMethod::CExtractedText;
using Elastos::Droid::R;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

//==================================================================
//                AdapterInputConnection::ImeState
//==================================================================

AdapterInputConnection::ImeState::ImeState(
    /* [in] */ const String& _text,
    /* [in] */ Int32 _selectionStart,
    /* [in] */ Int32 _selectionEnd,
    /* [in] */ Int32 _compositionStart,
    /* [in] */ Int32 _compositionEnd)
    : text(_text)
    , selectionStart(_selectionStart)
    , selectionEnd(_selectionEnd)
    , compositionStart(_compositionStart)
    , compositionEnd(_compositionEnd)
{
}

//==================================================================
//                      AdapterInputConnection
//==================================================================

const String AdapterInputConnection::TAG("AdapterInputConnection");
const Boolean AdapterInputConnection::DEBUG = FALSE;

/**
 * Selection value should be -1 if not known. See EditorInfo.java for details.
 */
const Int32 AdapterInputConnection::INVALID_SELECTION;
const Int32 AdapterInputConnection::INVALID_COMPOSITION;

//@VisibleForTesting
AdapterInputConnection::AdapterInputConnection(
    /* [in] */ IView* view,
    /* [in] */ ImeAdapter* imeAdapter,
    /* [in] */ IEditable* editable,
    /* [in] */ IEditorInfo* outAttrs)
    : mInternalView(view)
    , mImeAdapter(imeAdapter)
    , mEditable(editable)
    , mSingleLine(FALSE)
    , mNumNestedBatchEdits(0)
    , mLastUpdateSelectionStart(INVALID_SELECTION)
    , mLastUpdateSelectionEnd(INVALID_SELECTION)
    , mLastUpdateCompositionStart(INVALID_COMPOSITION)
    , mLastUpdateCompositionEnd(INVALID_COMPOSITION)
{
    BaseInputConnection::constructor(view, TRUE);
    mImeAdapter->SetInputConnection(this);

    // The editable passed in might have been in use by a prior keyboard and could have had
    // prior composition spans set.  To avoid keyboard conflicts, remove all composing spans
    // when taking ownership of an existing Editable.
    RemoveComposingSpans(ISpannable::Probe(mEditable));
    mSingleLine = TRUE;
    outAttrs->SetImeOptions(IEditorInfo::IME_FLAG_NO_FULLSCREEN
            | IEditorInfo::IME_FLAG_NO_EXTRACT_UI);
    outAttrs->SetInputType(IInputType::TYPE_CLASS_TEXT
             | IInputType::TYPE_TEXT_VARIATION_WEB_EDIT_TEXT);

    if (imeAdapter->GetTextInputType() == ImeAdapter::sTextInputTypeText) {
        // Normal text field
        Int32 inputType;
        outAttrs->GetInputType(&inputType);
        inputType |= IInputType::TYPE_TEXT_FLAG_AUTO_CORRECT;
        outAttrs->SetInputType(inputType);
        Int32 imeOptions;
        outAttrs->GetImeOptions(&imeOptions);
        imeOptions |= IEditorInfo::IME_ACTION_GO;
        outAttrs->SetImeOptions(imeOptions);
    }
    else if (imeAdapter->GetTextInputType() == ImeAdapter::sTextInputTypeTextArea ||
            imeAdapter->GetTextInputType() == ImeAdapter::sTextInputTypeContentEditable) {
        // TextArea or contenteditable.
        Int32 inputType;
        outAttrs->GetInputType(&inputType);
        inputType |= IInputType::TYPE_TEXT_FLAG_MULTI_LINE
                 | IInputType::TYPE_TEXT_FLAG_CAP_SENTENCES
                 | IInputType::TYPE_TEXT_FLAG_AUTO_CORRECT;
        outAttrs->SetInputType(inputType);
        Int32 imeOptions;
        outAttrs->GetImeOptions(&imeOptions);
        imeOptions |= IEditorInfo::IME_ACTION_NONE;
        outAttrs->SetInputType(imeOptions);
        mSingleLine = FALSE;
    }
    else if (imeAdapter->GetTextInputType() == ImeAdapter::sTextInputTypePassword) {
        // Password
        Int32 inputType;
        outAttrs->GetInputType(&inputType);
        inputType = IInputType::TYPE_CLASS_TEXT
                | IInputType::TYPE_TEXT_VARIATION_WEB_PASSWORD;
        outAttrs->SetInputType(inputType);
        Int32 imeOptions;
        outAttrs->GetImeOptions(&imeOptions);
        imeOptions |= IEditorInfo::IME_ACTION_GO;
        outAttrs->SetImeOptions(imeOptions);
    }
    else if (imeAdapter->GetTextInputType() == ImeAdapter::sTextInputTypeSearch) {
        // Search
        Int32 imeOptions;
        outAttrs->GetImeOptions(&imeOptions);
        imeOptions |= IEditorInfo::IME_ACTION_SEARCH;
        outAttrs->SetImeOptions(imeOptions);
    }
    else if (imeAdapter->GetTextInputType() == ImeAdapter::sTextInputTypeUrl) {
        // Url
        Int32 inputType;
        outAttrs->GetInputType(&inputType);
        inputType = IInputType::TYPE_CLASS_TEXT
                | IInputType::TYPE_TEXT_VARIATION_URI;
        outAttrs->SetInputType(inputType);
        Int32 imeOptions;
        outAttrs->GetImeOptions(&imeOptions);
        imeOptions |= IEditorInfo::IME_ACTION_GO;
        outAttrs->SetImeOptions(imeOptions);
    }
    else if (imeAdapter->GetTextInputType() == ImeAdapter::sTextInputTypeEmail) {
        // Email
        Int32 inputType;
        outAttrs->GetInputType(&inputType);
        inputType = IInputType::TYPE_CLASS_TEXT
                | IInputType::TYPE_TEXT_VARIATION_WEB_EMAIL_ADDRESS;
        outAttrs->SetInputType(inputType);
        Int32 imeOptions;
        outAttrs->GetImeOptions(&imeOptions);
        imeOptions |= IEditorInfo::IME_ACTION_GO;
        outAttrs->SetImeOptions(imeOptions);
    }
    else if (imeAdapter->GetTextInputType() == ImeAdapter::sTextInputTypeTel) {
        // Telephone
        // Number and telephone do not have both a Tab key and an
        // action in default OSK, so set the action to NEXT
        Int32 inputType;
        outAttrs->GetInputType(&inputType);
        inputType = IInputType::TYPE_CLASS_PHONE;
        outAttrs->SetInputType(inputType);
        Int32 imeOptions;
        outAttrs->GetImeOptions(&imeOptions);
        imeOptions |= IEditorInfo::IME_ACTION_NEXT;
        outAttrs->SetImeOptions(imeOptions);
    }
    else if (imeAdapter->GetTextInputType() == ImeAdapter::sTextInputTypeNumber) {
        // Number
        Int32 inputType;
        outAttrs->GetInputType(&inputType);
        inputType = IInputType::TYPE_CLASS_NUMBER
                | IInputType::TYPE_NUMBER_VARIATION_NORMAL
                | IInputType::TYPE_NUMBER_FLAG_DECIMAL;
        outAttrs->SetInputType(inputType);
        Int32 imeOptions;
        outAttrs->GetImeOptions(&imeOptions);
        imeOptions |= IEditorInfo::IME_ACTION_NEXT;
        outAttrs->SetImeOptions(imeOptions);
    }

    AutoPtr<ISelection> selection;
    CSelection::AcquireSingleton((ISelection**)&selection);
    Int32 start, end;
    AutoPtr<ICharSequence> editableCS = ICharSequence::Probe(mEditable);
    selection->GetSelectionStart(editableCS, &start);
    selection->GetSelectionEnd(editableCS, &end);
    outAttrs->SetInitialSelStart(start);
    outAttrs->SetInitialSelEnd(end);
    mLastUpdateSelectionStart = start;
    mLastUpdateSelectionEnd = end;

    //Int32 initialSelStart, initialSelEnd;
    //selection->GetinitialSelStart(&initialSelStart);
    //selection->GetinitialSelEnd(&initialSelEnd);
    AutoPtr<ISpannable> spannable = ISpannable::Probe(mEditable);
    selection->SetSelection(spannable, start, end);
    UpdateSelectionIfRequired();
}

/**
 * Updates the AdapterInputConnection's internal representation of the text being edited and
 * its selection and composition properties. The resulting Editable is accessible through the
 * getEditable() method. If the text has not changed, this also calls updateSelection on the
 * InputMethodManager.
 *
 * @param text The String contents of the field being edited.
 * @param selectionStart The character offset of the selection start, or the caret position if
 *                       there is no selection.
 * @param selectionEnd The character offset of the selection end, or the caret position if there
 *                     is no selection.
 * @param compositionStart The character offset of the composition start, or -1 if there is no
 *                         composition.
 * @param compositionEnd The character offset of the composition end, or -1 if there is no
 *                       selection.
 * @param isNonImeChange True when the update was caused by non-IME (e.g. Javascript).
 */
//@VisibleForTesting
void AdapterInputConnection::UpdateState(
    /* [in] */ const String& _text,
    /* [in] */ Int32 selectionStart,
    /* [in] */ Int32 selectionEnd,
    /* [in] */ Int32 compositionStart,
    /* [in] */ Int32 compositionEnd,
    /* [in] */ Boolean isNonImeChange)
{
    String text = _text;
    if (DEBUG) {
        String log("updateState [");
        log += text;
        log += "] [";
        log += StringUtils::ToString(selectionStart);
        log += " ";
        log += StringUtils::ToString(selectionEnd);
        log += "] [";
        log += StringUtils::ToString(compositionStart);
        log += " ";
        log += StringUtils::ToString(compositionEnd);
        log += "] [";
        log += StringUtils::BooleanToString(isNonImeChange);
        log += "]";
        Slogger::W(TAG, log);
    }
    // If this update is from the IME, no further state modification is necessary because the
    // state should have been updated already by the IM framework directly.
    if (!isNonImeChange) return;

    // Non-breaking spaces can cause the IME to get confused. Replace with normal spaces.
    text = text.Replace('\u00A0', ' ');//TODO is this OK??

    selectionStart = Elastos::Core::Math::Min(selectionStart, text.GetLength());
    selectionEnd = Elastos::Core::Math::Min(selectionEnd, text.GetLength());
    compositionStart = Elastos::Core::Math::Min(compositionStart, text.GetLength());
    compositionEnd = Elastos::Core::Math::Min(compositionEnd, text.GetLength());

    String prevText;
    IObject::Probe(mEditable)->ToString(&prevText);
    Boolean textUnchanged = prevText.Equals(text);

    if (!textUnchanged) {
        AutoPtr<ICharSequence> cs;
        CString::New(text, (ICharSequence**)&cs);
        Int32 len;
        mEditable->Replace(0, (ICharSequence::Probe(mEditable)->GetLength(&len), len), cs);
    }

    AutoPtr<ISelection> selection;
    CSelection::AcquireSingleton((ISelection**)&selection);
    AutoPtr<ISpannable> spannable = ISpannable::Probe(mEditable);
    selection->SetSelection(spannable, selectionStart, selectionEnd);

    if (compositionStart == compositionEnd) {
        RemoveComposingSpans(ISpannable::Probe(mEditable));
    }
    else {
        Boolean res;
        BaseInputConnection::SetComposingRegion(compositionStart, compositionEnd, &res);
    }

    UpdateSelectionIfRequired();
}

/**
 * @return Editable object which contains the state of current focused editable element.
 */
//@Override
ECode AdapterInputConnection::GetEditable(
    /* [out] */ IEditable** editable)
{
    VALIDATE_NOT_NULL(editable);
    *editable = mEditable;
    REFCOUNT_ADD(*editable)
    return NOERROR;
}

/**
 * Sends selection update to the InputMethodManager unless we are currently in a batch edit or
 * if the exact same selection and composition update was sent already.
 */
void AdapterInputConnection::UpdateSelectionIfRequired()
{
    if (mNumNestedBatchEdits != 0) return;

    AutoPtr<ISelection> selection;
    CSelection::AcquireSingleton((ISelection**)&selection);
    Int32 selectionStart;
    AutoPtr<ICharSequence> editableCS = ICharSequence::Probe(mEditable);
    selection->GetSelectionStart(editableCS, &selectionStart);
    Int32 selectionEnd;
    selection->GetSelectionEnd(editableCS, &selectionEnd);
    Int32 compositionStart = GetComposingSpanStart(ISpannable::Probe(mEditable));
    Int32 compositionEnd = GetComposingSpanEnd(ISpannable::Probe(mEditable));
    // Avoid sending update if we sent an exact update already previously.
    if (mLastUpdateSelectionStart == selectionStart &&
            mLastUpdateSelectionEnd == selectionEnd &&
            mLastUpdateCompositionStart == compositionStart &&
            mLastUpdateCompositionEnd == compositionEnd) {
        return;
    }

    if (DEBUG) {
        String log("updateSelectionIfRequired [");
        log += StringUtils::ToString(selectionStart);
        log += " ";
        log += StringUtils::ToString(selectionEnd);
        log += "] [";
        log += StringUtils::ToString(compositionStart);
        log += " ";
        log += StringUtils::ToString(compositionEnd);
        log += "]";
        Slogger::W(TAG, log);
    }

    // updateSelection should be called every time the selection or composition changes
    // if it happens not within a batch edit, or at the end of each top level batch edit.
    GetInputMethodManagerWrapper()->UpdateSelection(mInternalView,
            selectionStart, selectionEnd, compositionStart, compositionEnd);
    mLastUpdateSelectionStart = selectionStart;
    mLastUpdateSelectionEnd = selectionEnd;
    mLastUpdateCompositionStart = compositionStart;
    mLastUpdateCompositionEnd = compositionEnd;
}

/**
 * @see BaseInputConnection#setComposingText(java.lang.CharSequence, int)
 */
//@Override
ECode AdapterInputConnection::SetComposingText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DEBUG) {
        String log("setComposingText [");
        String textStr;
        text->ToString(&textStr);
        log += textStr;
        log += "] [";
        log += StringUtils::ToString(newCursorPosition);
        log += "]";
        Slogger::W(TAG, log);
    }

    if (MaybePerformEmptyCompositionWorkaround(text)) {
        *result = TRUE;
        return NOERROR;
    }
    Boolean res;
    BaseInputConnection::SetComposingText(text, newCursorPosition, &res);
    UpdateSelectionIfRequired();
    *result = mImeAdapter->CheckCompositionQueueAndCallNative(text, newCursorPosition, false);
    return NOERROR;
}

/**
 * @see BaseInputConnection#commitText(java.lang.CharSequence, int)
 */
//@Override
ECode AdapterInputConnection::CommitText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DEBUG) {
        String log("commitText [");
        String textStr;
        text->ToString(&textStr);
        log += textStr;
        log += "] [";
        log += StringUtils::ToString(newCursorPosition);
        log += "]";
        Slogger::W(TAG, log);
    }

    if (MaybePerformEmptyCompositionWorkaround(text)) {
        *result = TRUE;
        return NOERROR;
    }
    Boolean res;
    BaseInputConnection::CommitText(text, newCursorPosition, &res);
    UpdateSelectionIfRequired();
    Int32 length;
    text->GetLength(&length);
    *result = mImeAdapter->CheckCompositionQueueAndCallNative(text, newCursorPosition,
            length > 0);
    return NOERROR;
}

/**
 * @see BaseInputConnection#performEditorAction(int)
 */
//@Override
ECode AdapterInputConnection::PerformEditorAction(
    /* [in] */ Int32 actionCode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) {
        String log("performEditorAction [");
        log += StringUtils::ToString(actionCode);
        log += "]";
        Slogger::W(TAG, log);
    }

    if (actionCode == IEditorInfo::IME_ACTION_NEXT) {
        RestartInput();
        // Send TAB key event
        Int64 timeStampMs = SystemClock::GetUptimeMillis();
        mImeAdapter->SendSyntheticKeyEvent(
                ImeAdapter::sEventTypeRawKeyDown, timeStampMs, IKeyEvent::KEYCODE_TAB, 0);
    }
    else {
        mImeAdapter->SendKeyEventWithKeyCode(IKeyEvent::KEYCODE_ENTER,
                IKeyEvent::FLAG_SOFT_KEYBOARD | IKeyEvent::FLAG_KEEP_TOUCH_MODE
                | IKeyEvent::FLAG_EDITOR_ACTION);
    }

    *result = TRUE;

    return NOERROR;
}

/**
 * @see BaseInputConnection#performContextMenuAction(int)
 */
//@Override
ECode AdapterInputConnection::PerformContextMenuAction(
    /* [in] */ Int32 id,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) {
        String log("performContextMenuAction [");
        log += StringUtils::ToString(id);
        log += "]";
        Slogger::W(TAG, log);
    }

    switch (id) {
        case R::id::selectAll:
            return mImeAdapter->SelectAll();
        case R::id::cut:
            return mImeAdapter->Cut();
        case R::id::copy:
            return mImeAdapter->Copy();
        case R::id::paste:
            return mImeAdapter->Paste();
        default:
            *result = FALSE;
            return NOERROR;
    }

    return NOERROR;
}

/**
 * @see BaseInputConnection#getExtractedText(android.view.inputmethod.ExtractedTextRequest,
 *                                           int)
 */
//@Override
ECode AdapterInputConnection::GetExtractedText(
    /* [in] */ IExtractedTextRequest* request,
    /* [in] */ Int32 flags,
    /* [out] */ IExtractedText** text)
{
    VALIDATE_NOT_NULL(text);

    if (DEBUG) {
        Slogger::W(TAG, "getExtractedText");
    }

    AutoPtr<IExtractedText> et;
    CExtractedText::New((IExtractedText**)&et);
    AutoPtr<ICharSequence> editableCS = ICharSequence::Probe(mEditable);
    et->SetText(editableCS);
    Int32 length;
    editableCS->GetLength(&length);
    et->SetPartialEndOffset(length);
    AutoPtr<ISelection> selection;
    CSelection::AcquireSingleton((ISelection**)&selection);
    Int32 start;
    selection->GetSelectionStart(editableCS, &start);
    et->SetSelectionStart(start);
    Int32 end;
    selection->GetSelectionEnd(editableCS, &end);
    et->SetSelectionEnd(end);
    et->SetFlags(mSingleLine ? IExtractedText::FLAG_SINGLE_LINE : 0);

    *text = et;
    REFCOUNT_ADD(*text)

    return NOERROR;
}

/**
 * @see BaseInputConnection#beginBatchEdit()
 */
//@Override
ECode AdapterInputConnection::BeginBatchEdit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) {
        String log("beginBatchEdit [");
        log += StringUtils::BooleanToString(mNumNestedBatchEdits == 0);
        log += "]";
        Slogger::W(TAG, log);
    }

    mNumNestedBatchEdits++;
    *result = TRUE;
    return NOERROR;
}

/**
 * @see BaseInputConnection#endBatchEdit()
 */
//@Override
ECode AdapterInputConnection::EndBatchEdit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mNumNestedBatchEdits == 0) {
        *result = FALSE;
        return NOERROR;
    }
    --mNumNestedBatchEdits;

    if (DEBUG) {
        String log("endBatchEdit [");
        log += StringUtils::ToString(mNumNestedBatchEdits == 0);
        log += "]";
        Slogger::W(TAG, log);
    }

    if (mNumNestedBatchEdits == 0) UpdateSelectionIfRequired();

    *result = (mNumNestedBatchEdits != 0);

    return NOERROR;
}

/**
 * @see BaseInputConnection#deleteSurroundingText(int, int)
 */
//@Override
ECode AdapterInputConnection::DeleteSurroundingText(
    /* [in] */ Int32 beforeLength,
    /* [in] */ Int32 afterLength,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) {
        String log("deleteSurroundingText [");
        log += StringUtils::ToString(beforeLength);
        log += " ";
        log += StringUtils::ToString(afterLength);
        log += "]";
        Slogger::W(TAG, log);
    }

    AutoPtr<ISelection> selection;
    CSelection::AcquireSingleton((ISelection**)&selection);
    Int32 availableBefore;
    AutoPtr<ICharSequence> editableCS = ICharSequence::Probe(mEditable);
    selection->GetSelectionStart(editableCS, &availableBefore);
    Int32 end;
    selection->GetSelectionEnd(editableCS, &end);
    Int32 length;
    editableCS->GetLength(&length);
    Int32 availableAfter = length - end;
    beforeLength = Elastos::Core::Math::Min(beforeLength, availableBefore);
    afterLength = Elastos::Core::Math::Min(afterLength, availableAfter);
    Boolean res;
    BaseInputConnection::DeleteSurroundingText(beforeLength, afterLength, &res);
    UpdateSelectionIfRequired();

    *result = mImeAdapter->DeleteSurroundingText(beforeLength, afterLength);

    return NOERROR;
}

/**
 * @see BaseInputConnection#sendKeyEvent(android.view.KeyEvent)
 */
//@Override
ECode AdapterInputConnection::SendKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) {
        String log("sendKeyEvent [");
        Int32 action;
        event->GetAction(&action);
        log += StringUtils::ToString(action);
        log += "] [";
        Int32 keyCode;
        event->GetKeyCode(&keyCode);
        log += StringUtils::ToString(keyCode);
        log += "]";
        Slogger::W(TAG, log);
    }

    // If this is a key-up, and backspace/del or if the key has a character representation,
    // need to update the underlying Editable (i.e. the local representation of the text
    // being edited).
    Int32 action;
    event->GetAction(&action);
    if (action == IKeyEvent::ACTION_UP) {
        Int32 code;
        event->GetKeyCode(&code);
        if (code == IKeyEvent::KEYCODE_DEL) {
            Boolean bFlag = FALSE;
            DeleteSurroundingText(1, 0, &bFlag);
            *result = TRUE;
            return NOERROR;
        }
        else if (code == IKeyEvent::KEYCODE_FORWARD_DEL) {
            Boolean bFlag = FALSE;
            DeleteSurroundingText(0, 1, &bFlag);
            *result = TRUE;
            return NOERROR;
        }
        else {
            Int32 unicodeChar;
            event->GetUnicodeChar(&unicodeChar);
            if (unicodeChar != 0) {
                AutoPtr<ISelection> selection;
                CSelection::AcquireSingleton((ISelection**)&selection);
                Int32 selectionStart;
                AutoPtr<ICharSequence> editableCS = ICharSequence::Probe(mEditable);
                selection->GetSelectionStart(editableCS, &selectionStart);
                Int32 selectionEnd;
                selection->GetSelectionEnd(editableCS, &selectionEnd);
                if (selectionStart > selectionEnd) {
                    Int32 temp = selectionStart;
                    selectionStart = selectionEnd;
                    selectionEnd = temp;
                }

                AutoPtr<IString> iString;
                CString::New(StringUtils::ToString(unicodeChar), (IString**)&iString);
                AutoPtr<ICharSequence> strCS = ICharSequence::Probe(iString);
                mEditable->Replace(selectionStart, selectionEnd, strCS);
            }
        }
    }
    else if (action == IKeyEvent::ACTION_DOWN) {
        // TODO(aurimas): remove this workaround when crbug.com/278584 is fixed.
        Int32 code;
        event->GetKeyCode(&code);
        if (code == IKeyEvent::KEYCODE_ENTER) {
            Boolean bFlag1;
            BeginBatchEdit(&bFlag1);
            Boolean bFlag2;
            FinishComposingText(&bFlag2);
            mImeAdapter->TranslateAndSendNativeEvents(event);
            Boolean bFlag3;
            EndBatchEdit(&bFlag3);
            *result = TRUE;
            return NOERROR;
        }
        else if (code == IKeyEvent::KEYCODE_DEL) {
            *result = TRUE;
            return NOERROR;
        }
        else if (code == IKeyEvent::KEYCODE_FORWARD_DEL) {
            *result = TRUE;
            return NOERROR;
        }
    }

    mImeAdapter->TranslateAndSendNativeEvents(event);

    *result = TRUE;

    return NOERROR;
}

/**
 * @see BaseInputConnection#finishComposingText()
 */
//@Override
ECode AdapterInputConnection::FinishComposingText(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) {
        Slogger::W(TAG, "finishComposingText");
    }

    if (GetComposingSpanStart(ISpannable::Probe(mEditable)) == GetComposingSpanEnd(ISpannable::Probe(mEditable))) {
         return TRUE;
    }

    Boolean res;
    BaseInputConnection::FinishComposingText(&res);
    UpdateSelectionIfRequired();
    mImeAdapter->FinishComposingText();

    *result = TRUE;

    return NOERROR;
}

/**
 * @see BaseInputConnection#setSelection(int, int)
 */
//@Override
ECode AdapterInputConnection::SetSelection(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) {
        String log("setSelection [");
        log += StringUtils::ToString(start);
        log += " ";
        log += StringUtils::ToString(end);
        log += "]";
        Slogger::W(TAG, log);
    }

    Int32 textLength;
    AutoPtr<ICharSequence> editableCS = ICharSequence::Probe(mEditable);
    editableCS->GetLength(&textLength);
    if (start < 0 || end < 0 || start > textLength || end > textLength) {
        *result = TRUE;
        return NOERROR;
    }
    Boolean res;
    BaseInputConnection::SetSelection(start, end, &res);
    UpdateSelectionIfRequired();
    *result = mImeAdapter->SetEditableSelectionOffsets(start, end);
    return NOERROR;
}

/**
 * Informs the InputMethodManager and InputMethodSession (i.e. the IME) that the text
 * state is no longer what the IME has and that it needs to be updated.
 */
void AdapterInputConnection::RestartInput()
{
    if (DEBUG) {
        Slogger::W(TAG, "restartInput");
    }

    GetInputMethodManagerWrapper()->RestartInput(mInternalView);
    mNumNestedBatchEdits = 0;
}

/**
 * @see BaseInputConnection#setComposingRegion(int, int)
 */
//@Override
ECode AdapterInputConnection::SetComposingRegion(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (DEBUG) {
        String log("setComposingRegion [");
        log += StringUtils::ToString(start);
        log += " ";
        log += StringUtils::ToString(end);
        log += "]";
        Slogger::W(TAG, log);
    }

    Int32 textLength;
    AutoPtr<ICharSequence> editableCS = ICharSequence::Probe(mEditable);
    editableCS->GetLength(&textLength);
    Int32 a = Elastos::Core::Math::Min(start, end);
    Int32 b = Elastos::Core::Math::Max(start, end);
    if (a < 0) a = 0;
    if (b < 0) b = 0;
    if (a > textLength) a = textLength;
    if (b > textLength) b = textLength;

    if (a == b) {
        RemoveComposingSpans(ISpannable::Probe(mEditable));
    }
    else {
        Boolean res;
        BaseInputConnection::SetComposingRegion(a, b, &res);
    }

    UpdateSelectionIfRequired();

    *result = mImeAdapter->SetComposingRegion(a, b);

    return NOERROR;
}

Boolean AdapterInputConnection::IsActive()
{
    return GetInputMethodManagerWrapper()->IsActive(mInternalView);
}

AutoPtr<InputMethodManagerWrapper> AdapterInputConnection::GetInputMethodManagerWrapper()
{
    return mImeAdapter->GetInputMethodManagerWrapper();
}

/**
 * This method works around the issue crbug.com/373934 where Blink does not cancel
 * the composition when we send a commit with the empty text.
 *
 * TODO(aurimas) Remove this once crbug.com/373934 is fixed.
 *
 * @param text Text that software keyboard requested to commit.
 * @return Whether the workaround was performed.
 */
Boolean AdapterInputConnection::MaybePerformEmptyCompositionWorkaround(
    /* [in] */ ICharSequence* text)
{
    AutoPtr<ISelection> sel;
    CSelection::AcquireSingleton((ISelection**)&sel);
    AutoPtr<ICharSequence> editableCS = ICharSequence::Probe(mEditable);
    Int32 selectionStart;
    sel->GetSelectionStart(editableCS, &selectionStart);
    Int32 selectionEnd;
    sel->GetSelectionEnd(editableCS, &selectionEnd);
    Int32 compositionStart = GetComposingSpanStart(ISpannable::Probe(mEditable));
    Int32 compositionEnd = GetComposingSpanEnd(ISpannable::Probe(mEditable));
    if (TextUtils::IsEmpty(text) && (selectionStart == selectionEnd)
            && compositionStart != INVALID_COMPOSITION
            && compositionEnd != INVALID_COMPOSITION) {
        Boolean bFlag1;
        BeginBatchEdit(&bFlag1);
        Boolean bFlag2;
        FinishComposingText(&bFlag2);
        Int32 selection;
        sel->GetSelectionStart(editableCS, &selection);
        Boolean result;
        DeleteSurroundingText(selection - compositionStart, selection - compositionEnd, &result);
        Boolean bFlag3;
        EndBatchEdit(&bFlag3);
        return TRUE;
    }

    return FALSE;
}

//@VisibleForTesting
AutoPtr<AdapterInputConnection::ImeState> AdapterInputConnection::GetImeStateForTesting()
{
    String text;
    AutoPtr<ICharSequence> editableCS = ICharSequence::Probe(mEditable);
    editableCS->ToString(&text);
    AutoPtr<ISelection> selection;
    Int32 selectionStart;
    selection->GetSelectionStart(editableCS, &selectionStart);
    Int32 selectionEnd;
    selection->GetSelectionEnd(editableCS, &selectionEnd);
    Int32 compositionStart = GetComposingSpanStart(ISpannable::Probe(mEditable));
    Int32 compositionEnd = GetComposingSpanEnd(ISpannable::Probe(mEditable));
    AutoPtr<ImeState> imeState = new ImeState(text, selectionStart, selectionEnd, compositionStart, compositionEnd);
    return imeState;
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


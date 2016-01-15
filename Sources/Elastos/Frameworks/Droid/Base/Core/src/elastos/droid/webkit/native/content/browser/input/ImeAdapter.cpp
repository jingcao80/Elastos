
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/native/content/browser/input/ImeAdapter.h"
#include "elastos/droid/webkit/native/content/api/ImeAdapter_dec.h"
#include "elastos/droid/webkit/native/content/browser/input/InputDialogContainer.h"
// TODO #include "os/CHandler.h"
#include <elastos/utility/logging/Logger.h>

// TODO using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Text::EIID_ISpannableString;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::EIID_IInputEvent;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

//==================================================================
//                 ImeAdapter::DelayedDismissInput
//==================================================================

ImeAdapter::DelayedDismissInput::DelayedDismissInput(
    /* [in] */ ImeAdapter* owner,
    /* [in] */ Int64 nativeImeAdapter)
    : mOwner(owner)
    , mNativeImeAdapter(nativeImeAdapter)
{
}

//@Override
ECode ImeAdapter::DelayedDismissInput::Run()
{
    mOwner->Attach(mNativeImeAdapter, sTextInputTypeNone);
    mOwner->DismissInput(TRUE);
    return NOERROR;
}

//==================================================================
//            ImeAdapter::AdapterInputConnectionFactory
//==================================================================

AutoPtr<AdapterInputConnection> ImeAdapter::AdapterInputConnectionFactory::Get(
    /* [in] */ IView* view,
    /* [in] */ ImeAdapter* imeAdapter,
    /* [in] */ IEditable* editable,
    /* [in] */ IEditorInfo* outAttrs)
{
    AutoPtr<AdapterInputConnection> ret = new AdapterInputConnection(view, imeAdapter, editable, outAttrs);
    return ret;
}

//==================================================================
//                            ImeAdapter
//==================================================================

const Int32 ImeAdapter::COMPOSITION_KEY_CODE;

// Delay introduced to avoid hiding the keyboard if new show requests are received.
// The time required by the unfocus-focus events triggered by tab has been measured in soju:
// Mean: 18.633 ms, Standard deviation: 7.9837 ms.
// The value here should be higher enough to cover these cases, but not too high to avoid
// letting the user perceiving important delays.
const Int32 ImeAdapter::INPUT_DISMISS_DELAY;

// All the constants that are retrieved from the C++ code.
// They get set through initializeWebInputEvents and initializeTextInputTypes calls.
Int32 ImeAdapter::sEventTypeRawKeyDown = 0;
Int32 ImeAdapter::sEventTypeKeyUp = 0;
Int32 ImeAdapter::sEventTypeChar = 0;
Int32 ImeAdapter::sTextInputTypeNone = 0;
Int32 ImeAdapter::sTextInputTypeText = 0;
Int32 ImeAdapter::sTextInputTypeTextArea = 0;
Int32 ImeAdapter::sTextInputTypePassword = 0;
Int32 ImeAdapter::sTextInputTypeSearch = 0;
Int32 ImeAdapter::sTextInputTypeUrl = 0;
Int32 ImeAdapter::sTextInputTypeEmail = 0;
Int32 ImeAdapter::sTextInputTypeTel = 0;
Int32 ImeAdapter::sTextInputTypeNumber = 0;
Int32 ImeAdapter::sTextInputTypeContentEditable = 0;
Int32 ImeAdapter::sModifierShift = 0;
Int32 ImeAdapter::sModifierAlt = 0;
Int32 ImeAdapter::sModifierCtrl = 0;
Int32 ImeAdapter::sModifierCapsLockOn = 0;
Int32 ImeAdapter::sModifierNumLockOn = 0;

/**
 * @param wrapper InputMethodManagerWrapper that should receive all the call directed to
 *                InputMethodManager.
 * @param embedder The view that is used for callbacks from ImeAdapter.
 */
ImeAdapter::ImeAdapter(
    /* [in] */ InputMethodManagerWrapper* wrapper,
    /* [in] */ ImeAdapterDelegate* embedder)
    : mIsShowWithoutHideOutstanding(FALSE)
    , mNativeImeAdapterAndroid(0)
    , mInputMethodManagerWrapper(wrapper)
    , mViewEmbedder(embedder)
    , mTextInputType(0)
{
    assert(0);
    // TODO
    // CHandler::New((IHandler**)&mHandler);
}

/**
 * Overrides the InputMethodManagerWrapper that ImeAdapter uses to make calls to
 * InputMethodManager.
 * @param immw InputMethodManagerWrapper that should be used to call InputMethodManager.
 */
//@VisibleForTesting
void ImeAdapter::SetInputMethodManagerWrapper(
    /* [in] */ InputMethodManagerWrapper* immw)
{
    mInputMethodManagerWrapper = immw;
}

/**
 * Should be only used by AdapterInputConnection.
 * @return InputMethodManagerWrapper that should receive all the calls directed to
 *         InputMethodManager.
 */
AutoPtr<InputMethodManagerWrapper> ImeAdapter::GetInputMethodManagerWrapper()
{
    return mInputMethodManagerWrapper;
}

/**
 * Set the current active InputConnection when a new InputConnection is constructed.
 * @param inputConnection The input connection that is currently used with IME.
 */
void ImeAdapter::SetInputConnection(
    /* [in] */ AdapterInputConnection* inputConnection)
{
    mInputConnection = inputConnection;
}

/**
 * Should be only used by AdapterInputConnection.
 * @return The input type of currently focused element.
 */
Int32 ImeAdapter::GetTextInputType()
{
    return mTextInputType;
}

/**
 * @return Constant representing that a focused node is not an input field.
 */
Int32 ImeAdapter::GetTextInputTypeNone()
{
    return sTextInputTypeNone;
}

Int32 ImeAdapter::GetModifiers(
    /* [in] */ Int32 metaState)
{
    Int32 modifiers = 0;
    if ((metaState & IKeyEvent::META_SHIFT_ON) != 0) {
        modifiers |= sModifierShift;
    }

    if ((metaState & IKeyEvent::META_ALT_ON) != 0) {
        modifiers |= sModifierAlt;
    }

    if ((metaState & IKeyEvent::META_CTRL_ON) != 0) {
        modifiers |= sModifierCtrl;
    }

    if ((metaState & IKeyEvent::META_CAPS_LOCK_ON) != 0) {
        modifiers |= sModifierCapsLockOn;
    }

    if ((metaState & IKeyEvent::META_NUM_LOCK_ON) != 0) {
        modifiers |= sModifierNumLockOn;
    }

    return modifiers;
}

/**
 * Shows or hides the keyboard based on passed parameters.
 * @param nativeImeAdapter Pointer to the ImeAdapterAndroid object that is sending the update.
 * @param textInputType Text input type for the currently focused field in renderer.
 * @param showIfNeeded Whether the keyboard should be shown if it is currently hidden.
 */
void ImeAdapter::UpdateKeyboardVisibility(
    /* [in] */ Int64 nativeImeAdapter,
    /* [in] */ Int32 textInputType,
    /* [in] */ Boolean showIfNeeded)
{
    mHandler->RemoveCallbacks(mDismissInput);

    // If current input type is none and showIfNeeded is false, IME should not be shown
    // and input type should remain as none.
    if (mTextInputType == sTextInputTypeNone && !showIfNeeded) {
        return;
    }

    if (mNativeImeAdapterAndroid != nativeImeAdapter || mTextInputType != textInputType) {
        // Set a delayed task to perform unfocus. This avoids hiding the keyboard when tabbing
        // through text inputs or when JS rapidly changes focus to another text element.
        if (textInputType == sTextInputTypeNone) {
            mDismissInput = new DelayedDismissInput(this, nativeImeAdapter);
            Boolean result = FALSE;
            mHandler->PostDelayed(mDismissInput, INPUT_DISMISS_DELAY, &result);
            return;
        }

        Attach(nativeImeAdapter, textInputType);

        mInputMethodManagerWrapper->RestartInput(mViewEmbedder->GetAttachedView());
        if (showIfNeeded) {
            ShowKeyboard();
        }
    }
    else if (HasInputType() && showIfNeeded) {
        ShowKeyboard();
    }
}

void ImeAdapter::Attach(
    /* [in] */ Int64 nativeImeAdapter,
    /* [in] */ Int32 textInputType)
{
    if (mNativeImeAdapterAndroid != 0) {
        NativeResetImeAdapter(mNativeImeAdapterAndroid);
    }

    mNativeImeAdapterAndroid = nativeImeAdapter;
    mTextInputType = textInputType;
    if (nativeImeAdapter != 0) {
        NativeAttachImeAdapter(mNativeImeAdapterAndroid);
    }

    if (mTextInputType == sTextInputTypeNone) {
        DismissInput(FALSE);
    }
}

/**
 * Attaches the imeAdapter to its native counterpart. This is needed to start forwarding
 * keyboard events to WebKit.
 * @param nativeImeAdapter The pointer to the native ImeAdapter object.
 */
void ImeAdapter::Attach(
    /* [in] */ Int64 nativeImeAdapter)
{
    Attach(nativeImeAdapter, sTextInputTypeNone);
}

void ImeAdapter::ShowKeyboard()
{
    mIsShowWithoutHideOutstanding = TRUE;
    mInputMethodManagerWrapper->ShowSoftInput(mViewEmbedder->GetAttachedView(), 0,
            mViewEmbedder->GetNewShowKeyboardReceiver());
}

void ImeAdapter::DismissInput(
    /* [in] */ Boolean unzoomIfNeeded)
{
    mIsShowWithoutHideOutstanding  = FALSE;
    AutoPtr<IView> view = mViewEmbedder->GetAttachedView();
    if (mInputMethodManagerWrapper->IsActive(view)) {
        AutoPtr<IBinder> binder;
        view->GetWindowToken((IBinder**)&binder);
        mInputMethodManagerWrapper->HideSoftInputFromWindow(binder, 0,
                unzoomIfNeeded ? mViewEmbedder->GetNewShowKeyboardReceiver() : NULL);
    }

    mViewEmbedder->OnDismissInput();
}

Boolean ImeAdapter::HasInputType()
{
    return mTextInputType != sTextInputTypeNone;
}

Boolean ImeAdapter::IsTextInputType(
    /* [in] */ Int32 type)
{
    return type != sTextInputTypeNone && !InputDialogContainer::IsDialogInputType(type);
}

Boolean ImeAdapter::HasTextInputType()
{
    return IsTextInputType(mTextInputType);
}

/**
 * @return true if the selected text is of password.
 */
Boolean ImeAdapter::IsSelectionPassword()
{
    return mTextInputType == sTextInputTypePassword;
}

Boolean ImeAdapter::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    return TranslateAndSendNativeEvents(event);
}

Int32 ImeAdapter::ShouldSendKeyEventWithKeyCode(
    /* [in] */ const String& text)
{
    if (text.GetLength() != 1) return COMPOSITION_KEY_CODE;

    if (text.Equals("\n")) {
        return IKeyEvent::KEYCODE_ENTER;
    }
    else if (text.Equals("\t")) {
        return IKeyEvent::KEYCODE_TAB;
    }
    else {
        return COMPOSITION_KEY_CODE;
    }
}

void ImeAdapter::SendKeyEventWithKeyCode(
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 flags)
{
    Int64 eventTime = SystemClock::GetUptimeMillis();
    AutoPtr<IKeyEvent> event1;
    assert(0);
    assert(eventTime);//TODO remove warning for unused warning
    // TODO
    // CKeyEvent::New(eventTime, eventTime,
    //         IKeyEvent::ACTION_DOWN, keyCode, 0, 0,
    //         IKeyCharacterMap::VIRTUAL_KEYBOARD, 0,
    //         flags, (IKeyEvent**)&event1);
    TranslateAndSendNativeEvents(event1);

    AutoPtr<IKeyEvent> event2;
    assert(0);
    // TODO
    // CKeyEvent::New(SystemClock::UptimeMillis(), eventTime,
    //         IKeyEvent::ACTION_UP, keyCode, 0, 0,
    //         IKeyCharacterMap::VIRTUAL_KEYBOARD, 0,
    //         flags, (IKeyEvent**)&event2);
    TranslateAndSendNativeEvents(event2);
}

// Calls from Java to C++

Boolean ImeAdapter::CheckCompositionQueueAndCallNative(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition,
    /* [in] */ Boolean isCommit)
{
    if (mNativeImeAdapterAndroid == 0) return false;
    String textStr;
    text->ToString(&textStr);

    // Committing an empty string finishes the current composition.
    Boolean isFinish = textStr.IsEmpty();
    mViewEmbedder->OnImeEvent(isFinish);
    Int32 keyCode = ShouldSendKeyEventWithKeyCode(textStr);
    Int64 timeStampMs = SystemClock::GetUptimeMillis();

    if (keyCode != COMPOSITION_KEY_CODE) {
        SendKeyEventWithKeyCode(keyCode,
                IKeyEvent::FLAG_SOFT_KEYBOARD | IKeyEvent::FLAG_KEEP_TOUCH_MODE);
    }
    else {
        NativeSendSyntheticKeyEvent(mNativeImeAdapterAndroid, sEventTypeRawKeyDown,
                timeStampMs, keyCode, 0);
        if (isCommit) {
            NativeCommitText(mNativeImeAdapterAndroid, textStr);
        }
        else {
            NativeSetComposingText(mNativeImeAdapterAndroid, text, textStr, newCursorPosition);
        }

        NativeSendSyntheticKeyEvent(mNativeImeAdapterAndroid, sEventTypeKeyUp,
                timeStampMs, keyCode, 0);
    }

    return TRUE;
}

void ImeAdapter::FinishComposingText()
{
    if (mNativeImeAdapterAndroid == 0) return;
    NativeFinishComposingText(mNativeImeAdapterAndroid);
}

Boolean ImeAdapter::TranslateAndSendNativeEvents(
    /* [in] */ IKeyEvent* event)
{
    if (mNativeImeAdapterAndroid == 0) return FALSE;

    Int32 action;
    event->GetAction(&action);
    if (action != IKeyEvent::ACTION_DOWN &&
        action != IKeyEvent::ACTION_UP) {
        // action == KeyEvent.ACTION_MULTIPLE
        // TODO(bulach): confirm the actual behavior. Apparently:
        // If event.getKeyCode() == KEYCODE_UNKNOWN, we can send a
        // composition key down (229) followed by a commit text with the
        // string from event.getUnicodeChars().
        // Otherwise, we'd need to send an event with a
        // WebInputEvent::IsAutoRepeat modifier. We also need to verify when
        // we receive ACTION_MULTIPLE: we may receive it after an ACTION_DOWN,
        // and if that's the case, we'll need to review when to send the Char
        // event.
        return FALSE;
    }

    mViewEmbedder->OnImeEvent(FALSE);

    Int32 metaState;
    event->GetMetaState(&metaState);
    Int32 _action;
    event->GetAction(&_action);
    AutoPtr<IInputEvent> inputEvent = IInputEvent::Probe(event);
    Int64 eventTime;
    inputEvent->GetEventTime(&eventTime);
    Int32 keyCode;
    event->GetKeyCode(&keyCode);
    Int32 unicodeChar;
    event->GetUnicodeChar(&unicodeChar);

    return NativeSendKeyEvent(mNativeImeAdapterAndroid, event, metaState,
            GetModifiers(metaState), eventTime, keyCode,
                         /*isSystemKey=*/FALSE, unicodeChar);
}

Boolean ImeAdapter::SendSyntheticKeyEvent(
    /* [in] */ Int32 eventType,
    /* [in] */ Int64 timestampMs,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 unicodeChar)
{
    if (mNativeImeAdapterAndroid == 0) return FALSE;

    NativeSendSyntheticKeyEvent(
            mNativeImeAdapterAndroid, eventType, timestampMs, keyCode, unicodeChar);
    return TRUE;
}

/**
 * Send a request to the native counterpart to delete a given range of characters.
 * @param beforeLength Number of characters to extend the selection by before the existing
 *                     selection.
 * @param afterLength Number of characters to extend the selection by after the existing
 *                    selection.
 * @return Whether the native counterpart of ImeAdapter received the call.
 */
Boolean ImeAdapter::DeleteSurroundingText(
    /* [in] */ Int32 beforeLength,
    /* [in] */ Int32 afterLength)
{
    if (mNativeImeAdapterAndroid == 0) return FALSE;
    NativeDeleteSurroundingText(mNativeImeAdapterAndroid, beforeLength, afterLength);
    return TRUE;
}

/**
 * Send a request to the native counterpart to set the selection to given range.
 * @param start Selection start index.
 * @param end Selection end index.
 * @return Whether the native counterpart of ImeAdapter received the call.
 */
Boolean ImeAdapter::SetEditableSelectionOffsets(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (mNativeImeAdapterAndroid == 0) return FALSE;
    NativeSetEditableSelectionOffsets(mNativeImeAdapterAndroid, start, end);
    return TRUE;
}

/**
 * Send a request to the native counterpart to set compositing region to given indices.
 * @param start The start of the composition.
 * @param end The end of the composition.
 * @return Whether the native counterpart of ImeAdapter received the call.
 */
Boolean ImeAdapter::SetComposingRegion(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (mNativeImeAdapterAndroid == 0) return FALSE;
    NativeSetComposingRegion(mNativeImeAdapterAndroid, start, end);
    return TRUE;
}

/**
 * Send a request to the native counterpart to unselect text.
 * @return Whether the native counterpart of ImeAdapter received the call.
 */
Boolean ImeAdapter::Unselect()
{
    if (mNativeImeAdapterAndroid == 0) return FALSE;
    NativeUnselect(mNativeImeAdapterAndroid);
    return TRUE;
}

/**
 * Send a request to the native counterpart of ImeAdapter to select all the text.
 * @return Whether the native counterpart of ImeAdapter received the call.
 */
Boolean ImeAdapter::SelectAll()
{
    if (mNativeImeAdapterAndroid == 0) return FALSE;
    NativeSelectAll(mNativeImeAdapterAndroid);
    return TRUE;
}

/**
 * Send a request to the native counterpart of ImeAdapter to cut the selected text.
 * @return Whether the native counterpart of ImeAdapter received the call.
 */
Boolean ImeAdapter::Cut()
{
    if (mNativeImeAdapterAndroid == 0) return FALSE;
    NativeCut(mNativeImeAdapterAndroid);
    return TRUE;
}

/**
 * Send a request to the native counterpart of ImeAdapter to copy the selected text.
 * @return Whether the native counterpart of ImeAdapter received the call.
 */
Boolean ImeAdapter::Copy()
{
    if (mNativeImeAdapterAndroid == 0) return FALSE;
    NativeCopy(mNativeImeAdapterAndroid);
    return TRUE;
}

/**
 * Send a request to the native counterpart of ImeAdapter to paste the text from the clipboard.
 * @return Whether the native counterpart of ImeAdapter received the call.
 */
Boolean ImeAdapter::Paste()
{
    if (mNativeImeAdapterAndroid == 0) return FALSE;
    NativePaste(mNativeImeAdapterAndroid);
    return TRUE;
}

// Calls from C++ to Java

//@CalledByNative
void ImeAdapter::InitializeWebInputEvents(
    /* [in] */ Int32 eventTypeRawKeyDown,
    /* [in] */ Int32 eventTypeKeyUp,
    /* [in] */ Int32 eventTypeChar,
    /* [in] */ Int32 modifierShift,
    /* [in] */ Int32 modifierAlt,
    /* [in] */ Int32 modifierCtrl,
    /* [in] */ Int32 modifierCapsLockOn,
    /* [in] */ Int32 modifierNumLockOn)
{
    sEventTypeRawKeyDown = eventTypeRawKeyDown;
    sEventTypeKeyUp = eventTypeKeyUp;
    sEventTypeChar = eventTypeChar;
    sModifierShift = modifierShift;
    sModifierAlt = modifierAlt;
    sModifierCtrl = modifierCtrl;
    sModifierCapsLockOn = modifierCapsLockOn;
    sModifierNumLockOn = modifierNumLockOn;
}

//@CalledByNative
void ImeAdapter::InitializeTextInputTypes(
    /* [in] */ Int32 textInputTypeNone,
    /* [in] */ Int32 textInputTypeText,
    /* [in] */ Int32 textInputTypeTextArea,
    /* [in] */ Int32 textInputTypePassword,
    /* [in] */ Int32 textInputTypeSearch,
    /* [in] */ Int32 textInputTypeUrl,
    /* [in] */ Int32 textInputTypeEmail,
    /* [in] */ Int32 textInputTypeTel,
    /* [in] */ Int32 textInputTypeNumber,
    /* [in] */ Int32 textInputTypeContentEditable)
{
    sTextInputTypeNone = textInputTypeNone;
    sTextInputTypeText = textInputTypeText;
    sTextInputTypeTextArea = textInputTypeTextArea;
    sTextInputTypePassword = textInputTypePassword;
    sTextInputTypeSearch = textInputTypeSearch;
    sTextInputTypeUrl = textInputTypeUrl;
    sTextInputTypeEmail = textInputTypeEmail;
    sTextInputTypeTel = textInputTypeTel;
    sTextInputTypeNumber = textInputTypeNumber;
    sTextInputTypeContentEditable = textInputTypeContentEditable;
}

//@CalledByNative
void ImeAdapter::FocusedNodeChanged(
    /* [in] */ Boolean isEditable)
{
    if (mInputConnection != NULL && isEditable) {
        mInputConnection->RestartInput();
    }
}

//@CalledByNative
void ImeAdapter::PopulateUnderlinesFromSpans(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int64 underlines)
{
    if (ISpannableString::Probe(text) == NULL) return;

    assert(0);
    // TODO
    // AutoPtr<ISpannableString> spannableString = ISpannableString::Probe(text);
    // CharacterStyle spans[] =
    //         spannableString.getSpans(0, text.length(), CharacterStyle.class);
    // for (CharacterStyle span : spans) {
    //     if (span instanceof BackgroundColorSpan) {
    //         NativeAppendBackgroundColorSpan(underlines, spannableString.getSpanStart(span),
    //                 spannableString.getSpanEnd(span),
    //                 ((BackgroundColorSpan) span).getBackgroundColor());
    //     } else if (span instanceof UnderlineSpan) {
    //         nativeAppendUnderlineSpan(underlines, spannableString.getSpanStart(span),
    //                 spannableString.getSpanEnd(span));
    //     }
    // }
}

//@CalledByNative
void ImeAdapter::CancelComposition()
{
    if (mInputConnection != NULL) {
        mInputConnection->RestartInput();
    }
}

//@CalledByNative
void ImeAdapter::Detach()
{
    if (mDismissInput != NULL) {
        mHandler->RemoveCallbacks(mDismissInput);
    }
    mNativeImeAdapterAndroid = 0;
    mTextInputType = 0;
}

Boolean ImeAdapter::NativeSendSyntheticKeyEvent(
    /* [in] */ Int64 nativeImeAdapterAndroid,
    /* [in] */ Int32 eventType,
    /* [in] */ Int64 timestampMs,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 unicodeChar)
{
    return Elastos_ImeAdapter_nativeSendSyntheticKeyEvent(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid,
            eventType, timestampMs, keyCode, unicodeChar);
}

Boolean ImeAdapter::NativeSendKeyEvent(
    /* [in] */ Int64 nativeImeAdapterAndroid,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 action,
    /* [in] */ Int32 modifiers,
    /* [in] */ Int64 timestampMs,
    /* [in] */ Int32 keyCode,
    /* [in] */ Boolean isSystemKey,
    /* [in] */ Int32 unicodeChar)
{
    return Elastos_ImeAdapter_nativeSendKeyEvent(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid,
            TO_IINTERFACE(event), action, modifiers, timestampMs, keyCode, isSystemKey, unicodeChar);
}

void ImeAdapter::NativeAppendUnderlineSpan(
    /* [in] */ Int64 underlinePtr,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    Elastos_ImeAdapter_nativeAppendUnderlineSpan(underlinePtr, start, end);
}

void ImeAdapter::NativeAppendBackgroundColorSpan(
    /* [in] */ Int64 underlinePtr,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 backgroundColor)
{
    Elastos_ImeAdapter_nativeAppendBackgroundColorSpan(underlinePtr, start, end, backgroundColor);
}

void ImeAdapter::NativeSetComposingText(
    /* [in] */ Int64 nativeImeAdapterAndroid,
    /* [in] */ ICharSequence* text,
    /* [in] */ const String& textStr,
    /* [in] */ Int32 newCursorPosition)
{
    Elastos_ImeAdapter_nativeSetComposingText(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid,
            TO_IINTERFACE(text), textStr, newCursorPosition);
}

void ImeAdapter::NativeCommitText(
    /* [in] */ Int64 nativeImeAdapterAndroid,
    /* [in] */ const String& textStr)
{
    Elastos_ImeAdapter_nativeCommitText(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid, textStr);
}

void ImeAdapter::NativeFinishComposingText(
    /* [in] */ Int64 nativeImeAdapterAndroid)
{
    Elastos_ImeAdapter_nativeFinishComposingText(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid);
}

void ImeAdapter::NativeAttachImeAdapter(
    /* [in] */ Int64 nativeImeAdapterAndroid)
{
    Elastos_ImeAdapter_nativeAttachImeAdapter(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid);
}

void ImeAdapter::NativeSetEditableSelectionOffsets(
    /* [in] */ Int64 nativeImeAdapterAndroid,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    Elastos_ImeAdapter_nativeSetEditableSelectionOffsets(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid,
            start, end);
}

void ImeAdapter::NativeSetComposingRegion(
    /* [in] */ Int64 nativeImeAdapterAndroid,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    Elastos_ImeAdapter_nativeSetComposingRegion(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid, start, end);
}

void ImeAdapter::NativeDeleteSurroundingText(
    /* [in] */ Int64 nativeImeAdapterAndroid,
    /* [in] */ Int32 before,
    /* [in] */ Int32 after)
{
    Elastos_ImeAdapter_nativeDeleteSurroundingText(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid,
            before, after);
}

void ImeAdapter::NativeUnselect(
    /* [in] */ Int64 nativeImeAdapterAndroid)
{
    Elastos_ImeAdapter_nativeUnselect(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid);
}

void ImeAdapter::NativeSelectAll(
    /* [in] */ Int64 nativeImeAdapterAndroid)
{
    Elastos_ImeAdapter_nativeSelectAll(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid);
}

void ImeAdapter::NativeCut(
    /* [in] */ Int64 nativeImeAdapterAndroid)
{
    Elastos_ImeAdapter_nativeCut(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid);
}

void ImeAdapter::NativeCopy(
    /* [in] */ Int64 nativeImeAdapterAndroid)
{
    Elastos_ImeAdapter_nativeCopy(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid);
}

void ImeAdapter::NativePaste(
    /* [in] */ Int64 nativeImeAdapterAndroid)
{
    Elastos_ImeAdapter_nativePaste(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid);
}

void ImeAdapter::NativeResetImeAdapter(
    /* [in] */ Int64 nativeImeAdapterAndroid)
{
    Elastos_ImeAdapter_nativeResetImeAdapter(THIS_PROBE(IInterface), (Handle32)nativeImeAdapterAndroid);
}

void ImeAdapter::FocusedNodeChanged(
    /* [in] */ IInterface* obj,
    /* [in] */ Boolean isEditable)
{
    AutoPtr<ImeAdapter> mObj = (ImeAdapter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("ImeAdapter", "ImeAdapter::FocusedNodeChanged, mObj is NULL");
        return;
    }
    mObj->FocusedNodeChanged(isEditable);
}

void ImeAdapter::PopulateUnderlinesFromSpans(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* text,
    /* [in] */ Int64 underlines)
{
    AutoPtr<ImeAdapter> mObj = (ImeAdapter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("ImeAdapter", "ImeAdapter::PopulateUnderlinesFromSpans, mObj is NULL");
        return;
    }
    AutoPtr<ICharSequence> cs = ICharSequence::Probe(text);
    mObj->PopulateUnderlinesFromSpans(cs, underlines);
}

void ImeAdapter::CancelComposition(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ImeAdapter> mObj = (ImeAdapter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("ImeAdapter", "ImeAdapter::CancelComposition, mObj is NULL");
        return;
    }
    mObj->CancelComposition();
}

void ImeAdapter::Detach(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ImeAdapter> mObj = (ImeAdapter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("ImeAdapter", "ImeAdapter::Detach, mObj is NULL");
        return;
    }
    mObj->Detach();
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

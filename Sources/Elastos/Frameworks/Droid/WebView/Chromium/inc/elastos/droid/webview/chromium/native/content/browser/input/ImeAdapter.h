//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_IMEADAPTER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_IMEADAPTER_H__

#include "elastos/droid/ext/frameworkext.h"

#include <elastos/core/Runnable.h>
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/AdapterInputConnection.h"

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::Style::IBackgroundColorSpan;
using Elastos::Droid::Text::Style::ICharacterStyle;
using Elastos::Droid::Text::Style::IUnderlineSpan;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::InputMethod::IEditorInfo;

// import com.google.common.annotations.VisibleForTesting;

// import java.lang.CharSequence;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

/**
 * Adapts and plumbs android IME service onto the chrome text input API.
 * ImeAdapter provides an interface in both ways native <-> java:
 * 1. InputConnectionAdapter notifies native code of text composition state and
 *    dispatch key events from java -> WebKit.
 * 2. Native ImeAdapter notifies java side to clear composition text.
 *
 * The basic flow is:
 * 1. When InputConnectionAdapter gets called with composition or result text:
 *    If we receive a composition text or a result text, then we just need to
 *    dispatch a synthetic key event with special keycode 229, and then dispatch
 *    the composition or result text.
 * 2. Intercept dispatchKeyEvent() method for key events not handled by IME, we
 *   need to dispatch them to webkit and check webkit's reply. Then inject a
 *   new key event for further processing if webkit didn't handle it.
 *
 * Note that the native peer object does not take any strong reference onto the
 * instance of this java object, hence it is up to the client of this class (e.g.
 * the ViewEmbedder implementor) to hold a strong reference to it for the required
 * lifetime of the object.
 */
//@JNINamespace("content")
class ImeAdapter
    : public Object
{
public:
    /**
     * Interface for the delegate that needs to be notified of IME changes.
     */
    class ImeAdapterDelegate
        : public Object
    {
    public:
        /**
         * @param isFinish whether the event is occurring because input is finished.
         */
        virtual CARAPI_(void) OnImeEvent(
            /* [in] */ Boolean isFinish) = 0;

        /**
         * Called when a request to hide the keyboard is sent to InputMethodManager.
         */
        virtual CARAPI_(void) OnDismissInput() = 0;

        /**
         * @return View that the keyboard should be attached to.
         */
        virtual CARAPI_(AutoPtr<IView>) GetAttachedView() = 0;

        /**
         * @return Object that should be called for all keyboard show and hide requests.
         */
        virtual CARAPI_(AutoPtr<IResultReceiver>) GetNewShowKeyboardReceiver() = 0;
    };

    /**
     * Default factory for AdapterInputConnection classes.
     */
    class AdapterInputConnectionFactory
        : public Object
    {
    public:
        virtual CARAPI_(AutoPtr<AdapterInputConnection>) Get(
            /* [in] */ IView* view,
            /* [in] */ ImeAdapter* imeAdapter,
            /* [in] */ IEditable* editable,
            /* [in] */ IEditorInfo* outAttrs);
    };

private:
    class DelayedDismissInput
        : public Runnable
    {
    public:
        DelayedDismissInput(
            /* [in] */ ImeAdapter* onwer,
            /* [in] */ Handle64 nativeImeAdapter);

        //@Override
        CARAPI Run();

    private:
        ImeAdapter* mOwner;
        const Handle64 mNativeImeAdapter;
    };

public:
    /**
     * @param wrapper InputMethodManagerWrapper that should receive all the call directed to
     *                InputMethodManager.
     * @param embedder The view that is used for callbacks from ImeAdapter.
     */
    ImeAdapter(
        /* [in] */ InputMethodManagerWrapper* wrapper,
        /* [in] */ ImeAdapterDelegate* embedder);

    /**
     * Overrides the InputMethodManagerWrapper that ImeAdapter uses to make calls to
     * InputMethodManager.
     * @param immw InputMethodManagerWrapper that should be used to call InputMethodManager.
     */
    //@VisibleForTesting
    CARAPI_(void) SetInputMethodManagerWrapper(
        /* [in] */ InputMethodManagerWrapper* immw);

    /**
     * Should be only used by AdapterInputConnection.
     * @return InputMethodManagerWrapper that should receive all the calls directed to
     *         InputMethodManager.
     */
    CARAPI_(AutoPtr<InputMethodManagerWrapper>) GetInputMethodManagerWrapper();

    /**
     * Set the current active InputConnection when a new InputConnection is constructed.
     * @param inputConnection The input connection that is currently used with IME.
     */
    CARAPI_(void) SetInputConnection(
        /* [in] */ AdapterInputConnection* inputConnection);

    /**
     * Should be only used by AdapterInputConnection.
     * @return The input type of currently focused element.
     */
    CARAPI_(Int32) GetTextInputType();

    /**
     * @return Constant representing that a focused node is not an input field.
     */
    static CARAPI_(Int32) GetTextInputTypeNone();

    /**
     * Shows or hides the keyboard based on passed parameters.
     * @param nativeImeAdapter Pointer to the ImeAdapterAndroid object that is sending the update.
     * @param textInputType Text input type for the currently focused field in renderer.
     * @param showIfNeeded Whether the keyboard should be shown if it is currently hidden.
     */
    CARAPI_(void) UpdateKeyboardVisibility(
        /* [in] */ Handle64 nativeImeAdapter,
        /* [in] */ Int32 textInputType,
        /* [in] */ Boolean showIfNeeded);

    CARAPI_(void) Attach(
        /* [in] */ Handle64 nativeImeAdapter,
        /* [in] */ Int32 textInputType);

    /**
     * Attaches the imeAdapter to its native counterpart. This is needed to start forwarding
     * keyboard events to WebKit.
     * @param nativeImeAdapter The pointer to the native ImeAdapter object.
     */
    CARAPI_(void) Attach(
        /* [in] */ Handle64 nativeImeAdapter);

    CARAPI_(Boolean) HasTextInputType();

    /**
     * @return true if the selected text is of password.
     */
    CARAPI_(Boolean) IsSelectionPassword();

    CARAPI_(Boolean) DispatchKeyEvent(
        /* [in] */ IKeyEvent* event);

    CARAPI_(void) SendKeyEventWithKeyCode(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 flags);

    // Calls from Java to C++

    CARAPI_(Boolean) CheckCompositionQueueAndCallNative(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition,
        /* [in] */ Boolean isCommit);

    CARAPI_(void) FinishComposingText();

    CARAPI_(Boolean) TranslateAndSendNativeEvents(
        /* [in] */ IKeyEvent* event);

    CARAPI_(Boolean) SendSyntheticKeyEvent(
        /* [in] */ Int32 eventType,
        /* [in] */ Int64 timestampMs,
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 unicodeChar);

    /**
     * Send a request to the native counterpart to delete a given range of characters.
     * @param beforeLength Number of characters to extend the selection by before the existing
     *                     selection.
     * @param afterLength Number of characters to extend the selection by after the existing
     *                    selection.
     * @return Whether the native counterpart of ImeAdapter received the call.
     */
    CARAPI_(Boolean) DeleteSurroundingText(
        /* [in] */ Int32 beforeLength,
        /* [in] */ Int32 afterLength);

    /**
     * Send a request to the native counterpart to set the selection to given range.
     * @param start Selection start index.
     * @param end Selection end index.
     * @return Whether the native counterpart of ImeAdapter received the call.
     */
    CARAPI_(Boolean) SetEditableSelectionOffsets(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Send a request to the native counterpart to set compositing region to given indices.
     * @param start The start of the composition.
     * @param end The end of the composition.
     * @return Whether the native counterpart of ImeAdapter received the call.
     */
    CARAPI_(Boolean) SetComposingRegion(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Send a request to the native counterpart to unselect text.
     * @return Whether the native counterpart of ImeAdapter received the call.
     */
    CARAPI_(Boolean) Unselect();

    /**
     * Send a request to the native counterpart of ImeAdapter to select all the text.
     * @return Whether the native counterpart of ImeAdapter received the call.
     */
    CARAPI_(Boolean) SelectAll();

    /**
     * Send a request to the native counterpart of ImeAdapter to cut the selected text.
     * @return Whether the native counterpart of ImeAdapter received the call.
     */
    CARAPI_(Boolean) Cut();

    /**
     * Send a request to the native counterpart of ImeAdapter to copy the selected text.
     * @return Whether the native counterpart of ImeAdapter received the call.
     */
    CARAPI_(Boolean) Copy();

    /**
     * Send a request to the native counterpart of ImeAdapter to paste the text from the clipboard.
     * @return Whether the native counterpart of ImeAdapter received the call.
     */
    CARAPI_(Boolean) Paste();

    //@CalledByNative
    CARAPI_(void) Detach();

    static CARAPI_(void*) ElaImeAdapterCallback_Init();

private:

    static CARAPI_(void) FocusedNodeChanged(
        /* [in] */ IInterface* obj,
        /* [in] */ Boolean isEditable);

    static CARAPI_(void) PopulateUnderlinesFromSpans(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* text,
        /* [in] */ Int64 underlines);

    static CARAPI_(void) CancelComposition(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) Detach(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetModifiers(
        /* [in] */ Int32 metaState);

    CARAPI_(void) ShowKeyboard();

    CARAPI_(void) DismissInput(
        /* [in] */ Boolean unzoomIfNeeded);

    CARAPI_(Boolean) HasInputType();

    static CARAPI_(Boolean) IsTextInputType(
        /* [in] */ Int32 type);

    CARAPI_(Int32) ShouldSendKeyEventWithKeyCode(
        /* [in] */ const String& text);

    // Calls from C++ to Java

    //@CalledByNative
    static CARAPI_(void) InitializeWebInputEvents(
        /* [in] */ Int32 eventTypeRawKeyDown,
        /* [in] */ Int32 eventTypeKeyUp,
        /* [in] */ Int32 eventTypeChar,
        /* [in] */ Int32 modifierShift,
        /* [in] */ Int32 modifierAlt,
        /* [in] */ Int32 modifierCtrl,
        /* [in] */ Int32 modifierCapsLockOn,
        /* [in] */ Int32 modifierNumLockOn);

    //@CalledByNative
    static CARAPI_(void) InitializeTextInputTypes(
        /* [in] */ Int32 textInputTypeNone,
        /* [in] */ Int32 textInputTypeText,
        /* [in] */ Int32 textInputTypeTextArea,
        /* [in] */ Int32 textInputTypePassword,
        /* [in] */ Int32 textInputTypeSearch,
        /* [in] */ Int32 textInputTypeUrl,
        /* [in] */ Int32 textInputTypeEmail,
        /* [in] */ Int32 textInputTypeTel,
        /* [in] */ Int32 textInputTypeNumber,
        /* [in] */ Int32 textInputTypeContentEditable);

    //@CalledByNative
    CARAPI_(void) FocusedNodeChanged(
        /* [in] */ Boolean isEditable);

    //@CalledByNative
    CARAPI_(void) PopulateUnderlinesFromSpans(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int64 underlines);

    //@CalledByNative
    CARAPI_(void) CancelComposition();

    CARAPI_(Boolean) NativeSendSyntheticKeyEvent(
        /* [in] */ Handle64 nativeImeAdapterAndroid,
        /* [in] */ Int32 eventType,
        /* [in] */ Int64 timestampMs,
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 unicodeChar);

    CARAPI_(Boolean) NativeSendKeyEvent(
        /* [in] */ Handle64 nativeImeAdapterAndroid,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 action,
        /* [in] */ Int32 modifiers,
        /* [in] */ Int64 timestampMs,
        /* [in] */ Int32 keyCode,
        /* [in] */ Boolean isSystemKey,
        /* [in] */ Int32 unicodeChar);

    static CARAPI_(void) NativeAppendUnderlineSpan(
        /* [in] */ Handle64 underlinePtr,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    static CARAPI_(void) NativeAppendBackgroundColorSpan(
        /* [in] */ Handle64 underlinePtr,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 backgroundColor);

    CARAPI_(void) NativeSetComposingText(
        /* [in] */ Handle64 nativeImeAdapterAndroid,
        /* [in] */ ICharSequence* text,
        /* [in] */ const String& textStr,
        /* [in] */ Int32 newCursorPosition);

    CARAPI_(void) NativeCommitText(
        /* [in] */ Handle64 nativeImeAdapterAndroid,
        /* [in] */ const String& textStr);

    CARAPI_(void) NativeFinishComposingText(
        /* [in] */ Handle64 nativeImeAdapterAndroid);

    CARAPI_(void) NativeAttachImeAdapter(
        /* [in] */ Handle64 nativeImeAdapterAndroid);

    CARAPI_(void) NativeSetEditableSelectionOffsets(
        /* [in] */ Handle64 nativeImeAdapterAndroid,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI_(void) NativeSetComposingRegion(
        /* [in] */ Handle64 nativeImeAdapterAndroid,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI_(void) NativeDeleteSurroundingText(
        /* [in] */ Handle64 nativeImeAdapterAndroid,
        /* [in] */ Int32 before,
        /* [in] */ Int32 after);

    CARAPI_(void) NativeUnselect(
        /* [in] */ Handle64 nativeImeAdapterAndroid);

    CARAPI_(void) NativeSelectAll(
        /* [in] */ Handle64 nativeImeAdapterAndroid);

    CARAPI_(void) NativeCut(
        /* [in] */ Handle64 nativeImeAdapterAndroid);

    CARAPI_(void) NativeCopy(
        /* [in] */ Handle64 nativeImeAdapterAndroid);

    CARAPI_(void) NativePaste(
        /* [in] */ Handle64 nativeImeAdapterAndroid);

    CARAPI_(void) NativeResetImeAdapter(
        /* [in] */ Handle64 nativeImeAdapterAndroid);

public:
    // All the constants that are retrieved from the C++ code.
    // They get set through initializeWebInputEvents and initializeTextInputTypes calls.
    static Int32 sEventTypeRawKeyDown;
    static Int32 sEventTypeKeyUp;
    static Int32 sEventTypeChar;
    static Int32 sTextInputTypeNone;
    static Int32 sTextInputTypeText;
    static Int32 sTextInputTypeTextArea;
    static Int32 sTextInputTypePassword;
    static Int32 sTextInputTypeSearch;
    static Int32 sTextInputTypeUrl;
    static Int32 sTextInputTypeEmail;
    static Int32 sTextInputTypeTel;
    static Int32 sTextInputTypeNumber;
    static Int32 sTextInputTypeContentEditable;
    static Int32 sModifierShift;
    static Int32 sModifierAlt;
    static Int32 sModifierCtrl;
    static Int32 sModifierCapsLockOn;
    static Int32 sModifierNumLockOn;

    //@VisibleForTesting
    Boolean mIsShowWithoutHideOutstanding;

private:
    static const Int32 COMPOSITION_KEY_CODE = 229;

    // Delay introduced to avoid hiding the keyboard if new show requests are received.
    // The time required by the unfocus-focus events triggered by tab has been measured in soju:
    // Mean: 18.633 ms, Standard deviation: 7.9837 ms.
    // The value here should be higher enough to cover these cases, but not too high to avoid
    // letting the user perceiving important delays.
    static const Int32 INPUT_DISMISS_DELAY = 150;

    Handle64 mNativeImeAdapterAndroid;
    AutoPtr<InputMethodManagerWrapper> mInputMethodManagerWrapper;
    AutoPtr<AdapterInputConnection> mInputConnection;
    AutoPtr<ImeAdapterDelegate> mViewEmbedder;
    AutoPtr<IHandler> mHandler;
    AutoPtr<DelayedDismissInput> mDismissInput;
    Int32 mTextInputType;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_IMEADAPTER_H__

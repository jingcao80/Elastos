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

#ifndef __ELASTOS_DROID_INTERNAL_VIEW_INPUTCONNECTIONWRAPPER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_INPUTCONNECTIONWRAPPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::InputMethod::IExtractedTextRequest;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::View::InputMethod::ICorrectionInfo;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

class InputConnectionWrapper
    : public Object
    , public IInputConnection
{
public:
    class InputContextCallback
        : public Object
        , public IIInputContextCallback
        , public IBinder
    {
        friend class InputConnectionWrapper;
    public:
        /**
         * Returns an InputContextCallback object that is guaranteed not to be in use by
         * any other thread.  The returned object's 'have value' flag is cleared and its expected
         * sequence number is set to a new integer.  We use a sequence number so that replies that
         * occur after a timeout has expired are not interpreted as replies to a later request.
         */
        static CARAPI_(AutoPtr<InputContextCallback>) GetInstance();

        InputContextCallback();

        CAR_INTERFACE_DECL()

        CARAPI constructor();

        CARAPI SetTextBeforeCursor(
            /* [in] */ ICharSequence* textBeforeCursor,
            /* [in] */ Int32 seq);

        CARAPI SetTextAfterCursor(
            /* [in] */ ICharSequence* textAfterCursor,
            /* [in] */ Int32 seq);

        CARAPI SetSelectedText(
            /* [in] */ ICharSequence* selectedText,
            /* [in] */ Int32 seq);

        CARAPI SetCursorCapsMode(
            /* [in] */ Int32 capsMode,
            /* [in] */ Int32 seq);

        CARAPI SetExtractedText(
            /* [in] */ IExtractedText* extractedText,
            /* [in] */ Int32 seq);

        CARAPI SetRequestUpdateCursorAnchorInfoResult(
            /* [in] */ Boolean result,
            /* [in] */ Int32 seq);

        CARAPI ToString(
            /* [out] */ String* str);

        /**
         * Waits for a result for up to {@link #MAX_WAIT_TIME_MILLIS} milliseconds.
         *
         * <p>The caller must be synchronized on this callback object.
         */
        CARAPI_(void) WaitForResultLocked();

    private:
        /**
         * Makes the given InputContextCallback available for use in the future.
         */
        CARAPI_(void) Dispose();

    public:
        Int32 mSeq;
        Boolean mHaveValue;
        AutoPtr<ICharSequence> mTextBeforeCursor;
        AutoPtr<ICharSequence> mTextAfterCursor;
        AutoPtr<ICharSequence> mSelectedText;
        AutoPtr<IExtractedText> mExtractedText;
        Int32 mCursorCapsMode;
        Boolean mRequestUpdateCursorAnchorInfoResult;

    private:
        static const String TAG;
        // A 'pool' of one InputContextCallback.  Each ICW request will attempt to gain
        // exclusive access to this object.
        static AutoPtr<InputContextCallback> sInstance;
        static Int32 sSequenceNumber;
        static Object sLock; // for InputContextCallback class
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IIInputContext* inputContext);

    CARAPI GetTextAfterCursor(
        /* [in] */ Int32 n,
        /* [in] */ Int32 flags,
        /* [out] */ ICharSequence** text);

    CARAPI GetTextBeforeCursor(
        /* [in] */ Int32 n,
        /* [in] */ Int32 flags,
        /* [out] */ ICharSequence** text);

    CARAPI GetSelectedText(
        /* [in] */ Int32 flags,
        /* [out] */ ICharSequence** text);

    CARAPI GetCursorCapsMode(
        /* [in] */ Int32 reqModes,
        /* [out] */ Int32* capsMode);

    CARAPI GetExtractedText(
        /* [in] */ IExtractedTextRequest* request,
        /* [in] */ Int32 flags,
        /* [out] */ IExtractedText** extractedText);

    CARAPI CommitText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition,
        /* [out] */ Boolean* flag);

    CARAPI CommitCompletion(
        /* [in] */ ICompletionInfo* text,
        /* [out] */ Boolean* flag);

    CARAPI CommitCorrection(
        /* [in] */ ICorrectionInfo* correctionInfo,
        /* [out] */ Boolean* flag);

    CARAPI SetSelection(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ Boolean* flag);

    CARAPI PerformEditorAction(
        /* [in] */ Int32 editorAction,
        /* [out] */ Boolean* flag);

    CARAPI PerformContextMenuAction(
        /* [in] */ Int32 id,
        /* [out] */ Boolean* flag);

    CARAPI SetComposingRegion(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ Boolean* flag);

    CARAPI SetComposingText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition,
        /* [out] */ Boolean* flag);

    CARAPI FinishComposingText(
        /* [out] */ Boolean* flag);

    CARAPI BeginBatchEdit(
        /* [out] */ Boolean* flag);

    CARAPI EndBatchEdit(
        /* [out] */ Boolean* flag);

    CARAPI SendKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* flag);

    CARAPI ClearMetaKeyStates(
        /* [in] */ Int32 states,
        /* [out] */ Boolean* flag);

    CARAPI DeleteSurroundingText(
        /* [in] */ Int32 leftLength,
        /* [in] */ Int32 rightLength,
        /* [out] */ Boolean* flag);

    CARAPI ReportFullscreenMode(
        /* [in] */ Boolean enabled,
        /* [out] */ Boolean* flag);

    CARAPI PerformPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data,
        /* [out] */ Boolean* flag);

    CARAPI RequestCursorUpdates(
        /* [in] */ Int32 cursorUpdateMode,
        /* [out] */ Boolean* result);

private:
    static const Int32 MAX_WAIT_TIME_MILLIS;
    AutoPtr<IIInputContext> mIInputContext;
};

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_INPUTCONNECTIONWRAPPER_H__

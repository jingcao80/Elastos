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

#ifndef __ELASTOS_DROID_VIEW_IINPUTCONNECTIONWRQPPER_H__
#define __ELASTOS_DROID_VIEW_IINPUTCONNECTIONWRQPPER_H__

//#include "elastos/droid/os/HandlerBase.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::InputMethod::ICompletionInfo;
using Elastos::Droid::View::InputMethod::ICorrectionInfo;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::View::InputMethod::IExtractedTextRequest;
using Elastos::Droid::Internal::View::IIInputContextCallback;

namespace Elastos {
namespace Droid {
namespace View {

extern "C" const InterfaceID EIID_IInputConnectionWrapper;

class IInputConnectionWrapper
{
private:
    class SomeArgs
        : public ElLightRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()

        SomeArgs(
            /* [in] */ IInterface* arg1 = NULL,
            /* [in] */ IInterface* arg2 = NULL,
            /* [in] */ IIInputContextCallback* clback = NULL,
            /* [in] */ Int32 seq = 0);

    public:
        AutoPtr<IInterface> mArg1;
        AutoPtr<IInterface> mArg2;
        AutoPtr<IIInputContextCallback> mCallback;
        Int32 mSeq;
    };

    class MyHandler : public HandlerBase
    {
    public:
        TO_STRING_IMPL("IInputConnectionWrapper::MyHandler")

        MyHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ IInputConnectionWrapper* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        // IInputConnectionWrapper* mHost;
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    IInputConnectionWrapper();

    virtual ~IInputConnectionWrapper() {}

    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid) = 0;

    CARAPI Init(
        /* [in] */ ILooper* mainLooper,
        /* [in] */ IInputConnection* conn);

    CARAPI_(Boolean) IsActive();

    CARAPI GetTextAfterCursor(
        /* [in] */ Int32 length,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* callback);

    CARAPI GetTextBeforeCursor(
        /* [in] */ Int32 length,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* callback);

    CARAPI GetSelectedText(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* callback);

    CARAPI GetCursorCapsMode(
        /* [in] */ Int32 reqModes,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* callback);

    CARAPI GetExtractedText(
        /* [in] */ IExtractedTextRequest* request,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* callback);

    CARAPI CommitText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition);

    CARAPI CommitCompletion(
        /* [in] */ ICompletionInfo* text);

    CARAPI CommitCorrection(
        /* [in] */ ICorrectionInfo* info);

    CARAPI SetSelection(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI PerformEditorAction(
        /* [in] */ Int32 id);

    CARAPI PerformContextMenuAction(
        /* [in] */ Int32 id);

    CARAPI SetComposingRegion(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI SetComposingText(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 newCursorPosition);

    CARAPI FinishComposingText();

    CARAPI SendKeyEvent(
        /* [in] */ IKeyEvent* event);

    CARAPI ClearMetaKeyStates(
        /* [in] */ Int32 states);

    CARAPI DeleteSurroundingText(
        /* [in] */ Int32 leftLength,
        /* [in] */ Int32 rightLength);

    CARAPI BeginBatchEdit();

    CARAPI EndBatchEdit();

    CARAPI ReportFullscreenMode(
        /* [in] */ Boolean enabled);

    CARAPI PerformPrivateCommand(
        /* [in] */ const String& action,
        /* [in] */ IBundle* data);

    CARAPI GetDescription(
        /* [out] */ String* str);

private:
    CARAPI DispatchMessage(
        /* [in] */ IMessage* msg);

    CARAPI ExecuteMessage(
        /* [in] */ IMessage* msg);

    CARAPI_(AutoPtr<IMessage>) ObtainMessage(
        /* [in] */ Int32 what);

    CARAPI_(AutoPtr<IMessage>) ObtainMessage(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

    CARAPI_(AutoPtr<IMessage>) ObtainMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* arg1);

    CARAPI_(AutoPtr<IMessage>) ObtainMessageSC(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* callback);

    CARAPI_(AutoPtr<IMessage>) ObtainMessageSC(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* callback);

    CARAPI_(AutoPtr<IMessage>) ObtainMessageSC(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ IInterface* arg2,
        /* [in] */ Int32 seq,
        /* [in] */ IIInputContextCallback* callback);

    CARAPI_(AutoPtr<IMessage>) ObtainMessage(
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ IInterface* arg2);

    CARAPI_(AutoPtr<IMessage>) ObtainMessage(
        /* [in] */ Int32 what,
        /* [in] */ const String& arg1,
        /* [in] */ IInterface* arg2);

private:
    static String TAG;

    static const Int32 DO_GET_TEXT_AFTER_CURSOR;
    static const Int32 DO_GET_TEXT_BEFORE_CURSOR;
    static const Int32 DO_GET_SELECTED_TEXT;
    static const Int32 DO_GET_CURSOR_CAPS_MODE;
    static const Int32 DO_GET_EXTRACTED_TEXT;
    static const Int32 DO_COMMIT_TEXT;
    static const Int32 DO_COMMIT_COMPLETION;
    static const Int32 DO_COMMIT_CORRECTION;
    static const Int32 DO_SET_SELECTION;
    static const Int32 DO_PERFORM_EDITOR_ACTION;
    static const Int32 DO_PERFORM_CONTEXT_MENU_ACTION;
    static const Int32 DO_SET_COMPOSING_TEXT;
    static const Int32 DO_SET_COMPOSING_REGION;
    static const Int32 DO_FINISH_COMPOSING_TEXT;
    static const Int32 DO_SEND_KEY_EVENT;
    static const Int32 DO_DELETE_SURROUNDING_TEXT;
    static const Int32 DO_BEGIN_BATCH_EDIT;
    static const Int32 DO_END_BATCH_EDIT;
    static const Int32 DO_REPORT_FULLSCREEN_MODE;
    static const Int32 DO_PERFORM_PRIVATE_COMMAND;
    static const Int32 DO_CLEAR_META_KEY_STATES;

    AutoPtr<IWeakReference> mInputConnection;

    AutoPtr<ILooper> mMainLooper;
    AutoPtr<IHandler> mH;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_IINPUTCONNECTIONWRQPPER_H__

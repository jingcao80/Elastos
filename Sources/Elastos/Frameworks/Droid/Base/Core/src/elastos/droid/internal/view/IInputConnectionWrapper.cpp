
#include "Elastos.Droid.View.h"
#include "elastos/droid/internal/view/IInputConnectionWrapper.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/os/Looper.h"

using Elastos::Utility::Logging::Logger;
using Elastos::Core::CString;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::View::InputMethod::IExtractedText;
using Elastos::Droid::View::InputMethod::ICorrectionInfo;
using Elastos::Droid::View::InputMethod::EIID_IInputConnection;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

String IInputConnectionWrapper::TAG("IInputConnectionWrapper");

const Int32 IInputConnectionWrapper::DO_GET_TEXT_AFTER_CURSOR = 10;
const Int32 IInputConnectionWrapper::DO_GET_TEXT_BEFORE_CURSOR = 20;
const Int32 IInputConnectionWrapper::DO_GET_SELECTED_TEXT = 25;
const Int32 IInputConnectionWrapper::DO_GET_CURSOR_CAPS_MODE = 30;
const Int32 IInputConnectionWrapper::DO_GET_EXTRACTED_TEXT = 40;
const Int32 IInputConnectionWrapper::DO_COMMIT_TEXT = 50;
const Int32 IInputConnectionWrapper::DO_COMMIT_COMPLETION = 55;
const Int32 IInputConnectionWrapper::DO_COMMIT_CORRECTION = 56;
const Int32 IInputConnectionWrapper::DO_SET_SELECTION = 57;
const Int32 IInputConnectionWrapper::DO_PERFORM_EDITOR_ACTION = 58;
const Int32 IInputConnectionWrapper::DO_PERFORM_CONTEXT_MENU_ACTION = 59;
const Int32 IInputConnectionWrapper::DO_SET_COMPOSING_TEXT = 60;
const Int32 IInputConnectionWrapper::DO_SET_COMPOSING_REGION = 63;
const Int32 IInputConnectionWrapper::DO_FINISH_COMPOSING_TEXT = 65;
const Int32 IInputConnectionWrapper::DO_SEND_KEY_EVENT = 70;
const Int32 IInputConnectionWrapper::DO_DELETE_SURROUNDING_TEXT = 80;
const Int32 IInputConnectionWrapper::DO_BEGIN_BATCH_EDIT = 90;
const Int32 IInputConnectionWrapper::DO_END_BATCH_EDIT = 95;
const Int32 IInputConnectionWrapper::DO_REPORT_FULLSCREEN_MODE = 100;
const Int32 IInputConnectionWrapper::DO_PERFORM_PRIVATE_COMMAND = 120;
const Int32 IInputConnectionWrapper::DO_CLEAR_META_KEY_STATES = 130;
const Int32 IInputConnectionWrapper::DO_REQUEST_UPDATE_CURSOR_ANCHOR_INFO = 140;

//===================================================================
// IInputConnectionWrapper::SomeArgs
//===================================================================
IInputConnectionWrapper::SomeArgs::SomeArgs(
    /* [in] */ IInterface* arg1,
    /* [in] */ IInterface* arg2,
    /* [in] */ IIInputContextCallback* clback,
    /* [in] */ Int32 seq)
    : mArg1(arg1)
    , mArg2(arg2)
    , mCallback(clback)
    , mSeq(seq)
{}

//===================================================================
// IInputConnectionWrapper::MyHandler
//===================================================================
IInputConnectionWrapper::MyHandler::MyHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ IInputConnectionWrapper* host)
    : Handler(looper)
{
    AutoPtr<IWeakReferenceSource> wrs = (IWeakReferenceSource*)host->Probe(EIID_IWeakReferenceSource);
    if (wrs != NULL) {
        wrs->GetWeakReference((IWeakReference**)&mWeakHost);
    }
    else {
        Logger::E(IInputConnectionWrapper::TAG,
            "IInputConnectionWrapper's subclass does not implement IWeakReferenceSource.");
        assert(0);
    }
}

ECode IInputConnectionWrapper::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IIInputContext> obj;
    mWeakHost->Resolve(EIID_IIInputContext, (IInterface**)&obj);
    if (obj != NULL) {
        IInputConnectionWrapper* host = (IInputConnectionWrapper*)obj.Get();
        return host->ExecuteMessage(msg);
    }
    return NOERROR;
}

//===================================================================
// IInputConnectionWrapper
//===================================================================

CAR_INTERFACE_IMPL_2(IInputConnectionWrapper, Object, IIInputContext, IBinder)

IInputConnectionWrapper::IInputConnectionWrapper()
{
}

ECode IInputConnectionWrapper::constructor(
    /* [in] */ ILooper* mainLooper,
    /* [in] */ IInputConnection* conn)
{
    IWeakReferenceSource::Probe(conn)->GetWeakReference((IWeakReference**)&mInputConnection);
    mMainLooper = mainLooper;
    mH = new MyHandler(mMainLooper, this);
    return NOERROR;
}

ECode IInputConnectionWrapper::IsActive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

Boolean IInputConnectionWrapper::IsActive()
{
    Boolean bval;
    IsActive(&bval);
    return bval;
}

ECode IInputConnectionWrapper::GetTextAfterCursor(
    /* [in] */ Int32 length,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    return DispatchMessage(ObtainMessageSC(DO_GET_TEXT_AFTER_CURSOR, length, flags, seq, callback));
}

ECode IInputConnectionWrapper::GetTextBeforeCursor(
    /* [in] */ Int32 length,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    return DispatchMessage(ObtainMessageSC(DO_GET_TEXT_BEFORE_CURSOR, length, flags, seq, callback));
}

ECode IInputConnectionWrapper::GetSelectedText(
    /* [in] */ Int32 flags,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    return DispatchMessage(ObtainMessageSC(DO_GET_SELECTED_TEXT, flags, seq, callback));
}

ECode IInputConnectionWrapper::GetCursorCapsMode(
    /* [in] */ Int32 reqModes,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    return DispatchMessage(ObtainMessageSC(DO_GET_CURSOR_CAPS_MODE, reqModes, seq, callback));
}

ECode IInputConnectionWrapper::GetExtractedText(
    /* [in] */ IExtractedTextRequest* request,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    return DispatchMessage(ObtainMessageSC(DO_GET_EXTRACTED_TEXT, flags,
            request, seq, callback));
}

ECode IInputConnectionWrapper::CommitText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition)
{
    return DispatchMessage(ObtainMessage(DO_COMMIT_TEXT, newCursorPosition, text));
}

ECode IInputConnectionWrapper::CommitCompletion(
    /* [in] */ ICompletionInfo* text)
{
    return DispatchMessage(ObtainMessage(DO_COMMIT_COMPLETION, text));
}

ECode IInputConnectionWrapper::CommitCorrection(
    /* [in] */ ICorrectionInfo* info)
{
    return DispatchMessage(ObtainMessage(DO_COMMIT_CORRECTION, info));
}

ECode IInputConnectionWrapper::SetSelection(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    return DispatchMessage(ObtainMessage(DO_SET_SELECTION, start, end));
}

ECode IInputConnectionWrapper::PerformEditorAction(
    /* [in] */ Int32 id)
{
    return DispatchMessage(ObtainMessage(DO_PERFORM_EDITOR_ACTION, id, 0));
}

ECode IInputConnectionWrapper::PerformContextMenuAction(
    /* [in] */ Int32 id)
{
    return DispatchMessage(ObtainMessage(DO_PERFORM_CONTEXT_MENU_ACTION, id, 0));
}

ECode IInputConnectionWrapper::SetComposingRegion(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    return DispatchMessage(ObtainMessage(DO_SET_COMPOSING_REGION, start, end));
}

ECode IInputConnectionWrapper::SetComposingText(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 newCursorPosition)
{
    return DispatchMessage(ObtainMessage(DO_SET_COMPOSING_TEXT, newCursorPosition, text));
}

ECode IInputConnectionWrapper::FinishComposingText()
{
    return DispatchMessage(ObtainMessage(DO_FINISH_COMPOSING_TEXT));
}

ECode IInputConnectionWrapper::SendKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    return DispatchMessage(ObtainMessage(DO_SEND_KEY_EVENT, event));
}

ECode IInputConnectionWrapper::ClearMetaKeyStates(
    /* [in] */ Int32 states)
{
    return DispatchMessage(ObtainMessage(DO_CLEAR_META_KEY_STATES, states, 0));
}

ECode IInputConnectionWrapper::DeleteSurroundingText(
    /* [in] */ Int32 leftLength,
    /* [in] */ Int32 rightLength)
{
    return DispatchMessage(ObtainMessage(DO_DELETE_SURROUNDING_TEXT,
        leftLength, rightLength));
}

ECode IInputConnectionWrapper::BeginBatchEdit()
{
    return DispatchMessage(ObtainMessage(DO_BEGIN_BATCH_EDIT));
}

ECode IInputConnectionWrapper::EndBatchEdit()
{
    return DispatchMessage(ObtainMessage(DO_END_BATCH_EDIT));
}

ECode IInputConnectionWrapper::ReportFullscreenMode(
    /* [in] */ Boolean enabled)
{
    return DispatchMessage(ObtainMessage(DO_REPORT_FULLSCREEN_MODE, enabled ? 1 : 0, 0));
}

ECode IInputConnectionWrapper::PerformPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    return DispatchMessage(ObtainMessage(DO_PERFORM_PRIVATE_COMMAND, action, data));
}

ECode IInputConnectionWrapper::RequestUpdateCursorAnchorInfo(
    /* [in] */ Int32 cursorUpdateMode,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    return DispatchMessage(ObtainMessageSC(DO_REQUEST_UPDATE_CURSOR_ANCHOR_INFO,
        cursorUpdateMode, seq, callback));
}

ECode IInputConnectionWrapper::DispatchMessage(
    /* [in] */ IMessage* msg)
{
    // If we are calling this from the main thread, then we can call
    // right through.  Otherwise, we need to send the message to the
    // main thread.
    if (Looper::GetMyLooper() == mMainLooper) {
        ExecuteMessage(msg);
        msg->Recycle();
        return NOERROR;
    }

    Boolean result;
    return mH->SendMessage(msg, &result);
}

ECode IInputConnectionWrapper::ExecuteMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case DO_GET_TEXT_AFTER_CURSOR: {
            const SomeArgs* args = (SomeArgs*)IObject::Probe(obj);
            // try {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "getTextAfterCursor on inactive InputConnection");
                return args->mCallback->SetTextAfterCursor(NULL, args->mSeq);
            }
            AutoPtr<ICharSequence> text;
            ic->GetTextAfterCursor(arg1, arg2, (ICharSequence**)&text);
            return args->mCallback->SetTextAfterCursor(text, args->mSeq);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "Got RemoteException calling setTextAfterCursor", e);
            // }
        }
        case DO_GET_TEXT_BEFORE_CURSOR: {
            const SomeArgs* args = (SomeArgs*)IObject::Probe(obj);
            // try {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "getTextBeforeCursor on inactive InputConnection");
                return args->mCallback->SetTextBeforeCursor(NULL, args->mSeq);
            }
            AutoPtr<ICharSequence> text;
            ic->GetTextBeforeCursor(arg1, arg2, (ICharSequence**)&text);
            return args->mCallback->SetTextBeforeCursor(text, args->mSeq);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "Got RemoteException calling setTextAfterCursor", e);
            // }
        }
        case DO_GET_SELECTED_TEXT: {
            const SomeArgs* args = (SomeArgs*)IObject::Probe(obj);
            // try {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "getSelectedText on inactive InputConnection");
                return args->mCallback->SetSelectedText(NULL, args->mSeq);
            }
            AutoPtr<ICharSequence> text;
            ic->GetSelectedText(arg1, (ICharSequence**)&text);
            return args->mCallback->SetSelectedText(text, args->mSeq);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "Got RemoteException calling setTextAfterCursor", e);
            // }
        }
        case DO_GET_CURSOR_CAPS_MODE: {
            const SomeArgs* args = (SomeArgs*)IObject::Probe(obj);
            // try {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "getCursorCapsMode on inactive InputConnection");
                return args->mCallback->SetCursorCapsMode(0, args->mSeq);
            }
            Int32 capsMode = 0;
            ic->GetCursorCapsMode(arg1, &capsMode);
            return args->mCallback->SetCursorCapsMode(capsMode, args->mSeq);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "Got RemoteException calling setTextAfterCursor", e);
            // }
        }
        case DO_GET_EXTRACTED_TEXT: {
            const SomeArgs* args = (SomeArgs*)IObject::Probe(obj);
            // try {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "getExtractedText on inactive InputConnection");
                return args->mCallback->SetExtractedText(NULL, args->mSeq);
            }
            AutoPtr<IExtractedText> extText;
            ic->GetExtractedText(IExtractedTextRequest::Probe(args->mArg1),
                    arg1, (IExtractedText**)&extText);
            return args->mCallback->SetExtractedText(extText, args->mSeq);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "Got RemoteException calling setTextAfterCursor", e);
            // }
        }
        case DO_COMMIT_TEXT: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "commitText on inactive InputConnection");
                return NOERROR;
            }
            Boolean ret = FALSE;
            return ic->CommitText(ICharSequence::Probe(obj), arg1, &ret);
        }
        case DO_SET_SELECTION: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "setSelection on inactive InputConnection");
                return NOERROR;
            }
            Boolean ret = FALSE;
            return ic->SetSelection(arg1, arg2, &ret);
        }
        case DO_PERFORM_EDITOR_ACTION: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "performEditorAction on inactive InputConnection");
                return NOERROR;
            }
            Boolean ret = FALSE;
            return ic->PerformEditorAction(arg1, &ret);
        }
        case DO_PERFORM_CONTEXT_MENU_ACTION: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "performContextMenuAction on inactive InputConnection");
                return NOERROR;
            }
            Boolean ret = FALSE;
            return ic->PerformContextMenuAction(arg1, &ret);
        }
        case DO_COMMIT_COMPLETION: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "commitCompletion on inactive InputConnection");
                return NOERROR;
            }

            Boolean ret = FALSE;
            return ic->CommitCompletion(ICompletionInfo::Probe(obj), &ret);
        }
        case DO_COMMIT_CORRECTION: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "commitCorrection on inactive InputConnection");
                return NOERROR;
            }

            Boolean ret = FALSE;
            return ic->CommitCorrection(ICorrectionInfo::Probe(obj), &ret);
        }
        case DO_SET_COMPOSING_TEXT: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "setComposingText on inactive InputConnection");
                return NOERROR;
            }
            Boolean ret = FALSE;
            return ic->SetComposingText(ICharSequence::Probe(obj), arg1, &ret);
        }
        case DO_SET_COMPOSING_REGION: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "setComposingRegion on inactive InputConnection");
                return NOERROR;
            }
            Boolean ret = FALSE;
            return ic->SetComposingRegion(arg1, arg2, &ret);
        }
        case DO_FINISH_COMPOSING_TEXT: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            // Note we do NOT check IsActive() here, because this is safe
            // for an IME to call at any time, and we need to allow it
            // through to clean up our state after the IME has switched to
            // another client.
            if (ic == NULL) {
                Logger::W(TAG, "finishComposingText on inactive InputConnection");
                return NOERROR;;
            }
            Boolean ret = FALSE;
            return ic->FinishComposingText(&ret);
        }
        case DO_SEND_KEY_EVENT: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "sendKeyEvent on inactive InputConnection");
                return NOERROR;
            }
            Boolean ret = FALSE;
            return ic->SendKeyEvent(IKeyEvent::Probe(obj), &ret);
        }
        case DO_CLEAR_META_KEY_STATES: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "clearMetaKeyStates on inactive InputConnection");
                return NOERROR;;
            }
            Boolean ret = FALSE;
            return ic->ClearMetaKeyStates(arg1, &ret);
        }
        case DO_DELETE_SURROUNDING_TEXT: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "deleteSurroundingText on inactive InputConnection");
                return NOERROR;
            }
            Boolean ret = FALSE;
            return ic->DeleteSurroundingText(arg1, arg2, &ret);
        }
        case DO_BEGIN_BATCH_EDIT: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "beginBatchEdit on inactive InputConnection");
                return NOERROR;
            }
            Boolean ret = FALSE;
            return ic->BeginBatchEdit(&ret);
        }
        case DO_END_BATCH_EDIT: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "endBatchEdit on inactive InputConnection");
                return NOERROR;
            }
            Boolean ret = FALSE;
            return ic->EndBatchEdit(&ret);
        }
        case DO_REPORT_FULLSCREEN_MODE: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "showStatusIcon on inactive InputConnection");
                return NOERROR;
            }
            Boolean ret = FALSE;
            return ic->ReportFullscreenMode(arg1 == 1, &ret);
        }
        case DO_PERFORM_PRIVATE_COMMAND: {
            AutoPtr<IInterface> base;
            FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
            AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
            if (ic == NULL || !IsActive()) {
                Logger::W(TAG, "performPrivateCommand on inactive InputConnection");
                return NOERROR;
            }
            const SomeArgs* args = (SomeArgs*)IObject::Probe(obj);
            String text;
            ICharSequence::Probe(args->mArg1)->ToString(&text);
            Boolean ret = FALSE;
            return ic->PerformPrivateCommand(text, IBundle::Probe(args->mArg2), &ret);
        }
        case DO_REQUEST_UPDATE_CURSOR_ANCHOR_INFO: {
            const SomeArgs* args = (SomeArgs*)IObject::Probe(obj);
            // try {
                AutoPtr<IInterface> base;
                FAIL_RETURN(mInputConnection->Resolve(EIID_IInterface, (IInterface**)&base))
                AutoPtr<IInputConnection> ic = IInputConnection::Probe(base);
                if (ic == NULL || !IsActive()) {
                    Logger::W(TAG, "requestCursorAnchorInfo on inactive InputConnection");
                    args->mCallback->SetRequestUpdateCursorAnchorInfoResult(FALSE, args->mSeq);
                    return NOERROR;
                }
                Boolean result;
                ic->RequestCursorUpdates(arg1, &result);
                args->mCallback->SetRequestUpdateCursorAnchorInfoResult(result, args->mSeq);
            // } catch (RemoteException e) {
            //     Log.w(TAG, "Got RemoteException calling requestCursorAnchorInfo", e);
            // }
            return NOERROR;
        }
    }

    Logger::W(TAG, "Unhandled message code: %d", what);
    return NOERROR;
}

AutoPtr<IMessage> IInputConnectionWrapper::ObtainMessage(
    /* [in] */ Int32 what)
{
    AutoPtr<IMessage> msg;
    mH->ObtainMessage(what, (IMessage**)&msg);
    return msg;
}

AutoPtr<IMessage> IInputConnectionWrapper::ObtainMessage(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2)
{
    AutoPtr<IMessage> msg;
    mH->ObtainMessage(what, arg1, arg2, (IMessage**)&msg);
    return msg;
}

AutoPtr<IMessage> IInputConnectionWrapper::ObtainMessage(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* arg1)
{
    AutoPtr<IMessage> msg;
    mH->ObtainMessage(what, arg1, (IMessage**)&msg);
    return msg;
}

AutoPtr<IMessage> IInputConnectionWrapper::ObtainMessageSC(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    AutoPtr<SomeArgs> args = new SomeArgs(NULL, NULL, callback, seq);

    AutoPtr<IMessage> msg;
    mH->ObtainMessage(what, arg1, 0, (IObject*)args, (IMessage**)&msg);
    return msg;
}

AutoPtr<IMessage> IInputConnectionWrapper::ObtainMessageSC(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    AutoPtr<SomeArgs> args = new SomeArgs(NULL, NULL, callback, seq);
    AutoPtr<IMessage> msg;
    mH->ObtainMessage(what, arg1, arg2, (IObject*)args, (IMessage**)&msg);
    return msg;
}

AutoPtr<IMessage> IInputConnectionWrapper::ObtainMessageSC(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* arg1,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    AutoPtr<SomeArgs> args = new SomeArgs(arg1, NULL, callback, seq);
    AutoPtr<IMessage> msg;
    mH->ObtainMessage(what, 0, 0, (IObject*)args, (IMessage**)&msg);
    return msg;
}

AutoPtr<IMessage> IInputConnectionWrapper::ObtainMessageSC(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ IInterface* arg2,
    /* [in] */ Int32 seq,
    /* [in] */ IIInputContextCallback* callback)
{
    AutoPtr<SomeArgs> args = new SomeArgs(arg2, NULL, callback, seq);
    AutoPtr<IMessage> msg;
    mH->ObtainMessage(what, arg1, 0, (IObject*)args, (IMessage**)&msg);
    return msg;
}

AutoPtr<IMessage> IInputConnectionWrapper::ObtainMessage(
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1,
    /* [in] */ IInterface* arg2)
{
    AutoPtr<IMessage> msg;
    mH->ObtainMessage(what, arg1, 0, arg2, (IMessage**)&msg);
    return msg;
}

AutoPtr<IMessage> IInputConnectionWrapper::ObtainMessage(
    /* [in] */ Int32 what,
    /* [in] */ const String& arg1,
    /* [in] */ IInterface* arg2)
{
    AutoPtr<ICharSequence> text;
    CString::New(arg1, (ICharSequence**)&text);

    AutoPtr<SomeArgs> args = new SomeArgs(text, arg2);
    AutoPtr<IMessage> msg;
    mH->ObtainMessage(what, 0, 0, (IObject*)args, (IMessage**)&msg);
    return msg;
}

ECode IInputConnectionWrapper::ToString(
    /* [out] */ String* info)
{
    return Object::ToString(info);
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

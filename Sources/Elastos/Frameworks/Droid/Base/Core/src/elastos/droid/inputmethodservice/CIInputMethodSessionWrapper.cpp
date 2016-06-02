
#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/inputmethodservice/CIInputMethodSessionWrapper.h"
#include "elastos/droid/internal/os/CHandlerCaller.h"
#include "elastos/droid/internal/os/SomeArgs.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::Internal::Os::CHandlerCaller;
using Elastos::Droid::Internal::Os::EIID_IHandlerCallerCallback;
using Elastos::Droid::Internal::View::EIID_IIInputMethodSession;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::InputMethod::EIID_ILocalInputMethodSessionEventCallback;
using Elastos::Droid::View::InputMethod::IInputMethodSession;
using Elastos::Droid::View::InputMethod::EIID_ICompletionInfo;
using Elastos::Core::CString;
using Elastos::Core::CArrayOf;
using Elastos::Core::IArrayOf;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

CAR_INTERFACE_IMPL(CIInputMethodSessionWrapper::ImeInputEventReceiver, \
    InputEventReceiver, ILocalInputMethodSessionEventCallback)

CIInputMethodSessionWrapper::ImeInputEventReceiver::ImeInputEventReceiver()
{
    CSparseArray::New((ISparseArray**)&mPendingEvents);
}


ECode CIInputMethodSessionWrapper::ImeInputEventReceiver::constructor(
    /* [in] */ IInputChannel* inputChannel,
    /* [in] */ ILooper* looper,
    /* [in] */ CIInputMethodSessionWrapper* host)
{
    FAIL_RETURN(InputEventReceiver::constructor(inputChannel, looper));
    mHost = host;
    return NOERROR;
}

ECode CIInputMethodSessionWrapper::ImeInputEventReceiver::OnInputEvent(
    /* [in] */ IInputEvent* event)
{
    if (mHost->mInputMethodSession == NULL) {
        // The session has been finished.
        FinishInputEvent(event, FALSE);
        return NOERROR;
    }

    Int32 seq = 0;
    event->GetSequenceNumber(&seq);
    mPendingEvents->Put(seq, event);
    if (IKeyEvent::Probe(event)) {
        AutoPtr<IKeyEvent> keyEvent = IKeyEvent::Probe(event);
        mHost->mInputMethodSession->DispatchKeyEvent(seq, keyEvent, this);
    } else {
        AutoPtr<IMotionEvent> motionEvent = IMotionEvent::Probe(event);
        Boolean isfs = FALSE;
        if (IInputEvent::Probe(motionEvent)->IsFromSource(IInputDevice::SOURCE_CLASS_TRACKBALL, &isfs), isfs) {
            mHost->mInputMethodSession->DispatchTrackballEvent(seq, motionEvent, this);
        } else {
            mHost->mInputMethodSession->DispatchGenericMotionEvent(seq, motionEvent, this);
        }
    }
    return NOERROR;
}

ECode CIInputMethodSessionWrapper::ImeInputEventReceiver::FinishedEvent(
    /* [in] */ Int32 seq,
    /* [in] */ Boolean handled)
{
    Int32 index = 0;
    mPendingEvents->IndexOfKey(seq, &index);
    if (index >= 0) {
        AutoPtr<IInterface> event;
        mPendingEvents->ValueAt(index, (IInterface**)&event);
        mPendingEvents->RemoveAt(index);
        FinishInputEvent(IInputEvent::Probe(event), handled);
    }
    return NOERROR;
}

String CIInputMethodSessionWrapper::TAG("CIInputMethodSessionWrapper");
const Boolean CIInputMethodSessionWrapper::DEBUG = FALSE;
const Int32 CIInputMethodSessionWrapper::DO_FINISH_INPUT = 60;
const Int32 CIInputMethodSessionWrapper::DO_DISPLAY_COMPLETIONS = 65;
const Int32 CIInputMethodSessionWrapper::DO_UPDATE_EXTRACTED_TEXT = 67;
const Int32 CIInputMethodSessionWrapper::DO_UPDATE_SELECTION = 90;
const Int32 CIInputMethodSessionWrapper::DO_UPDATE_CURSOR = 95;
const Int32 CIInputMethodSessionWrapper::DO_UPDATE_CURSOR_ANCHOR_INFO = 99;
const Int32 CIInputMethodSessionWrapper::DO_APP_PRIVATE_COMMAND = 100;
const Int32 CIInputMethodSessionWrapper::DO_TOGGLE_SOFT_INPUT = 105;
const Int32 CIInputMethodSessionWrapper::DO_FINISH_SESSION = 110;
const Int32 CIInputMethodSessionWrapper::DO_VIEW_CLICKED = 115;

CAR_OBJECT_IMPL(CIInputMethodSessionWrapper);
CAR_INTERFACE_IMPL_3(CIInputMethodSessionWrapper, Object, IIInputMethodSession, IBinder, IHandlerCallerCallback);
ECode CIInputMethodSessionWrapper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IInputMethodSession* inputMethodSession,
    /* [in] */ IInputChannel* channel)
{
    CHandlerCaller::New(context, NULL, this,
        TRUE /*asyncHandler*/, FALSE, (IHandlerCaller**)&mCaller);
    mInputMethodSession = inputMethodSession;
    mChannel = channel;
    if (channel != NULL) {
        AutoPtr<ILooper> looper;
        context->GetMainLooper((ILooper**)&looper);
        mReceiver = new ImeInputEventReceiver();
        mReceiver->constructor(channel, looper, this);
    }

    return NOERROR;
}

AutoPtr<IInputMethodSession> CIInputMethodSessionWrapper::GetInternalInputMethodSession()
{
    return mInputMethodSession;
}

ECode CIInputMethodSessionWrapper::ExecuteMessage(
    /* [in] */ IMessage* msg)
{
    if (mInputMethodSession == NULL) {
        // The session has been finished. Args needs to be recycled
        // for cases below.
        Int32 what = 0;
        msg->GetWhat(&what);
        switch (what) {
            case DO_UPDATE_SELECTION:
            case DO_APP_PRIVATE_COMMAND: {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                SomeArgs* args = (SomeArgs*)IObject::Probe(obj);
                args->Recycle();
            }
        }
        return NOERROR;
    }

    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case DO_FINISH_INPUT:
            mInputMethodSession->FinishInput();
            return NOERROR;
        case DO_DISPLAY_COMPLETIONS: {
            AutoPtr<ArrayOf<ICompletionInfo*> > array;
            AutoPtr<IArrayOf> container = IArrayOf::Probe(obj);
            if (container) {
                Int32 count;
                container->GetLength(&count);
                array = ArrayOf<ICompletionInfo*>::Alloc(count);
                for (Int32 i = 0; i < count; i++) {
                    AutoPtr<IInterface> current;
                    container->Get(i, (IInterface**)&current);
                    array->Set(i++, ICompletionInfo::Probe(current));
                }
            }
            else {
                array = ArrayOf<ICompletionInfo*>::Alloc(0);
            }

            mInputMethodSession->DisplayCompletions(array);
            return NOERROR;
        }
        case DO_UPDATE_EXTRACTED_TEXT:
            mInputMethodSession->UpdateExtractedText(arg1, IExtractedText::Probe(obj));
            return NOERROR;
        case DO_UPDATE_SELECTION: {
            SomeArgs* args = (SomeArgs*)IObject::Probe(obj);
            mInputMethodSession->UpdateSelection(args->mArgi1, args->mArgi2,
                args->mArgi3, args->mArgi4, args->mArgi5, args->mArgi6);
            args->Recycle();
            return NOERROR;
        }
        case DO_UPDATE_CURSOR: {
            mInputMethodSession->UpdateCursor(IRect::Probe(obj));
            return NOERROR;
        }
        case DO_UPDATE_CURSOR_ANCHOR_INFO: {
            mInputMethodSession->UpdateCursorAnchorInfo(ICursorAnchorInfo::Probe(obj));
            return NOERROR;
        }
        case DO_APP_PRIVATE_COMMAND: {
            SomeArgs* args = (SomeArgs*)IObject::Probe(obj);
            ICharSequence* seq = ICharSequence::Probe(args->mArg1);
            String info;
            seq->ToString(&info);
            mInputMethodSession->AppPrivateCommand(info, IBundle::Probe(args->mArg2));
            args->Recycle();
            return NOERROR;
        }
        case DO_TOGGLE_SOFT_INPUT: {
            mInputMethodSession->ToggleSoftInput(arg1, arg2);
            return NOERROR;
        }
        case DO_FINISH_SESSION: {
            DoFinishSession();
            return NOERROR;
        }
        case DO_VIEW_CLICKED: {
            mInputMethodSession->ViewClicked(arg1 == 1);
            return NOERROR;
        }
    }

    Logger::W(TAG, "Unhandled message code: %d", what);
    return NOERROR;
}

void CIInputMethodSessionWrapper::DoFinishSession()
{
    mInputMethodSession = NULL;
    if (mReceiver != NULL) {
        mReceiver->Dispose();
        mReceiver = NULL;
    }
    if (mChannel != NULL) {
        mChannel->Dispose();
        mChannel = NULL;
    }
}

ECode CIInputMethodSessionWrapper::FinishInput()
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessage(DO_FINISH_INPUT, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodSessionWrapper::DisplayCompletions(
    /* [in] */ ArrayOf<ICompletionInfo*>* completions)
{
    AutoPtr<IArrayOf> container;
    if (completions != NULL) {
        CArrayOf::New(EIID_ICompletionInfo, completions->GetLength(), (IArrayOf**)&container);
        for (Int32 i = 0; i < completions->GetLength(); ++i) {
            container->Set(i, (*completions)[i]);
        }
    }

    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageO(DO_DISPLAY_COMPLETIONS, container, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodSessionWrapper::UpdateExtractedText(
    /* [in] */ Int32 token,
    /* [in] */ IExtractedText* text)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageIO(DO_UPDATE_EXTRACTED_TEXT, token, text, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodSessionWrapper::UpdateSelection(
    /* [in] */ Int32 oldSelStart,
    /* [in] */ Int32 oldSelEnd,
    /* [in] */ Int32 newSelStart,
    /* [in] */ Int32 newSelEnd,
    /* [in] */ Int32 candidatesStart,
    /* [in] */ Int32 candidatesEnd)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageIIIIII(DO_UPDATE_SELECTION,
        oldSelStart, oldSelEnd, newSelStart, newSelEnd,
        candidatesStart, candidatesEnd, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodSessionWrapper::ViewClicked(
    /* [in] */ Boolean focusChanged)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageI(DO_VIEW_CLICKED, focusChanged ? 1 : 0, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodSessionWrapper::UpdateCursor(
    /* [in] */ IRect* newCursor)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageO(DO_UPDATE_CURSOR, newCursor, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodSessionWrapper::UpdateCursorAnchorInfo(
    /* [in] */ ICursorAnchorInfo* cursorAnchorInfo)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageO(DO_UPDATE_CURSOR_ANCHOR_INFO, cursorAnchorInfo, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodSessionWrapper::AppPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    AutoPtr<ICharSequence> seq;
    CString::New(action, (ICharSequence**)&seq);
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageOO(DO_APP_PRIVATE_COMMAND, seq, data, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodSessionWrapper::ToggleSoftInput(
    /* [in] */ Int32 showFlags,
    /* [in] */ Int32 hideFlags)
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessageII(DO_TOGGLE_SOFT_INPUT, showFlags, hideFlags, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodSessionWrapper::FinishSession()
{
    AutoPtr<IMessage> msg;
    mCaller->ObtainMessage(DO_FINISH_SESSION, (IMessage**)&msg);
    return mCaller->ExecuteOrSendMessage(msg);
}

ECode CIInputMethodSessionWrapper::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("CIInputMethodSessionWrapper");
    return NOERROR;
}

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos

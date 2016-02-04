#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Net.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/media/tv/CTvInputManagerSession.h"
#include "elastos/droid/media/tv/CTvInputManager.h"
#include "elastos/droid/os/CLooperHelper.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

DEFINE_CONVERSION_FOR(Elastos::Droid::Media::Tv::CTvInputManagerSession::PendingEvent, IInterface)

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

//==============================================================================
// CTvInputManagerSession::InputEventHandler
//==============================================================================

const Int32 CTvInputManagerSession::InputEventHandler::MSG_SEND_INPUT_EVENT = 1;
const Int32 CTvInputManagerSession::InputEventHandler::MSG_TIMEOUT_INPUT_EVENT = 2;
const Int32 CTvInputManagerSession::InputEventHandler::MSG_FLUSH_INPUT_EVENT = 3;

CTvInputManagerSession::InputEventHandler::InputEventHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ CTvInputManagerSession* host)
    : Handler(looper, NULL, TRUE)
    , mHost(host)
{
}

ECode CTvInputManagerSession::InputEventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    Int32 arg1;
    msg->GetArg1(&arg1);
    switch (what) {
        case MSG_SEND_INPUT_EVENT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->SendInputEventAndReportResultOnMainLooper((PendingEvent*)(IObject*)obj.Get());
            return NOERROR;
        }
        case MSG_TIMEOUT_INPUT_EVENT: {
            mHost->FinishedInputEvent(arg1, FALSE, TRUE);
            return NOERROR;
        }
        case MSG_FLUSH_INPUT_EVENT: {
            mHost->FinishedInputEvent(arg1, FALSE, FALSE);
            return NOERROR;
        }
    }
    return NOERROR;
}

//==============================================================================
// CTvInputManagerSession::TvInputEventSender
//==============================================================================

CTvInputManagerSession::TvInputEventSender::TvInputEventSender(
    /* [in] */ IInputChannel* inputChannel,
    /* [in] */ ILooper* looper,
    /* [in] */ CTvInputManagerSession* host)
    : mHost(host)
{
    InputEventSender::constructor(inputChannel, looper);
}

ECode CTvInputManagerSession::TvInputEventSender::OnInputEventFinished(
    /* [in] */ Int32 seq,
    /* [in] */ Boolean handled)
{
    return mHost->FinishedInputEvent(seq, handled, FALSE);
}

//==============================================================================
// CTvInputManagerSession::PendingEvent
//==============================================================================

CTvInputManagerSession::PendingEvent::PendingEvent(
    /* [in] */ CTvInputManagerSession* host)
    : mHost(host)
{
}

ECode CTvInputManagerSession::PendingEvent::Recycle()
{
    mEvent = NULL;
    mEventToken = NULL;
    mCallback = NULL;
    mEventHandler = NULL;
    mHandled = FALSE;
    return NOERROR;
}

ECode CTvInputManagerSession::PendingEvent::Run()
{
    mCallback->OnFinishedInputEvent(mEventToken, mHandled);

    synchronized(mEventHandler) {
        mHost->RecyclePendingEventLocked(this);
    }
    return NOERROR;
}

//==============================================================================
// CTvInputManagerSession
//==============================================================================

Int64 CTvInputManagerSession::INPUT_SESSION_NOT_RESPONDING_TIMEOUT = 2500;

CAR_INTERFACE_IMPL(CTvInputManagerSession, Object, ITvInputManagerSession)

CAR_OBJECT_IMPL(CTvInputManagerSession)

CTvInputManagerSession::CTvInputManagerSession()
    : mUserId(0)
    , mSeq(0)
{
    AutoPtr<ILooper> mainLooper;
    AutoPtr<ILooperHelper> helper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    helper->GetMainLooper((ILooper**)&mainLooper);
    mHandler = new InputEventHandler(mainLooper, this);

    mPendingEventPool = new Pools::SimplePool<PendingEvent>(20);
    CSparseArray::New(20, (ISparseArray**)&mPendingEvents);

    CArrayList::New((IArrayList**)&mAudioTracks);
    CArrayList::New((IArrayList**)&mVideoTracks);
    CArrayList::New((IArrayList**)&mSubtitleTracks);
}

CTvInputManagerSession::~CTvInputManagerSession()
{
}

ECode CTvInputManagerSession::constructor(
    /* [in] */ IBinder* token,
    /* [in] */ IInputChannel* channel,
    /* [in] */ IITvInputManager* service,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 seq,
    /* [in] */ ISparseArray* sessionCallbackRecordMap)
{
    mToken = token;
    mChannel = channel;
    mService = service;
    mUserId = userId;
    mSeq = seq;
    mSessionCallbackRecordMap = sessionCallbackRecordMap;
    return NOERROR;
}

ECode CTvInputManagerSession::ReleaseResources()
{
    if (mToken == NULL) {
        Logger::W(CTvInputManager::TAG, "The session has been already released");
        return NOERROR;
    }
    // try {
    mService->ReleaseSession(mToken, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }

    ReleaseInternal();
    return NOERROR;
}

ECode CTvInputManagerSession::SetSurface(
    /* [in] */ ISurface* surface)
{
    if (mToken == NULL) {
        Logger::W(CTvInputManager::TAG, "The session has been already released");
        return NOERROR;
    }
    // surface can be NULL.
    // try {
    return mService->SetSurface(mToken, surface, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerSession::DispatchSurfaceChanged(
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (mToken == NULL) {
        Logger::W(CTvInputManager::TAG, "The session has been already released");
        return NOERROR;
    }
    // try {
    return mService->DispatchSurfaceChanged(mToken, format, width, height, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerSession::SetStreamVolume(
    /* [in] */ Float volume)
{
    if (mToken == NULL) {
        Logger::W(CTvInputManager::TAG, "The session has been already released");
        return NOERROR;
    }
    // try {
    if (volume < 0.0f || volume > 1.0f) {
        // throw new IllegalArgumentException("volume should be between 0.0f and 1.0f");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mService->SetVolume(mToken, volume, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerSession::Tune(
    /* [in] */ IUri* channelUri)
{
    return Tune(channelUri, NULL);
}

ECode CTvInputManagerSession::Tune(
    /* [in] */ IUri* channelUri,
    /* [in] */ IBundle* params)
{
    if (channelUri == NULL) {
        // throw new IllegalArgumentException("channelUri cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mToken == NULL) {
        Logger::W(CTvInputManager::TAG, "The session has been already released");
        return NOERROR;
    }
    mAudioTracks->Clear();
    mVideoTracks->Clear();
    mSubtitleTracks->Clear();
    mSelectedAudioTrackId = NULL;
    mSelectedVideoTrackId = NULL;
    mSelectedSubtitleTrackId = NULL;
    // try {
    return mService->Tune(mToken, channelUri, params, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerSession::SetCaptionEnabled(
    /* [in] */ Boolean enabled)
{
    if (mToken == NULL) {
        Logger::W(CTvInputManager::TAG, "The session has been already released");
        return NOERROR;
    }
    // try {
    return mService->SetCaptionEnabled(mToken, enabled, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerSession::SelectTrack(
    /* [in] */ Int32 type,
    /* [in] */ const String& trackId)
{
    if (type == ITvTrackInfo::TYPE_AUDIO) {
        if (trackId != NULL && !ContainsTrack(IList::Probe(mAudioTracks), trackId)) {
            Logger::W(CTvInputManager::TAG, String("Invalid audio trackId: ") + trackId);
            return NOERROR;
        }
    } else if (type == ITvTrackInfo::TYPE_VIDEO) {
        if (trackId != NULL && !ContainsTrack(IList::Probe(mVideoTracks), trackId)) {
            Logger::W(CTvInputManager::TAG, String("Invalid video trackId: ") + trackId);
            return NOERROR;
        }
    } else if (type == ITvTrackInfo::TYPE_SUBTITLE) {
        if (trackId != NULL && !ContainsTrack(IList::Probe(mSubtitleTracks), trackId)) {
            Logger::W(CTvInputManager::TAG, String("Invalid subtitle trackId: ") + trackId);
            return NOERROR;
        }
    } else {
        // throw new IllegalArgumentException("invalid type: " + type);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mToken == NULL) {
        Logger::W(CTvInputManager::TAG, "The session has been already released");
        return NOERROR;
    }
    // try {
    return mService->SelectTrack(mToken, type, trackId, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerSession::GetTracks(
    /* [in] */ Int32 type,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    if (type == ITvTrackInfo::TYPE_AUDIO) {
        if (mAudioTracks == NULL) {
            *result = NULL;
            return NOERROR;
        }
        *result = IList::Probe(mAudioTracks);
        REFCOUNT_ADD(*result)
        return NOERROR;
    } else if (type == ITvTrackInfo::TYPE_VIDEO) {
        if (mVideoTracks == NULL) {
            *result = NULL;
            return NOERROR;
        }
        *result = IList::Probe(mVideoTracks);
        REFCOUNT_ADD(*result)
        return NOERROR;
    } else if (type == ITvTrackInfo::TYPE_SUBTITLE) {
        if (mSubtitleTracks == NULL) {
            *result = NULL;
            return NOERROR;
        }
        *result = IList::Probe(mSubtitleTracks);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    // throw new IllegalArgumentException("invalid type: " + type);
    *result = NULL;
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CTvInputManagerSession::GetSelectedTrack(
    /* [in] */ Int32 type,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (type == ITvTrackInfo::TYPE_AUDIO) {
        *result = mSelectedAudioTrackId;
        return NOERROR;
    } else if (type == ITvTrackInfo::TYPE_VIDEO) {
        *result = mSelectedVideoTrackId;
        return NOERROR;
    } else if (type == ITvTrackInfo::TYPE_SUBTITLE) {
        *result = mSelectedSubtitleTrackId;
        return NOERROR;
    }
    // throw new IllegalArgumentException("invalid type: " + type);
    *result = String(NULL);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CTvInputManagerSession::SendAppPrivateCommand(
    /* [in] */ const String& action,
    /* [in] */ IBundle* data)
{
    if (mToken == NULL) {
        Logger::W(CTvInputManager::TAG, "The session has been already released");
        return NOERROR;
    }
    // try {
    return mService->SendAppPrivateCommand(mToken, action, data, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerSession::DispatchInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ IInterface* token,
    /* [in] */ ITvInputManagerSessionFinishedInputEventCallback* callback,
    /* [in] */ IHandler* handler,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (event == NULL) {
        // throw new IllegalArgumentException("event cannot be NULL");
        return NOERROR;
    }
    if (callback != NULL && handler == NULL) {
        // throw new IllegalArgumentException("handler cannot be NULL");
        return NOERROR;
    }
    synchronized(mHandler) {
        if (mChannel == NULL) {
            *result = ITvInputManagerSession::DISPATCH_NOT_HANDLED;
            return NOERROR;
        }
        AutoPtr<PendingEvent> p = ObtainPendingEventLocked(event, token, callback, handler);

        AutoPtr<ILooperHelper> helper;
        CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
        AutoPtr<ILooper> mainLooper;
        helper->GetMainLooper((ILooper**)&mainLooper);
        AutoPtr<ILooper> myLooper;
        helper->GetMyLooper((ILooper**)&myLooper);

        if (myLooper == mainLooper) {
            // Already running on the main thread so we can send the event immediately.
            *result = SendInputEventOnMainLooperLocked(p);
            return NOERROR;
        }

        // Post the event to the main thread.
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(InputEventHandler::MSG_SEND_INPUT_EVENT, (IInterface*)(IObject*)p, (IMessage**)&msg);
        msg->SetAsynchronous(TRUE);
        Boolean b;
        mHandler->SendMessage(msg, &b);
        *result = ITvInputManagerSession::DISPATCH_IN_PROGRESS;
        return NOERROR;
    }
    return NOERROR;
}

ECode CTvInputManagerSession::SetMain()
{
    if (mToken == NULL) {
        Logger::W(CTvInputManager::TAG, "The session has been already released");
        return NOERROR;
    }
    // try {
    return mService->SetMainSession(mToken, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerSession::CreateOverlayView(
    /* [in] */ IView* view,
    /* [in] */ IRect* frame)
{
    if (view == NULL) {
        // throw new IllegalArgumentException("view cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (frame == NULL) {
        // throw new IllegalArgumentException("frame cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IBinder> binder;
    view->GetWindowToken((IBinder**)&binder);
    if (binder == NULL) {
        // throw new IllegalStateException("view must be attached to a window");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mToken == NULL) {
        Logger::W(CTvInputManager::TAG, "The session has been already released");
        return NOERROR;
    }
    // try {
    return mService->CreateOverlayView(mToken, binder, frame, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerSession::RelayoutOverlayView(
    /* [in] */ IRect* frame)
{
    if (frame == NULL) {
        // throw new IllegalArgumentException("frame cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mToken == NULL) {
        Logger::W(CTvInputManager::TAG, "The session has been already released");
        return NOERROR;
    }
    // try {
    return mService->RelayoutOverlayView(mToken, frame, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerSession::RemoveOverlayView()
{
    if (mToken == NULL) {
        Logger::W(CTvInputManager::TAG, "The session has been already released");
        return NOERROR;
    }
    // try {
    return mService->RemoveOverlayView(mToken, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerSession::RequestUnblockContent(
    /* [in] */ ITvContentRating* unblockedRating)
{
    if (mToken == NULL) {
        Logger::W(CTvInputManager::TAG, "The session has been already released");
        return NOERROR;
    }
    if (unblockedRating == NULL) {
        // throw new IllegalArgumentException("unblockedRating cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    String str;
    unblockedRating->FlattenToString(&str);
    return mService->RequestUnblockContent(mToken, str, mUserId);
    // } catch (RemoteException e) {
    //     throw new RuntimeException(e);
    // }
}

ECode CTvInputManagerSession::FinishedInputEvent(
    /* [in] */ Int32 seq,
    /* [in] */ Boolean handled,
    /* [in] */ Boolean timeout)
{
    AutoPtr<PendingEvent> p;
    synchronized(mHandler) {
        Int32 index;
        mPendingEvents->IndexOfKey(seq, &index);
        if (index < 0) {
            return NOERROR; // spurious, event already finished or timed out
        }

        AutoPtr<IInterface> obj;
        mPendingEvents->ValueAt(index, (IInterface**)&obj);
        p = (PendingEvent*)(IObject*)obj.Get();
        mPendingEvents->RemoveAt(index);

        if (timeout) {
            // Logger::W(CTvInputManager::TAG, "Timeout waiting for seesion to handle input event after "
            //         + INPUT_SESSION_NOT_RESPONDING_TIMEOUT + " ms: " + mToken);
        } else {
            mHandler->RemoveMessages(InputEventHandler::MSG_TIMEOUT_INPUT_EVENT, (IInterface*)(IObject*)p);
        }
    }

    InvokeFinishedInputEventCallback(p, handled);
    return NOERROR;
}

ECode CTvInputManagerSession::GetToken(
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mToken;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

Boolean CTvInputManagerSession::ContainsTrack(
    /* [in] */ IList* tracks,
    /* [in] */ const String& trackId)
{
    AutoPtr<ArrayOf<IInterface* > > array;
    tracks->ToArray((ArrayOf<IInterface* >**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<ITvTrackInfo> track = ITvTrackInfo::Probe((*array)[i]);
        String id;
        track->GetId(&id);
        if (id.Equals(trackId)) {
            return TRUE;
        }
    }
    return FALSE;
}

void CTvInputManagerSession::SendInputEventAndReportResultOnMainLooper(
    /* [in] */ PendingEvent* p)
{
    synchronized(mHandler) {
        Int32 result = SendInputEventOnMainLooperLocked(p);
        if (result == ITvInputManagerSession::DISPATCH_IN_PROGRESS) {
            return;
        }
    }

    InvokeFinishedInputEventCallback(p, FALSE);
}

Int32 CTvInputManagerSession::SendInputEventOnMainLooperLocked(
    /* [in] */ PendingEvent* p)
{
    if (mChannel != NULL) {
        if (mSender == NULL) {
            AutoPtr<ILooper> looper;
            mHandler->GetLooper((ILooper**)&looper);
            mSender = new TvInputEventSender(mChannel, looper, this);
        }

        AutoPtr<IInputEvent> event = p->mEvent;
        Int32 seq;
        event->GetSequenceNumber(&seq);
        Boolean b;
        mSender->SendInputEvent(seq, event, &b);
        if (b) {
            mPendingEvents->Put(seq, (IInterface*)(IObject*)p);
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(InputEventHandler::MSG_TIMEOUT_INPUT_EVENT, (IInterface*)(IObject*)p, (IMessage**)&msg);
            msg->SetAsynchronous(TRUE);
            mHandler->SendMessageDelayed(msg, INPUT_SESSION_NOT_RESPONDING_TIMEOUT, &b);
            return ITvInputManagerSession::DISPATCH_IN_PROGRESS;
        }

        // Logger::W(CTvInputManager::TAG, "Unable to send input event to session: " + mToken + " dropping:"
        //         + event);
    }
    return ITvInputManagerSession::DISPATCH_NOT_HANDLED;
}

void CTvInputManagerSession::InvokeFinishedInputEventCallback(
    /* [in] */ PendingEvent* p,
    /* [in] */ Boolean handled)
{
    p->mHandled = handled;
    AutoPtr<ILooper> looper;
    p->mEventHandler->GetLooper((ILooper**)&looper);
    Boolean b;
    looper->IsCurrentThread(&b);
    if (b) {
        // Already running on the callback handler thread so we can send the callback
        // immediately.
        p->Run();
    } else {
        // Post the event to the callback handler thread.
        // In this case, the callback will be responsible for recycling the event.
        AutoPtr<IMessageHelper> helper;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
        AutoPtr<IMessage> msg;
        helper->Obtain(p->mEventHandler, p, (IMessage**)&msg);
        msg->SetAsynchronous(TRUE);
        msg->SendToTarget();
    }
}

void CTvInputManagerSession::FlushPendingEventsLocked()
{
    mHandler->RemoveMessages(InputEventHandler::MSG_FLUSH_INPUT_EVENT);

    Int32 count;
    mPendingEvents->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        Int32 seq;
        mPendingEvents->KeyAt(i, &seq);
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(InputEventHandler::MSG_FLUSH_INPUT_EVENT, seq, 0, (IMessage**)&msg);
        msg->SetAsynchronous(TRUE);
        msg->SendToTarget();
    }
}

AutoPtr<CTvInputManagerSession::PendingEvent> CTvInputManagerSession::ObtainPendingEventLocked(
    /* [in] */ IInputEvent* event,
    /* [in] */ IInterface* token,
    /* [in] */ ITvInputManagerSessionFinishedInputEventCallback* callback,
    /* [in] */ IHandler* handler)
{
    AutoPtr<PendingEvent> p;
    p = mPendingEventPool->AcquireItem();
    if (p == NULL) {
        p = new PendingEvent(this);
    }
    p->mEvent = event;
    p->mEventToken = token;
    p->mCallback = callback;
    p->mEventHandler = handler;
    return p;
}

void CTvInputManagerSession::RecyclePendingEventLocked(
    /* [in] */ PendingEvent* p)
{
    p->Recycle();
    mPendingEventPool->ReleaseItem(p);
}

ECode CTvInputManagerSession::ReleaseInternal()
{
    mToken = NULL;
    synchronized(mHandler) {
        if (mChannel != NULL) {
            if (mSender != NULL) {
                FlushPendingEventsLocked();
                mSender->Dispose();
                mSender = NULL;
            }
            mChannel->Dispose();
            mChannel = NULL;
        }
    }
    synchronized(mSessionCallbackRecordMap) {
        mSessionCallbackRecordMap->Remove(mSeq);
    }
    return NOERROR;
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

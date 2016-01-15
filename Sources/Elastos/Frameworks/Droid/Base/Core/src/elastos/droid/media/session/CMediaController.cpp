#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/media/session/CMediaController.h"
#include "elastos/droid/media/session/CMediaControllerPlaybackInfo.h"
#include "elastos/droid/media/session/CMediaControllerTransportControls.h"
#include "elastos/droid/media/session/CMediaSessionToken.h"
#include "elastos/droid/media/session/CParcelableVolumeInfo.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/utility/CArrayMap.h"
// TODO: Need CKeyEventHelper
// #include "elastos/droid/view/CKeyEventHelper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IHandler;
// TODO: Need CKeyEventHelper
// using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL(CMediaController::CallbackStub, Object, IISessionControllerCallback)

const String CMediaController::TAG("MediaController");

const Int32 CMediaController::MSG_EVENT = 1;
const Int32 CMediaController::MSG_UPDATE_PLAYBACK_STATE = 2;
const Int32 CMediaController::MSG_UPDATE_METADATA = 3;
const Int32 CMediaController::MSG_UPDATE_VOLUME = 4;
const Int32 CMediaController::MSG_UPDATE_QUEUE = 5;
const Int32 CMediaController::MSG_UPDATE_QUEUE_TITLE = 6;
const Int32 CMediaController::MSG_UPDATE_EXTRAS = 7;
const Int32 CMediaController::MSG_DESTROYED = 8;

ECode CMediaController::CallbackStub::OnSessionDestroyed()
{
    if (mHost != NULL) {
        mHost->PostMessage(MSG_DESTROYED, NULL, NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnEvent(
    /* [in] */ const String& event,
    /* [in] */ IBundle * extras)
{
    if (mHost != NULL) {
        AutoPtr<ICharSequence> csq;
        CString::New(event, (ICharSequence**)&csq);
        mHost->PostMessage(MSG_EVENT, csq, extras);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnPlaybackStateChanged(
    /* [in] */ IPlaybackState * state)
{
    if (mHost != NULL) {
        mHost->PostMessage(MSG_UPDATE_PLAYBACK_STATE, state, NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnMetadataChanged(
    /* [in] */ IMediaMetadata * metadata)
{
    if (mHost != NULL) {
        mHost->PostMessage(MSG_UPDATE_METADATA, metadata, NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnQueueChanged(
    /* [in] */ IParceledListSlice * parceledQueue)
{
    AutoPtr<IList> queue;
    if(parceledQueue != NULL) {
        parceledQueue->GetList((IList**)&queue);
    }
    if (mHost != NULL) {
        mHost->PostMessage(MSG_UPDATE_QUEUE, queue, NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnQueueTitleChanged(
    /* [in] */ ICharSequence * title)
{
    if (mHost != NULL) {
        mHost->PostMessage(MSG_UPDATE_QUEUE_TITLE, title, NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnExtrasChanged(
    /* [in] */ IBundle * extras)
{
    if (mHost != NULL) {
        mHost->PostMessage(MSG_UPDATE_EXTRAS, extras, NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnVolumeInfoChanged(
    /* [in] */ IParcelableVolumeInfo * pvi)
{
    if (mHost != NULL) {
        AutoPtr<IMediaControllerPlaybackInfo> info;
        AutoPtr<CParcelableVolumeInfo> pvInfo = (CParcelableVolumeInfo*)pvi;
        CMediaControllerPlaybackInfo::New(pvInfo->mVolumeType, pvInfo->mAudioAttrs, pvInfo->mControlType,
                pvInfo->mMaxVolume, pvInfo->mCurrentVolume, (IMediaControllerPlaybackInfo**)&info);
        mHost->PostMessage(MSG_UPDATE_VOLUME, info, NULL);
    }
    return NOERROR;
}

CMediaController::MessageHandler::MessageHandler(
    /* [in] */ ILooper * looper,
    /* [in] */ IMediaControllerCallback * cb,
    /* [in] */ CMediaController * host)
    : Handler(looper, NULL, TRUE)
    , mCallback(cb)
    , mHost(host)
{
}

ECode CMediaController::MessageHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<IBundle> data;
    msg->GetData((IBundle**)&data);
    String str;

    switch (what) {
        case MSG_EVENT:
            ICharSequence::Probe(obj)->ToString(&str);
            mCallback->OnSessionEvent(str, data);
            break;
        case MSG_UPDATE_PLAYBACK_STATE:
            mCallback->OnPlaybackStateChanged(IPlaybackState::Probe(obj));
            break;
        case MSG_UPDATE_METADATA:
            mCallback->OnMetadataChanged(IMediaMetadata::Probe(obj));
            break;
        case MSG_UPDATE_QUEUE:
            mCallback->OnQueueChanged(IList::Probe(obj));
            break;
        case MSG_UPDATE_QUEUE_TITLE:
            mCallback->OnQueueTitleChanged(ICharSequence::Probe(obj));
            break;
        case MSG_UPDATE_EXTRAS:
            mCallback->OnExtrasChanged(IBundle::Probe(obj));
            break;
        case MSG_UPDATE_VOLUME:
            mCallback->OnAudioInfoChanged(IMediaControllerPlaybackInfo::Probe(obj));
            break;
        case MSG_DESTROYED:
            mCallback->OnSessionDestroyed();
            break;
    }
    return NOERROR;
}

ECode CMediaController::MessageHandler::Post(
    /* [in] */ Int32 what,
    /* [in] */ IInterface * obj,
    /* [in] */ IBundle * data)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(what, obj, (IMessage**)&msg);
    return msg->SendToTarget();
}

CAR_INTERFACE_IMPL(CMediaController, Object, IMediaController)

CAR_OBJECT_IMPL(CMediaController)

CMediaController::CMediaController()
    : mCbRegistered(FALSE)
{
    mCbStub = new CallbackStub(this);
    CArrayList::New((IArrayList**)&mCallbacks);
}

CMediaController::~CMediaController()
{
}

ECode CMediaController::constructor(
    /* [in] */ IContext * context,
    /* [in] */ IISessionController * sessionBinder)
{
    if (sessionBinder == NULL) {
        // throw new IllegalArgumentException("Session token cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (context == NULL) {
        // throw new IllegalArgumentException("Context cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSessionBinder = sessionBinder;
    CMediaControllerTransportControls::New(this, (IMediaControllerTransportControls**)&mTransportControls);
    CMediaSessionToken::New(sessionBinder, (IMediaSessionToken**)&mToken);
    mContext = context;
    return NOERROR;
}

ECode CMediaController::constructor(
    /* [in] */ IContext * context,
    /* [in] */ IMediaSessionToken * token)
{
    AutoPtr<IISessionController> controller;
    ((CMediaSessionToken*)token)->GetBinder((IISessionController**)&controller);
    return constructor(context, controller);
}

ECode CMediaController::GetTransportControls(
    /* [out] */ IMediaControllerTransportControls ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTransportControls;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaController::DispatchMediaButtonEvent(
    /* [in] */ IKeyEvent * keyEvent,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (keyEvent == NULL) {
        // throw new IllegalArgumentException("KeyEvent may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 code;
    keyEvent->GetKeyCode(&code);
    Boolean b;
    AutoPtr<IKeyEventHelper> helper;
// TODO: Need CKeyEventHelper
    // CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
    helper->IsMediaKey(code, &b);
    if (!b) {
        *result = FALSE;
        return NOERROR;
    }
    // try {
    return mSessionBinder->SendMediaButton(keyEvent, result);
    // } catch (RemoteException e) {
    //     // System is dead. =(
    // }
}

ECode CMediaController::GetPlaybackState(
    /* [out] */ IPlaybackState ** result)
{
    // try {
    return mSessionBinder->GetPlaybackState(result);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Error calling getPlaybackState.", e);
    //     return null;
    // }
}

ECode CMediaController::GetMetadata(
    /* [out] */ IMediaMetadata ** result)
{
    // try {
    return mSessionBinder->GetMetadata(result);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Error calling getMetadata.", e);
    //     return null;
    // }
}

ECode CMediaController::GetQueue(
    /* [out] */ IList ** result)
{
    // try {
        AutoPtr<IParceledListSlice> queue;
        mSessionBinder->GetQueue((IParceledListSlice**)&queue);
        if (queue != NULL) {
            return queue->GetList(result);
        }
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Error calling getQueue.", e);
    // }
    *result = NULL;
    return NOERROR;
}

ECode CMediaController::GetQueueTitle(
    /* [out] */ ICharSequence ** result)
{
    // try {
    return mSessionBinder->GetQueueTitle(result);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Error calling getQueueTitle", e);
    // }
}

ECode CMediaController::GetExtras(
    /* [out] */ IBundle ** result)
{
    // try {
    return mSessionBinder->GetExtras(result);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Error calling getExtras", e);
    // }
}

ECode CMediaController::GetRatingType(
    /* [out] */ Int32 * result)
{
    // try {
    return mSessionBinder->GetRatingType(result);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Error calling getRatingType.", e);
    //     return Rating.RATING_NONE;
    // }
}

ECode CMediaController::GetFlags(
    /* [out] */ Int64 * result)
{
    // try {
    return mSessionBinder->GetFlags(result);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Error calling getFlags.", e);
    // }
}

ECode CMediaController::GetPlaybackInfo(
    /* [out] */ IMediaControllerPlaybackInfo ** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<IParcelableVolumeInfo> info;
    mSessionBinder->GetVolumeAttributes((IParcelableVolumeInfo**)&info);
    AutoPtr<CParcelableVolumeInfo> i = (CParcelableVolumeInfo*)info.Get();
    return CMediaControllerPlaybackInfo::New(i->mVolumeType, i->mAudioAttrs, i->mControlType,
                i->mMaxVolume, i->mCurrentVolume, result);

    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Error calling getAudioInfo.", e);
    // }
}

ECode CMediaController::GetSessionActivity(
    /* [out] */ IPendingIntent ** result)
{
    // try {
    return mSessionBinder->GetLaunchPendingIntent(result);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Error calling getPendingIntent.", e);
    // }
}

ECode CMediaController::GetSessionToken(
    /* [out] */ IMediaSessionToken ** result)
{
    VALIDATE_NOT_NULL(result)
    *result =  mToken;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaController::SetVolumeTo(
    /* [in] */ Int32 value,
    /* [in] */ Int32 flags)
{
    // try {
    String name;
    mContext->GetPackageName(&name);
    return mSessionBinder->SetVolumeTo(value, flags, name);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Error calling setVolumeTo.", e);
    // }
}

ECode CMediaController::AdjustVolume(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 flags)
{
    // try {
    String name;
    mContext->GetPackageName(&name);
    return mSessionBinder->AdjustVolume(direction, flags, name);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Error calling adjustVolumeBy.", e);
    // }
}

ECode CMediaController::RegisterCallback(
    /* [in] */ IMediaControllerCallback * callback)
{
    return RegisterCallback(callback, NULL);
}

ECode CMediaController::RegisterCallback(
    /* [in] */ IMediaControllerCallback * callback,
    /* [in] */ IHandler * _handler)
{
    if (callback == NULL) {
        // throw new IllegalArgumentException("callback must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IHandler> handler = _handler;
    if (handler == NULL) {
        CHandler::New((IHandler**)&handler);
    }
    synchronized(mLock) {
        AddCallbackLocked(callback, handler);
    }
    return NOERROR;
}

ECode CMediaController::UnregisterCallback(
    /* [in] */ IMediaControllerCallback * callback)
{
    if (callback == NULL) {
        // throw new IllegalArgumentException("callback must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    synchronized(mLock) {
        RemoveCallbackLocked(callback);
    }
    return NOERROR;
}

ECode CMediaController::SendCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle * args,
    /* [in] */ IResultReceiver * cb)
{
    if (command.IsEmpty()) {
        // throw new IllegalArgumentException("command cannot be null or empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    return mSessionBinder->SendCommand(command, args, cb);
    // } catch (RemoteException e) {
    //     Log.d(TAG, "Dead object in sendCommand.", e);
    // }
}

ECode CMediaController::GetPackageName(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    if (mPackageName.IsNull()) {
        // try {
        mSessionBinder->GetPackageName(&mPackageName);
        // } catch (RemoteException e) {
        //     Log.d(TAG, "Dead object in getPackageName.", e);
        // }
    }
    *result = mPackageName;
    return NOERROR;
}

ECode CMediaController::GetTag(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    if (mTag.IsNull()) {
        // try {
        mSessionBinder->GetTag(&mTag);
        // } catch (RemoteException e) {
        //     Log.d(TAG, "Dead object in getTag.", e);
        // }
    }
    *result = mTag;
    return NOERROR;
}

ECode CMediaController::ControlsSameSession(
    /* [in] */ IMediaController * other,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    if (other == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IISessionController> binder;
    ((CMediaController*)other)->GetSessionBinder((IISessionController**)&binder);
    *result = (mSessionBinder == binder);
    return NOERROR;
}

ECode CMediaController::GetSessionBinder(
    /* [out] */ IISessionController ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSessionBinder;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void CMediaController::AddCallbackLocked(
    /* [in] */ IMediaControllerCallback * cb,
    /* [in] */ IHandler * handler)
{
    AutoPtr<MessageHandler> mh;
    GetHandlerForCallbackLocked(cb, (MessageHandler**)&mh);
    if (mh != NULL) {
        Logger::W(TAG, String("Callback is already added, ignoring"));
        return;
    }
    AutoPtr<ILooper> loop;
    handler->GetLooper((ILooper**)&loop);
    AutoPtr<MessageHandler> holder = new MessageHandler(loop, cb, this);
    mCallbacks->Add((IInterface*)(IObject*)holder);

    if (!mCbRegistered) {
        // try {
        mSessionBinder->RegisterCallbackListener(mCbStub);
        mCbRegistered = TRUE;
        // } catch (RemoteException e) {
        //     Log.e(TAG, "Dead object in registerCallback", e);
        // }
    }
}

Boolean CMediaController::RemoveCallbackLocked(
    /* [in] */ IMediaControllerCallback * cb)
{
    Boolean success = FALSE;
    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<MessageHandler> handler;
        mCallbacks->Get(i, (IInterface**)&handler);
        if (cb == handler->mCallback) {
            mCallbacks->Remove(i);
            success = TRUE;
        }
    }
    mCallbacks->GetSize(&size);
    if (mCbRegistered && size == 0) {
        // try {
        mSessionBinder->UnregisterCallbackListener(mCbStub);
        // } catch (RemoteException e) {
        //     Log.e(TAG, "Dead object in removeCallbackLocked");
        // }
        mCbRegistered = FALSE;
    }
    return success;
}

ECode CMediaController::GetHandlerForCallbackLocked(
    /* [in] */ IMediaControllerCallback * cb,
    /* [out] */ MessageHandler ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (cb == NULL) {
        // throw new IllegalArgumentException("Callback cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<MessageHandler> handler;
        mCallbacks->Get(i, (IInterface**)&handler);
        if (cb == handler->mCallback) {
            *result = handler;
            return NOERROR;
        }
    }
    return NOERROR;
}

void CMediaController::PostMessage(
    /* [in] */ Int32 what,
    /* [in] */ IInterface * obj,
    /* [in] */ IBundle * data)
{
    synchronized(mLock) {
        Int32 size;
        mCallbacks->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<MessageHandler> mh;
            mCallbacks->Get(i, (IInterface**)&mh);
            mh->Post(what, obj, data);
        }
    }
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

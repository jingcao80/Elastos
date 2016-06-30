#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/media/session/CMediaController.h"
#include "elastos/droid/media/session/CMediaControllerPlaybackInfo.h"
#include "elastos/droid/media/session/CMediaControllerTransportControls.h"
#include "elastos/droid/media/session/CMediaSessionToken.h"
#include "elastos/droid/media/session/CParcelableVolumeInfo.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/view/CKeyEventHelper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::CoreUtils;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger64;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CAR_INTERFACE_IMPL(CMediaController::CallbackStub, Object, IISessionControllerCallback)
CAR_INTERFACE_IMPL(CMediaController::PlaybackInfo, Object, IMediaControllerPlaybackInfo)

const String CMediaController::TAG("MediaController");

const Int32 CMediaController::MSG_EVENT = 1;
const Int32 CMediaController::MSG_UPDATE_PLAYBACK_STATE = 2;
const Int32 CMediaController::MSG_UPDATE_METADATA = 3;
const Int32 CMediaController::MSG_UPDATE_VOLUME = 4;
const Int32 CMediaController::MSG_UPDATE_QUEUE = 5;
const Int32 CMediaController::MSG_UPDATE_QUEUE_TITLE = 6;
const Int32 CMediaController::MSG_UPDATE_EXTRAS = 7;
const Int32 CMediaController::MSG_FOLDER_INFO_BROWSED_PLAYER = 8;
const Int32 CMediaController::MSG_UPDATE_NOWPLAYING_ENTRIES = 9;
const Int32 CMediaController::MSG_UPDATE_NOWPLAYING_CONTENT_CHANGE = 10;
const Int32 CMediaController::MSG_PLAY_ITEM_RESPONSE = 11;
const Int32 CMediaController::MSG_DESTROYED = 12;

CMediaController::PlaybackInfo::PlaybackInfo(
    /* [in] */ Int32 type,
    /* [in] */ IAudioAttributes* attrs,
    /* [in] */ Int32 control,
    /* [in] */ Int32 max,
    /* [in] */ Int32 current)
    : mVolumeType(type)
    , mVolumeControl(control)
    , mMaxVolume(max)
    , mCurrentVolume(current)
    , mAudioAttrs(attrs)
{
}

ECode  CMediaController::PlaybackInfo::GetPlaybackType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVolumeType;
    return NOERROR;
}

ECode  CMediaController::PlaybackInfo::GetAudioAttributes(
    /* [out] */ IAudioAttributes** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAudioAttrs;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode  CMediaController::PlaybackInfo::GetVolumeControl(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVolumeControl;
    return NOERROR;
}

ECode  CMediaController::PlaybackInfo::GetMaxVolume(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMaxVolume;
    return NOERROR;
}

ECode  CMediaController::PlaybackInfo::GetCurrentVolume(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCurrentVolume;
    return NOERROR;
}

CMediaController::CallbackStub::CallbackStub(
    /* [in] */ CMediaController * controller)
    : mController(controller)
{
}

ECode CMediaController::CallbackStub::OnSessionDestroyed()
{
    if (mController != NULL) {
        mController->PostMessage(MSG_DESTROYED, NULL, NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnEvent(
    /* [in] */ const String& event,
    /* [in] */ IBundle * extras)
{
    if (mController != NULL) {
        AutoPtr<ICharSequence> csq;
        CString::New(event, (ICharSequence**)&csq);
        mController->PostMessage(MSG_EVENT, TO_IINTERFACE(csq), extras);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnPlaybackStateChanged(
    /* [in] */ IPlaybackState * state)
{
    if (mController != NULL) {
        mController->PostMessage(MSG_UPDATE_PLAYBACK_STATE, TO_IINTERFACE(state), NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnMetadataChanged(
    /* [in] */ IMediaMetadata * metadata)
{
    if (mController != NULL) {
        mController->PostMessage(MSG_UPDATE_METADATA, TO_IINTERFACE(metadata), NULL);
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
    if (mController != NULL) {
        mController->PostMessage(MSG_UPDATE_QUEUE, TO_IINTERFACE(queue), NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnQueueTitleChanged(
    /* [in] */ ICharSequence * title)
{
    if (mController != NULL) {
        mController->PostMessage(MSG_UPDATE_QUEUE_TITLE, TO_IINTERFACE(title), NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnExtrasChanged(
    /* [in] */ IBundle * extras)
{
    if (mController != NULL) {
        mController->PostMessage(MSG_UPDATE_EXTRAS, TO_IINTERFACE(extras), NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnVolumeInfoChanged(
    /* [in] */ IParcelableVolumeInfo * pvi)
{
    if (mController != NULL) {
        AutoPtr<IMediaControllerPlaybackInfo> info;
        AutoPtr<CParcelableVolumeInfo> pvInfo = (CParcelableVolumeInfo*)pvi;
        CMediaControllerPlaybackInfo::New(pvInfo->mVolumeType, pvInfo->mAudioAttrs, pvInfo->mControlType,
                pvInfo->mMaxVolume, pvInfo->mCurrentVolume, (IMediaControllerPlaybackInfo**)&info);
        mController->PostMessage(MSG_UPDATE_VOLUME, TO_IINTERFACE(info), NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnUpdateFolderInfoBrowsedPlayer(
    /* [in] */ const String& stringUri)
{
    Logger::D(TAG, "CallBackStub: onUpdateFolderInfoBrowsedPlayer");
    if (mController != NULL) {
        AutoPtr<ICharSequence> charSequenceTmp;
        CString::New(stringUri, (ICharSequence**)&charSequenceTmp);
        mController->PostMessage(MSG_FOLDER_INFO_BROWSED_PLAYER, TO_IINTERFACE(charSequenceTmp), NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnUpdateNowPlayingEntries(
    /* [in] */ ArrayOf<Int64>* playList)
{
    Logger::D(TAG, "CallBackStub: onUpdateNowPlayingEntries");
    if (mController != NULL) {
        AutoPtr<IList> listTmp;
        CArrayList::New((IList**)&listTmp);
        for (Int32 idx=0; idx<playList->GetLength(); ++idx) {
            AutoPtr<IInteger64> intTmp = CoreUtils::Convert((*playList)[idx]);
            listTmp->Add(TO_IINTERFACE(intTmp));
        }
        mController->PostMessage(MSG_UPDATE_NOWPLAYING_ENTRIES, TO_IINTERFACE(listTmp), NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnUpdateNowPlayingContentChange()
{
    Logger::D(TAG, "CallBackStub: onUpdateNowPlayingContentChange");
    if (mController != NULL) {
        mController->PostMessage(MSG_UPDATE_NOWPLAYING_CONTENT_CHANGE, NULL, NULL);
    }
    return NOERROR;
}

ECode CMediaController::CallbackStub::OnPlayItemResponse(
    /* [in] */ Boolean success)
{
    Logger::D(TAG, "CallBackStub: onPlayItemResponse");
    if (mController != NULL) {
        AutoPtr<IBoolean> boolTmp = CoreUtils::Convert(success);
        mController->PostMessage(MSG_PLAY_ITEM_RESPONSE, TO_IINTERFACE(boolTmp), NULL);
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
        case MSG_FOLDER_INFO_BROWSED_PLAYER:
            {
                ICharSequence* charSequenceTmp = ICharSequence::Probe(obj);
                charSequenceTmp->ToString(&str);
                mCallback->OnUpdateFolderInfoBrowsedPlayer(str);
            }
            break;
        case MSG_UPDATE_NOWPLAYING_ENTRIES:
            {
                assert(0);
                // TODO: interface's type is? IList ? IArrayList?
                // mCallback->OnUpdateNowPlayingEntries((long[]) msg.obj);
            }
            break;
        case MSG_UPDATE_NOWPLAYING_CONTENT_CHANGE:
            mCallback->OnUpdateNowPlayingContentChange();
            break;
        case MSG_PLAY_ITEM_RESPONSE:
            {
                IBoolean* boolTmp = IBoolean::Probe(obj);
                Boolean val = FALSE;
                boolTmp->GetValue(&val);
                mCallback->OnPlayItemResponse(val);
            }
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
    CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
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
    {    AutoLock syncLock(mLock);
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
    {    AutoLock syncLock(mLock);
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
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<MessageHandler> handler = (MessageHandler*)IObject::Probe(obj);
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
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<MessageHandler> handler = (MessageHandler*)IObject::Probe(obj);
        if (cb == handler->mCallback) {
            *result = handler;
            REFCOUNT_ADD(*result)
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
    {    AutoLock syncLock(mLock);
        Int32 size;
        mCallbacks->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mCallbacks->Get(i, (IInterface**)&obj);
            AutoPtr<MessageHandler> mh = (MessageHandler*)IObject::Probe(obj);
            mh->Post(what, obj, data);
        }
    }
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

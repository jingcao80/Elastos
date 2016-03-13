#include "elastos/droid/content/pm/CParceledListSlice.h"
#include "elastos/droid/media/session/CMediaController.h"
#include "elastos/droid/media/session/CMediaSession.h"
#include "elastos/droid/media/session/CMediaSessionCallback.h"
#include "elastos/droid/media/session/CMediaSessionCallbackStub.h"
#include "elastos/droid/media/session/CMediaSessionToken.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::CParceledListSlice;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Media::IRating;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Core::CString;
using Elastos::Core::CInteger64;
using Elastos::Core::IInteger64;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

const String CMediaSession::TAG("MediaSession");

const Int32 CMediaSession::CallbackMessageHandler::MSG_PLAY = 1;
const Int32 CMediaSession::CallbackMessageHandler::MSG_PLAY_MEDIA_ID = 2;
const Int32 CMediaSession::CallbackMessageHandler::MSG_PLAY_SEARCH = 3;
const Int32 CMediaSession::CallbackMessageHandler::MSG_SKIP_TO_ITEM = 4;
const Int32 CMediaSession::CallbackMessageHandler::MSG_PAUSE = 5;
const Int32 CMediaSession::CallbackMessageHandler::MSG_STOP = 6;
const Int32 CMediaSession::CallbackMessageHandler::MSG_NEXT = 7;
const Int32 CMediaSession::CallbackMessageHandler::MSG_PREVIOUS = 8;
const Int32 CMediaSession::CallbackMessageHandler::MSG_FAST_FORWARD = 9;
const Int32 CMediaSession::CallbackMessageHandler::MSG_REWIND = 10;
const Int32 CMediaSession::CallbackMessageHandler::MSG_SEEK_TO = 11;
const Int32 CMediaSession::CallbackMessageHandler::MSG_RATE = 12;
const Int32 CMediaSession::CallbackMessageHandler::MSG_CUSTOM_ACTION = 13;
const Int32 CMediaSession::CallbackMessageHandler::MSG_MEDIA_BUTTON = 14;
const Int32 CMediaSession::CallbackMessageHandler::MSG_COMMAND = 15;
const Int32 CMediaSession::CallbackMessageHandler::MSG_SET_BROWSED_PLAYER = 16;
const Int32 CMediaSession::CallbackMessageHandler::MSG_SET_PLAY_ITEM = 17;
const Int32 CMediaSession::CallbackMessageHandler::MSG_GET_NOW_PLAYING_ITEMS = 18;

CAR_INTERFACE_IMPL(CMediaSession, Object, IMediaSession)

CAR_OBJECT_IMPL(CMediaSession)

CMediaSession::Command::Command(
    /* [in] */ const String& command,
    /* [in] */ IBundle * extras,
    /* [in] */ IResultReceiver * stub)
    : mCommand(command)
    , mExtras(extras)
    , mStub(stub)
{
}

CMediaSession::CallbackMessageHandler::CallbackMessageHandler(
    /* [in] */ CMediaSession* host,
    /* [in] */ ILooper* looper,
    /* [in] */ IMediaSessionCallback * callback)
    : Handler(looper, NULL, TRUE)
    , mCallback(callback)
    , mHost(host)
{
}

ECode CMediaSession::CallbackMessageHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<IBundle> data;
    msg->GetData((IBundle**)&data);

    switch (what) {
        case MSG_PLAY: {
            mCallback->OnPlay();
            break;
        }
        case MSG_PLAY_MEDIA_ID: {
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            mCallback->OnPlayFromMediaId(str, data);
            break;
        }
        case MSG_PLAY_SEARCH: {
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            mCallback->OnPlayFromSearch(str, data);
            break;
        }
        case MSG_SKIP_TO_ITEM: {
            Int64 i;
            IInteger64::Probe(obj)->GetValue(&i);
            mCallback->OnSkipToQueueItem(i);
            break;
        }
        case MSG_PAUSE: {
            mCallback->OnPause();
            break;
        }
        case MSG_STOP: {
            mCallback->OnStop();
            break;
        }
        case MSG_NEXT: {
            mCallback->OnSkipToNext();
            break;
        }
        case MSG_PREVIOUS: {
            mCallback->OnSkipToPrevious();
            break;
        }
        case MSG_FAST_FORWARD: {
            mCallback->OnFastForward();
            break;
        }
        case MSG_REWIND: {
            mCallback->OnRewind();
            break;
        }
        case MSG_SEEK_TO: {
            Int64 i;
            IInteger64::Probe(obj)->GetValue(&i);
            mCallback->OnSeekTo(i);
            break;
        }
        case MSG_RATE: {
            mCallback->OnSetRating(IRating::Probe(obj));
            break;
        }
        case MSG_CUSTOM_ACTION: {
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            mCallback->OnCustomAction(str, data);
            break;
        }
        case MSG_MEDIA_BUTTON: {
            Boolean b;
            mCallback->OnMediaButtonEvent(IIntent::Probe(obj), &b);
            break;
        }
        case MSG_COMMAND: {
            AutoPtr<Command> cmd = (Command*)(IObject*)obj.Get();
            mCallback->OnCommand(cmd->mCommand, cmd->mExtras, cmd->mStub);
            break;
        }
        case MSG_SET_BROWSED_PLAYER: {
            Logger::D(TAG, "MSG_SET_BROWSED_PLAYER received in CallbackMessageHandler");
            mCallback->SetBrowsedPlayer();
            break;
        }
        case MSG_SET_PLAY_ITEM: {
            Logger::D(TAG, "MSG_SET_PLAY_ITEM received in CallbackMessageHandler");
            IObject* objTmp = IObject::Probe(obj);
            PlayItemToken* playItemToken = (PlayItemToken*)objTmp;
            Int32 scope = 0;
            playItemToken->GetScope(&scope);
            Int64 uid = 0;
            playItemToken->GetUid(&uid);
            mCallback->SetPlayItem(scope, uid);
            break;
        }
        case MSG_GET_NOW_PLAYING_ITEMS: {
            Logger::D(TAG, "MSG_GET_NOW_PLAYING_ITEMS received in CallbackMessageHandler");
            mCallback->GetNowPlayingEntries();
            break;
        }
    }
    return NOERROR;
}

ECode CMediaSession::CallbackMessageHandler::Post(
    /* [in] */ Int32 what,
    /* [in] */ IInterface * obj,
    /* [in] */ IBundle * bundle)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(what, obj, (IMessage**)&msg);
    msg->SetData(bundle);
    msg->SendToTarget();
    return NOERROR;
}

ECode CMediaSession::CallbackMessageHandler::Post(
    /* [in] */ Int32 what,
    /* [in] */ IInterface * obj)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(what, obj, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode CMediaSession::CallbackMessageHandler::Post(
    /* [in] */ Int32 what)
{
    return Post(what, NULL);
}

ECode CMediaSession::CallbackMessageHandler::Post(
    /* [in] */ Int32 what,
    /* [in] */ IInterface * obj,
    /* [in] */ Int32 arg1)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(what, arg1, 0, obj, (IMessage**)&msg);
    return msg->SendToTarget();
}

CAR_INTERFACE_IMPL(CMediaSession::VolumeProviderCallback, Object,
        IVolumeProviderCallback)

ECode CMediaSession::VolumeProviderCallback::OnVolumeChanged(
    /* [in] */ IVolumeProvider* volumeProvider)
{
    return mHost->NotifyRemoteVolumeChanged(volumeProvider);
}

CMediaSession::PlayItemToken::PlayItemToken(
    /* [in] */ Int64 uid,
    /* [in] */ Int32 scope)
    : mUid(uid)
    , mScope(scope)
{
}

ECode CMediaSession::PlayItemToken::GetScope(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mScope;
    return NOERROR;
}

ECode CMediaSession::PlayItemToken::GetUid(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUid;
    return NOERROR;
}

CMediaSession::CMediaSession()
    : mActive(FALSE)
{
}

CMediaSession::~CMediaSession()
{
}

ECode CMediaSession::constructor(
    /* [in] */ IContext * context,
    /* [in] */ const String& tag)
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 myUserId;
    helper->GetMyUserId(&myUserId);
    return constructor(context, tag, myUserId);
}

ECode CMediaSession::constructor(
    /* [in] */ IContext * context,
    /* [in] */ const String& tag,
    /* [in] */ Int32 userId)
{
    if (context == NULL) {
        // throw new IllegalArgumentException("context cannot be NULL.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (tag.IsEmpty()) {
        // throw new IllegalArgumentException("tag cannot be NULL or empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    CMediaSessionCallbackStub::New(this, (IISessionCallback**)&mCbStub);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::MEDIA_SESSION_SERVICE, (IInterface**)&obj);
    AutoPtr<IMediaSessionManager> manager = IMediaSessionManager::Probe(obj);
    // try {
    manager->CreateSession(mCbStub, tag, userId, (IISession**)&mBinder);
    AutoPtr<IISessionController> controller;
    mBinder->GetController((IISessionController**)&controller);
    CMediaSessionToken::New(controller, (IMediaSessionToken**)&mSessionToken);
    CMediaController::New(context, mSessionToken, (IMediaController**)&mController);
    // } catch (RemoteException e) {
    //     throw new RuntimeException("Remote error creating session.", e);
    // }
    return NOERROR;
}

ECode CMediaSession::SetCallback(
    /* [in] */ IMediaSessionCallback * callback)
{
    return SetCallback(callback, NULL);
}

ECode CMediaSession::SetCallback(
    /* [in] */ IMediaSessionCallback * callback,
    /* [in] */ IHandler * _handler)
{
    synchronized(mLock) {
        if (callback == NULL) {
            if (mCallback != NULL) {
                ((CMediaSessionCallback*)(mCallback->mCallback.Get()))->mSession = NULL;
            }
            mCallback = NULL;
            return NOERROR;
        }
        if (mCallback != NULL) {
            // We're updating the callback, clear the session from the old
            // one.
            ((CMediaSessionCallback*)(mCallback->mCallback.Get()))->mSession = NULL;
        }
        AutoPtr<IHandler> handler = _handler;
        if (handler == NULL) {
            CHandler::New((IHandler**)&handler);
        }
        ((CMediaSessionCallback*)callback)->mSession = this;
        AutoPtr<ILooper> looper;
        handler->GetLooper((ILooper**)&looper);
        AutoPtr<CallbackMessageHandler> msgHandler = new CallbackMessageHandler(this, looper, callback);
        mCallback = msgHandler;
    }
    return NOERROR;
}

ECode CMediaSession::SetSessionActivity(
    /* [in] */ IPendingIntent * pi)
{
    // try {
    return mBinder->SetLaunchPendingIntent(pi);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Failure in setLaunchPendingIntent.", e);
    // }
}

ECode CMediaSession::SetMediaButtonReceiver(
    /* [in] */ IPendingIntent * mbr)
{
    // try {
    return mBinder->SetMediaButtonReceiver(mbr);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Failure in setMediaButtonReceiver.", e);
    // }
}

ECode CMediaSession::SetFlags(
    /* [in] */ Int32 flags)
{
    // try {
    return mBinder->SetFlags(flags);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Failure in setFlags.", e);
    // }
}

ECode CMediaSession::SetPlaybackToLocal(
    /* [in] */ IAudioAttributes * attributes)
{
    if (attributes == NULL) {
        // throw new IllegalArgumentException("Attributes cannot be NULL for local playback.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    return mBinder->SetPlaybackToLocal(attributes);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Failure in setPlaybackToLocal.", e);
    // }
}

ECode CMediaSession::SetPlaybackToRemote(
    /* [in] */ IVolumeProvider * volumeProvider)
{
    if (volumeProvider == NULL) {
        // throw new IllegalArgumentException("volumeProvider may not be NULL!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mVolumeProvider = volumeProvider;
    AutoPtr<VolumeProviderCallback> cb = new VolumeProviderCallback(this);
    volumeProvider->SetCallback(cb);

    // try {
    Int32 control;
    Int32 maxVol;
    Int32 curVol;

    volumeProvider->GetVolumeControl(&control);
    volumeProvider->GetMaxVolume(&maxVol);
    volumeProvider->GetCurrentVolume(&curVol);

    mBinder->SetPlaybackToRemote(control, maxVol);
    mBinder->SetCurrentVolume(curVol);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Failure in setPlaybackToRemote.", e);
    // }
    return NOERROR;
}

ECode CMediaSession::SetActive(
    /* [in] */ Boolean active)
{
    if (mActive == active) {
        return NOERROR;
    }
    // try {
    mBinder->SetActive(active);
    mActive = active;
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Failure in setActive.", e);
    // }
    return NOERROR;
}

ECode CMediaSession::IsActive(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mActive;
    return NOERROR;
}

ECode CMediaSession::SendSessionEvent(
    /* [in] */ const String& event,
    /* [in] */ IBundle * extras)
{
    if (event.IsEmpty()) {
        // throw new IllegalArgumentException("event cannot be NULL or empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    return mBinder->SendEvent(event, extras);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Error sending event", e);
    // }
}

ECode CMediaSession::ReleaseResources()
{
    // try {
    return mBinder->Destroy();
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Error releasing session: ", e);
    // }
}

ECode CMediaSession::GetSessionToken(
    /* [out] */ IMediaSessionToken ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSessionToken;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaSession::GetController(
    /* [out] */ IMediaController ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mController;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaSession::SetPlaybackState(
    /* [in] */ IPlaybackState * state)
{
    mPlaybackState = state;
    // try {
    return mBinder->SetPlaybackState(state);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Dead object in setPlaybackState.", e);
    // }
}

ECode CMediaSession::SetMetadata(
    /* [in] */ IMediaMetadata * metadata)
{
    // try {
    return mBinder->SetMetadata(metadata);
    // } catch (RemoteException e) {
    //     Log.wtf(TAG, "Dead object in setPlaybackState.", e);
    // }
}

ECode CMediaSession::SetQueue(
    /* [in] */ IList * queue)
{
    // try {
    AutoPtr<IParceledListSlice> list;
    if(queue != NULL) {
        CParceledListSlice::New(queue, (IParceledListSlice**)&list);
    }
    return mBinder->SetQueue(list);
    // } catch (RemoteException e) {
    //     Log.wtf("Dead object in setQueue.", e);
    // }
}

ECode CMediaSession::SetQueueTitle(
    /* [in] */ ICharSequence * title)
{
    // try {
    return mBinder->SetQueueTitle(title);
    // } catch (RemoteException e) {
    //     Log.wtf("Dead object in setQueueTitle.", e);
    // }
}

ECode CMediaSession::SetExtras(
    /* [in] */ IBundle * extras)
{
    // try {
    return mBinder->SetExtras(extras);
    // } catch (RemoteException e) {
    //     Log.wtf("Dead object in setExtras.", e);
    // }
}

ECode CMediaSession::PlayItemResponse(
    /* [in] */ Boolean success)
{
    Logger::D(TAG, "MediaSession: playItemResponse");
    // try {
        mBinder->PlayItemResponse(success);
    // } catch (RemoteException e) {
        // Log.wtf(TAG, "Dead object in playItemResponse.", e);
    // }
    return NOERROR;
}

ECode CMediaSession::UpdateNowPlayingEntries(
    /* [in] */ ArrayOf<Int64>* playList)
{
    Logger::D(TAG, "MediaSession: updateNowPlayingEntries");
    // try {
        mBinder->UpdateNowPlayingEntries(playList);
    // } catch (RemoteException e) {
        // Log.wtf(TAG, "Dead object in updateNowPlayingEntries.", e);
    // }
    return NOERROR;
}

ECode CMediaSession::UpdateFolderInfoBrowsedPlayer(
    /* [in] */ const String& stringUri)
{
    Logger::D(TAG, "MediaSession: updateFolderInfoBrowsedPlayer");
    // try {
        mBinder->UpdateFolderInfoBrowsedPlayer(stringUri);
    // } catch (RemoteException e) {
        // Log.wtf(TAG, "Dead object in updateFolderInfoBrowsedPlayer.", e);
    // }
    return NOERROR;
}

ECode CMediaSession::UpdateNowPlayingContentChange()
{
    Logger::D(TAG, "MediaSession: updateNowPlayingContentChange");
    // try {
        mBinder->UpdateNowPlayingContentChange();
    // } catch (RemoteException e) {
        // Log.wtf(TAG, "Dead object in updateNowPlayingContentChange.", e);
    // }
    return NOERROR;
}

ECode CMediaSession::NotifyRemoteVolumeChanged(
    /* [in] */ IVolumeProvider * provider)
{
    if (provider == NULL || provider != mVolumeProvider) {
        Logger::W(TAG, "Received update from stale volume provider");
        return NOERROR;
    }
    // try {
    Int32 vol;
    provider->GetCurrentVolume(&vol);
    return mBinder->SetCurrentVolume(vol);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Error in notifyVolumeChanged", e);
    // }
}

ECode CMediaSession::IsActiveState(
    /* [in] */ Int32 state,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    switch (state) {
        case IPlaybackState::STATE_FAST_FORWARDING:
        case IPlaybackState::STATE_REWINDING:
        case IPlaybackState::STATE_SKIPPING_TO_PREVIOUS:
        case IPlaybackState::STATE_SKIPPING_TO_NEXT:
        case IPlaybackState::STATE_BUFFERING:
        case IPlaybackState::STATE_CONNECTING:
        case IPlaybackState::STATE_PLAYING:
            *result = TRUE;
            return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CMediaSession::DispatchPlay()
{
    return PostToCallback(CallbackMessageHandler::MSG_PLAY);
}

ECode CMediaSession::DispatchPlayFromMediaId(
    /* [in] */ const String& mediaId,
    /* [in] */ IBundle * extras)
{
    AutoPtr<ICharSequence> csq;
    CString::New(mediaId, (ICharSequence**)&csq);
    return PostToCallback(CallbackMessageHandler::MSG_PLAY_MEDIA_ID, csq, extras);
}

ECode CMediaSession::DispatchPlayFromSearch(
    /* [in] */ const String& query,
    /* [in] */ IBundle * extras)
{
    AutoPtr<ICharSequence> csq;
    CString::New(query, (ICharSequence**)&csq);
    return PostToCallback(CallbackMessageHandler::MSG_PLAY_SEARCH, csq, extras);
}

ECode CMediaSession::DispatchSkipToItem(
    /* [in] */ Int64 id)
{
    AutoPtr<IInteger64> n;
    CInteger64::New(id, (IInteger64**)&n);
    return PostToCallback(CallbackMessageHandler::MSG_SKIP_TO_ITEM, n);
}

ECode CMediaSession::DispatchPause()
{
    return PostToCallback(CallbackMessageHandler::MSG_PAUSE);
}

ECode CMediaSession::DispatchStop()
{
    return PostToCallback(CallbackMessageHandler::MSG_STOP);
}

ECode CMediaSession::DispatchNext()
{
    return PostToCallback(CallbackMessageHandler::MSG_NEXT);
}

ECode CMediaSession::DispatchPrevious()
{
    return PostToCallback(CallbackMessageHandler::MSG_PREVIOUS);
}

ECode CMediaSession::DispatchFastForward()
{
    return PostToCallback(CallbackMessageHandler::MSG_FAST_FORWARD);
}

ECode CMediaSession::DispatchRewind()
{
    return PostToCallback(CallbackMessageHandler::MSG_REWIND);
}

ECode CMediaSession::DispatchSeekTo(
    /* [in] */ Int64 pos)
{
    AutoPtr<IInteger64> n;
    CInteger64::New(pos, (IInteger64**)&n);
    return PostToCallback(CallbackMessageHandler::MSG_SEEK_TO, n);
}

ECode CMediaSession::DispatchRate(
    /* [in] */ IRating * rating)
{
    return PostToCallback(CallbackMessageHandler::MSG_RATE, rating);
}

ECode CMediaSession::DispatchCustomAction(
    /* [in] */ const String& action,
    /* [in] */ IBundle * args)
{
    AutoPtr<ICharSequence> csq;
    CString::New(action, (ICharSequence**)&csq);
    return PostToCallback(CallbackMessageHandler::MSG_CUSTOM_ACTION, csq, args);
}

ECode CMediaSession::DispatchSetBrowsedPlayerCommand()
{
    return PostToCallback(CallbackMessageHandler::MSG_SET_BROWSED_PLAYER);
}

ECode CMediaSession::DispatchSetPlayItemCommand(
    /* [in] */ Int64 uid,
    /* [in] */ Int32 scope)
{
    AutoPtr<PlayItemToken> playItemToken = new PlayItemToken(uid, scope);
    return PostToCallback(CallbackMessageHandler::MSG_SET_PLAY_ITEM, TO_IINTERFACE(playItemToken));
}

ECode CMediaSession::DispatchGetNowPlayingItemsCommand()
{
    return PostToCallback(CallbackMessageHandler::MSG_GET_NOW_PLAYING_ITEMS);
}

ECode CMediaSession::DispatchMediaButton(
    /* [in] */ IIntent * mediaButtonIntent)
{
    return PostToCallback(CallbackMessageHandler::MSG_MEDIA_BUTTON, mediaButtonIntent);
}

ECode CMediaSession::PostToCallback(
    /* [in] */ Int32 what)
{
    return PostToCallback(what, NULL);
}

ECode CMediaSession::PostCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle * args,
    /* [in] */ IResultReceiver * resultCb)
{
    AutoPtr<Command> cmd = new Command(command, args, resultCb);
    return PostToCallback(CallbackMessageHandler::MSG_COMMAND, (IInterface*)(IObject*)cmd.Get());
}

ECode CMediaSession::PostToCallback(
    /* [in] */ Int32 what,
    /* [in] */ IInterface * obj)
{
    return PostToCallback(what, obj, NULL);
}

ECode CMediaSession::PostToCallback(
    /* [in] */ Int32 what,
    /* [in] */ IInterface * obj,
    /* [in] */ IBundle * extras)
{
    synchronized(mLock) {
        if (mCallback != NULL) {
            mCallback->Post(what, obj, extras);
        }
    }
    return NOERROR;
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

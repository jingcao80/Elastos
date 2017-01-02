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

#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/Intent.h"
#include "elastos/droid/media/RemoteController.h"
#include "elastos/droid/media/CRemoteControlClient.h"
#include "elastos/droid/media/session/CMediaSessionLegacyHelper.h"
#include "elastos/droid/media/session/CPlaybackState.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/view/KeyEvent.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/os/Looper.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/os/CHandler.h>
#include <elastos/core/Math.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Intent;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Media::Session::EIID_IMediaSessionManagerOnActiveSessionsChangedListener;
using Elastos::Droid::Media::Session::CMediaSessionLegacyHelper;
using Elastos::Droid::Media::Session::CPlaybackState;
using Elastos::Droid::Media::Session::IMediaController;
using Elastos::Droid::Media::Session::IMediaControllerTransportControls;
using Elastos::Droid::Media::Session::IMediaSessionToken;
using Elastos::Droid::View::KeyEvent;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::AutoLock;
using Elastos::Core::Math;
using Elastos::Core::StringUtils;

namespace Elastos{
namespace Droid {
namespace Media {
//================================================================================
//                      RemoteController::RcDisplay
//================================================================================
RemoteController::RcDisplay::RcDisplay(
    /* [in] */ RemoteController* rc)
    : mController(rc)
{}

RemoteController::RcDisplay::~RcDisplay()
{}

CAR_INTERFACE_IMPL(RemoteController::RcDisplay, Object, IIRemoteControlDisplay)

ECode RemoteController::RcDisplay::SetCurrentClientId(
    /* [in] */ Int32 genId,
    /* [in] */ IPendingIntent* clientMediaIntent,
    /* [in] */ Boolean clearing)
{
    if (mController == NULL) {
        return NOERROR;
    }
    Boolean isNew = FALSE;
    {    AutoLock syncLock(mController->mGenLock);
        if (mController->mClientGenerationIdCurrent != genId) {
            mController->mClientGenerationIdCurrent = genId;
            isNew = TRUE;
        }
    }
    if (clientMediaIntent != NULL) {
        SendMsg(mController->mEventHandler, MSG_NEW_PENDING_INTENT, SENDMSG_REPLACE,
                genId /*arg1*/, 0, clientMediaIntent /*obj*/, 0 /*delay*/);
    }
    if (isNew || clearing) {
        SendMsg(mController->mEventHandler, MSG_CLIENT_CHANGE, SENDMSG_REPLACE,
                genId /*arg1*/, clearing ? 1 : 0, NULL /*obj*/, 0 /*delay*/);
    }
    return NOERROR;
}

ECode RemoteController::RcDisplay::SetEnabled(
    /* [in] */ Boolean enabled)
{
    if (mController == NULL) {
        return NOERROR;
    }
    SendMsg(mController->mEventHandler, MSG_DISPLAY_ENABLE, SENDMSG_REPLACE,
            enabled ? 1 : 0 /*arg1*/, 0, NULL /*obj*/, 0 /*delay*/);
    return NOERROR;
}

ECode RemoteController::RcDisplay::SetPlaybackState(
    /* [in] */ Int32 genId,
    /* [in] */ Int32 state,
    /* [in] */ Int64 stateChangeTimeMs,
    /* [in] */ Int64 currentPosMs,
    /* [in] */ Float speed)
{
    if (mController == NULL) {
        return NOERROR;
    }
    if (DEBUG) {
        Slogger::D(TAG, "> new playback state: genId=%d state=%d changeTime=%lld pos=%lldms speed=%f", genId
                ,state ,stateChangeTimeMs, currentPosMs, speed);
    }

    {    AutoLock syncLock(mController->mGenLock);
        if (mController->mClientGenerationIdCurrent != genId) {
            return NOERROR;
        }
    }
    AutoPtr<PlaybackInfo> playbackInfo =
        new PlaybackInfo(state, stateChangeTimeMs, currentPosMs, speed);
    AutoPtr<IPlaybackInfo> obj = IPlaybackInfo::Probe(playbackInfo);
    SendMsg(mController->mEventHandler, MSG_NEW_PLAYBACK_INFO, SENDMSG_REPLACE,
            genId /*arg1*/, 0, obj.Get() /*obj*/, 0 /*delay*/);

    return NOERROR;
}

ECode RemoteController::RcDisplay::SetTransportControlInfo(
    /* [in] */ Int32 genId,
    /* [in] */ Int32 transportControlFlags,
    /* [in] */ Int32 posCapabilities)
{
    if (mController == NULL) {
        return NOERROR;
    }
    {    AutoLock syncLock(mController->mGenLock);
        if (mController->mClientGenerationIdCurrent != genId) {
            return NOERROR;
        }
    }
    SendMsg(mController->mEventHandler, MSG_NEW_TRANSPORT_INFO, SENDMSG_REPLACE,
            genId /*arg1*/, transportControlFlags /*arg2*/,
            NULL /*obj*/, 0 /*delay*/);

    return NOERROR;
}

ECode RemoteController::RcDisplay::SetMetadata(
    /* [in] */ Int32 genId,
    /* [in] */ IBundle* metadata)
{
    if (mController == NULL) {
        return NOERROR;
    }
    if (DEBUG) {
        Slogger::E(TAG, "setMetadata(\"%d\")", genId);
    }
    if (metadata == NULL) {
        return NOERROR;
    }
    {    AutoLock syncLock(mController->mGenLock);
        if (mController->mClientGenerationIdCurrent != genId) {
            return NOERROR;
        }
    }
    SendMsg(mController->mEventHandler, MSG_NEW_METADATA, SENDMSG_QUEUE,
            genId /*arg1*/, 0 /*arg2*/,
            metadata /*obj*/, 0 /*delay*/);

    return NOERROR;
}

ECode RemoteController::RcDisplay::SetArtwork(
    /* [in] */ Int32 genId,
    /* [in] */ IBitmap* artwork)
{
    if (mController == NULL) {
        return NOERROR;
    }
    if (DEBUG) {
        Slogger::V(TAG, "setArtwork(\"%d\")", genId);
    }

    {    AutoLock syncLock(mController->mGenLock);
        if (mController->mClientGenerationIdCurrent != genId) {
            return NOERROR;
        }
    }
    AutoPtr<IBundle> metadata;
    CBundle::New(1, (IBundle**)&metadata);
    metadata->PutParcelable(StringUtils::ToString(IMediaMetadataEditor::BITMAP_KEY_ARTWORK), IParcelable::Probe(artwork));
    SendMsg(mController->mEventHandler, MSG_NEW_METADATA, SENDMSG_QUEUE,
            genId /*arg1*/, 0 /*arg2*/,
            metadata /*obj*/, 0 /*delay*/);
    return NOERROR;
}

ECode RemoteController::RcDisplay::SetAllMetadata(
    /* [in] */ Int32 genId,
    /* [in] */ IBundle* metadata,
    /* [in] */ IBitmap* artwork)
{
    if (mController == NULL) {
        return NOERROR;
    }
    if (DEBUG) {
        Slogger::E(TAG, "setAllMetadata(\"%d\")", genId); }
    if ((metadata == NULL) && (artwork == NULL)) {
        return NOERROR;
    }
    {    AutoLock syncLock(mController->mGenLock);
        if (mController->mClientGenerationIdCurrent != genId) {
            return NOERROR;
        }
    }
    if (metadata == NULL) {
        CBundle::New(1, (IBundle**)&metadata);
    }
    if (artwork != NULL) {
        metadata->PutParcelable(StringUtils::ToString(IMediaMetadataEditor::BITMAP_KEY_ARTWORK),
                IParcelable::Probe(artwork));
    }
    SendMsg(mController->mEventHandler, MSG_NEW_METADATA, SENDMSG_QUEUE,
            genId /*arg1*/, 0 /*arg2*/,
            metadata /*obj*/, 0 /*delay*/);

    return NOERROR;
}
//================================================================================
//                      RemoteController::MediaControllerCallback
//================================================================================
RemoteController::MediaControllerCallback::MediaControllerCallback(
    /* [in] */ RemoteController* host)
    : mHost(host)
{}

RemoteController::MediaControllerCallback::~MediaControllerCallback()
{}

CAR_INTERFACE_IMPL(RemoteController::MediaControllerCallback, Object, IMediaControllerCallback)

ECode RemoteController::MediaControllerCallback::OnPlaybackStateChanged(
    /* [in] */ IPlaybackState* state)
{
    mHost->OnNewPlaybackState(state);
    return NOERROR;
}

ECode RemoteController::MediaControllerCallback::OnMetadataChanged(
    /* [in] */ IMediaMetadata* metadata)
{
    mHost->OnNewMediaMetadata(metadata);
    return NOERROR;
}

ECode RemoteController::MediaControllerCallback::OnUpdateFolderInfoBrowsedPlayer(
    /* [in] */ const String& stringUri)
{
    Logger::D(TAG, "MediaControllerCallback: onUpdateFolderInfoBrowsedPlayer");
    mHost->OnFolderInfoBrowsedPlayer(stringUri);
    return NOERROR;
}

ECode RemoteController::MediaControllerCallback::OnUpdateNowPlayingEntries(
    /* [in] */ ArrayOf<Int64>* playList)
{
    Logger::D(TAG, "MediaControllerCallback: onUpdateNowPlayingEntries");
    mHost->OnNowPlayingEntriesUpdate(playList);
    return NOERROR;
}

ECode RemoteController::MediaControllerCallback::OnUpdateNowPlayingContentChange()
{
    Logger::D(TAG, "MediaControllerCallback: onUpdateNowPlayingContentChange");
    mHost->OnNowPlayingContentChange();
    return NOERROR;
}

ECode RemoteController::MediaControllerCallback::OnPlayItemResponse(
    /* [in] */ Boolean success)
{
    Logger::D(TAG, "MediaControllerCallback: onPlayItemResponse");
    mHost->OnSetPlayItemResponse(success);
    return NOERROR;
}

//================================================================================
//                      RemoteController::TopTransportSessionListener
//================================================================================
RemoteController::TopTransportSessionListener::TopTransportSessionListener(
    /* [in] */ RemoteController* host)
    : mHost(host)
{}

RemoteController::TopTransportSessionListener::~TopTransportSessionListener()
{}

CAR_INTERFACE_IMPL(RemoteController::TopTransportSessionListener, Object, IMediaSessionManagerOnActiveSessionsChangedListener)

ECode RemoteController::TopTransportSessionListener::OnActiveSessionsChanged(
    /* [in] */ IList * controllers)
{
    VALIDATE_NOT_NULL(controllers);
    Int32 size;
    controllers->GetSize(&size);
    AutoPtr<IInterface> obj;
    AutoPtr<IMediaController> controller;
    for (Int32 i = 0; i < size; ++i) {
        obj = NULL;
        controllers->Get(i, (IInterface**)&obj);
        controller = NULL;
        controller = IMediaController::Probe(obj);
        Int64 flags;
        controller->GetFlags(&flags);
        // We only care about sessions that handle transport controls,
        // which will be true for apps using RCC
        if ((flags & IMediaSession::FLAG_HANDLES_TRANSPORT_CONTROLS) != 0) {
            mHost->UpdateController(IMediaController::Probe(controller));
            return NOERROR;
        }
    }
    mHost->UpdateController(NULL);
    return NOERROR;
}

//================================================================================
//                      RemoteController::EventHandler
//================================================================================
RemoteController::EventHandler::EventHandler(
    /* [in] */ RemoteController* host,
    /* [in] */ ILooper* looper)
    : mHost(host)
{
    Handler::constructor(looper);
}

RemoteController::EventHandler::~EventHandler()
{}

ECode RemoteController::EventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    Int32 arg1;
    msg->GetArg1(&arg1);
    Int32 arg2;
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<IPendingIntent> pi = IPendingIntent::Probe(obj);
    AutoPtr<PlaybackInfo> bi = (PlaybackInfo*)IObject::Probe(obj);
    switch(what) {
        case 0:
            mHost->OnNewPendingIntent(arg1, pi.Get());
            break;
        case 1:
            mHost->OnNewPlaybackInfo(arg1, bi.Get());
            break;
        case 2:
            mHost->OnNewTransportInfo(arg1, arg2);
            break;
        case 3:
            mHost->OnNewMetadata(arg1, IBundle::Probe(obj));
            break;
        case 4:
            mHost->OnClientChange(arg1, arg2 == 1);
            break;
        case 5:
            mHost->OnDisplayEnable(arg1 == 1);
            break;
        case 6:
            // same as new playback info but using new apis
            mHost->OnNewPlaybackState(IPlaybackState::Probe(obj));
            break;
        case 7:
            mHost->OnNewMediaMetadata(IMediaMetadata::Probe(obj));
            break;
        default:
            Slogger::E(TAG, "unknown event %d", what);
    }
    return NOERROR;
}

//================================================================================
//                      RemoteController::PlaybackInfo
//================================================================================
RemoteController::PlaybackInfo::PlaybackInfo(
    /* [in] */ Int32 state,
    /* [in] */ Int64 stateChangeTimeMs,
    /* [in] */ Int64 currentPosMs,
    /* [in] */ Float speed)
{
    mState = state;
    mStateChangeTimeMs = stateChangeTimeMs;
    mCurrentPosMs = currentPosMs;
    mSpeed = speed;
}

RemoteController::PlaybackInfo::~PlaybackInfo()
{}

CAR_INTERFACE_IMPL(RemoteController::PlaybackInfo, Object, IPlaybackInfo)

//================================================================================
//                      RemoteController::MetadataEditor
//================================================================================
RemoteController::MetadataEditor::MetadataEditor(
    /* [in] */ RemoteController* host)
    : mHost(host)
{}

RemoteController::MetadataEditor::MetadataEditor(
    /* [in] */ RemoteController* host,
    /* [in] */ IBundle* metadata,
    /* [in] */ Int64 editableKeys)
{
    mHost = host;
    mEditorMetadata = metadata;
    mEditableKeys = editableKeys;

    AutoPtr<IParcelable> pl;
    metadata->GetParcelable(
            StringUtils::ToString(IMediaMetadataEditor::BITMAP_KEY_ARTWORK), (IParcelable**)&pl);
    mEditorArtwork = IBitmap::Probe(pl);
    if (mEditorArtwork != NULL) {
        CleanupBitmapFromBundle(IMediaMetadataEditor::BITMAP_KEY_ARTWORK);
    }

    mMetadataChanged = TRUE;
    mArtworkChanged = TRUE;
    mApplied = FALSE;
}

CAR_INTERFACE_IMPL_2(RemoteController::MetadataEditor, Object, IMediaMetadataEditor, IRemoteControllerMetadataEditor)

ECode RemoteController::MetadataEditor::Apply()
{
    // "applying" a metadata bundle in RemoteController is only for sending edited
    // key values back to the RemoteControlClient, so here we only care about the only
    // editable key we support: RATING_KEY_BY_USER
    if (!mMetadataChanged) {
        return NOERROR;
    }
    {    AutoLock syncLock(mHost->mInfoLock);
        if (mHost->mCurrentSession != NULL) {
            Boolean flag = FALSE;
            mEditorMetadata->ContainsKey(
                    StringUtils::ToString(IMediaMetadataEditor::RATING_KEY_BY_USER), &flag);
            if (flag) {
                AutoPtr<IInterface> obj;
                MediaMetadataEditor::GetObject(
                        IMediaMetadataEditor::RATING_KEY_BY_USER, NULL, (IInterface**)&obj);
                AutoPtr<IRating> rating = IRating::Probe(obj);
                if (rating != NULL) {
                    AutoPtr<IMediaControllerTransportControls> ret;
                    mHost->mCurrentSession->GetTransportControls((IMediaControllerTransportControls**)&ret);
                    ret->SetRating(rating.Get());
                }
            }
        }
    }
    // NOT setting mApplied to true as this type of MetadataEditor will be applied
    // multiple times, whenever the user of a RemoteController needs to change the
    // metadata (e.g. user changes the rating of a song more than once during playback)
    mApplied = FALSE;
    return NOERROR;
}

void RemoteController::MetadataEditor::CleanupBitmapFromBundle(
    /* [in] */ Int32 key)
{
    Int32 ret;
    METADATA_KEYS_TYPE->Get(key, METADATA_TYPE_INVALID, &ret);
    if (ret == METADATA_TYPE_BITMAP) {
        mEditorMetadata->Remove(StringUtils::ToString(key));
    }
}

//================================================================================
//                      RemoteController
//================================================================================
const Int32 RemoteController::MAX_BITMAP_DIMENSION = 512;
const Int32 RemoteController::TRANSPORT_UNKNOWN = 0;
const String RemoteController::TAG(String("RemoteController"));
const Boolean RemoteController::DEBUG = FALSE;
const Boolean RemoteController::USE_SESSIONS = TRUE;
/*const*/ Object RemoteController::mGenLock;
/*const*/ Object RemoteController::mInfoLock;

const Int32 RemoteController::MSG_NEW_PENDING_INTENT = 0;
const Int32 RemoteController::MSG_NEW_PLAYBACK_INFO =  1;
const Int32 RemoteController::MSG_NEW_TRANSPORT_INFO = 2;
const Int32 RemoteController::MSG_NEW_METADATA = 3; // msg always has non-null obj parameter
const Int32 RemoteController::MSG_CLIENT_CHANGE = 4;
const Int32 RemoteController::MSG_DISPLAY_ENABLE = 5;
const Int32 RemoteController::MSG_NEW_PLAYBACK_STATE = 6;
const Int32 RemoteController::MSG_NEW_MEDIA_METADATA = 7;

/** If the msg is already queued, replace it with this one. */
const Int32 RemoteController::SENDMSG_REPLACE = 0;
/** If the msg is already queued, ignore this one and leave the old. */
const Int32 RemoteController::SENDMSG_NOOP = 1;
/** If the msg is already queued, queue this one and leave the old. */
const Int32 RemoteController::SENDMSG_QUEUE = 2;

RemoteController::RemoteController()
    : mMaxBitmapDimension(0)
    , mClientGenerationIdCurrent(0)
    , mIsRegistered(FALSE)
    , mArtworkWidth(-1)
    , mArtworkHeight(-1)
    , mEnabled(TRUE)
{}

RemoteController::~RemoteController()
{}

CAR_INTERFACE_IMPL(RemoteController, Object, IRemoteController)

ECode RemoteController::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IRemoteControllerOnClientUpdateListener* updateListener)
{
    ECode ec = constructor(context, updateListener, NULL);
    if (SUCCEEDED(ec)) {
        return ec;
    } else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

ECode RemoteController::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IRemoteControllerOnClientUpdateListener* updateListener,
    /* [in] */ ILooper* looper)
{
    if (context == NULL) {
        Slogger::E(TAG, "Invalid NULL Context");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (updateListener == NULL) {
        Slogger::E(TAG, "Invalid NULL OnClientUpdateListener");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (looper != NULL) {
        mEventHandler = new EventHandler(this, looper);
    } else {
        AutoPtr<ILooper> l = Looper::GetMyLooper();
        if (l != NULL) {
            mEventHandler = new EventHandler(this, l.Get());
        } else {
            Slogger::E(TAG, "Calling thread not associated with a looper");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    mOnClientUpdateListener = updateListener;
    mContext = context;
    mRcd = new RcDisplay(this);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioManager = IAudioManager::Probe(obj);

    obj = NULL;
    context->GetSystemService(IContext::MEDIA_SESSION_SERVICE, (IInterface**)&obj);
    mSessionManager = IMediaSessionManager::Probe(obj);
    mSessionListener = new TopTransportSessionListener(this);

    if (CActivityManager::IsLowRamDeviceStatic()) {
        mMaxBitmapDimension = MAX_BITMAP_DIMENSION;
    } else {
        AutoPtr<IResources> src;
        context->GetResources((IResources**)&src);
        AutoPtr<IDisplayMetrics> dm;
        src->GetDisplayMetrics((IDisplayMetrics**)&dm);
        Int32 widthPixels;
        Int32 heightPixels;
        dm->GetWidthPixels(&widthPixels);
        dm->GetHeightPixels(&heightPixels);
        mMaxBitmapDimension = Elastos::Core::Math::Max(widthPixels, heightPixels);
    }
    return NOERROR;
}

ECode RemoteController::GetRemoteControlClientPackageName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String packageName;
    if (USE_SESSIONS) {
        {    AutoLock syncLock(mInfoLock);
            if (mCurrentSession != NULL) {
                return mCurrentSession->GetPackageName(&packageName);
            } else {
                *result = NULL;
                return NOERROR;
            }
        }
    } else {
        if (mClientPendingIntentCurrent != NULL) {
            return mClientPendingIntentCurrent->GetCreatorPackage(&packageName);
        } else {
            *result = NULL;
            return NOERROR;
        }
    }
}

ECode RemoteController::GetEstimatedMediaPosition(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    if (USE_SESSIONS) {
        {    AutoLock syncLock(mInfoLock);
            if (mCurrentSession != NULL) {
                AutoPtr<IPlaybackState> state;
                mCurrentSession->GetPlaybackState((IPlaybackState**)&state);
                if (state != NULL) {
                    return state->GetPosition(result);
                }
            }
        }
    } else {
        AutoPtr<PlaybackInfo> lastPlaybackInfo;
        {    AutoLock syncLock(mInfoLock);
            lastPlaybackInfo = mLastPlaybackInfo;
        }
        if (lastPlaybackInfo != NULL) {
            Boolean flag = FALSE;
            CRemoteControlClient::PlaybackPositionShouldMove(lastPlaybackInfo->mState, &flag);
            if (!flag) {
                *result = lastPlaybackInfo->mCurrentPosMs;
                return NOERROR;
            }

            // Take the current position at the time of state change and
            // estimate.
            Int64 thenPos = lastPlaybackInfo->mCurrentPosMs;
            if (thenPos < 0) {
                *result = -1;
                return NOERROR;
            }

            Int64 now = SystemClock::GetElapsedRealtime();
            Int64 then = lastPlaybackInfo->mStateChangeTimeMs;
            Int64 sinceThen = now - then;
            Int64 scaledSinceThen = (Int64) (sinceThen * lastPlaybackInfo->mSpeed);
            *result = thenPos + scaledSinceThen;
            return NOERROR;
        }
    }
    *result = -1;
    return NOERROR;
}

ECode RemoteController::SendMediaKeyEvent(
    /* [in] */ IKeyEvent* keyEvent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Int32 keyCode;
    keyEvent->GetKeyCode(&keyCode);
    if (!KeyEvent::IsMediaKey(keyCode)) {
        Slogger::E(TAG, "not a media key event");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (USE_SESSIONS) {
        {    AutoLock syncLock(mInfoLock);
            if (mCurrentSession != NULL) {
                return mCurrentSession->DispatchMediaButtonEvent(keyEvent, result);
            }
            *result = FALSE;
            return NOERROR;
        }
    } else {
        AutoPtr<IPendingIntent> pi;
        {    AutoLock syncLock(mInfoLock);
            if (!mIsRegistered) {
                Slogger::E(TAG,
                        "Cannot use sendMediaKeyEvent() from an unregistered RemoteController");
                *result = FALSE;
                return NOERROR;
            }
            if (!mEnabled) {
                Slogger::E(TAG, "Cannot use sendMediaKeyEvent() from a disabled RemoteController");
                *result = FALSE;
                return NOERROR;
            }
            pi = mClientPendingIntentCurrent;
        }
        if (pi != NULL) {
            AutoPtr<Intent> intent = new Intent();
            intent->constructor(IIntent::ACTION_MEDIA_BUTTON);
            intent->PutExtra(IIntent::EXTRA_KEY_EVENT, IParcelable::Probe(keyEvent));
            // try {
                ECode ec = pi->Send(mContext, 0, intent.Get());
                if (ec == (ECode)E_CANCELED_EXCEPTION) {
                    Slogger::E(TAG, "Error sending intent for media button down: ");
                    *result = FALSE;
                    return NOERROR;
                }
            // } catch (CanceledException e) {
                // Log.e(TAG, "Error sending intent for media button down: ", e);
                // return false;
            // }
        } else {
            Slogger::I(TAG, "No-op when sending key click, no receiver right now");
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode RemoteController::SeekTo(
    /* [in] */ Int64 timeMs,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Logger::E(TAG, "seekTo() in RemoteController");
    if (!mEnabled) {
        Slogger::E(TAG, "Cannot use seekTo() from a disabled RemoteController");
        *result = FALSE;
        return NOERROR;
    }
    if (timeMs < 0) {
        Slogger::E(TAG, "illegal negative time value");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    {    AutoLock syncLock(mInfoLock);
        if (mCurrentSession != NULL) {
            AutoPtr<IMediaControllerTransportControls> mt;
            mCurrentSession->GetTransportControls((IMediaControllerTransportControls**)&mt);
            mt->SeekTo(timeMs);
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode RemoteController::SetRemoteControlClientPlayItem(
    /* [in] */ Int64 uid,
    /* [in] */ Int32 scope)
{
    Logger::E(TAG, "setRemoteControlClientPlayItem()");
    if (!mEnabled) {
        Logger::E(TAG, "Cannot use setRemoteControlClientPlayItem() from a disabled RemoteController");
        return NOERROR;
    }
    {    AutoLock syncLock(mInfoLock);
        if (mCurrentSession != NULL) {
            AutoPtr<IMediaControllerTransportControls> control;
            mCurrentSession->GetTransportControls((IMediaControllerTransportControls**)&control);
            control->SetRemoteControlClientPlayItem(uid, scope);
        }
    }
    return NOERROR;
}

ECode RemoteController::GetRemoteControlClientNowPlayingEntries()
{
    Logger::E(TAG, "getRemoteControlClientNowPlayingEntries()");
    if (!mEnabled) {
        Logger::E(TAG, "Cannot use getRemoteControlClientNowPlayingEntries() from a disabled RemoteController");
        return NOERROR;
    }
    {    AutoLock syncLock(mInfoLock);
        if (mCurrentSession != NULL) {
            AutoPtr<IMediaControllerTransportControls> control;
            mCurrentSession->GetTransportControls((IMediaControllerTransportControls**)&control);
            control->GetRemoteControlClientNowPlayingEntries();
        }
    }
    return NOERROR;
}

ECode RemoteController::SetRemoteControlClientBrowsedPlayer()
{
    Logger::E(TAG, "setRemoteControlClientBrowsedPlayer()");
    if (!mEnabled) {
        Logger::E(TAG, "Cannot use setRemoteControlClientBrowsedPlayer() from a disabled RemoteController");
        return NOERROR;
    }
    {    AutoLock syncLock(mInfoLock);
        if (mCurrentSession != NULL) {
            AutoPtr<IMediaControllerTransportControls> control;
            mCurrentSession->GetTransportControls((IMediaControllerTransportControls**)&control);
            control->SetRemoteControlClientBrowsedPlayer();
        }
    }
    return NOERROR;
}

ECode RemoteController::SetArtworkConfiguration(
    /* [in] */ Boolean wantBitmap,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    {    AutoLock syncLock(mInfoLock);
        if (wantBitmap) {
            if ((width > 0) && (height > 0)) {
                if (width > mMaxBitmapDimension) { width = mMaxBitmapDimension; }
                if (height > mMaxBitmapDimension) { height = mMaxBitmapDimension; }
                mArtworkWidth = width;
                mArtworkHeight = height;
            } else {
                Slogger::E(TAG, "Invalid dimensions");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        } else {
            mArtworkWidth = -1;
            mArtworkHeight = -1;
        }
    }
    *result = -1;
    return NOERROR;
}

ECode RemoteController::SetArtworkConfiguration(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return SetArtworkConfiguration(TRUE, width, height, result);
}

ECode RemoteController::ClearArtworkConfiguration(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return SetArtworkConfiguration(FALSE, -1, -1, result);
}

ECode RemoteController::SetSynchronizationMode(
    /* [in] */ Int32 sync,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if ((sync != POSITION_SYNCHRONIZATION_NONE) && (sync != POSITION_SYNCHRONIZATION_CHECK)) {
        Slogger::E(TAG, "Unknown synchronization mode %d", sync);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!mIsRegistered) {
        Slogger::E(TAG, "Cannot set synchronization mode on an unregistered RemoteController");
        *result = FALSE;
        return NOERROR;
    }
    mAudioManager->RemoteControlDisplayWantsPlaybackPositionSync(mRcd.Get(),
            POSITION_SYNCHRONIZATION_CHECK == sync);
    *result = TRUE;
    return NOERROR;
}

ECode RemoteController::EditMetadata(
    /* [out] */ IRemoteControllerMetadataEditor** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<MetadataEditor> editor = new MetadataEditor(this);
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    ((MediaMetadataEditor*)editor)->mEditorMetadata = bundle.Get();
    editor->mEditorArtwork = NULL;
    editor->mMetadataChanged = TRUE;
    editor->mArtworkChanged = TRUE;
    editor->mEditableKeys = 0;
    *result = IRemoteControllerMetadataEditor::Probe(editor);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode RemoteController::StartListeningToSessions()
{
    AutoPtr<IComponentName> listenerComponent;
    // CComponentName::New(mContext,
    //     EIID_IRemoteControllerOnClientUpdateListener, (IComponentName**)&listenerComponent);
    AutoPtr<IHandler> handler;
    AutoPtr<ILooper> looper = Looper::GetMyLooper();
    if (looper == NULL) {
        AutoPtr<ILooper> p = Looper::GetMainLooper();
        CHandler::New(p, (IHandler**)&handler);
    }
    Int32 userId = UserHandle::GetMyUserId();
    mSessionManager->AddOnActiveSessionsChangedListener(mSessionListener, listenerComponent.Get(),
            userId, handler);
    AutoPtr<IList> list;
    mSessionManager->GetActiveSessions(listenerComponent.Get(), (IList**)&list);
    mSessionListener->OnActiveSessionsChanged(list.Get());
    if (DEBUG) {
        Slogger::D(TAG, "Registered session listener with component %p for user %d", listenerComponent.Get(), userId);
    }
    return NOERROR;
}

ECode RemoteController::StopListeningToSessions()
{
    mSessionManager->RemoveOnActiveSessionsChangedListener(mSessionListener.Get());
    if (DEBUG) {
        Int32 userId = UserHandle::GetMyUserId();
        Slogger::D(TAG, "Unregistered session listener for user %d", userId);
    }
    return NOERROR;
}

ECode RemoteController::OnFolderInfoBrowsedPlayer(
    /* [in] */ const String& stringUri)
{
    Logger::D(TAG, "RemoteController: onFolderInfoBrowsedPlayer");
    AutoPtr<IRemoteControllerOnClientUpdateListener> l;
    {    AutoLock syncLock(mInfoLock);
        l = mOnClientUpdateListener;
    }
    if (l != NULL) {
        l->OnClientFolderInfoBrowsedPlayer(stringUri);
    }
    return NOERROR;
}

ECode RemoteController::OnNowPlayingEntriesUpdate(
    /* [in] */ ArrayOf<Int64>* playList)
{
    Logger::D(TAG, "RemoteController: onUpdateNowPlayingEntries");
    AutoPtr<IRemoteControllerOnClientUpdateListener> l;
    {    AutoLock syncLock(mInfoLock);
        l = mOnClientUpdateListener;
    }
    if (l != NULL) {
        l->OnClientUpdateNowPlayingEntries(playList);
    }
    return NOERROR;
}

ECode RemoteController::OnNowPlayingContentChange()
{
    Logger::D(TAG, "RemoteController: onNowPlayingContentChange");
    AutoPtr<IRemoteControllerOnClientUpdateListener> l;
    {    AutoLock syncLock(mInfoLock);
        l = mOnClientUpdateListener;
    }
    if (l != NULL) {
        l->OnClientNowPlayingContentChange();
    }
    return NOERROR;
}

ECode RemoteController::OnSetPlayItemResponse(
    /* [in] */ Boolean success)
{
    Logger::D(TAG, "RemoteController: onPlayItemResponse");
    AutoPtr<IRemoteControllerOnClientUpdateListener> l;
    {    AutoLock syncLock(mInfoLock);
        l = mOnClientUpdateListener;
    }
    if (l != NULL) {
        l->OnClientPlayItemResponse(success);
    }
    return NOERROR;
}

ECode RemoteController::SetIsRegistered(
    /* [in] */ Boolean registered)
{
    {    AutoLock syncLock(mInfoLock);
        mIsRegistered = registered;
    }
    return NOERROR;
}

ECode RemoteController::GetRcDisplay(
    /* [out] */ IIRemoteControlDisplay** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IIRemoteControlDisplay::Probe(mRcd);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode RemoteController::GetArtworkSize(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    {    AutoLock syncLock(mInfoLock);
        AutoPtr<ArrayOf<Int32> > size = ArrayOf<Int32>::Alloc(2);
        (*size)[0] = mArtworkWidth;
        (*size)[1] = mArtworkHeight;
        *result = size.Get();
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
}

ECode RemoteController::GetUpdateListener(
    /* [out] */ IRemoteControllerOnClientUpdateListener** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOnClientUpdateListener;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void RemoteController::SendMsg(
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 msg,
    /* [in] */ Int32 existingMsgPolicy,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 delayMs)
{
    if (handler == NULL) {
        Slogger::E(TAG, "NULL event handler, will not deliver message %d", msg);
        return;
    }
    Boolean flag = FALSE;
    if (existingMsgPolicy == SENDMSG_REPLACE) {
        handler->RemoveMessages(msg);
    } else if (existingMsgPolicy == SENDMSG_NOOP && (handler->HasMessages(msg, &flag), flag)) {
        return;
    }
    AutoPtr<IMessage> m;
    handler->ObtainMessage(msg, arg1, arg2, obj, (IMessage**)&m);
    handler->SendMessageDelayed(m.Get(), (Int64)delayMs, &flag);
}

void RemoteController::OnNewPendingIntent(
    /* [in] */ Int32 genId,
    /* [in] */ IPendingIntent* pi)
{
    {    AutoLock syncLock(mGenLock);
        if (mClientGenerationIdCurrent != genId) {
            return;
        }
    }
    {    AutoLock syncLock(mInfoLock);
        mClientPendingIntentCurrent = pi;
    }
}

void RemoteController::OnNewPlaybackInfo(
    /* [in] */ Int32 genId,
    /* [in] */ PlaybackInfo* pi)
{
    {    AutoLock syncLock(mGenLock);
        if (mClientGenerationIdCurrent != genId) {
            return;
        }
    }
    AutoPtr<IRemoteControllerOnClientUpdateListener> l;
    {    AutoLock syncLock(mInfoLock);
        l = mOnClientUpdateListener;
        mLastPlaybackInfo = pi;
    }
    if (l != NULL) {
        if (pi->mCurrentPosMs == IRemoteControlClient::PLAYBACK_POSITION_ALWAYS_UNKNOWN) {
            l->OnClientPlaybackStateUpdate(pi->mState);
        } else {
            l->OnClientPlaybackStateUpdate(pi->mState, pi->mStateChangeTimeMs, pi->mCurrentPosMs,
                    pi->mSpeed);
        }
    }
}

void RemoteController::OnNewTransportInfo(
    /* [in] */ Int32 genId,
    /* [in] */ Int32 transportControlFlags)
{
    {    AutoLock syncLock(mGenLock);
        if (mClientGenerationIdCurrent != genId) {
            return;
        }
    }
    AutoPtr<IRemoteControllerOnClientUpdateListener> l;
    {    AutoLock syncLock(mInfoLock);
        l = mOnClientUpdateListener;
    }
    if (l != NULL) {
        l->OnClientTransportControlUpdate(transportControlFlags);
    }
}

void RemoteController::OnNewMetadata(
    /* [in] */ Int32 genId,
    /* [in] */ IBundle* metadata)
{
    {    AutoLock syncLock(mGenLock);
        if (mClientGenerationIdCurrent != genId) {
            return;
        }
    }
    AutoPtr<IRemoteControllerOnClientUpdateListener> l;
    AutoPtr<MetadataEditor> metadataEditor;
    // prepare the received Bundle to be used inside a MetadataEditor
    Int64 editableKeys;
    metadata->GetInt64(StringUtils::ToString(IMediaMetadataEditor::KEY_EDITABLE_MASK),
        0, &editableKeys);
    if (editableKeys != 0) {
        metadata->Remove(StringUtils::ToString(IMediaMetadataEditor::KEY_EDITABLE_MASK));
    }
    {    AutoLock syncLock(mInfoLock);
        l = mOnClientUpdateListener;
        if ((mMetadataEditor != NULL) && (mMetadataEditor->mEditorMetadata != NULL)) {
            if (TO_IINTERFACE(mMetadataEditor->mEditorMetadata) != TO_IINTERFACE(metadata)) {
                // existing metadata, merge existing and new
                mMetadataEditor->mEditorMetadata->PutAll(metadata);
            }

            AutoPtr<IParcelable> pl;
            metadata->GetParcelable(StringUtils::ToString(IMediaMetadataEditor::BITMAP_KEY_ARTWORK),
                (IParcelable**)&pl);
            AutoPtr<IBitmap> bitmap = IBitmap::Probe(pl);
            mMetadataEditor->PutBitmap(IMediaMetadataEditor::BITMAP_KEY_ARTWORK, bitmap.Get());
            mMetadataEditor->CleanupBitmapFromBundle(IMediaMetadataEditor::BITMAP_KEY_ARTWORK);
        } else {
            mMetadataEditor = new MetadataEditor(this, metadata, editableKeys);
        }
        metadataEditor = mMetadataEditor;
    }
    if (l != NULL) {
        l->OnClientMetadataUpdate(metadataEditor);
    }
}

void RemoteController::OnClientChange(
    /* [in] */ Int32 genId,
    /* [in] */ Boolean clearing)
{
    {    AutoLock syncLock(mGenLock);
        if (mClientGenerationIdCurrent != genId) {
            return;
        }
    }
    AutoPtr<IRemoteControllerOnClientUpdateListener> l;
    {    AutoLock syncLock(mInfoLock);
        l = mOnClientUpdateListener;
        mMetadataEditor = NULL;
    }
    if (l != NULL) {
        l->OnClientChange(clearing);
    }
}

void RemoteController::OnDisplayEnable(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IRemoteControllerOnClientUpdateListener> l;
    {    AutoLock syncLock(mInfoLock);
        mEnabled = enabled;
        l = mOnClientUpdateListener;
    }
    if (!enabled) {
        // when disabling, reset all info sent to the userId
        Int32 genId;
        {    AutoLock syncLock(mGenLock);
            genId = mClientGenerationIdCurrent;
        }
        Int64 realTime = SystemClock::GetElapsedRealtime();
        // send "stopped" state, happened "now", playback position is 0, speed 0.0f
        AutoPtr<PlaybackInfo> pi = new PlaybackInfo(IRemoteControlClient::PLAYSTATE_STOPPED,
                realTime /*stateChangeTimeMs*/,
                0 /*currentPosMs*/, 0.0f /*speed*/);
        SendMsg(mEventHandler, MSG_NEW_PLAYBACK_INFO, SENDMSG_REPLACE,
                genId /*arg1*/, 0 /*arg2, ignored*/, IPlaybackInfo::Probe(pi) /*obj*/, 0 /*delay*/);
        // send "blank" transport control info: no controls are supported
        SendMsg(mEventHandler, MSG_NEW_TRANSPORT_INFO, SENDMSG_REPLACE,
                genId /*arg1*/, 0 /*arg2, no flags*/,
                NULL /*obj, ignored*/, 0 /*delay*/);
        // send dummy metadata with empty string for title and artist, duration of 0
        AutoPtr<IBundle> metadata;
        CBundle::New(3, (IBundle**)&metadata);
        metadata->PutString(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_TITLE), String(""));
        metadata->PutString(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_ARTIST), String(""));
        metadata->PutInt64(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_DURATION), 0);
        SendMsg(mEventHandler, MSG_NEW_METADATA, SENDMSG_QUEUE,
                genId /*arg1*/, 0 /*arg2, ignored*/, metadata /*obj*/, 0 /*delay*/);
    }
}

void RemoteController::UpdateController(
    /* [in] */ IMediaController* controller)
{
    if (DEBUG) {
        Slogger::D(TAG, "Updating controller to %p previous controller is %p", controller, mCurrentSession.Get());
    }
    {    AutoLock syncLock(mInfoLock);
        AutoPtr<IMediaSessionToken> msToken;
        controller->GetSessionToken((IMediaSessionToken**)&msToken);
        AutoPtr<IMediaSessionToken> _msToken;
        mCurrentSession->GetSessionToken((IMediaSessionToken**)&_msToken);
        if (controller == NULL) {
            if (mCurrentSession != NULL) {
                Logger::V(TAG, "Updating current controller as null");
                String packageName;
                mCurrentSession->GetPackageName(&packageName);
                mAudioManager->UpdateMediaPlayerList(packageName, FALSE);

                mCurrentSession->UnregisterCallback(mSessionCb.Get());
                mCurrentSession = NULL;
                SendMsg(mEventHandler, MSG_CLIENT_CHANGE, SENDMSG_REPLACE,
                        0 /* genId */, 1 /* clearing */, NULL /* obj */, 0 /* delay */);
            }
        } else if (mCurrentSession == NULL
                || !(msToken == _msToken)) {
            if (mCurrentSession != NULL) {
                String packageName;
                controller->GetPackageName(&packageName);
                String currSessPackageName;
                mCurrentSession->GetPackageName(&currSessPackageName);
                Logger::V(TAG, String("Updating current controller package as ") +
                     packageName + String(" from ") + currSessPackageName);
                mCurrentSession->UnregisterCallback(mSessionCb.Get());
            }
            else {
                String packageName;
                controller->GetPackageName(&packageName);
                Logger::V(TAG, String("Updating current controller package as ") +
                    packageName + String(" from null"));
            }
            SendMsg(mEventHandler, MSG_CLIENT_CHANGE, SENDMSG_REPLACE,
                    0 /* genId */, 0 /* clearing */, NULL /* obj */, 0 /* delay */);
            mCurrentSession = controller;
            mCurrentSession->RegisterCallback(mSessionCb.Get(), IHandler::Probe(mEventHandler));

            String currSessPackageName;
            mCurrentSession->GetPackageName(&currSessPackageName);
            mAudioManager->UpdateMediaPlayerList(currSessPackageName, TRUE);

            AutoPtr<IPlaybackState> state;
            controller->GetPlaybackState((IPlaybackState**)&state);
            SendMsg(mEventHandler, MSG_NEW_PLAYBACK_STATE, SENDMSG_REPLACE,
                    0 /* genId */, 0, state.Get() /* obj */, 0 /* delay */);

            AutoPtr<IMediaMetadata> metadata;
            controller->GetMetadata((IMediaMetadata**)&metadata);
            SendMsg(mEventHandler, MSG_NEW_MEDIA_METADATA, SENDMSG_REPLACE,
                    0 /* arg1 */, 0 /* arg2 */, metadata.Get() /* obj */, 0 /* delay */);
        }
        // else same controller, no need to update
    }
}

void RemoteController::OnNewPlaybackState(
    /* [in] */ IPlaybackState* state)
{
    AutoPtr<IRemoteControllerOnClientUpdateListener> l;
    {    AutoLock syncLock(mInfoLock);
        l = mOnClientUpdateListener;
    }
    if (l != NULL) {
        Int32 playstate;
        if(state == NULL)
        {
            playstate = IRemoteControlClient::PLAYSTATE_NONE;
        } else {
            Int32 ist;
            state->GetState(&ist);
            Int32 ret;
            CPlaybackState::GetRccStateFromState(ist, &ret);
        }

        Int64 position;
        state->GetPosition(&position);
        if (state == NULL || position == IPlaybackState::PLAYBACK_POSITION_UNKNOWN) {
            l->OnClientPlaybackStateUpdate(playstate);
        } else {
            Int64 pUpdateTime;
            state->GetLastPositionUpdateTime(&pUpdateTime);
            Float speed;
            state->GetPlaybackSpeed(&speed);
            l->OnClientPlaybackStateUpdate(playstate, pUpdateTime, position, speed);
        }
        if (state != NULL) {
            Int64 actions;
            state->GetActions(&actions);
            Int32 rc;
            CPlaybackState::GetRccControlFlagsFromActions(actions, &rc);
            l->OnClientTransportControlUpdate(rc);
        }
    }
}

void RemoteController::OnNewMediaMetadata(
    /* [in] */ IMediaMetadata* metadata)
{
    if (metadata == NULL) {
        // RemoteController only handles non-null metadata
        return;
    }
    AutoPtr<IRemoteControllerOnClientUpdateListener> l;
    AutoPtr<MetadataEditor> metadataEditor;
    // prepare the received Bundle to be used inside a MetadataEditor
    {    AutoLock syncLock(mInfoLock);
        l = mOnClientUpdateListener;
        Int32 ratingType;
        mCurrentSession->GetRatingType(&ratingType);
        Boolean canRate = mCurrentSession != NULL
                && ratingType != IRating::RATING_NONE;
        Int64 editableKeys = canRate ? IMediaMetadataEditor::RATING_KEY_BY_USER : 0;
        AutoPtr<IBundle> legacyMetadata;
        CMediaSessionLegacyHelper::GetOldMetadata(metadata,
                mArtworkWidth, mArtworkHeight, (IBundle**)&legacyMetadata);
        mMetadataEditor = new MetadataEditor(this, legacyMetadata.Get(), editableKeys);
        metadataEditor = mMetadataEditor;
    }
    if (l != NULL) {
        l->OnClientMetadataUpdate(IRemoteControllerMetadataEditor::Probe(metadataEditor));
    }
}

} // namespace Elastos
} // namespace Droid
} // namespace Media
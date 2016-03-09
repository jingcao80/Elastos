
#include "elastos/droid/media/CRemoteControlClient.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/media/session/CPlaybackState.h"
#include "elastos/droid/media/session/CPlaybackStateBuilder.h"
#include "elastos/droid/media/session/MediaSessionCallback.h"
#include "elastos/droid/media/CMediaMetadataBuilder.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CLooperHelper.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/CServiceManager.h"
#include <elastos/core/Math.h>
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CRectF.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::Session::EIID_IMediaSessionCallback;
using Elastos::Droid::Media::Session::CPlaybackState;
using Elastos::Droid::Media::Session::CPlaybackStateBuilder;
using Elastos::Droid::Media::Session::IPlaybackStateBuilder;
using Elastos::Droid::Media::Session::MediaSessionCallback;
using Elastos::Droid::Media::CMediaMetadataBuilder;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::BitmapConfig_NONE;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::CRectF;

namespace Elastos {
namespace Droid {
namespace Media {

const String CRemoteControlClient::TAG("RemoteControlClient");
const Boolean CRemoteControlClient::DEBUG = FALSE;

const Int32 CRemoteControlClient::PLAYBACK_TYPE_MIN = IRemoteControlClient::PLAYBACK_TYPE_LOCAL;
const Int32 CRemoteControlClient::PLAYBACK_TYPE_MAX = IRemoteControlClient::PLAYBACK_TYPE_REMOTE;
const Int64 CRemoteControlClient::POSITION_REFRESH_PERIOD_PLAYING_MS = 15000;
const Int64 CRemoteControlClient::POSITION_REFRESH_PERIOD_MIN_MS = 2000;
const Int32 CRemoteControlClient::MSG_POSITION_DRIFT_CHECK = 11;
const Int64 CRemoteControlClient::POSITION_DRIFT_MAX_MS = 500;
//----------------------------------------
//    CRemoteControlClient::MetadataEditor
//----------------------------------------

CAR_INTERFACE_IMPL_2(CRemoteControlClient::MetadataEditor, Object, IRemoteControlClientMetadataEditor, IMediaMetadataEditor);

CRemoteControlClient::MetadataEditor::MetadataEditor(
    /* [in] */ CRemoteControlClient* owner)
    : mOwner(owner)
{}

ECode CRemoteControlClient::MetadataEditor::Clone(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    assert(0);
//    throw new CloneNotSupportedException();
    return E_NOT_IMPLEMENTED;
}

/*synchronized*/
ECode CRemoteControlClient::MetadataEditor::PutString(
    /* [in] */ Int32 key,
    /* [in] */ const String& value,
    /* [out] */ IRemoteControlClientMetadataEditor** result)
{
    VALIDATE_NOT_NULL(result);
    MediaMetadataEditor::PutString(key, value);

    if (mMetadataBuilder != NULL) {
        // MediaMetadata supports all the same fields as MetadataEditor
        String metadataKey;
        CMediaMetadata::GetKeyFromMetadataEditorKey(key, &metadataKey);
        // But just in case, don't add things we don't understand
        if (!metadataKey.IsNull()) {
            mMetadataBuilder->PutText(metadataKey, StringUtils::ParseCharSequence(value).Get());
        }
    }

    *result = this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/*synchronized*/
ECode CRemoteControlClient::MetadataEditor::PutInt64(
    /* [in] */ Int32 key,
    /* [in] */ Int64 value,
    /* [out] */ IRemoteControlClientMetadataEditor** result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mThisLock);

    MediaMetadataEditor::PutInt64(key, value);
    if (mMetadataBuilder != NULL) {
        // MediaMetadata supports all the same fields as MetadataEditor
        String metadataKey;
        CMediaMetadata::GetKeyFromMetadataEditorKey(key, &metadataKey);
        // But just in case, don't add things we don't understand
        if (!metadataKey.IsNull()) {
            mMetadataBuilder->PutLong(metadataKey, value);
        }
    }
    *result = this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/*synchronized*/
ECode CRemoteControlClient::MetadataEditor::PutBitmap( // throws IllegalArgumentException
    /* [in] */ Int32 key,
    /* [in] */ IBitmap* bitmap,
    /* [out] */ IRemoteControlClientMetadataEditor** result)
{
    VALIDATE_NOT_NULL(result);
    MediaMetadataEditor::PutBitmap(key, bitmap);
    AutoLock lock(mThisLock);

    if (mMetadataBuilder != NULL) {
        // MediaMetadata supports all the same fields as MetadataEditor
        String metadataKey;
        CMediaMetadata::GetKeyFromMetadataEditorKey(key, &metadataKey);
        // But just in case, don't add things we don't understand
        if (!metadataKey.IsNull()) {
            mMetadataBuilder->PutBitmap(metadataKey, bitmap);
        }
    }
    *result = this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRemoteControlClient::MetadataEditor::PutObject(
    /* [in] */ Int32 key,
    /* [in] */ IInterface* object,
    /* [out] */ IRemoteControlClientMetadataEditor** result)
{
    VALIDATE_NOT_NULL(result);
    MediaMetadataEditor::PutObject(key, object);
    if (mMetadataBuilder != NULL &&
        (key == IMediaMetadataEditor::RATING_KEY_BY_USER ||
        key == IMediaMetadataEditor::RATING_KEY_BY_OTHERS)) {
        String metadataKey;
        CMediaMetadata::GetKeyFromMetadataEditorKey(key, &metadataKey);
        if (!metadataKey.IsNull()) {
            mMetadataBuilder->PutRating(metadataKey, IRating::Probe(object));
        }
    }
    *result = this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

/*synchronized*/
ECode CRemoteControlClient::MetadataEditor::Clear()
{
    AutoLock lock(mThisLock);
    return MediaMetadataEditor::Clear();
}

/*synchronized*/
ECode CRemoteControlClient::MetadataEditor::Apply()
{
    AutoLock lock(mThisLock);

    if (mApplied) {
        Logger::E(CRemoteControlClient::TAG, "Can't apply a previously applied MetadataEditor");
        return NOERROR;
    }
    synchronized(mThisLock) {
        // Still build the old metadata so when creating a new editor
        // you get the expected values.
        // assign the edited data
        mOwner->mMetadata = NULL;
        CBundle::New(mEditorMetadata, (IBundle**)&mOwner->mMetadata);
        // add the information about editable keys
        mOwner->mMetadata->PutInt64(StringUtils::ToString(IMediaMetadataEditor::KEY_EDITABLE_MASK), mEditableKeys);
        if ((mOwner->mOriginalArtwork != NULL) && (!Object::Equals(mOwner->mOriginalArtwork, mEditorArtwork))) {
                mOwner->mOriginalArtwork->Recycle();
        }
        mOwner->mOriginalArtwork = mEditorArtwork;
        mEditorArtwork = NULL;

        // USE_SESSIONS
        if (mOwner->mSession != NULL && mMetadataBuilder != NULL) {
            mMetadataBuilder->Build((IMediaMetadata**)&(mOwner->mMediaMetadata));
            mOwner->mSession->SetMetadata(mOwner->mMediaMetadata);
        }
        mApplied = TRUE;
    }
    return NOERROR;
}

CRemoteControlClient::CRemoteControlClient()
    : mPlaybackState(PLAYSTATE_NONE)
    , mPlaybackStateChangeTimeMs(0)
    , mPlaybackPositionMs(PLAYBACK_POSITION_INVALID)
    , mPlaybackSpeed(PLAYBACK_SPEED_1X)
    , mTransportControlFlags(FLAGS_KEY_MEDIA_NONE)
    , mCurrentClientGenId(-1)
    , mInternalClientGenId(-2)
    , mNeedsPositionSync(FALSE)
{
}

CRemoteControlClient::~CRemoteControlClient()
{}

CAR_OBJECT_IMPL(CRemoteControlClient);

CAR_INTERFACE_IMPL(CRemoteControlClient, Object, IRemoteControlClient);

ECode CRemoteControlClient::constructor(
    /* [in] */ IPendingIntent* mediaButtonIntent)
{
    CBundle::New((IBundle**)&mMetadata);

    mRcMediaIntent = mediaButtonIntent;

    AutoPtr<ILooper> looper;
    AutoPtr<ILooperHelper> looperHelper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&looperHelper);
    if ((looperHelper->GetMyLooper((ILooper**)&looper), looper) != NULL) {
        mEventHandler = new EventHandler(this, looper, this);
    } else if ((looperHelper->GetMainLooper((ILooper**)&looper), looper) != NULL) {
        mEventHandler = new EventHandler(this, looper, this);
    } else {
        mEventHandler = NULL;
        Logger::E(TAG, "RemoteControlClient() couldn't find main application thread");
    }

    AutoPtr<MediaSessionCallback> ms = new MediaSessionCallback();
    mTransportListener = IMediaSessionCallback::Probe(ms);
    return NOERROR;
}

ECode CRemoteControlClient::constructor(
    /* [in] */ IPendingIntent* mediaButtonIntent,
    /* [in] */ ILooper* looper)
{
    mRcMediaIntent = mediaButtonIntent;

    mEventHandler = new EventHandler(this, looper, this);
    return NOERROR;
}

ECode CRemoteControlClient::RegisterWithSession(
    /* [in] */ IMediaSessionLegacyHelper * helper)
{
    helper->AddRccListener(mRcMediaIntent, mTransportListener);
    helper->GetSession(mRcMediaIntent.Get(), (IMediaSession**)&mSession);
    return SetTransportControlFlags(mTransportControlFlags);
}

ECode CRemoteControlClient::UnregisterWithSession(
    /* [in] */ IMediaSessionLegacyHelper * helper)
{
    helper->RemoveRccListener(mRcMediaIntent);
    mSession = NULL;
    return NOERROR;
}

ECode CRemoteControlClient::GetMediaSession(
    /* [out] */ IMediaSession ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSession;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRemoteControlClient::EditMetadata(
    /* [in] */ Boolean startEmpty,
    /* [out] */ IRemoteControlClientMetadataEditor** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<MetadataEditor> editor = new MetadataEditor(this);

    if (startEmpty) {
        CBundle::New((IBundle**)(&(editor->mEditorMetadata)));
        editor->mEditorArtwork = NULL;
        editor->mMetadataChanged = TRUE;
        editor->mArtworkChanged = TRUE;
        editor->mEditableKeys = 0;
    }
    else {
        CBundle::New(mMetadata, (IBundle**)(&(editor->mEditorMetadata)));
        editor->mEditorArtwork = mOriginalArtwork;
        editor->mMetadataChanged = FALSE;
        editor->mArtworkChanged = FALSE;
    }
    // USE_SESSIONS
    AutoPtr<IMediaMetadataBuilder> mmb;
    if (startEmpty || mMediaMetadata == NULL) {
        mmb = NULL;
        CMediaMetadataBuilder::New((IMediaMetadataBuilder**)&mmb);
        editor->mMetadataBuilder = mmb.Get();
    } else {
        mmb = NULL;
        CMediaMetadataBuilder::New(mMediaMetadata, (IMediaMetadataBuilder**)&mmb);
        editor->mMetadataBuilder = mmb.Get();
    }

    *result = editor;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRemoteControlClient::SetPlaybackState(
    /* [in] */ Int32 state)
{
    SetPlaybackStateInt(state, PLAYBACK_POSITION_ALWAYS_UNKNOWN, PLAYBACK_SPEED_1X,
        FALSE /* legacy API, converting to method with position and speed */);
    return NOERROR;
}

ECode CRemoteControlClient::SetPlaybackState(
    /* [in] */ Int32 state,
    /* [in] */ Int64 timeInMs,
    /* [in] */ Float playbackSpeed)
{
    SetPlaybackStateInt(state, timeInMs, playbackSpeed, TRUE);
    return NOERROR;
}

void CRemoteControlClient::SetPlaybackStateInt(
    /* [in] */ Int32 state,
    /* [in] */ Int64 timeInMs,
    /* [in] */ Float playbackSpeed,
    /* [in] */ Boolean hasPosition)
{
    synchronized(mCacheLock) {
        if ((mPlaybackState != state) || (mPlaybackPositionMs != timeInMs)
                || (mPlaybackSpeed != playbackSpeed)) {
            // store locally
            mPlaybackState = state;
            // distinguish between an application not knowing the current playback position
            // at the moment and an application using the API where only the playback state
            // is passed, not the playback position.
            if (hasPosition) {
                if (timeInMs < 0) {
                    mPlaybackPositionMs = IRemoteControlClient::PLAYBACK_POSITION_INVALID;
                } else {
                    mPlaybackPositionMs = timeInMs;
                }
            } else {
                mPlaybackPositionMs = IRemoteControlClient::PLAYBACK_POSITION_ALWAYS_UNKNOWN;
            }
            mPlaybackSpeed = playbackSpeed;
            // keep track of when the state change occurred
            mPlaybackStateChangeTimeMs = SystemClock::GetElapsedRealtime();

            // USE_SESSIONS
            if (mSession != NULL) {
                Int32 pbState;
                CPlaybackState::GetStateFromRccState(state, &pbState);
                Int64 position = hasPosition ? mPlaybackPositionMs
                        : IPlaybackState::PLAYBACK_POSITION_UNKNOWN;

                AutoPtr<IPlaybackStateBuilder> bob;
                CPlaybackStateBuilder::New(mSessionPlaybackState.Get(), (IPlaybackStateBuilder**)&bob);
                bob->SetState(pbState, position, playbackSpeed, SystemClock::GetElapsedRealtime());
                bob->SetErrorMessage(NULL);
                bob->Build((IPlaybackState**)&mSessionPlaybackState);
                bob->Build((IPlaybackState **)&mSessionPlaybackState);

                mSession->SetPlaybackState(mSessionPlaybackState.Get());
            }
        }
    }
}

Int64 CRemoteControlClient::GetCheckPeriodFromSpeed(
    /*[in] */ Float speed)
{
    if (Elastos::Core::Math::Abs(speed) <= 1.0f) {
        return POSITION_REFRESH_PERIOD_PLAYING_MS;
    } else {
        return Elastos::Core::Math::Max((Int64)(POSITION_REFRESH_PERIOD_PLAYING_MS / Elastos::Core::Math::Abs(speed)),
                POSITION_REFRESH_PERIOD_MIN_MS);
    }
}

void CRemoteControlClient::OnPositionDriftCheck()
{
    if (DEBUG) {
     Slogger::D(TAG, "onPositionDriftCheck()");
    }
    synchronized(mCacheLock) {
        if ((mEventHandler == NULL) || (mPositionProvider == NULL) || !mNeedsPositionSync) {
            return;
        }
        if ((mPlaybackPositionMs < 0) || (mPlaybackSpeed == 0.0f)) {
            if (DEBUG) {
                Slogger::D(TAG, " no valid position or 0 speed, no check needed");
            }
            return;
        }
        Int64 estPos = mPlaybackPositionMs + (Int64)
                ((SystemClock::GetElapsedRealtime() - mPlaybackStateChangeTimeMs) / mPlaybackSpeed);
        Int64 actPos;
        mPositionProvider->OnGetPlaybackPosition(&actPos);
        if (actPos >= 0) {
            if (Elastos::Core::Math::Abs(estPos - actPos) > POSITION_DRIFT_MAX_MS) {
                // drift happened, report the new position
                if (DEBUG) {
                    Slogger::W(TAG, " drift detected: actual=%ld  est=%ld", actPos, estPos);
                }
                SetPlaybackState(mPlaybackState, actPos, mPlaybackSpeed);
            } else {
                if (DEBUG) { Slogger::D(TAG, " no drift: actual=%ld  est=%ld", actPos, estPos);
                }
                // no drift, schedule the next drift check
                AutoPtr<IMessage> message;
                mEventHandler->ObtainMessage(MSG_POSITION_DRIFT_CHECK, (IMessage**)&message);
                Boolean flag = FALSE;
                mEventHandler->SendMessageDelayed(message.Get(), GetCheckPeriodFromSpeed(mPlaybackSpeed), &flag);
            }
        } else {
            // invalid position (negative value), can't check for drift
            mEventHandler->RemoveMessages(MSG_POSITION_DRIFT_CHECK);
        }
    }

}

ECode CRemoteControlClient::SetTransportControlFlags(
    /* [in] */ Int32 transportControlFlags)
{
    synchronized(mCacheLock) {
        // store locally
        mTransportControlFlags = transportControlFlags;

        // USE_SESSIONS
        if (mSession != NULL) {
            AutoPtr<IPlaybackStateBuilder> bob;
            CPlaybackStateBuilder::New(mSessionPlaybackState, (IPlaybackStateBuilder**)&bob);
            Int64 vol;
            CPlaybackState::GetActionsFromRccControlFlags(transportControlFlags, &vol);
            bob->SetActions(vol);
            bob->Build((IPlaybackState**)&mSessionPlaybackState);
            return mSession->SetPlaybackState(mSessionPlaybackState);
        }

    }

    return NOERROR;
}

ECode CRemoteControlClient::SetMetadataUpdateListener(
    /* [in] */ IRemoteControlClientOnMetadataUpdateListener * l)
{
    synchronized(mCacheLock) {
        mMetadataUpdateListener = l;
    }
    return NOERROR;
}

ECode CRemoteControlClient::SetPlaybackPositionUpdateListener(
    /* [in] */ IRemoteControlClientOnPlaybackPositionUpdateListener * l)
{
    synchronized(mCacheLock) {
        mPositionUpdateListener = l;
    }
    return NOERROR;
}

ECode CRemoteControlClient::SetOnGetPlaybackPositionListener(
    /* [in] */ IRemoteControlClientOnGetPlaybackPositionListener * l)
{
    synchronized(mCacheLock) {
        mPositionProvider = l;
        Boolean flag = FALSE;
        if ((mPositionProvider != NULL) && (mEventHandler != NULL)
                && (PlaybackPositionShouldMove(mPlaybackState, &flag), flag)) {
            // playback position is already moving, but now we have a position provider,
            // so schedule a drift check right now
            AutoPtr<IMessage> message;
            mEventHandler->ObtainMessage(MSG_POSITION_DRIFT_CHECK, (IMessage**)&message);
            Boolean ret = FALSE;
            return mEventHandler->SendMessageDelayed(
                    message.Get(), 0 /*check now*/, &ret);
        }
    }
    return NOERROR;
}

ECode CRemoteControlClient::PlaybackPositionShouldMove(
    /* [in] */ Int32 playstate,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    switch(playstate) {
        case PLAYSTATE_STOPPED:
        case PLAYSTATE_PAUSED:
        case PLAYSTATE_BUFFERING:
        case PLAYSTATE_ERROR:
        case PLAYSTATE_SKIPPING_FORWARDS:
        case PLAYSTATE_SKIPPING_BACKWARDS:
        {
            *result = FALSE;
            return NOERROR;
        }
        case PLAYSTATE_PLAYING:
        case PLAYSTATE_FAST_FORWARDING:
        case PLAYSTATE_REWINDING:
        default:
        {
            *result = TRUE;
            return NOERROR;
        }

    }
}

ECode CRemoteControlClient::GetRcMediaIntent(
    /* [out] */ IPendingIntent** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRcMediaIntent;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//--------------------------------------
//    CRemoteControlClient::EventHandler
//--------------------------------------

CRemoteControlClient::EventHandler::EventHandler(
    /* [in] */ IRemoteControlClient* rcc,
    /* [in] */ ILooper* looper,
    /* [in] */ CRemoteControlClient* owner)
    : mOwner(owner)
{
}

ECode CRemoteControlClient::EventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case MSG_POSITION_DRIFT_CHECK:
        {
            mOwner->OnPositionDriftCheck();
            break;
        }
        default:
        {
            Logger::E(TAG, "Unknown event %d in RemoteControlClient handler", what);
            break;
        }
    }

    return NOERROR;
}

void CRemoteControlClient::OnSeekTo(
    /* [in] */ Int32 generationId,
    /* [in] */ Int64 timeMs)
{
    synchronized (mCacheLock) {
        if ((mCurrentClientGenId == generationId) && (mPositionUpdateListener != NULL)) {
            mPositionUpdateListener->OnPlaybackPositionUpdate(timeMs);
        }
    }
}

void CRemoteControlClient::OnUpdateMetadata(
    /* [in] */ Int32 generationId,
    /* [in] */ Int32 key,
    /* [in] */ IInterface* value)
{
    synchronized (mCacheLock) {
        if ((mCurrentClientGenId == generationId) && (mMetadataUpdateListener != NULL)) {
                mMetadataUpdateListener->OnMetadataUpdate(key, value);
            }
        }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.App.h"
#include "elastos/droid/media/session/CMediaSessionLegacyHelper.h"
#include "elastos/droid/media/session/CMediaSession.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/view/CKeyEvent.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Core::StringUtils;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CMediaSessionLegacyHelper::MediaButtonListener::MediaButtonListener(
    /* [in] */ IPendingIntent * pi,
    /* [in] */ IContext * context)
    : mPendingIntent(pi)
    , mContext(context)
{
}

ECode CMediaSessionLegacyHelper::MediaButtonListener::OnMediaButtonEvent(
    /* [in] */ IIntent * mediaButtonIntent,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    CMediaSessionLegacyHelper::SendKeyEvent(mPendingIntent, mContext, mediaButtonIntent);
    *result = TRUE;
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::MediaButtonListener::OnPlay()
{
    SendKeyEvent(IKeyEvent::KEYCODE_MEDIA_PLAY);
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::MediaButtonListener::OnPause()
{
    SendKeyEvent(IKeyEvent::KEYCODE_MEDIA_PAUSE);
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::MediaButtonListener::OnSkipToNext()
{
    SendKeyEvent(IKeyEvent::KEYCODE_MEDIA_NEXT);
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::MediaButtonListener::OnSkipToPrevious()
{
    SendKeyEvent(IKeyEvent::KEYCODE_MEDIA_PREVIOUS);
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::MediaButtonListener::OnFastForward()
{
    SendKeyEvent(IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD);
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::MediaButtonListener::OnRewind()
{
    SendKeyEvent(IKeyEvent::KEYCODE_MEDIA_REWIND);
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::MediaButtonListener::OnStop()
{
    SendKeyEvent(IKeyEvent::KEYCODE_MEDIA_STOP);
    return NOERROR;
}

void CMediaSessionLegacyHelper::MediaButtonListener::SendKeyEvent(
    /* [in] */ Int32 keyCode)
{
    AutoPtr<IKeyEvent> ke;
    CKeyEvent::New(IKeyEvent::ACTION_DOWN, keyCode, (IKeyEvent**)&ke);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MEDIA_BUTTON, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);

    intent->PutExtra(IIntent::EXTRA_KEY_EVENT, IParcelable::Probe(ke));
    CMediaSessionLegacyHelper::SendKeyEvent(mPendingIntent, mContext, intent);

    ke = NULL;
    CKeyEvent::New(IKeyEvent::ACTION_UP, keyCode, (IKeyEvent**)&ke);
    intent->PutExtra(IIntent::EXTRA_KEY_EVENT, IParcelable::Probe(ke));
    CMediaSessionLegacyHelper::SendKeyEvent(mPendingIntent, mContext, intent);

    if (DEBUG) {
        // Logger::D(TAG, "Sent " + keyCode + " to pending intent " + mPendingIntent);
    }
}

CMediaSessionLegacyHelper::SessionHolder::SessionCallback::OnMediaButtonEvent(
    /* [in] */ IIntent * mediaButtonIntent,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    if (mHost->mMediaButtonListener != NULL) {
        Boolean b;
        mHost->mMediaButtonListener->OnMediaButtonEvent(mediaButtonIntent, &b);
    }
    *result = TRUE;
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::SessionHolder::SessionCallback::OnPlay()
{
    if (mHost->mMediaButtonListener != NULL) {
        mHost->mMediaButtonListener->OnPlay();
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::SessionHolder::SessionCallback::OnPause()
{
    if (mHost->mMediaButtonListener != NULL) {
        mHost->mMediaButtonListener->OnPause();
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::SessionHolder::SessionCallback::OnSkipToNext()
{
    if (mHost->mMediaButtonListener != NULL) {
        mHost->mMediaButtonListener->OnSkipToNext();
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::SessionHolder::SessionCallback::OnSkipToPrevious()
{
    if (mHost->mMediaButtonListener != NULL) {
        mHost->mMediaButtonListener->OnSkipToPrevious();
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::SessionHolder::SessionCallback::OnFastForward()
{
    if (mHost->mMediaButtonListener != NULL) {
        mHost->mMediaButtonListener->OnFastForward();
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::SessionHolder::SessionCallback::OnRewind()
{
    if (mHost->mMediaButtonListener != NULL) {
        mHost->mMediaButtonListener->OnRewind();
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::SessionHolder::SessionCallback::OnStop()
{
    if (mHost->mMediaButtonListener != NULL) {
        mHost->mMediaButtonListener->OnStop();
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::SessionHolder::SessionCallback::OnSeekTo(
    /* [in] */ Int64 pos)
{
    if (mHost->mRccListener != NULL) {
        mHost->mRccListener->OnSeekTo(pos);
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::SessionHolder::SessionCallback::OnSetRating(
    /* [in] */ IRating * rating)
{
    if (mHost->mRccListener != NULL) {
        mHost->mRccListener->OnSetRating(rating);
    }
    return NOERROR;
}

CMediaSessionLegacyHelper::SessionHolder::SessionHolder(
    /* [in] */ IMediaSession * session,
    /* [in] */ IPendingIntent * pi,
    /* [in] */ CMediaSessionLegacyHelper * host)
    : mSession(session)
    , mPi(pi)
    , mFlags(0)
    , mHost(host)
{
}

ECode CMediaSessionLegacyHelper::SessionHolder::Update()
{
    if (mMediaButtonListener == NULL && mRccListener == NULL) {
        mSession->SetCallback(NULL);
        mSession->ReleaseResources();
        mCb = NULL;
        IMap::Probe(mHost->mSessions)->Remove(mPi);
    }
    else if (mCb == NULL) {
        mCb = new SessionCallback(this);
        AutoPtr<IHandler> handler;
        AutoPtr<ILooper> looper = Looper::GetMainLooper();
        CHandler::New(looper, (IHandler**)&handler);
        mSession->SetCallback(IMediaSessionCallback::Probe(mCb), handler);
    }
    return NOERROR;
}

String CMediaSessionLegacyHelper::TAG("MediaSessionHelper");
Boolean CMediaSessionLegacyHelper::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
Object CMediaSessionLegacyHelper::sLock;
AutoPtr<IMediaSessionLegacyHelper> CMediaSessionLegacyHelper::sInstance;

CAR_INTERFACE_IMPL(CMediaSessionLegacyHelper, Object, IMediaSessionLegacyHelper)

CAR_OBJECT_IMPL(CMediaSessionLegacyHelper)

CMediaSessionLegacyHelper::CMediaSessionLegacyHelper()
{
    AutoPtr<ILooper> looper = Looper::GetMainLooper();
    CHandler::New(looper, (IHandler**)&mHandler);
    CArrayMap::New((IArrayMap**)&mSessions);
}

CMediaSessionLegacyHelper::~CMediaSessionLegacyHelper()
{
}

ECode CMediaSessionLegacyHelper::constructor(
    /* [in] */ IContext * context)
{
    mContext = context;
    context->GetSystemService(IContext::MEDIA_SESSION_SERVICE, (IInterface**)&mSessionManager);
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::GetSession(
    /* [in] */ IPendingIntent * pi,
    /* [out] */ IMediaSession ** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<SessionHolder> holder;
    IMap::Probe(mSessions)->Get(pi, (IInterface**)&result);
    *result = (holder == NULL) ? NULL : holder->mSession;
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::SendMediaButtonEvent(
    /* [in] */ IKeyEvent * keyEvent,
    /* [in] */ Boolean needWakeLock)
{
    if (keyEvent == NULL) {
        Logger::W(TAG, "Tried to send a NULL key event. Ignoring.");
        return NOERROR;
    }
    mSessionManager->DispatchMediaKeyEvent(keyEvent, needWakeLock);
    if (DEBUG) {
        // Logger::D(TAG, "dispatched media key " + keyEvent);
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::SendVolumeKeyEvent(
    /* [in] */ IKeyEvent * keyEvent,
    /* [in] */ Boolean musicOnly)
{
    if (keyEvent == NULL) {
        Logger::W(TAG, "Tried to send a NULL key event. Ignoring.");
        return NOERROR;
    }

    Int32 action;
    keyEvent->GetAction(&action);
    Boolean down = action == IKeyEvent::ACTION_DOWN;
    Boolean up = action == IKeyEvent::ACTION_UP;

    Int32 direction = 0;
    Int32 keyCode;
    keyEvent->GetKeyCode(&keyCode);
    switch (keyCode) {
        case IKeyEvent::KEYCODE_VOLUME_UP:
            direction = IAudioManager::ADJUST_RAISE;
            break;
        case IKeyEvent::KEYCODE_VOLUME_DOWN:
            direction = IAudioManager::ADJUST_LOWER;
            break;
        case IKeyEvent::KEYCODE_VOLUME_MUTE:
            // TODO
            break;
    }
    if ((down || up) && direction != 0) {
        Int32 flags;
        // If this is action up we want to send a beep for non-music events
        if (up) {
            direction = 0;
        }
        if (musicOnly) {
            // This flag is used when the screen is off to only affect
            // active media
            flags = IAudioManager::FLAG_ACTIVE_MEDIA_ONLY;
        }
        else {
            // These flags are consistent with the home screen
            if (up) {
                flags = IAudioManager::FLAG_PLAY_SOUND | IAudioManager::FLAG_VIBRATE;
            }
            else {
                flags = IAudioManager::FLAG_SHOW_UI | IAudioManager::FLAG_VIBRATE;
            }
        }

        mSessionManager->DispatchAdjustVolume(IAudioManager::USE_DEFAULT_STREAM_TYPE,
                direction, flags);
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::SendAdjustVolumeBy(
    /* [in] */ Int32 suggestedStream,
    /* [in] */ Int32 delta,
    /* [in] */ Int32 flags)
{
    mSessionManager->DispatchAdjustVolume(suggestedStream, delta, flags);
    if (DEBUG) {
        Logger::D(TAG, "dispatched volume adjustment");
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::IsGlobalPriorityActive(
    /* [out] */ Boolean * result)
{
    return mSessionManager->IsGlobalPriorityActive(result);
}

ECode CMediaSessionLegacyHelper::AddRccListener(
    /* [in] */ IPendingIntent * pi,
    /* [in] */ IMediaSessionCallback * listener)
{
    if (pi == NULL) {
        Logger::W(TAG, "Pending intent was NULL, can't add rcc listener.");
        return NOERROR;
    }
    AutoPtr<SessionHolder> holder = GetHolder(pi, TRUE);
    if (holder == NULL) {
        return NOERROR;
    }
    if (holder->mRccListener != NULL) {
        if (holder->mRccListener.Get() == listener) {
            if (DEBUG) {
                Logger::D(TAG, "addRccListener listener already added.");
            }
            // This is already the registered listener, ignore
            return NOERROR;
        }
    }
    holder->mRccListener = listener;
    holder->mFlags |= IMediaSession::FLAG_HANDLES_TRANSPORT_CONTROLS;
    holder->mSession->SetFlags(holder->mFlags);
    holder->Update();
    if (DEBUG) {
        // Logger::D(TAG, "Added rcc listener for " + pi + ".");
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::RemoveRccListener(
    /* [in] */ IPendingIntent * pi)
{
    if (pi == NULL) {
        return NOERROR;
    }
    AutoPtr<SessionHolder> holder = GetHolder(pi, FALSE);
    if (holder != NULL && holder->mRccListener != NULL) {
        holder->mRccListener = NULL;
        holder->mFlags &= ~IMediaSession::FLAG_HANDLES_TRANSPORT_CONTROLS;
        holder->mSession->SetFlags(holder->mFlags);
        holder->Update();
        if (DEBUG) {
            // Logger::D(TAG, "Removed rcc listener for " + pi + ".");
        }
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::AddMediaButtonListener(
    /* [in] */ IPendingIntent * pi,
    /* [in] */ IComponentName * mbrComponent,
    /* [in] */ IContext * context)
{
    if (pi == NULL) {
        Logger::W(TAG, "Pending intent was NULL, can't addMediaButtonListener.");
        return NOERROR;
    }
    AutoPtr<SessionHolder> holder = GetHolder(pi, TRUE);
    if (holder == NULL) {
        return NOERROR;
    }
    if (holder->mMediaButtonListener != NULL) {
        // Already have this listener registered
        if (DEBUG) {
            // Logger::D(TAG, "addMediaButtonListener already added " + pi);
        }
    }
    holder->mMediaButtonListener = new MediaButtonListener(pi, context);
    // TODO determine if handling transport performer commands should also
    // set this flag
    holder->mFlags |= IMediaSession::FLAG_HANDLES_MEDIA_BUTTONS;
    holder->mSession->SetFlags(holder->mFlags);
    holder->mSession->SetMediaButtonReceiver(pi);
    holder->Update();
    if (DEBUG) {
        // Logger::D(TAG, "addMediaButtonListener added " + pi);
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::RemoveMediaButtonListener(
    /* [in] */ IPendingIntent * pi)
{
    if (pi == NULL) {
        return NOERROR;
    }
    AutoPtr<SessionHolder> holder = GetHolder(pi, FALSE);
    if (holder != NULL && holder->mMediaButtonListener != NULL) {
        holder->mFlags &= ~IMediaSession::FLAG_HANDLES_MEDIA_BUTTONS;
        holder->mSession->SetFlags(holder->mFlags);
        holder->mMediaButtonListener = NULL;

        holder->Update();
        if (DEBUG) {
            // Logger::D(TAG, "removeMediaButtonListener removed " + pi);
        }
    }
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::GetHelper(
    /* [in] */ IContext * context,
    /* [out] */ IMediaSessionLegacyHelper ** result)
{
    VALIDATE_NOT_NULL(result)

    if (DEBUG) {
        // Logger::D(TAG, "Attempting to get helper with context " + context);
    }
    synchronized(sLock) {
        if (sInstance == NULL) {
            CMediaSessionLegacyHelper::New(context, (IMediaSessionLegacyHelper**)&sInstance);
        }
    }
    *result = sInstance;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaSessionLegacyHelper::GetOldMetadata(
    /* [in] */ IMediaMetadata * metadata,
    /* [in] */ Int32 artworkWidth,
    /* [in] */ Int32 artworkHeight,
    /* [out] */ IBundle ** result)
{
    VALIDATE_NOT_NULL(result)

    Boolean includeArtwork = artworkWidth != -1 && artworkHeight != -1;

    Boolean b;
    Boolean b2;
    String str;
    Int64 val;
    AutoPtr<IRating> rating;
    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_ALBUM, &b);
    AutoPtr<IBundle> oldMetadata;
    CBundle::New((IBundle**)&oldMetadata);

    if (b) {
        metadata->GetString(IMediaMetadata::METADATA_KEY_ALBUM, &str);
        oldMetadata->PutString(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_ALBUM), str);
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_ART, &b);
    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_ALBUM_ART, &b2);
    if (includeArtwork && b) {
        AutoPtr<IBitmap> art;
        metadata->GetBitmap(IMediaMetadata::METADATA_KEY_ART, (IBitmap**)&art);
        oldMetadata->PutParcelable(StringUtils::ToString(IMediaMetadataEditor::BITMAP_KEY_ARTWORK),
                IParcelable::Probe(ScaleBitmapIfTooBig(art, artworkWidth, artworkHeight)));
    } else if (includeArtwork && b2) {
        // Fall back to album art if the track art wasn't available
        AutoPtr<IBitmap> art;
        metadata->GetBitmap(IMediaMetadata::METADATA_KEY_ALBUM_ART, (IBitmap**)&art);
        oldMetadata->PutParcelable(StringUtils::ToString(IMediaMetadataEditor::BITMAP_KEY_ARTWORK),
                IParcelable::Probe(ScaleBitmapIfTooBig(art, artworkWidth, artworkHeight)));
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_ALBUM_ARTIST, &b);
    if (b) {
        metadata->GetString(IMediaMetadata::METADATA_KEY_ALBUM_ARTIST, &str);
        oldMetadata->PutString(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_ALBUMARTIST), str);
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_ARTIST, &b);
    if (b) {
        metadata->GetString(IMediaMetadata::METADATA_KEY_ARTIST, &str);
        oldMetadata->PutString(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_ARTIST), str);
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_AUTHOR, &b);
    if (b) {
        metadata->GetString(IMediaMetadata::METADATA_KEY_AUTHOR, &str);
        oldMetadata->PutString(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_AUTHOR), str);
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_COMPILATION, &b);
    if (b) {
        metadata->GetString(IMediaMetadata::METADATA_KEY_COMPILATION, &str);
        oldMetadata->PutString(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_COMPILATION), str);
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_COMPOSER, &b);
    if (b) {
        metadata->GetString(IMediaMetadata::METADATA_KEY_COMPOSER, &str);
        oldMetadata->PutString(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_COMPOSER), str);
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_DATE, &b);
    if (b) {
        metadata->GetString(IMediaMetadata::METADATA_KEY_DATE, &str);
        oldMetadata->PutString(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_DATE), str);
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_DISC_NUMBER, &b);
    if (b) {
        metadata->GetInt64(IMediaMetadata::METADATA_KEY_DISC_NUMBER, &val);
        oldMetadata->PutInt64(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_DISC_NUMBER), val);
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_DURATION, &b);
    if (b) {
        metadata->GetInt64(IMediaMetadata::METADATA_KEY_DURATION, &val);
        oldMetadata->PutInt64(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_DURATION), val);
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_GENRE, &b);
    if (b) {
        metadata->GetString(IMediaMetadata::METADATA_KEY_GENRE, &str);
        oldMetadata->PutString(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_GENRE), str);
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_NUM_TRACKS, &b);
    if (b) {
        metadata->GetInt64(IMediaMetadata::METADATA_KEY_NUM_TRACKS, &val);
        oldMetadata->PutInt64(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_NUM_TRACKS), val);
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_RATING, &b);
    if (b) {
        metadata->GetRating(IMediaMetadata::METADATA_KEY_RATING, (IRating**)&rating);
        oldMetadata->PutParcelable(StringUtils::ToString(IMediaMetadataEditor::RATING_KEY_BY_OTHERS)
            , IParcelable::Probe(rating));
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_USER_RATING, &b);
    if (b) {
        metadata->GetRating(IMediaMetadata::METADATA_KEY_USER_RATING, (IRating**)&rating);
        oldMetadata->PutParcelable(StringUtils::ToString(IMediaMetadataEditor::RATING_KEY_BY_USER)
            , IParcelable::Probe(rating));
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_TITLE, &b);
    if (b) {
        metadata->GetString(IMediaMetadata::METADATA_KEY_TITLE, &str);
        oldMetadata->PutString(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_TITLE), str);
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_TRACK_NUMBER, &b);
    if (b) {
        metadata->GetInt64(IMediaMetadata::METADATA_KEY_TRACK_NUMBER, &val);
        oldMetadata->PutInt64(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_CD_TRACK_NUMBER), val);
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_WRITER, &b);
    if (b) {
        metadata->GetString(IMediaMetadata::METADATA_KEY_WRITER, &str);
        oldMetadata->PutString(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_WRITER), str);
    }

    metadata->ContainsKey(IMediaMetadata::METADATA_KEY_YEAR, &b);
    if (b) {
        metadata->GetString(IMediaMetadata::METADATA_KEY_YEAR, &str);
        oldMetadata->PutString(StringUtils::ToString(IMediaMetadataRetriever::METADATA_KEY_YEAR), str);
    }

    *result = oldMetadata;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IBitmap> CMediaSessionLegacyHelper::ScaleBitmapIfTooBig(
    /* [in] */ IBitmap * bitmap,
    /* [in] */ Int32 maxWidth,
    /* [in] */ Int32 maxHeight)
{
    if (bitmap != NULL) {
        Int32 width;
        bitmap->GetWidth(&width);
        Int32 height;
        bitmap->GetHeight(&height);
        if (width > maxWidth || height > maxHeight) {
            Float scale = Elastos::Core::Math::Min((Float) maxWidth / width, (Float) maxHeight / height);
            Int32 newWidth = Elastos::Core::Math::Round(scale * width);
            Int32 newHeight = Elastos::Core::Math::Round(scale * height);
            Elastos::Droid::Graphics::BitmapConfig newConfig;
            bitmap->GetConfig(&newConfig);
            if (newConfig == Elastos::Droid::Graphics::BitmapConfig_NONE) {
                newConfig = Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
            }
            AutoPtr<IBitmap> outBitmap;
            CBitmap::CreateBitmap(newWidth, newHeight, newConfig, (IBitmap**)&outBitmap);
            AutoPtr<ICanvas> canvas;
            CCanvas::New(outBitmap, (ICanvas**)&canvas);
            AutoPtr<IPaint> paint;
            CPaint::New((IPaint**)&paint);
            paint->SetAntiAlias(TRUE);
            paint->SetFilterBitmap(TRUE);
            Int32 outWidth, outHeight;
            outBitmap->GetWidth(&outWidth);
            outBitmap->GetHeight(&outHeight);
            AutoPtr<IRectF> rectF;
            CRectF::New(0, 0, outWidth, outHeight, (IRectF**)&rectF);
            canvas->DrawBitmap(bitmap, NULL, rectF, paint);
            bitmap = outBitmap;
        }
    }
    return bitmap;
}

AutoPtr<CMediaSessionLegacyHelper::SessionHolder> CMediaSessionLegacyHelper::GetHolder(
    /* [in] */ IPendingIntent * pi,
    /* [in] */ Boolean createIfMissing)
{
    AutoPtr<SessionHolder> holder;
    IMap::Probe(mSessions)->Get(pi, (IInterface**)&holder);
    if (holder == NULL && createIfMissing) {
        String pkg;
        pi->GetCreatorPackage(&pkg);
        AutoPtr<IMediaSession> session;
        CMediaSession::New(mContext, TAG + "-" + pkg, (IMediaSession**)&session);
        session->SetActive(TRUE);
        holder = new SessionHolder(session, pi, this);
        IMap::Probe(mSessions)->Put(pi, (IInterface*)(IObject*)holder);
    }
    return holder;
}

void CMediaSessionLegacyHelper::SendKeyEvent(
    /* [in] */ IPendingIntent * pi,
    /* [in] */ IContext * context,
    /* [in] */ IIntent * intent)
{
    // try {
    pi->Send(context, 0, intent);
    // } catch (CanceledException e) {
    //     Log.e(TAG, "Error sending media key down event:", e);
    //     // Don't bother sending up if down failed
    //     return;
    // }
}

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

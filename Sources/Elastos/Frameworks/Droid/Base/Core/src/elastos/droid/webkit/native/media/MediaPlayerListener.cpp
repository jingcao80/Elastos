
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/native/media/MediaPlayerListener.h"
#include "elastos/droid/webkit/native/media/api/MediaPlayerListener_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IMediaPlayer;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Media {

// These values are mirrored as enums in media/base/android/media_player_bridge.h.
// Please ensure they stay in sync.
const Int32 MediaPlayerListener::MEDIA_ERROR_FORMAT;
const Int32 MediaPlayerListener::MEDIA_ERROR_DECODE;
const Int32 MediaPlayerListener::MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK;
const Int32 MediaPlayerListener::MEDIA_ERROR_INVALID_CODE;

// These values are copied from android media player.
const Int32 MediaPlayerListener::MEDIA_ERROR_MALFORMED;
const Int32 MediaPlayerListener::MEDIA_ERROR_TIMED_OUT;

CAR_INTERFACE_IMPL_7(MediaPlayerListener, Object, IMediaPlayerOnPreparedListener, IMediaPlayerOnCompletionListener,
        IMediaPlayerOnBufferingUpdateListener, IMediaPlayerOnSeekCompleteListener,
        IMediaPlayerOnVideoSizeChangedListener,
        IMediaPlayerOnErrorListener, IAudioManagerOnAudioFocusChangeListener);

MediaPlayerListener::MediaPlayerListener(
    /* [in] */ Int64 nativeMediaPlayerListener,
    /* [in] */ IContext* context)
    : mNativeMediaPlayerListener(nativeMediaPlayerListener)
    , mContext(context)
{
}

//@Override
ECode MediaPlayerListener::OnError(
    /* [in] */ IMediaPlayer* mp,
    /* [in] */ Int32 what,
    /* [in] */ Int32 extra,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Int32 errorType;
    switch (what) {
        case IMediaPlayer::MEDIA_ERROR_UNKNOWN:
            switch (extra) {
                case MEDIA_ERROR_MALFORMED:
                    errorType = MEDIA_ERROR_DECODE;
                    break;
                case MEDIA_ERROR_TIMED_OUT:
                    errorType = MEDIA_ERROR_INVALID_CODE;
                    break;
                default:
                    errorType = MEDIA_ERROR_FORMAT;
                    break;
            }
            break;
        case IMediaPlayer::MEDIA_ERROR_SERVER_DIED:
            errorType = MEDIA_ERROR_DECODE;
            break;
        case IMediaPlayer::MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK:
            errorType = MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK;
            break;
        default:
            // There are some undocumented error codes for android media player.
            // For example, when surfaceTexture got deleted before we setVideoSuface
            // to NULL, mediaplayer will report error -38. These errors should be ignored
            // and not be treated as an error to webkit.
            errorType = MEDIA_ERROR_INVALID_CODE;
            break;
    }

    NativeOnMediaError(mNativeMediaPlayerListener, errorType);

    *result = TRUE;
    return NOERROR;
}

//@Override
ECode MediaPlayerListener::OnVideoSizeChanged(
    /* [in] */ IMediaPlayer* mp,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    NativeOnVideoSizeChanged(mNativeMediaPlayerListener, width, height);
    return NOERROR;
}

//@Override
ECode MediaPlayerListener::OnSeekComplete(
    /* [in] */ IMediaPlayer* mp)
{
    NativeOnSeekComplete(mNativeMediaPlayerListener);
    return NOERROR;
}

//@Override
ECode MediaPlayerListener::OnBufferingUpdate(
    /* [in] */ IMediaPlayer* mp,
    /* [in] */ Int32 percent)
{
    NativeOnBufferingUpdate(mNativeMediaPlayerListener, percent);
    return NOERROR;
}

//@Override
ECode MediaPlayerListener::OnCompletion(
    /* [in] */ IMediaPlayer* mp)
{
    NativeOnPlaybackComplete(mNativeMediaPlayerListener);
    return NOERROR;
}

//@Override
ECode MediaPlayerListener::OnPrepared(
    /* [in] */ IMediaPlayer* mp)
{
    NativeOnMediaPrepared(mNativeMediaPlayerListener);
    return NOERROR;
}

//@Override
ECode MediaPlayerListener::OnAudioFocusChange(
    /* [in] */ Int32 focusChange)
{
    if (focusChange == IAudioManager::AUDIOFOCUS_LOSS ||
            focusChange == IAudioManager::AUDIOFOCUS_LOSS_TRANSIENT) {
        NativeOnMediaInterrupted(mNativeMediaPlayerListener);
    }
    return NOERROR;
}

//@CalledByNative
void MediaPlayerListener::ReleaseResources()
{
    if (mContext != NULL) {
        // Unregister the wish for audio focus.
        AutoPtr<IAudioManager> am;
        mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&am);
        if (am != NULL) {
            Int32 result;
            am->AbandonAudioFocus(this, &result);
        }
    }
}

//@CalledByNative return IMediaPlayerListener
AutoPtr<IInterface> MediaPlayerListener::Create(
    /* [in] */ Int64 nativeMediaPlayerListener,
    /* [in] */ IContext* context,
    /* [in] */ MediaPlayerBridge* mediaPlayerBridge)
{
    AutoPtr<MediaPlayerListener> listener =
        new MediaPlayerListener(nativeMediaPlayerListener, context);
    mediaPlayerBridge->SetOnBufferingUpdateListener(listener);
    mediaPlayerBridge->SetOnCompletionListener(listener);
    mediaPlayerBridge->SetOnErrorListener(listener);
    mediaPlayerBridge->SetOnPreparedListener(listener);
    mediaPlayerBridge->SetOnSeekCompleteListener(listener);
    mediaPlayerBridge->SetOnVideoSizeChangedListener(listener);

    AutoPtr<IAudioManager> am;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&am);
    Int32 result;
    am->RequestAudioFocus(
            listener,
            IAudioManager::STREAM_MUSIC,
            // Request permanent focus.
            IAudioManager::AUDIOFOCUS_GAIN_TRANSIENT_MAY_DUCK,
            &result);

    AutoPtr<IInterface> iListener = TO_IINTERFACE(listener);
    return iListener;
}

/**
 * See media/base/android/media_player_listener.cc for all the following functions.
 */
void MediaPlayerListener::NativeOnMediaError(
    /* [in] */ Int64 nativeMediaPlayerListener,
    /* [in] */ Int32 errorType)
{
    Elastos_MediaPlayerListener_nativeOnMediaError(THIS_PROBE(IInterface), (Handle32)nativeMediaPlayerListener, errorType);
}

void MediaPlayerListener::NativeOnVideoSizeChanged(
    /* [in] */ Int64 nativeMediaPlayerListener,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Elastos_MediaPlayerListener_nativeOnVideoSizeChanged(THIS_PROBE(IInterface), (Handle32)nativeMediaPlayerListener, width, height);
}

void MediaPlayerListener::NativeOnBufferingUpdate(
    /* [in] */ Int64 nativeMediaPlayerListener,
    /* [in] */ Int32 percent)
{
    Elastos_MediaPlayerListener_nativeOnBufferingUpdate(THIS_PROBE(IInterface), (Handle32)nativeMediaPlayerListener, percent);
}

void MediaPlayerListener::NativeOnMediaPrepared(
    /* [in] */ Int64 nativeMediaPlayerListener)
{
    Elastos_MediaPlayerListener_nativeOnMediaPrepared(THIS_PROBE(IInterface), (Handle32)nativeMediaPlayerListener);
}

void MediaPlayerListener::NativeOnPlaybackComplete(
    /* [in] */ Int64 nativeMediaPlayerListener)
{
    Elastos_MediaPlayerListener_nativeOnPlaybackComplete(THIS_PROBE(IInterface), (Handle32)nativeMediaPlayerListener);
}

void MediaPlayerListener::NativeOnSeekComplete(
    /* [in] */ Int64 nativeMediaPlayerListener)
{
    Elastos_MediaPlayerListener_nativeOnSeekComplete(THIS_PROBE(IInterface), (Handle32)nativeMediaPlayerListener);
}

void MediaPlayerListener::NativeOnMediaInterrupted(
    /* [in] */ Int64 nativeMediaPlayerListener)
{
    Elastos_MediaPlayerListener_nativeOnMediaInterrupted(THIS_PROBE(IInterface), (Handle32)nativeMediaPlayerListener);
}

//callback function definition
void MediaPlayerListener::ReleaseResources(
    /* [in] */ IInterface* obj)
{
    AutoPtr<MediaPlayerListener> mObj = (MediaPlayerListener*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("MediaPlayerListener", "MediaPlayerListener::ReleaseResources, mObj is NULL");
        return;
    }
    mObj->ReleaseResources();
}

AutoPtr<IInterface> MediaPlayerListener::Create(
    /* [in] */ Int64 nativeMediaPlayerListener,
    /* [in] */ IInterface* context,
    /* [in] */ IInterface* mediaPlayerBridge)
{
    AutoPtr<IContext> c = IContext::Probe(context);
    AutoPtr<MediaPlayerBridge> m = (MediaPlayerBridge*)(IObject::Probe(mediaPlayerBridge));
    return MediaPlayerListener::Create(nativeMediaPlayerListener, c, m);
}



} // namespace Media
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_MEDIAPLAYERLISTENER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_MEDIAPLAYERLISTENER_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/media/MediaPlayerBridge.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IMediaPlayer;
using Elastos::Droid::Media::IMediaPlayerOnPreparedListener;
using Elastos::Droid::Media::EIID_IMediaPlayerOnPreparedListener;
using Elastos::Droid::Media::IMediaPlayerOnCompletionListener;
using Elastos::Droid::Media::EIID_IMediaPlayerOnCompletionListener;
using Elastos::Droid::Media::IMediaPlayerOnBufferingUpdateListener;
using Elastos::Droid::Media::EIID_IMediaPlayerOnBufferingUpdateListener;
using Elastos::Droid::Media::IMediaPlayerOnSeekCompleteListener;
using Elastos::Droid::Media::EIID_IMediaPlayerOnSeekCompleteListener;
using Elastos::Droid::Media::IMediaPlayerOnVideoSizeChangedListener;
using Elastos::Droid::Media::EIID_IMediaPlayerOnVideoSizeChangedListener;
using Elastos::Droid::Media::IMediaPlayerOnErrorListener;
using Elastos::Droid::Media::EIID_IMediaPlayerOnErrorListener;
using Elastos::Droid::Media::IAudioManagerOnAudioFocusChangeListener;
using Elastos::Droid::Media::EIID_IAudioManagerOnAudioFocusChangeListener;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Media {

// This class implements all the listener interface for android mediaplayer.
// Callbacks will be sent to the native class for processing.
//@JNINamespace("media")
class MediaPlayerListener
    : public Object
    , public IMediaPlayerOnPreparedListener
    , public IMediaPlayerOnCompletionListener
    , public IMediaPlayerOnBufferingUpdateListener
    , public IMediaPlayerOnSeekCompleteListener
    , public IMediaPlayerOnVideoSizeChangedListener
    , public IMediaPlayerOnErrorListener
    , public IAudioManagerOnAudioFocusChangeListener
{
public:
    CAR_INTERFACE_DECL()
    //@Override
    CARAPI OnError(
        /* [in] */ IMediaPlayer* mp,
        /* [in] */ Int32 what,
        /* [in] */ Int32 extra,
        /* [out] */ Boolean *result);

    //@Override
    CARAPI OnVideoSizeChanged(
        /* [in] */ IMediaPlayer* mp,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    //@Override
    CARAPI OnSeekComplete(
        /* [in] */ IMediaPlayer* mp);

    //@Override
    CARAPI OnBufferingUpdate(
        /* [in] */ IMediaPlayer* mp,
        /* [in] */ Int32 percent);

    //@Override
    CARAPI OnCompletion(
        /* [in] */ IMediaPlayer* mp);

    //@Override
    CARAPI OnPrepared(
        /* [in] */ IMediaPlayer* mp);

    //@Override
    CARAPI OnAudioFocusChange(
        /* [in] */ Int32 focusChange);

    //@CalledByNative
    CARAPI_(void) ReleaseResources();

private:
    MediaPlayerListener(
        /* [in] */ Handle64 nativeMediaPlayerListener,
        /* [in] */ IContext* context);

    //@CalledByNative return IMediaPlayerListener
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ Handle64 nativeMediaPlayerListener,
        /* [in] */ IContext* context,
        /* [in] */ MediaPlayerBridge* mediaPlayerBridge);

    /**
     * See media/base/android/media_player_listener.cc for all the following functions.
     */
    CARAPI_(void) NativeOnMediaError(
        /* [in] */ Handle64 nativeMediaPlayerListener,
        /* [in] */ Int32 errorType);

    CARAPI_(void) NativeOnVideoSizeChanged(
        /* [in] */ Handle64 nativeMediaPlayerListener,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI_(void) NativeOnBufferingUpdate(
        /* [in] */ Handle64 nativeMediaPlayerListener,
        /* [in] */ Int32 percent);

    CARAPI_(void) NativeOnMediaPrepared(
        /* [in] */ Handle64 nativeMediaPlayerListener);

    CARAPI_(void) NativeOnPlaybackComplete(
        /* [in] */ Handle64 nativeMediaPlayerListener);

    CARAPI_(void) NativeOnSeekComplete(
        /* [in] */ Handle64 nativeMediaPlayerListener);

    CARAPI_(void) NativeOnMediaInterrupted(
        /* [in] */ Handle64 nativeMediaPlayerListener);

//callback function declaration
public:
    static CARAPI_(void*) ElaMediaPlayerListenerCallback_Init();

private:
    static CARAPI_(void) ReleaseResources(
        /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ Handle64 nativeMediaPlayerListener,
        /* [in] */ IInterface* context,
        /* [in] */ IInterface* mediaPlayerBridge);


public:
    // These values are copied from android media player.
    static const Int32 MEDIA_ERROR_MALFORMED = -1007;
    static const Int32 MEDIA_ERROR_TIMED_OUT = -110;

private:
    // These values are mirrored as enums in media/base/android/media_player_bridge.h.
    // Please ensure they stay in sync.
    static const Int32 MEDIA_ERROR_FORMAT = 0;
    static const Int32 MEDIA_ERROR_DECODE = 1;
    static const Int32 MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 2;
    static const Int32 MEDIA_ERROR_INVALID_CODE = 3;

    // Used to determine the class instance to dispatch the native call to.
    Handle64 mNativeMediaPlayerListener;
    const AutoPtr<IContext> mContext;
};

} // namespace Media
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_MEDIAPLAYERLISTENER_H__

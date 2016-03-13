#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_MEDIAPLAYERBRIDGE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_MEDIAPLAYERBRIDGE_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/AsyncTask.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Media::IMediaPlayer;
using Elastos::Droid::Media::IMediaPlayerOnBufferingUpdateListener;
using Elastos::Droid::Media::IMediaPlayerOnCompletionListener;
using Elastos::Droid::Media::IMediaPlayerOnErrorListener;
using Elastos::Droid::Media::IMediaPlayerOnPreparedListener;
using Elastos::Droid::Media::IMediaPlayerOnSeekCompleteListener;
using Elastos::Droid::Media::IMediaPlayerOnVideoSizeChangedListener;

using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Media {

/**
* A wrapper around android.media.MediaPlayer that allows the native code to use it.
* See media/base/android/media_player_bridge.cc for the corresponding native code.
*/
//@JNINamespace("media")
class MediaPlayerBridge
    : public Object
{
private:
    class LoadDataUriTask
        : public AsyncTask
    {
    public:
        LoadDataUriTask(
            /* [in] */ MediaPlayerBridge* owner,
            /* [in] */ IContext* context,
            /* [in] */ const String& data);

        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    private:
        CARAPI_(void) DeleteFile();

    private:
        MediaPlayerBridge* mOwner;

        const String mData;
        const AutoPtr<IContext> mContext;
        AutoPtr<IFile> mTempFile;
    };

protected:
    class AllowedOperations
        : public Object
    {
    public:
        AllowedOperations(
            /* [in] */ Boolean canPause,
            /* [in] */ Boolean canSeekForward,
            /* [in] */ Boolean canSeekBackward);

        //@CalledByNative("AllowedOperations")
        CARAPI_(Boolean) CanPause();

        //@CalledByNative("AllowedOperations")
        CARAPI_(Boolean) CanSeekForward();

        //@CalledByNative("AllowedOperations")
        CARAPI_(Boolean) CanSeekBackward();

        static CARAPI_(Boolean) CanPause(
            /* [in] */ IInterface* obj);

        static CARAPI_(Boolean) CanSeekForward(
            /* [in] */ IInterface* obj);

        static CARAPI_(Boolean) CanSeekBackward(
            /* [in] */ IInterface* obj);

    private:
        const Boolean mCanPause;
        const Boolean mCanSeekForward;
        const Boolean mCanSeekBackward;
    };

protected:
    MediaPlayerBridge();

    MediaPlayerBridge(
        /* [in] */ Handle64 nativeMediaPlayerBridge);

    //@CalledByNative
    CARAPI_(void) Destroy();

    CARAPI_(AutoPtr<IMediaPlayer>) GetLocalPlayer();

    //@CalledByNative
    CARAPI_(void) SetSurface(
        /* [in] */ ISurface* surface);

    //@CalledByNative
    CARAPI_(Boolean) PrepareAsync();

    //@CalledByNative
    CARAPI_(Boolean) IsPlaying();

    //@CalledByNative
    CARAPI_(Int32) GetVideoWidth();

    //@CalledByNative
    CARAPI_(Int32) GetVideoHeight();

    //@CalledByNative
    CARAPI_(Int32) GetCurrentPosition();

    //@CalledByNative
    CARAPI_(Int32) GetDuration();

    //@CalledByNative
    CARAPI_(void) ReleaseReSources();

    //@CalledByNative
    CARAPI_(void) SetVolume(
        /* [in] */ Double volume);

    //@CalledByNative
    CARAPI_(void) Start();

    //@CalledByNative
    CARAPI_(void) Pause();

    //@CalledByNative
    CARAPI_(void) SeekTo(
        /* [in] */ Int32 msec);

    //@CalledByNative
    CARAPI_(Boolean) SetDataSource(
        /* [in] */ IContext* context,
        /* [in] */ const String& url,
        /* [in] */ const String& cookies,
        /* [in] */ const String& userAgent,
        /* [in] */ Boolean hideUrlLog);

    //@CalledByNative
    CARAPI_(Boolean) SetDataSourceFromFd(
        /* [in] */ Int32 fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 length);

    //@CalledByNative
    CARAPI_(Boolean) SetDataUriDataSource(
        /* [in] */ IContext* context,
        /* [in] */ const String& url);

    CARAPI_(void) SetOnBufferingUpdateListener(
        /* [in] */ IMediaPlayerOnBufferingUpdateListener* listener);

    CARAPI_(void) SetOnCompletionListener(
        /* [in] */ IMediaPlayerOnCompletionListener* listener);

    CARAPI_(void) SetOnErrorListener(
        /* [in] */ IMediaPlayerOnErrorListener* listener);

    CARAPI_(void) SetOnPreparedListener(
        /* [in] */ IMediaPlayerOnPreparedListener* listener);

    CARAPI_(void) SetOnSeekCompleteListener(
        /* [in] */ IMediaPlayerOnSeekCompleteListener* listener);

    CARAPI_(void) SetOnVideoSizeChangedListener(
        /* [in] */ IMediaPlayerOnVideoSizeChangedListener* listener);

    /**
     * Returns an AllowedOperations object to show all the operations that are
     * allowed on the media player.
     */
    //@CalledByNative
    CARAPI_(AutoPtr<AllowedOperations>) GetAllowedOperations();

private:
    //@CalledByNative return IInterface
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ Handle64 nativeMediaPlayerBridge);

    CARAPI_(void) NativeOnDidSetDataUriDataSource(
        /* [in] */ Handle64 nativeMediaPlayerBridge,
        /* [in] */ Boolean success);

//callback function declaration
public:
    static CARAPI_(void*) ElaMediaPlayerBridgeCallback_Init();
private:
    static CARAPI_(void) Destroy(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) SetSurface(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* surface);

    static CARAPI_(Boolean) PrepareAsync(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) IsPlaying(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetVideoWidth(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetVideoHeight(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetCurrentPosition(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetDuration(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) ReleaseResources(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) SetVolume(
        /* [in] */ IInterface* obj,
        /* [in] */ Double volume);

    static CARAPI_(void) Start(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) Pause(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) SeekTo(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 msec);

    static CARAPI_(Boolean) SetDataSource(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* context,
        /* [in] */ const String& url,
        /* [in] */ const String& cookies,
        /* [in] */ const String& userAgent,
        /* [in] */ Boolean hideUrlLog);

    static CARAPI_(Boolean) SetDataSourceFromFd(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 length);

    static CARAPI_(Boolean) SetDataUriDataSource(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* context,
        /* [in] */ const String& url);

    static CARAPI_(AutoPtr<IInterface>) GetAllowedOperations(
        /* [in] */ IInterface* obj);

private:
    static const String TAG;

    // Local player to forward this to. We don't initialize it here since the subclass might not
    // want it.
    AutoPtr<LoadDataUriTask> mLoadDataUriTask;
    AutoPtr<IMediaPlayer> mPlayer;
    Handle64 mNativeMediaPlayerBridge;
    friend class MediaPlayerListener;
};

} // namespace Media
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_MEDIAPLAYERBRIDGE_H__

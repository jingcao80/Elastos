
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAPLAYERHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAPLAYERHELPER_H__

#include "_Elastos_Droid_Media_CMediaPlayerHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::View::ISurfaceHolder;

CarClass(CMediaPlayerHelper)
    , public Singleton
    , public IMediaPlayerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Convenience method to create a MediaPlayer for a given Uri.
     * On success, {@link #prepare()} will already have been called and must not be called again.
     * <p>When done with the MediaPlayer, you should call  {@link #release()},
     * to free the resources. If not released, too many MediaPlayer instances will
     * result in an exception.</p>
     *
     * @param context the Context to use
     * @param uri the Uri from which to get the datasource
     * @return a MediaPlayer object, or NULL if creation failed
     */
    CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [in] */ IUri* uri,
        /* [out] */ IMediaPlayer** player);

    /**
     * Convenience method to create a MediaPlayer for a given Uri.
     * On success, {@link #prepare()} will already have been called and must not be called again.
     * <p>When done with the MediaPlayer, you should call  {@link #release()},
     * to free the resources. If not released, too many MediaPlayer instances will
     * result in an exception.</p>
     *
     * @param context the Context to use
     * @param uri the Uri from which to get the datasource
     * @param holder the SurfaceHolder to use for displaying the video
     * @return a MediaPlayer object, or NULL if creation failed
     */
    CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [in] */ IUri* uri,
        /* [in] */ ISurfaceHolder* holder,
        /* [out] */ IMediaPlayer** player);

    /**
     * Same factory method as {@link #create(Context, Uri, SurfaceHolder)} but that lets you specify
     * the audio attributes and session ID to be used by the new MediaPlayer instance.
     * @param context the Context to use
     * @param uri the Uri from which to get the datasource
     * @param holder the SurfaceHolder to use for displaying the video, may be null.
     * @param audioAttributes the {@link AudioAttributes} to be used by the media player.
     * @param audioSessionId the audio session ID to be used by the media player,
     *     see {@link AudioManager#generateAudioSessionId()} to obtain a new session.
     * @return a MediaPlayer object, or null if creation failed
     */
    CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [in] */ IUri* uri,
        /* [in] */ ISurfaceHolder* holder,
        /* [in] */ IAudioAttributes* audioAttributes,
        /* [in] */ Int32 audioSessionId,
        /* [out] */ IMediaPlayer** result);

    /**
     * Convenience method to create a MediaPlayer for a given resource id.
     * On success, {@link #prepare()} will already have been called and must not be called again.
     * <p>When done with the MediaPlayer, you should call  {@link #release()},
     * to free the resources. If not released, too many MediaPlayer instances will
     * result in an exception.</p>
     *
     * @param context the Context to use
     * @param resid the raw resource id (<var>R.raw.&lt;something></var>) for
     *              the resource to use as the datasource
     * @return a MediaPlayer object, or NULL if creation failed
     */
    CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 resid,
        /* [out] */ IMediaPlayer** player);

    /**
     * Same factory method as {@link #create(Context, int)} but that lets you specify the audio
     * attributes and session ID to be used by the new MediaPlayer instance.
     * @param context the Context to use
     * @param resid the raw resource id (<var>R.raw.&lt;something></var>) for
     *              the resource to use as the datasource
     * @param audioAttributes the {@link AudioAttributes} to be used by the media player.
     * @param audioSessionId the audio session ID to be used by the media player,
     *     see {@link AudioManager#generateAudioSessionId()} to obtain a new session.
     * @return a MediaPlayer object, or null if creation failed
     */
    CARAPI Create(
        /* [in] */ IContext* ctx,
        /* [in] */ Int32 resid,
        /* [in] */ IAudioAttributes* audioAttributes,
        /* [in] */ Int32 audioSessionId,
        /* [out] */ IMediaPlayer** result);

    /**
     * @param reply Parcel with audio/video duration info for battery
                    tracking usage
     * @return The status code.
     * {@hide}
     */
    CARAPI NativePullBatteryData(
        /* [in] */ IParcel* reply,
        /* [out] */ Int32* result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAPLAYERHELPER_H__

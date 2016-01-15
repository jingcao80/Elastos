
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
     * set the status of the raw data mode.
     * <p>
     *
     * @param enable  whether to enable the raw data mode.
     */
    CARAPI SetRawDataMode(
        /* [in] */ Int32 rawDataMode);

    CARAPI GetRawDataMode(
        /* [out] */ Int32* rawDataMode);

    /* Rotate the video. */
    CARAPI IsRotatable(
        /* [out] */ Boolean* rotatable);

    CARAPI SetRotation(
        /* [in] */ Int32 value);

    /*  Notify hdmi status. */
    CARAPI SetHdmiState(
        /* [in] */ Boolean bHdmiPlugged);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAPLAYERHELPER_H__

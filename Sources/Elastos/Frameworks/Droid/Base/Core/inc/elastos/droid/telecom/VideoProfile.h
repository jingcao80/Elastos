#ifndef __ELASTOS_DROID_TELECOM_VIDEOPROFILE_H__
#define __ELASTOS_DROID_TELECOM_VIDEOPROFILE_H__

#include "Elastos.Droid.Telecom.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * Represents attributes of video calls.
 *
 * {@hide}
 */
class VideoProfile
    : public Object
    , public IVideoProfile
    , public IParcelable
{
public:
    /**
    * The video state of the call, stored as a bit-field describing whether video transmission and
    * receipt it enabled, as well as whether the video is currently muted.
    */
    class VideoState
        : public Object
        , public IVideoProfileVideoState
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Whether the video state is audio only.
         * @param videoState The video state.
         * @return Returns true if the video state is audio only.
         */
        static CARAPI IsAudioOnly(
            /* [in] */ Int32 videoState,
            /* [out] */ Boolean* result);

        /**
         * Whether the video state is any of the video type
         * @param videoState The video state.
         * @return Returns true if the video state TX or RX or Bidirectional
         */
        static CARAPI IsVideo(
            /* [in] */ Int32 videoState,
            /* [out] */ Boolean* result);

        /**
         * Whether the video transmission is enabled.
         * @param videoState The video state.
         * @return Returns true if the video transmission is enabled.
         */
        static CARAPI IsTransmissionEnabled(
            /* [in] */ Int32 videoState,
            /* [out] */ Boolean* result);

        /**
         * Whether the video reception is enabled.
         * @param videoState The video state.
         * @return Returns true if the video transmission is enabled.
         */
        static CARAPI IsReceptionEnabled(
            /* [in] */ Int32 videoState,
            /* [out] */ Boolean* result);

        /**
         * Whether the video signal is bi-directional.
         * @param videoState
         * @return Returns true if the video signal is bi-directional.
         */
        static CARAPI IsBidirectional(
            /* [in] */ Int32 videoState,
            /* [out] */ Boolean* result);

        /**
         * Whether the video is paused.
         * @param videoState The video state.
         * @return Returns true if the video is paused.
         */
        static CARAPI IsPaused(
            /* [in] */ Int32 videoState,
            /* [out] */ Boolean* result);

        /**
         * Determines if a specified state is set in a videoState bit-mask.
         *
         * @param videoState The video state bit-mask.
         * @param state The state to check.
         * @return {@code True} if the state is set.
         * {@hide}
         */
        static CARAPI HasState(
            /* [in] */ Int32 videoState,
            /* [in] */ Int32 state,
            /* [out] */ Boolean* result);
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Creates an instance of the VideoProfile
     *
     * @param videoState The video state.
     */
    CARAPI constructor(
        /* [in] */ Int32 videoState);

    /**
     * Creates an instance of the VideoProfile
     *
     * @param videoState The video state.
     * @param quality The video quality.
     */
    CARAPI constructor(
        /* [in] */ Int32 videoState,
        /* [in] */ Int32 quality);

    /**
     * The video state of the call.
     * Valid values: {@link VideoProfile.VideoState#AUDIO_ONLY},
     * {@link VideoProfile.VideoState#BIDIRECTIONAL},
     * {@link VideoProfile.VideoState#TX_ENABLED},
     * {@link VideoProfile.VideoState#RX_ENABLED},
     * {@link VideoProfile.VideoState#PAUSED}.
     */
    CARAPI GetVideoState(
        /* [out] */ Int32* result);

    /**
     * The desired video quality for the call.
     * Valid values: {@link VideoProfile#QUALITY_HIGH}, {@link VideoProfile#QUALITY_MEDIUM},
     * {@link VideoProfile#QUALITY_LOW}, {@link VideoProfile#QUALITY_DEFAULT}.
     */
    CARAPI GetQuality(
        /* [out] */ Int32* result);

    CARAPI ReadFromParcel(
    /* [in] */ IParcel* in);

    /**
     * Flatten this object in to a Parcel.
     *
     * @param dest  The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be written.
     *              May be 0 or {@link #PARCELABLE_WRITE_RETURN_VALUE}.
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    Int32 mVideoState;

    Int32 mQuality;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_VIDEOPROFILE_H__
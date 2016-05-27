#ifndef __ELASTOS_DROID_MEDIA_TV_CTVTRACKINFO_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVTRACKINFO_H__

#include "_Elastos_Droid_Media_Tv_CTvTrackInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * Encapsulates the format of tracks played in {@link TvInputService}.
 */
CarClass(CTvTrackInfo)
    , public Object
    , public ITvTrackInfo
    , public IParcelable
{
public:
    friend class CTvTrackInfoBuilder;

    CTvTrackInfo();

    virtual ~CTvTrackInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 type,
        /* [in] */ const String& id,
        /* [in] */ const String& language,
        /* [in] */ Int32 audioChannelCount,
        /* [in] */ Int32 audioSampleRate,
        /* [in] */ Int32 videoWidth,
        /* [in] */ Int32 videoHeight,
        /* [in] */ Float videoFrameRate,
        /* [in] */ IBundle * extra);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Returns the type of the track. The type should be one of the followings:
     * {@link #TYPE_AUDIO}, {@link #TYPE_VIDEO} and {@link #TYPE_SUBTITLE}.
     */
    CARAPI GetType(
        /* [out] */ Int32* result);

    /**
     * Returns the ID of the track.
     */
    CARAPI GetId(
        /* [out] */ String* result);

    /**
     * Returns the language information encoded by either ISO 639-1 or ISO 639-2/T. If the language
     * is unknown or could not be determined, the corresponding value will be {@code null}.
     */
    CARAPI GetLanguage(
        /* [out] */ String* result);

    /**
     * Returns the audio channel count. Valid only for {@link #TYPE_AUDIO} tracks.
     */
    CARAPI GetAudioChannelCount(
        /* [out] */ Int32* result);

    /**
     * Returns the audio sample rate, in the unit of Hz. Valid only for {@link #TYPE_AUDIO} tracks.
     */
    CARAPI GetAudioSampleRate(
        /* [out] */ Int32* result);

    /**
     * Returns the width of the video, in the unit of pixels. Valid only for {@link #TYPE_VIDEO}
     * tracks.
     */
    CARAPI GetVideoWidth(
        /* [out] */ Int32* result);

    /**
     * Returns the height of the video, in the unit of pixels. Valid only for {@link #TYPE_VIDEO}
     * tracks.
     */
    CARAPI GetVideoHeight(
        /* [out] */ Int32* result);

    /**
     * Returns the frame rate of the video, in the unit of fps (frames per second). Valid only for
     * {@link #TYPE_VIDEO} tracks.
     */
    CARAPI GetVideoFrameRate(
        /* [out] */ Float* result);

    /**
     * Returns the extra information about the current track.
     */
    CARAPI GetExtra(
        /* [out] */ IBundle** result);

private:
    Int32 mType;
    String mId;
    String mLanguage;
    Int32 mAudioChannelCount;
    Int32 mAudioSampleRate;
    Int32 mVideoWidth;
    Int32 mVideoHeight;
    Float mVideoFrameRate;
    AutoPtr<IBundle> mExtra;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVTRACKINFO_H__

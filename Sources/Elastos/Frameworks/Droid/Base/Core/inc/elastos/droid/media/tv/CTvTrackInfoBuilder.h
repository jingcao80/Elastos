#ifndef __ELASTOS_DROID_MEDIA_TV_CTVTRACKINFOBUILDER_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVTRACKINFOBUILDER_H__

#include "_Elastos_Droid_Media_Tv_CTvTrackInfoBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * A builder class for creating {@link TvTrackInfo} objects.
 */
CarClass(CTvTrackInfoBuilder)
    , public Object
    , public ITvTrackInfoBuilder
{
public:
    CTvTrackInfoBuilder();

    virtual ~CTvTrackInfoBuilder();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Create a {@link Builder}. Any field that should be included in the {@link TvTrackInfo}
     * must be added.
     *
     * @param type The type of the track.
     * @param id The ID of the track that uniquely identifies the current track among all the
     *            other tracks in the same TV program.
     */
    CARAPI constructor(
        /* [in] */ Int32 type,
        /* [in] */ const String& id);

    /**
     * Sets the language information of the current track.
     *
     * @param language The language string encoded by either ISO 639-1 or ISO 639-2/T.
     */
    CARAPI SetLanguage(
        /* [in] */ const String& language);

    /**
     * Sets the audio channel count. Valid only for {@link #TYPE_AUDIO} tracks.
     *
     * @param audioChannelCount The audio channel count.
     */
    CARAPI SetAudioChannelCount(
        /* [in] */ Int32 audioChannelCount);

    /**
     * Sets the audio sample rate, in the unit of Hz. Valid only for {@link #TYPE_AUDIO}
     * tracks.
     *
     * @param audioSampleRate The audio sample rate.
     */
    CARAPI SetAudioSampleRate(
        /* [in] */ Int32 audioSampleRate);

    /**
     * Sets the width of the video, in the unit of pixels. Valid only for {@link #TYPE_VIDEO}
     * tracks.
     *
     * @param videoWidth The width of the video.
     */
    CARAPI SetVideoWidth(
        /* [in] */ Int32 videoWidth);

    /**
     * Sets the height of the video, in the unit of pixels. Valid only for {@link #TYPE_VIDEO}
     * tracks.
     *
     * @param videoHeight The height of the video.
     */
    CARAPI SetVideoHeight(
        /* [in] */ Int32 videoHeight);

    /**
     * Sets the frame rate of the video, in the unit fps (frames per rate). Valid only for
     * {@link #TYPE_VIDEO} tracks.
     *
     * @param videoFrameRate The frame rate of the video.
     */
    CARAPI SetVideoFrameRate(
        /* [in] */ Float videoFrameRate);

    /**
     * Sets the extra information about the current track.
     *
     * @param extra The extra information.
     */
    CARAPI SetExtra(
        /* [in] */ IBundle* extra);

    /**
     * Creates a {@link TvTrackInfo} instance with the specified fields.
     *
     * @return The new {@link TvTrackInfo} instance
     */
    CARAPI Build(
        /* [out] */ ITvTrackInfo ** result);

private:
    String mId;
    String mLanguage;
    Int32 mType;
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

#endif //__ELASTOS_DROID_MEDIA_TV_CTVTRACKINFOBUILDER_H__

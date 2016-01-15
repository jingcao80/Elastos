
#ifndef __ELASTOS_DROID_MEDIA_CCAMCORDERPROFILE_H__
#define __ELASTOS_DROID_MEDIA_CCAMCORDERPROFILE_H__

#include "_Elastos_Droid_Media_CCamcorderProfile.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CCamcorderProfile)
    , public Object
    , public ICamcorderProfile
{
public:
    CCamcorderProfile();

    virtual ~CCamcorderProfile();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int32 duration,
        /* [in] */ Int32 quality,
        /* [in] */ Int32 fileFormat,
        /* [in] */ Int32 videoCodec,
        /* [in] */ Int32 videoBitRate,
        /* [in] */ Int32 videoFrameRate,
        /* [in] */ Int32 videoWidth,
        /* [in] */ Int32 videoHeight,
        /* [in] */ Int32 audioCodec,
        /* [in] */ Int32 audioBitRate,
        /* [in] */ Int32 audioSampleRate,
        /* [in] */ Int32 audioChannels);

    /**
     * Returns the camcorder profile for the first back-facing camera on the
     * device at the given quality level. If the device has no back-facing
     * camera, this returns null.
     * @param quality the target quality level for the camcorder profile
     * @see #get(Int32, Int32)
     */
    static CARAPI Get(
        /* [in] */ Int32 quality,
        /* [out] */ ICamcorderProfile** result);

    /**
     * Returns the camcorder profile for the given camera at the given
     * quality level.
     *
     * Quality levels QUALITY_LOW, QUALITY_HIGH are guaranteed to be supported, while
     * other levels may or may not be supported. The supported levels can be checked using
     * {@link #hasProfile(int, int)}.
     * QUALITY_LOW refers to the lowest quality available, while QUALITY_HIGH refers to
     * the highest quality available.
     * QUALITY_LOW/QUALITY_HIGH have to match one of qcif, cif, 480p, 720p, 1080p or 2160p.
     * E.g. if the device supports 480p, 720p, 1080p and 2160p, then low is 480p and high is
     * 2160p.
     *
     * The same is true for time lapse quality levels, i.e. QUALITY_TIME_LAPSE_LOW,
     * QUALITY_TIME_LAPSE_HIGH are guaranteed to be supported and have to match one of
     * qcif, cif, 480p, 720p, 1080p, or 2160p.
     *
     * For high speed quality levels, they may or may not be supported. If a subset of the levels
     * are supported, QUALITY_HIGH_SPEED_LOW and QUALITY_HIGH_SPEED_HIGH are guaranteed to be
     * supported and have to match one of 480p, 720p, or 1080p.
     *
     * A camcorder recording session with higher quality level usually has higher output
     * bit rate, better video and/or audio recording quality, larger video frame
     * resolution and higher audio sampling rate, etc, than those with lower quality
     * level.
     *
     * @param cameraId the id for the camera
     * @param quality the target quality level for the camcorder profile.
     * @see #QUALITY_LOW
     * @see #QUALITY_HIGH
     * @see #QUALITY_QCIF
     * @see #QUALITY_CIF
     * @see #QUALITY_480P
     * @see #QUALITY_720P
     * @see #QUALITY_1080P
     * @see #QUALITY_2160P
     * @see #QUALITY_TIME_LAPSE_LOW
     * @see #QUALITY_TIME_LAPSE_HIGH
     * @see #QUALITY_TIME_LAPSE_QCIF
     * @see #QUALITY_TIME_LAPSE_CIF
     * @see #QUALITY_TIME_LAPSE_480P
     * @see #QUALITY_TIME_LAPSE_720P
     * @see #QUALITY_TIME_LAPSE_1080P
     * @see #QUALITY_TIME_LAPSE_2160P
     * @see #QUALITY_HIGH_SPEED_LOW
     * @see #QUALITY_HIGH_SPEED_HIGH
     * @see #QUALITY_HIGH_SPEED_480P
     * @see #QUALITY_HIGH_SPEED_720P
     * @see #QUALITY_HIGH_SPEED_1080P
     * @see #QUALITY_HIGH_SPEED_2160P
    */
    static CARAPI Get(
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 quality,
        /* [out] */ ICamcorderProfile** result);

    /**
     * Returns true if camcorder profile exists for the first back-facing
     * camera at the given quality level.
     * @param quality the target quality level for the camcorder profile
     */
    static CARAPI HasProfile(
        /* [in] */ Int32 quality,
        /* [out] */ Boolean* result);

    /**
     * Returns true if camcorder profile exists for the given camera at
     * the given quality level.
     * @param cameraId the id for the camera
     * @param quality the target quality level for the camcorder profile
     */
    static CARAPI HasProfile(
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 quality,
        /* [out] */ Boolean* result);

    CARAPI SetDuration(
        /* [in] */ Int32 duration);

    CARAPI GetDuration(
        /* [out] */ Int32* duration);

    CARAPI SetQuality(
        /* [in] */ Int32 quality);

    CARAPI GetQuality(
        /* [out] */ Int32* quality);

    CARAPI SetFileFormat(
        /* [in] */ Int32 fileFormat);

    CARAPI GetFileFormat(
        /* [out] */ Int32* fileFormat);

    CARAPI SetVideoCodec(
        /* [in] */ Int32 videoCodec);

    CARAPI GetVideoCodec(
        /* [out] */ Int32* videoCodec);

    CARAPI SetVideoBitRate(
        /* [in] */ Int32 videoBitRate);

    CARAPI GetVideoBitRate(
        /* [out] */ Int32* videoBitRate);

    CARAPI SetVideoFrameRate(
        /* [in] */ Int32 videoFrameRate);

    CARAPI GetVideoFrameRate(
        /* [out] */ Int32* videoFrameRate);

    CARAPI SetVideoFrameWidth(
        /* [in] */ Int32 videoFrameWidth);

    CARAPI GetVideoFrameWidth(
        /* [out] */ Int32* videoFrameWidth);

    CARAPI SetVideoFrameHeight(
        /* [in] */ Int32 videoFrameHeight);

    CARAPI GetVideoFrameHeight(
        /* [out] */ Int32* videoFrameHeight);

    CARAPI SetAudioCodec(
        /* [in] */ Int32 audioCodec);

    CARAPI GetAudioCodec(
        /* [out] */ Int32* audioCodec);

    CARAPI SetAudioBitRate(
        /* [in] */ Int32 audioBitRate);

    CARAPI GetAudioBitRate(
        /* [out] */ Int32* audioBitRate);

    CARAPI SetAudioSampleRate(
        /* [in] */ Int32 audioSampleRate);

    CARAPI GetAudioSampleRate(
        /* [out] */ Int32* audioSampleRate);

    CARAPI SetAudioChannels(
        /* [in] */ Int32 audioChannels);

    CARAPI GetAudioChannels(
        /* [out] */ Int32* audioChannels);

private:
    static CARAPI NativeGetCamcorderProfile(
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 quality,
        /* [out] */ ICamcorderProfile** profile);

    static CARAPI_(Boolean) NativeHasCamcorderProfile(
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 quality);

    static CARAPI_(Boolean) IsCamcorderQualityKnown(
        /* [in] */ Int32 quality);

public:
    /**
     * Default recording duration in seconds before the session is terminated.
     * This is useful for applications like MMS has limited file size requirement.
     */
    Int32 mDuration;

    /**
     * The quality level of the camcorder profile
     */
    Int32 mQuality;

    /**
     * The file output format of the camcorder profile
     * @see android.media.MediaRecorder.OutputFormat
     */
    Int32 mFileFormat;

    /**
     * The video encoder being used for the video track
     * @see android.media.MediaRecorder.VideoEncoder
     */
    Int32 mVideoCodec;

    /**
     * The target video output bit rate in bits per second
     */
    Int32 mVideoBitRate;

    /**
     * The target video frame rate in frames per second
     */
    Int32 mVideoFrameRate;

    /**
     * The target video frame width in pixels
     */
    Int32 mVideoFrameWidth;

    /**
     * The target video frame height in pixels
     */
    Int32 mVideoFrameHeight;

    /**
     * The audio encoder being used for the audio track.
     * @see android.media.MediaRecorder.AudioEncoder
     */
    Int32 mAudioCodec;

    /**
     * The target audio output bit rate in bits per second
     */
    Int32 mAudioBitRate;

    /**
     * The audio sampling rate used for the audio track
     */
    Int32 mAudioSampleRate;

    /**
     * The number of audio channels used for the audio track
     */
    Int32 mAudioChannels;

private:
    // Start and end of quality list
    static const Int32 QUALITY_LIST_START;
    static const Int32 QUALITY_LIST_END;

    // Start and end of timelapse quality list
    static const Int32 QUALITY_TIME_LAPSE_LIST_START;
    static const Int32 QUALITY_TIME_LAPSE_LIST_END;

    // Start and end of high speed quality list
    static const Int32 QUALITY_HIGH_SPEED_LIST_START;
    static const Int32 QUALITY_HIGH_SPEED_LIST_END;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CCAMCORDERPROFILE_H__

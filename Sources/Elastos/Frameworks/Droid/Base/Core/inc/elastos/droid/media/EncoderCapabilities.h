
#ifndef __ELASTOS_DROID_MEDIA_ENCODERCAPABILITIES_H__
#define __ELASTOS_DROID_MEDIA_ENCODERCAPABILITIES_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Media {

class EncoderCapabilities
{
public:
    /**
     * The VideoEncoderCap class represents a video encoder's
     * supported parameter range in:
     *
     * <ul>
     * <li>Resolution: the frame size (width/height) in pixels;
     * <li>Bit rate: the compressed output bit rate in bits per second;
     * <li>Frame rate: the output number of frames per second.
     * </ul>
     *
     */
    class VideoEncoderCap
        : public Object
    {
    public:
        // Private constructor called by JNI
        VideoEncoderCap(
            /* [in] */ Int32 codec,
            /* [in] */ Int32 mMinBitRate,
            /* [in] */ Int32 mMaxBitRate,
            /* [in] */ Int32 minFrameRate,
            /* [in] */ Int32 maxFrameRate,
            /* [in] */ Int32 minFrameWidth,
            /* [in] */ Int32 maxFrameWidth,
            /* [in] */ Int32 minFrameHeight,
            /* [in] */ Int32 maxFrameHeight);

    public:
        // These are not modifiable externally, thus are public accessible
        Int32 mCodec;                                 // @see android.media.MediaRecorder.VideoEncoder
        Int32 mMinBitRate, mMaxBitRate;               // min and max bit rate (bps)
        Int32 mMinFrameRate, mMaxFrameRate;           // min and max frame rate (fps)
        Int32 mMinFrameWidth, mMaxFrameWidth;         // min and max frame width (pixel)
        Int32 mMinFrameHeight, mMaxFrameHeight;       // minn and max frame height (pixel)
    };

    /**
     * The AudioEncoderCap class represents an audio encoder's
     * parameter range in:
     *
     * <ul>
     * <li>Bit rate: the compressed output bit rate in bits per second;
     * <li>Sample rate: the sampling rate used for recording the audio in samples per second;
     * <li>Number of channels: the number of channels the audio is recorded.
     * </ul>
     *
     */
    class AudioEncoderCap
        : public Object
    {
    public:
        // Private constructor called by JNI
        AudioEncoderCap(
            /* [in] */ Int32 codec,
            /* [in] */ Int32 minBitRate,
            /* [in] */ Int32 maxBitRate,
            /* [in] */ Int32 minSampleRate,
            /* [in] */ Int32 maxSampleRate,
            /* [in] */ Int32 minChannels,
            /* [in] */ Int32 maxChannels);

    public:
        // These are not modifiable externally, thus are public accessible
        Int32 mCodec;                         // @see android.media.MediaRecorder.AudioEncoder
        Int32 mMinChannels, mMaxChannels;     // min and max number of channels
        Int32 mMinSampleRate, mMaxSampleRate; // min and max sample rate (hz)
        Int32 mMinBitRate, mMaxBitRate;       // min and max bit rate (bps)
    };

    /**
     * Returns the array of supported output file formats.
     * @see android.media.MediaRecorder.OutputFormat
     */
    static CARAPI GetOutputFileFormats(
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
     * Returns the capabilities of the supported video encoders.
     * @see android.media.EncoderCapabilities.VideoEncoderCap
     */
    static CARAPI GetVideoEncoders(
        /* [out] */ IList** result);

    /**
     * Returns the capabilities of the supported audio encoders.
     * @see android.media.EncoderCapabilities.AudioEncoderCap
     */
    static CARAPI GetAudioEncoders(
        /* [out] */ IList** result);

private:
    // Implemented by JNI
    static CARAPI_(Int32) Native_get_num_file_formats();

    static CARAPI_(Int32) Native_get_file_format(
        /* [in] */ Int32 index);

    static CARAPI_(Int32) Native_get_num_video_encoders();

    static CARAPI_(AutoPtr<VideoEncoderCap>) Native_get_video_encoder_cap(
        /* [in] */ Int32 index);

    static CARAPI_(Int32) Native_get_num_audio_encoders();

    static CARAPI_(AutoPtr<AudioEncoderCap>) Native_get_audio_encoder_cap(
        /* [in] */ Int32 index);

    static CARAPI_(Boolean) InitStatic();

private:
    static String TAG;

    static Boolean mInit;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_ENCODERCAPABILITIES_H__

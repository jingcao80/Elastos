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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_MEDIACODECBRIDGE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_MEDIACODECBRIDGE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Media::IAudioTrack;
using Elastos::Droid::Media::IMediaCodec;
using Elastos::Droid::Media::IMediaCrypto;
using Elastos::Droid::Media::IMediaFormat;
using Elastos::Droid::View::ISurface;

using Elastos::IO::IByteBuffer;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

// import java.nio.ByteBuffer;
// import java.util.ArrayList;
// import java.util.HashMap;
// import java.util.Map;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Media {

/**
 * A wrapper of the MediaCodec class to facilitate exception capturing and
 * audio rendering.
 */
//@JNINamespace("media")
class MediaCodecBridge
    : public Object
{
public:
    class DequeueInputResult
        : public Object
    {
    public:
        DequeueInputResult(
            /* [in] */ Int32 status,
            /* [in] */ Int32 index);

        //@CalledByNative("DequeueInputResult")
        CARAPI_(Int32) Status();

        //@CalledByNative("DequeueInputResult")
        CARAPI_(Int32) Index();

        static CARAPI_(Int32) Status(
        /* [in] */IInterface* obj);

        static CARAPI_(Int32) Index(
        /* [in] */IInterface* obj);

    private:
        const Int32 mStatus;
        const Int32 mIndex;
    };

    /**
     * This class represents supported android codec information.
     */
    class CodecInfo
        : public Object
    {
    public:
        CodecInfo(
            /* [in] */ const String& codecType,
            /* [in] */ const String& codecName,
            /* [in] */ Int32 direction);

        //@CalledByNative("CodecInfo")
        CARAPI_(String) CodecType();

        //@CalledByNative("CodecInfo")
        CARAPI_(String) CodecName();

        //@CalledByNative("CodecInfo")
        CARAPI_(Int32) Direction();

        static CARAPI_(String) CodecType(
            /* [in] */IInterface* obj);

        static CARAPI_(String) CodecName(
            /* [in] */IInterface* obj);

        static CARAPI_(Int32) Direction(
            /* [in] */IInterface* obj);

    private:
        const String mCodecType;  // e.g. "video/x-vnd.on2.vp8".
        const String mCodecName;  // e.g. "OMX.google.vp8.decoder".
        const Int32 mDirection;
    };

    class DequeueOutputResult
        : public Object
    {
    public:
        DequeueOutputResult(
            /* [in] */ Int32 status,
            /* [in] */ Int32 index,
            /* [in] */ Int32 flags,
            /* [in] */ Int32 offset,
            /* [in] */ Int64 presentationTimeMicroseconds,
            /* [in] */ Int32 numBytes);

        //@CalledByNative("DequeueOutputResult")
        CARAPI_(Int32) Status();

        //@CalledByNative("DequeueOutputResult")
        CARAPI_(Int32) Index();

        //@CalledByNative("DequeueOutputResult")
        CARAPI_(Int32) Flags();

        //@CalledByNative("DequeueOutputResult")
        CARAPI_(Int32) Offset();

        //@CalledByNative("DequeueOutputResult")
        CARAPI_(Int64) PresentationTimeMicroseconds();

        //@CalledByNative("DequeueOutputResult")
        CARAPI_(Int32) NumBytes();

        static CARAPI_(Int32) Status(
            /* [in] */IInterface* obj);

        static CARAPI_(Int32) Index(
            /* [in] */IInterface* obj);

        static CARAPI_(Int32) Flags(
            /* [in] */IInterface* obj);

        static CARAPI_(Int32) Offset(
            /* [in] */IInterface* obj);

        static CARAPI_(Int64) PresentationTimeMicroseconds(
            /* [in] */IInterface* obj);

        static CARAPI_(Int32) NumBytes(
            /* [in] */IInterface* obj);

    private:
        const Int32 mStatus;
        const Int32 mIndex;
        const Int32 mFlags;
        const Int32 mOffset;
        const Int64 mPresentationTimeMicroseconds;
        const Int32 mNumBytes;
    };

private:
    MediaCodecBridge(
        /* [in] */ IMediaCodec* mediaCodec,
        /* [in] */ const String& mime,
        /* [in] */ Boolean adaptivePlaybackSupported);

    /**
     * Get a list of supported android codec mimes.
     */
    //@CalledByNative return CodecInfo
    static CARAPI_(AutoPtr<ArrayOf<IInterface*> >) GetCodecsInfo();

    static CARAPI_(String) GetDecoderNameForMime(
        /* [in] */ const String& mime);

    //@CalledByNative return MediaCodecBridge
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ const String& mime,
        /* [in] */ Boolean isSecure,
        /* [in] */ Int32 direction);

    //@CalledByNative
    CARAPI_(void) ReleaseResources();

    //@CalledByNative
    CARAPI_(Boolean) Start();

    //@CalledByNative
    CARAPI_(AutoPtr<DequeueInputResult>) DequeueInputBuffer(
        /* [in] */ Int64 timeoutUs);

    //@CalledByNative
    CARAPI_(Int32) Flush();

    //@CalledByNative
    CARAPI_(void) Stop();

    //@CalledByNative
    CARAPI_(Int32) GetOutputHeight();

    //@CalledByNative
    CARAPI_(Int32) GetOutputWidth();

    //@CalledByNative
    CARAPI_(AutoPtr<IByteBuffer>) GetInputBuffer(
        /* [in] */ Int32 index);

    //@CalledByNative
    CARAPI_(AutoPtr<IByteBuffer>) GetOutputBuffer(
        /* [in] */ Int32 index);

    //@CalledByNative
    CARAPI_(Int32) GetInputBuffersCount();

    //@CalledByNative
    CARAPI_(Int32) GetOutputBuffersCount();

    //@CalledByNative
    CARAPI_(Int32) GetOutputBuffersCapacity();

    //@CalledByNative
    CARAPI_(Boolean) GetOutputBuffers();

    //@CalledByNative
    CARAPI_(Int32) QueueInputBuffer(
        /* [in] */ Int32 index,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size,
        /* [in] */ Int64 presentationTimeUs,
        /* [in] */ Int32 flags);

    //@CalledByNative
    CARAPI_(void) SetVideoBitrate(
        /* [in] */ Int32 bps);

    //@CalledByNative
    CARAPI_(void) RequestKeyFrameSoon();

    //@CalledByNative
    CARAPI_(Int32) QueueSecureInputBuffer(
        /* [in] */ Int32 index,
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Byte>* iv,
        /* [in] */ ArrayOf<Byte>* keyId,
        /* [in] */ ArrayOf<Int32>* numBytesOfClearData,
        /* [in] */ ArrayOf<Int32>* numBytesOfEncryptedData,
        /* [in] */ Int32 numSubSamples,
        /* [in] */ Int64 presentationTimeUs);

    //@CalledByNative
    CARAPI_(void) ReleaseOutputBuffer(
        /* [in] */ Int32 index,
        /* [in] */ Boolean render);

    //@CalledByNative
    CARAPI_(AutoPtr<DequeueOutputResult>) DequeueOutputBuffer(
        /* [in] */ Int64 timeoutUs);

    //@CalledByNative
    CARAPI_(Boolean) ConfigureVideo(
        /* [in] */ IMediaFormat* format,
        /* [in] */ ISurface* surface,
        /* [in] */ IMediaCrypto* crypto,
        /* [in] */ Int32 flags);

    //@CalledByNative return IMediaFormat
    static CARAPI_(AutoPtr<IInterface>) CreateAudioFormat(
        /* [in] */ const String& mime,
        /* [in] */ Int32 sampleRate,
        /* [in] */ Int32 channelCount);

    //@CalledByNative return IMediaFormat
    static CARAPI_(AutoPtr<IInterface>) CreateVideoDecoderFormat(
        /* [in] */ const String& mime,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    //@CalledByNative return IMediaFormat
    static CARAPI_(AutoPtr<IInterface>) CreateVideoEncoderFormat(
        /* [in] */ const String& mime,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 bitRate,
        /* [in] */ Int32 frameRate,
        /* [in] */ Int32 iFrameInterval,
        /* [in] */ Int32 colorFormat);

    //@CalledByNative
    CARAPI_(Boolean) IsAdaptivePlaybackSupported(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    static CARAPI_(Boolean) CodecSupportsAdaptivePlayback(
        /* [in] */ IMediaCodec* mediaCodec,
        /* [in] */ const String& mime);

    //@CalledByNative
    static CARAPI_(void) SetCodecSpecificData(
        /* [in] */ IMediaFormat* format,
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Byte>* bytes);

    //@CalledByNative
    static CARAPI_(void) SetFrameHasADTSHeader(
        /* [in] */ IMediaFormat* format);

    //@CalledByNative
    CARAPI_(Boolean) ConfigureAudio(
        /* [in] */ IMediaFormat* format,
        /* [in] */ IMediaCrypto* crypto,
        /* [in] */ Int32 flags,
        /* [in] */ Boolean playAudio);

    /**
     *  Play the audio buffer that is passed in.
     *
     *  @param buf Audio buffer to be rendered.
     *  @return The number of frames that have already been consumed by the
     *  hardware. This number resets to 0 after each flush call.
     */
    //@CalledByNative
    CARAPI_(Int64) PlayOutputBuffer(
        /* [in] */ ArrayOf<Byte>* buf);

    //@CalledByNative
    CARAPI_(void) SetVolume(
        /* [in] */ Double volume);

    CARAPI_(void) ResetLastPresentationTimeIfNeeded(
        /* [in] */ Int64 presentationTimeUs);

    CARAPI_(Int32) GetAudioFormat(
        /* [in] */ Int32 channelCount);

//callback function declaration
public:
    static CARAPI_(void*) ElaMediaCodecBridgeCallback_Init();

private:
    static CARAPI_(void) ReleaseResource(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) Start(
        /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<IInterface>) DequeueInputBuffer(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 timeoutUs);

    static CARAPI_(Int32) Flush(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) Stop(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetOutputHeight(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetOutputWidth(
        /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<IInterface>) GetInputBuffer(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 index);

    static CARAPI_(AutoPtr<IInterface>) GetOutputBuffer(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 index);

    static CARAPI_(Int32) GetInputBuffersCount(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetOutputBuffersCount(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetOutputBuffersCapacity(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) GetOutputBuffers(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) QueueInputBuffer(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 index,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size,
        /* [in] */ Int64 presentationTimeUs,
        /* [in] */ Int32 flags);

    static CARAPI_(void) SetVideoBitrate(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 bps);

    static CARAPI_(void) RequestKeyFrameSoon(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) QueueSecureInputBuffer(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 index,
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Byte>* iv,
        /* [in] */ ArrayOf<Byte>* keyId,
        /* [in] */ ArrayOf<Int32>* numBytesOfClearData,
        /* [in] */ ArrayOf<Int32>* numBytesOfEncryptedData,
        /* [in] */ Int32 numSubSamples,
        /* [in] */ Int64 presentationTimeUs);

    static CARAPI_(void) ReleaseOutputBuffer(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 index,
        /* [in] */ Boolean render);

    static CARAPI_(AutoPtr<IInterface>) DequeueOutputBuffer(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 timeoutUs);

    static CARAPI_(Boolean) ConfigureVideo(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* format,
        /* [in] */ IInterface* surface,
        /* [in] */ IInterface* crypto,
        /* [in] */ Int32 flags);

    static CARAPI_(Boolean) IsAdaptivePlaybackSupported(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    static CARAPI_(void) SetCodecSpecificData(
        /* [in] */ IInterface* format,
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Byte>* bytes);

    static CARAPI_(void) SetFrameHasADTSHeader(
        /* [in] */ IInterface* format);

    static CARAPI_(Boolean) ConfigureAudio(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* format,
        /* [in] */ IInterface* crypto,
        /* [in] */ Int32 flags,
        /* [in] */ Boolean playAudio);

    static CARAPI_(Int64) PlayOutputBuffer(
        /* [in] */ IInterface* obj,
        /* [in] */ ArrayOf<Byte>* buf);

    static CARAPI_(void) SetVolume(
        /* [in] */ IInterface* obj,
        /* [in] */ Double volume);

private:
    static const String TAG;

    // Error code for MediaCodecBridge. Keep this value in sync with
    // MediaCodecStatus in media_codec_bridge.h.
    static const Int32 MEDIA_CODEC_OK = 0;
    static const Int32 MEDIA_CODEC_DEQUEUE_INPUT_AGAIN_LATER = 1;
    static const Int32 MEDIA_CODEC_DEQUEUE_OUTPUT_AGAIN_LATER = 2;
    static const Int32 MEDIA_CODEC_OUTPUT_BUFFERS_CHANGED = 3;
    static const Int32 MEDIA_CODEC_OUTPUT_FORMAT_CHANGED = 4;
    static const Int32 MEDIA_CODEC_INPUT_END_OF_STREAM = 5;
    static const Int32 MEDIA_CODEC_OUTPUT_END_OF_STREAM = 6;
    static const Int32 MEDIA_CODEC_NO_KEY = 7;
    static const Int32 MEDIA_CODEC_STOPPED = 8;
    static const Int32 MEDIA_CODEC_ERROR = 9;

    // Codec direction.  Keep this in sync with media_codec_bridge.h.
    static const Int32 MEDIA_CODEC_DECODER = 0;
    static const Int32 MEDIA_CODEC_ENCODER = 1;

    // Max adaptive playback size to be supplied to the decoder.
    static const Int32 MAX_ADAPTIVE_PLAYBACK_WIDTH = 1920;
    static const Int32 MAX_ADAPTIVE_PLAYBACK_HEIGHT = 1080;

    // After a flush(), dequeueOutputBuffer() can often produce empty presentation timestamps
    // for several frames. As a result, the player may find that the time does not increase
    // after decoding a frame. To detect this, we check whether the presentation timestamp from
    // dequeueOutputBuffer() is larger than input_timestamp - MAX_PRESENTATION_TIMESTAMP_SHIFT_US
    // after a flush. And we set the presentation timestamp from dequeueOutputBuffer() to be
    // non-decreasing for the remaining frames.
    static const Int64 MAX_PRESENTATION_TIMESTAMP_SHIFT_US = 100000;

    AutoPtr< ArrayOf<IByteBuffer*> > mInputBuffers;
    AutoPtr< ArrayOf<IByteBuffer*> > mOutputBuffers;

    AutoPtr<IMediaCodec> mMediaCodec;
    AutoPtr<IAudioTrack> mAudioTrack;
    Boolean mFlushed;
    Int64 mLastPresentationTimeUs;
    String mMime;
    Boolean mAdaptivePlaybackSupported;
};

} // namespace Media
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_MEDIACODECBRIDGE_H__

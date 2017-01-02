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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIAMUXER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAMUXER_H__

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Media_CMediaMuxer.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICloseGuard;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * MediaMuxer facilitates muxing elementary streams. Currently only supports an
 * mp4 file as the output and at most one audio and/or one video elementary
 * stream.
 * <p>
 * It is generally used like this:
 *
 * <pre>
 * MediaMuxer muxer = new MediaMuxer("temp.mp4", OutputFormat.MUXER_OUTPUT_MPEG_4);
 * // More often, the MediaFormat will be retrieved from MediaCodec.getOutputFormat()
 * // or MediaExtractor.getTrackFormat().
 * MediaFormat audioFormat = new MediaFormat(...);
 * MediaFormat videoFormat = new MediaFormat(...);
 * int audioTrackIndex = muxer.addTrack(audioFormat);
 * int videoTrackIndex = muxer.addTrack(videoFormat);
 * ByteBuffer inputBuffer = ByteBuffer.allocate(bufferSize);
 * boolean finished = false;
 * BufferInfo bufferInfo = new BufferInfo();
 *
 * muxer.start();
 * while(!finished) {
 *   // getInputBuffer() will fill the inputBuffer with one frame of encoded
 *   // sample from either MediaCodec or MediaExtractor, set isAudioSample to
 *   // true when the sample is audio data, set up all the fields of bufferInfo,
 *   // and return true if there are no more samples.
 *   finished = getInputBuffer(inputBuffer, isAudioSample, bufferInfo);
 *   if (!finished) {
 *     int currentTrackIndex = isAudioSample ? audioTrackIndex : videoTrackIndex;
 *     muxer.writeSampleData(currentTrackIndex, inputBuffer, bufferInfo);
 *   }
 * };
 * muxer.stop();
 * muxer.release();
 * </pre>
 */
CarClass(CMediaMuxer)
    , public Object
    , public IMediaMuxer
{
public:
    CMediaMuxer();

    virtual ~CMediaMuxer();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Constructor.
     * Creates a media muxer that writes to the specified path.
     * @param path The path of the output media file.
     * @param format The format of the output media file.
     * @see android.media.MediaMuxer.OutputFormat
     * @throws IOException if failed to open the file for write
     */
    CARAPI constructor(
        /* [in] */ const String& path,
        /* [in] */ Int32 format);

    /**
     * Sets the orientation hint for output video playback.
     * <p>This method should be called before {@link #start}. Calling this
     * method will not rotate the video frame when muxer is generating the file,
     * but add a composition matrix containing the rotation angle in the output
     * video if the output format is
     * {@link OutputFormat#MUXER_OUTPUT_MPEG_4} so that a video player can
     * choose the proper orientation for playback. Note that some video players
     * may choose to ignore the composition matrix in a video during playback.
     * By default, the rotation degree is 0.</p>
     * @param degrees the angle to be rotated clockwise in degrees.
     * The supported angles are 0, 90, 180, and 270 degrees.
     */
    CARAPI SetOrientationHint(
        /* [in] */ Int32 degrees);

    /**
     * Set and store the geodata (latitude and longitude) in the output file.
     * This method should be called before {@link #start}. The geodata is stored
     * in udta box if the output format is
     * {@link OutputFormat#MUXER_OUTPUT_MPEG_4}, and is ignored for other output
     * formats. The geodata is stored according to ISO-6709 standard.
     *
     * @param latitude Latitude in degrees. Its value must be in the range [-90,
     * 90].
     * @param longitude Longitude in degrees. Its value must be in the range
     * [-180, 180].
     * @throws IllegalArgumentException If the given latitude or longitude is out
     * of range.
     * @throws IllegalStateException If this method is called after {@link #start}.
     */
    CARAPI SetLocation(
        /* [in] */ Float latitude,
        /* [in] */ Float longitude);

    /**
     * Starts the muxer.
     * <p>Make sure this is called after {@link #addTrack} and before
     * {@link #writeSampleData}.</p>
     */
    CARAPI Start();

    /**
     * Stops the muxer.
     * <p>Once the muxer stops, it can not be restarted.</p>
     */
    CARAPI Stop();

    /**
     * Adds a track with the specified format.
     * @param format The media format for the track.
     * @return The track index for this newly added track, and it should be used
     * in the {@link #writeSampleData}.
     */
    CARAPI AddTrack(
        /* [in] */ IMediaFormat* format,
        /* [out] */ Int32* result);

    /**
     * Writes an encoded sample into the muxer.
     * <p>The application needs to make sure that the samples are written into
     * the right tracks. Also, it needs to make sure the samples for each track
     * are written in chronological order (e.g. in the order they are provided
     * by the encoder.)</p>
     * @param byteBuf The encoded sample.
     * @param trackIndex The track index for this sample.
     * @param bufferInfo The buffer information related to this sample.
     * MediaMuxer uses the flags provided in {@link MediaCodec.BufferInfo},
     * to signal sync frames.
     */
    CARAPI WriteSampleData(
        /* [in] */ Int32 trackIndex,
        /* [in] */ IByteBuffer* byteBuf,
        /* [in] */ IMediaCodecBufferInfo* bufferInfo);

    /**
     * Make sure you call this when you're done to free up any resources
     * instead of relying on the garbage collector to do this for you at
     * some point in the future.
     */
    CARAPI ReleaseResources();

private:
    // All the native functions are listed here.
    static CARAPI_(Int64) NativeSetup(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 format);

    static CARAPI_(void) NativeRelease(
        /* [in] */ Int64 nativeObject);

    static CARAPI NativeStart(
        /* [in] */ Int64 nativeObject);

    static CARAPI NativeStop(
        /* [in] */ Int64 nativeObject);

    static CARAPI NativeAddTrack(
        /* [in] */ Int64 nativeObject,
        /* [in] */ ArrayOf<String>* keys,
        /* [in] */ ArrayOf<IInterface*>* values,
        /* [out] */ Int32* result);

    static CARAPI NativeSetOrientationHint(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Int32 degrees);

    static CARAPI NativeSetLocation(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Int32 latitude,
        /* [in] */ Int32 longitude);

    static CARAPI NativeWriteSampleData(
        /* [in] */ Int64 nativeObject,
        /* [in] */ Int32 trackIndex,
        /* [in] */ IByteBuffer* byteBuf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 size,
        /* [in] */ Int64 presentationTimeUs,
        /* [in] */ Int32 flags);

private:
    // Muxer internal states.
    static const Int32 MUXER_STATE_UNINITIALIZED;
    static const Int32 MUXER_STATE_INITIALIZED;
    static const Int32 MUXER_STATE_STARTED;
    static const Int32 MUXER_STATE_STOPPED;

    Int32 mState;

    AutoPtr<ICloseGuard> mCloseGuard;
    Int32 mLastTrackIndex;

    Int64 mNativeObject;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CMEDIAMUXER_H__

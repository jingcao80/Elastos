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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIAFORMAT_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAFORMAT_H__

#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Media_CMediaFormat.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::IO::IByteBuffer;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Encapsulates the information describing the format of media data,
 * be it audio or video.
 *
 * The format of the media data is specified as string/value pairs.
 *
 * Keys common to all formats, <b>all keys not marked optional are mandatory</b>:
 *
 * <table>
 * <tr><th>Name</th><th>Value Type</th><th>Description</th></tr>
 * <tr><td>{@link #KEY_MIME}</td><td>String</td><td>The type of the format.</td></tr>
 * <tr><td>{@link #KEY_MAX_INPUT_SIZE}</td><td>Integer</td><td>optional, maximum size of a buffer of input data</td></tr>
 * <tr><td>{@link #KEY_BIT_RATE}</td><td>Integer</td><td><b>encoder-only</b>, desired bitrate in bits/second</td></tr>
 * </table>
 *
 * Video formats have the following keys:
 * <table>
 * <tr><th>Name</th><th>Value Type</th><th>Description</th></tr>
 * <tr><td>{@link #KEY_WIDTH}</td><td>Integer</td><td></td></tr>
 * <tr><td>{@link #KEY_HEIGHT}</td><td>Integer</td><td></td></tr>
 * <tr><td>{@link #KEY_COLOR_FORMAT}</td><td>Integer</td><td><b>encoder-only</b></td></tr>
 * <tr><td>{@link #KEY_FRAME_RATE}</td><td>Integer or Float</td><td><b>encoder-only</b></td></tr>
 * <tr><td>{@link #KEY_I_FRAME_INTERVAL}</td><td>Integer</td><td><b>encoder-only</b></td></tr>
 * </table>
 *
 * Audio formats have the following keys:
 * <table>
 * <tr><th>Name</th><th>Value Type</th><th>Description</th></tr>
 * <tr><td>{@link #KEY_CHANNEL_COUNT}</td><td>Integer</td><td></td></tr>
 * <tr><td>{@link #KEY_SAMPLE_RATE}</td><td>Integer</td><td></td></tr>
 * <tr><td>{@link #KEY_IS_ADTS}</td><td>Integer</td><td>optional, if <em>decoding</em> AAC audio content, setting this key to 1 indicates that each audio frame is prefixed by the ADTS header.</td></tr>
 * <tr><td>{@link #KEY_AAC_PROFILE}</td><td>Integer</td><td><b>encoder-only</b>, optional, if content is AAC audio, specifies the desired profile.</td></tr>
 * <tr><td>{@link #KEY_CHANNEL_MASK}</td><td>Integer</td><td>optional, a mask of audio channel assignments</td></tr>
 * <tr><td>{@link #KEY_FLAC_COMPRESSION_LEVEL}</td><td>Integer</td><td><b>encoder-only</b>, optional, if content is FLAC audio, specifies the desired compression level.</td></tr>
 * </table>
 *
 */
CarClass(CMediaFormat)
    , public Object
    , public IMediaFormat
{
public:
    CMediaFormat();

    virtual ~CMediaFormat();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IMap* map);

    /**
     * Returns true iff a key of the given name exists in the format.
     */
    CARAPI ContainsKey(
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    /**
     * Returns the value of an integer key.
     */
    CARAPI GetInt32(
        /* [in] */ const String& name,
        /* [out] */ Int32* result);

    /**
     * Returns the value of an integer key, or the default value if the
     * key is missing or is for another type value.
     * @hide
     */
    CARAPI GetInt32(
        /* [in] */ const String& name,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32* result);

    /**
     * Returns the value of a long key.
     */
    CARAPI GetInt64(
        /* [in] */ const String& name,
        /* [out] */ Int64* result);

    /**
     * Returns the value of a float key.
     */
    CARAPI GetFloat(
        /* [in] */ const String& name,
        /* [out] */ Float* result);

    /**
     * Returns the value of a string key.
     */
    CARAPI GetString(
        /* [in] */ const String& name,
        /* [out] */ String* result);

    /**
     * Returns the value of a ByteBuffer key.
     */
    CARAPI GetByteBuffer(
        /* [in] */ const String& name,
        /* [out] */ IByteBuffer** result);

    /**
     * Returns whether a feature is to be enabled ({@code true}) or disabled
     * ({@code false}).
     *
     * @param feature the name of a {@link MediaCodecInfo.CodecCapabilities} feature.
     *
     * @throws IllegalArgumentException if the feature was neither set to be enabled
     *        nor to be disabled.
     */
    CARAPI GetFeatureEnabled(
        /* [in] */ const String& feature,
        /* [out] */ Boolean* result);

    /**
     * Sets the value of an integer key.
     */
    CARAPI SetInt32(
        /* [in] */ const String& name,
        /* [in] */ Int32 value);

    /**
     * Sets the value of a long key.
     */
    CARAPI SetInt64(
        /* [in] */ const String& name,
        /* [in] */ Int64 value);

    /**
     * Sets the value of a float key.
     */
    CARAPI SetFloat(
        /* [in] */ const String& name,
        /* [in] */ Float value);

    /**
     * Sets the value of a string key.
     */
    CARAPI SetString(
        /* [in] */ const String& name,
        /* [in] */ const String& value);

    /**
     * Sets the value of a ByteBuffer key.
     */
    CARAPI SetByteBuffer(
        /* [in] */ const String& name,
        /* [in] */ IByteBuffer* bytes);

    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Creates a minimal audio format.
     * @param mime The mime type of the content.
     * @param sampleRate The sampling rate of the content.
     * @param channelCount The number of audio channels in the content.
     */
    static CARAPI CreateAudioFormat(
        /* [in] */ const String& mime,
        /* [in] */ Int32 sampleRate,
        /* [in] */ Int32 channelCount,
        /* [out] */ IMediaFormat** result);

    /**
     * Creates a minimal subtitle format.
     * @param mime The mime type of the content.
     * @param language The language of the content, using either ISO 639-1 or 639-2/T
     *        codes.  Specify null or "und" if language information is only included
     *        in the content.  (This will also work if there are multiple language
     *        tracks in the content.)
     */
    static CARAPI CreateSubtitleFormat(
        /* [in] */ const String& mime,
        /* [in] */ const String& language,
        /* [out] */ IMediaFormat** result);

    /**
     * Creates a minimal video format.
     * @param mime The mime type of the content.
     * @param width The width of the content (in pixels)
     * @param height The height of the content (in pixels)
     */
   static CARAPI CreateVideoFormat(
        /* [in] */ const String& mime,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ IMediaFormat** result);

    CARAPI GetMap(
        /* [out] */ IMap** result);

    /**
     * Sets whether a feature is to be enabled ({@code true}) or disabled
     * ({@code false}).
     *
     * If {@code enabled} is {@code true}, the feature is requested to be present.
     * Otherwise, the feature is requested to be not present.
     *
     * @param feature the name of a {@link MediaCodecInfo.CodecCapabilities} feature.
     *
     * @see MediaCodecList#findDecoderForFormat
     * @see MediaCodecList#findEncoderForFormat
     * @see MediaCodecInfo.CodecCapabilities#isFormatSupported
     */
    CARAPI SetFeatureEnabled(
        /* [in] */ const String& feature,
        /* [in] */ Boolean enabled);

private:
    AutoPtr<IMap> mMap;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAFORMAT_H__

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

#ifndef __ELASTOS_DROID_MEDIA_CAUDIOFORMAT_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOFORMAT_H__

#include "_Elastos_Droid_Media_CAudioFormat.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * The AudioFormat class is used to access a number of audio format and
 * channel configuration constants. They are for instance used
 * in {@link AudioTrack} and {@link AudioRecord}.
 *
 */
CarClass(CAudioFormat)
    , public Object
    , public IAudioFormat
{
public:
    friend class CAudioFormatBuilder;

    CAudioFormat();

    virtual ~CAudioFormat();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /** @removed */
    CARAPI constructor();

    /**
     * Private constructor with an ignored argument to differentiate from the removed default ctor
     * @param ignoredArgument
     */
    CARAPI constructor(
        /* [in] */ Int32 ignoredArgument);

    /**
     * Constructor used by the JNI
     */
    // Update sound trigger JNI in core/jni/android_hardware_SoundTrigger.cpp when modifying this
    // constructor
    CARAPI constructor(
        /* [in] */ Int32 encoding,
        /* [in] */ Int32 sampleRate,
        /* [in] */ Int32 channelMask);

    /**
     * Return the encoding.
     * @return one of the values that can be set in {@link Builder#setEncoding(int)} or
     * {@link AudioFormat#ENCODING_INVALID} if not set.
     */
    CARAPI GetEncoding(
        /* [out] */ Int32* result);

    /**
     * Return the sample rate.
     * @return one of the values that can be set in {@link Builder#setSampleRate(int)} or
     * 0 if not set.
     */
    CARAPI GetSampleRate(
        /* [out] */ Int32* result);

    /**
     * Return the channel mask.
     * @return one of the values that can be set in {@link Builder#setChannelMask(int)} or
     * {@link AudioFormat#CHANNEL_INVALID} if not set.
     */
    CARAPI GetChannelMask(
        /* [out] */ Int32* result);

    /** @hide */
    CARAPI GetPropertySetMask(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * @hide
     * Return the number of channels from an input channel mask
     * @param mask a combination of the CHANNEL_IN_* definitions, even CHANNEL_IN_DEFAULT
     * @return number of channels for the mask
     */
    static CARAPI ChannelCountFromInChannelMask(
        /* [in] */ Int32 mask,
        /* [out] */ Int32* result);

    /**
     * @hide
     * Return the number of channels from an output channel mask
     * @param mask a combination of the CHANNEL_OUT_* definitions, but not CHANNEL_OUT_DEFAULT
     * @return number of channels for the mask
     */
    static CARAPI ChannelCountFromOutChannelMask(
        /* [in] */ Int32 mask,
        /* [out] */ Int32* result);

    /**
     * @hide
     * Return a channel mask ready to be used by native code
     * @param mask a combination of the CHANNEL_OUT_* definitions, but not CHANNEL_OUT_DEFAULT
     * @return a native channel mask
     */
    static CARAPI ConvertChannelOutMaskToNativeMask(
        /* [in] */ Int32 javaMask,
        /* [out] */ Int32* result);

    /**
     * @hide
     * Return a java output channel mask
     * @param mask a native channel mask
     * @return a combination of the CHANNEL_OUT_* definitions
     */
    static CARAPI ConvertNativeChannelMaskToOutMask(
        /* [in] */ Int32 nativeMask,
        /* [out] */ Int32* result);

    /** @hide */
    static CARAPI GetBytesPerSample(
        /* [in] */ Int32 audioFormat,
        /* [out] */ Int32* result);

    /** @hide */
    static CARAPI IsValidEncoding(
        /* [in] */ Int32 audioFormat,
        /* [out] */ Boolean* result);

    /** @hide */
    static CARAPI IsEncodingLinearPcm(
        /* [in] */ Int32 audioFormat,
        /* [out] */ Boolean* result);

private:
    Int32 mEncoding;
    Int32 mSampleRate;
    Int32 mChannelMask;
    Int32 mPropertySetMask;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIOFORMAT_H__

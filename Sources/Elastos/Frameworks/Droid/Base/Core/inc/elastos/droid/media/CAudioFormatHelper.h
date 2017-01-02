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

#ifndef __ELASTOS_DROID_MEDIA_CAUDIOFORMATHELPER_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOFORMATHELPER_H__

#include "_Elastos_Droid_Media_CAudioFormatHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CAudioFormatHelper)
    , public Singleton
    , public IAudioFormatHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * @hide
     * Return the number of channels from an input channel mask
     * @param mask a combination of the CHANNEL_IN_* definitions, even CHANNEL_IN_DEFAULT
     * @return number of channels for the mask
     */
    CARAPI ChannelCountFromInChannelMask(
        /* [in] */ Int32 mask,
        /* [out] */ Int32* result);

    /**
     * @hide
     * Return the number of channels from an output channel mask
     * @param mask a combination of the CHANNEL_OUT_* definitions, but not CHANNEL_OUT_DEFAULT
     * @return number of channels for the mask
     */
    CARAPI ChannelCountFromOutChannelMask(
        /* [in] */ Int32 mask,
        /* [out] */ Int32* result);

    /**
     * @hide
     * Return a channel mask ready to be used by native code
     * @param mask a combination of the CHANNEL_OUT_* definitions, but not CHANNEL_OUT_DEFAULT
     * @return a native channel mask
     */
    CARAPI ConvertChannelOutMaskToNativeMask(
        /* [in] */ Int32 javaMask,
        /* [out] */ Int32* result);

    /**
     * @hide
     * Return a java output channel mask
     * @param mask a native channel mask
     * @return a combination of the CHANNEL_OUT_* definitions
     */
    CARAPI ConvertNativeChannelMaskToOutMask(
        /* [in] */ Int32 nativeMask,
        /* [out] */ Int32* result);

    /** @hide */
    CARAPI GetBytesPerSample(
        /* [in] */ Int32 audioFormat,
        /* [out] */ Int32* result);

    /** @hide */
    CARAPI IsValidEncoding(
        /* [in] */ Int32 audioFormat,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI IsEncodingLinearPcm(
        /* [in] */ Int32 audioFormat,
        /* [out] */ Boolean* result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CAUDIOFORMATHELPER_H__

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

#include "elastos/droid/media/CAudioFormatHelper.h"
#include "elastos/droid/media/CAudioFormat.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioFormatHelper, Singleton, IAudioFormatHelper)

CAR_SINGLETON_IMPL(CAudioFormatHelper)

ECode CAudioFormatHelper::ChannelCountFromInChannelMask(
    /* [in] */ Int32 mask,
    /* [out] */ Int32* result)
{
    return CAudioFormat::ChannelCountFromInChannelMask(mask, result);
}

ECode CAudioFormatHelper::ChannelCountFromOutChannelMask(
    /* [in] */ Int32 mask,
    /* [out] */ Int32* result)
{
    return CAudioFormat::ChannelCountFromOutChannelMask(mask, result);
}

ECode CAudioFormatHelper::ConvertChannelOutMaskToNativeMask(
    /* [in] */ Int32 javaMask,
    /* [out] */ Int32* result)
{
    return CAudioFormat::ConvertChannelOutMaskToNativeMask(javaMask, result);
}

ECode CAudioFormatHelper::ConvertNativeChannelMaskToOutMask(
    /* [in] */ Int32 nativeMask,
    /* [out] */ Int32* result)
{
    return CAudioFormat::ConvertNativeChannelMaskToOutMask(nativeMask, result);
}

ECode CAudioFormatHelper::GetBytesPerSample(
    /* [in] */ Int32 audioFormat,
    /* [out] */ Int32* result)
{
    return CAudioFormat::GetBytesPerSample(audioFormat, result);
}

ECode CAudioFormatHelper::IsValidEncoding(
    /* [in] */ Int32 audioFormat,
    /* [out] */ Boolean* result)
{
    return CAudioFormat::IsValidEncoding(audioFormat, result);
}

ECode CAudioFormatHelper::IsEncodingLinearPcm(
    /* [in] */ Int32 audioFormat,
    /* [out] */ Boolean* result)
{
    return CAudioFormat::IsEncodingLinearPcm(audioFormat, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos

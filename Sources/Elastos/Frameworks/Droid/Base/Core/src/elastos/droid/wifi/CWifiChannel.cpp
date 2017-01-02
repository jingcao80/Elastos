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

#include "elastos/droid/wifi/CWifiChannel.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CWifiChannel, Object, IParcelable, IWifiChannel)

CAR_OBJECT_IMPL(CWifiChannel)

ECode CWifiChannel::constructor()
{
    mFreqMHz = 0;
    mChannelNum = 0;
    mIsDFS = FALSE;
    return NOERROR;
}

ECode CWifiChannel::GetFreqMHz(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFreqMHz;
    return NOERROR;
}

ECode CWifiChannel::SetFreqMHz(
    /* [in] */ Int32 freqMHz)
{
    mFreqMHz = freqMHz;
    return NOERROR;
}

ECode CWifiChannel::GetChannelNum(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mChannelNum;
    return NOERROR;
}

ECode CWifiChannel::SetChannelNum(
    /* [in] */ Int32 channelNum)
{
    mChannelNum = channelNum;
    return NOERROR;
}

ECode CWifiChannel::GetIsDFS(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsDFS;
    return NOERROR;
}

ECode CWifiChannel::SetIsDFS(
    /* [in] */ Boolean isDFS)
{
    mIsDFS = isDFS;
    return NOERROR;
}

ECode CWifiChannel::GetIbssAllowed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIbssAllowed;
    return NOERROR;
}

ECode CWifiChannel::SetIbssAllowed(
    /* [in] */ Boolean ibssAllowed)
{
    mIbssAllowed = ibssAllowed;
    return NOERROR;
}

/** check for validity */
ECode CWifiChannel::IsValid(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mFreqMHz < MIN_FREQ_MHZ || mFreqMHz > MAX_FREQ_MHZ) {
        *result = FALSE;
        return NOERROR;
    }

    if (mChannelNum < MIN_CHANNEL_NUM || mChannelNum > MAX_CHANNEL_NUM) {
        *result = FALSE;
        return NOERROR;
    }

    *result = TRUE;

    return NOERROR;
}

/** implement Parcelable interface */
//@Override
ECode CWifiChannel::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

/** implement Parcelable interface */
//@Override
ECode CWifiChannel::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteInt32(mFreqMHz);
    out->WriteInt32(mChannelNum);
    out->WriteInt32(mIsDFS ? 1 : 0);
    out->WriteInt32(mIbssAllowed ? 1 : 0);
    return NOERROR;
}

ECode CWifiChannel::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

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

#include "elastos/droid/telephony/CIccOpenLogicalChannelResponse.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL_2(CIccOpenLogicalChannelResponse, Object, IIccOpenLogicalChannelResponse, IParcelable)

CAR_OBJECT_IMPL(CIccOpenLogicalChannelResponse)

CIccOpenLogicalChannelResponse::CIccOpenLogicalChannelResponse()
    : mChannel()
    , mStatus()
{
}

ECode CIccOpenLogicalChannelResponse::constructor()
{
    return NOERROR;
}

ECode CIccOpenLogicalChannelResponse::constructor(
    /* [in] */ Int32 channel,
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* selectResponse)
{
    mChannel = channel;
    mStatus = status;
    mSelectResponse = selectResponse;
    return NOERROR;
}

ECode CIccOpenLogicalChannelResponse::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mChannel);
    source->ReadInt32(&mStatus);
    source->ReadArrayOf((HANDLE*)(&mSelectResponse));
    return NOERROR;
}

ECode CIccOpenLogicalChannelResponse::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mChannel);
    dest->WriteInt32(mStatus);
    dest->WriteArrayOf((HANDLE)mSelectResponse.Get());
    return NOERROR;
}

ECode CIccOpenLogicalChannelResponse::GetChannel(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mChannel;
    return NOERROR;
}

ECode CIccOpenLogicalChannelResponse::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStatus;
    return NOERROR;
}

ECode CIccOpenLogicalChannelResponse::GetSelectResponse(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSelectResponse;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CIccOpenLogicalChannelResponse::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;

    sb.Append("Channel: ");
    sb.Append(mChannel);
    sb.Append(" Status: ");
    sb.Append(mStatus);

    *str = sb.ToString();
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

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

#include "elastos/droid/net/NetworkQuotaInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(NetworkQuotaInfo, Object, IParcelable, INetworkQuotaInfo)

ECode NetworkQuotaInfo::constructor()
{
    return NOERROR;
}

ECode NetworkQuotaInfo::constructor(
    /* [in] */ Int64 estimatedBytes,
    /* [in] */ Int64 softLimitBytes,
    /* [in] */ Int64 hardLimitBytes)
{
    mEstimatedBytes = estimatedBytes;
    mSoftLimitBytes = softLimitBytes;
    mHardLimitBytes = hardLimitBytes;
    return NOERROR;
}

ECode NetworkQuotaInfo::GetEstimatedBytes(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mEstimatedBytes;
    return NOERROR;
}

ECode NetworkQuotaInfo::GetSoftLimitBytes(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mSoftLimitBytes;
    return NOERROR;
}

ECode NetworkQuotaInfo::GetHardLimitBytes(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mHardLimitBytes;
    return NOERROR;
}

ECode NetworkQuotaInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    source->ReadInt64(&mEstimatedBytes);
    source->ReadInt64(&mSoftLimitBytes);
    source->ReadInt64(&mHardLimitBytes);
    return NOERROR;
}

ECode NetworkQuotaInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    dest->WriteInt64(mEstimatedBytes);
    dest->WriteInt64(mSoftLimitBytes);
    dest->WriteInt64(mHardLimitBytes);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos

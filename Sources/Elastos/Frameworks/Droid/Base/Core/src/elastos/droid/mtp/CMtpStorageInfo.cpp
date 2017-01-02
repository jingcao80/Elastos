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

#include "elastos/droid/mtp/CMtpStorageInfo.h"

namespace Elastos {
namespace Droid {
namespace Mtp {

CMtpStorageInfo::CMtpStorageInfo()
    : mStorageId(0)
    , mMaxCapacity(0)
    , mFreeSpace(0)
    , mDescription(NULL)
    , mVolumeIdentifier(NULL)
{}

CAR_INTERFACE_IMPL(CMtpStorageInfo, Object, IMtpStorageInfo)

CAR_OBJECT_IMPL(CMtpStorageInfo)

ECode CMtpStorageInfo::GetStorageId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mStorageId;
    return NOERROR;
}

ECode CMtpStorageInfo::GetMaxCapacity(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mMaxCapacity;
    return NOERROR;
}

ECode CMtpStorageInfo::GetFreeSpace(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mFreeSpace;
    return NOERROR;
}

ECode CMtpStorageInfo::GetDescription(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mDescription;
    return NOERROR;
}

ECode CMtpStorageInfo::GetVolumeIdentifier(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mVolumeIdentifier;
    return NOERROR;
}

ECode CMtpStorageInfo::SetStorageId(
    /* [in] */ Int32 result)
{
    mStorageId = result;
    return NOERROR;
}

ECode CMtpStorageInfo::SetMaxCapacity(
    /* [in] */ Int64 result)
{
    mMaxCapacity = result;
    return NOERROR;
}

ECode CMtpStorageInfo::SetFreeSpace(
    /* [in] */ Int64 result)
{
    mFreeSpace = result;
    return NOERROR;
}

ECode CMtpStorageInfo::SetDescription(
    /* [in] */ const String& result)
{
    mDescription = result;
    return NOERROR;
}

ECode CMtpStorageInfo::SetVolumeIdentifier(
    /* [in] */ const String& result)
{
    mVolumeIdentifier = result;
    return NOERROR;
}

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos
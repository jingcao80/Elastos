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

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/mtp/MtpStorage.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Mtp {

MtpStorage::MtpStorage()
    : mStorageId(0)
    , mPath(NULL)
    , mDescription(NULL)
    , mReserveSpace(0)
    , mRemovable(FALSE)
    , mMaxFileSize(0)
{}

MtpStorage::~MtpStorage()
{}

CAR_INTERFACE_IMPL(MtpStorage, Object, IMtpStorage)

ECode MtpStorage::constructor(
    /* [in] */ IStorageVolume* volume,
    /* [in] */ IContext* context)
{
    volume->GetStorageId(&mStorageId);
    volume->GetPath(&mPath);
    Int32 descId;
    volume->GetDescriptionId(&descId);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetString(descId, &mDescription);
    Int32 space;
    volume->GetMtpReserveSpace(&space);
    mReserveSpace = space * 1024LL * 1024LL;
    volume->IsRemovable(&mRemovable);
    volume->GetMaxFileSize(&mMaxFileSize);
    return NOERROR;
}

ECode MtpStorage::GetStorageId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mStorageId;
    return NOERROR;
}

ECode MtpStorage::GetStorageId(
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    // storage ID is 0x00010001 for primary storage,
    // then 0x00020001, 0x00030001, etc. for secondary storages
    VALIDATE_NOT_NULL(result);

    *result = ((index + 1) << 16) + 1;
    return NOERROR;
}

ECode MtpStorage::GetPath(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mPath;
    return NOERROR;
}

ECode MtpStorage::GetDescription(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mDescription;
    return NOERROR;
}

ECode MtpStorage::GetReserveSpace(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mReserveSpace;
    return NOERROR;
}

ECode MtpStorage::IsRemovable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRemovable;
    return NOERROR;
}

ECode MtpStorage::GetMaxFileSize(
        /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mMaxFileSize;
    return NOERROR;
}

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos

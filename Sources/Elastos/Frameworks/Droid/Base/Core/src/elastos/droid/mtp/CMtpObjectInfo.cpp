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
#include "elastos/droid/mtp/CMtpObjectInfo.h"

namespace Elastos {
namespace Droid {
namespace Mtp {

CMtpObjectInfo::CMtpObjectInfo()
    : mHandle(0)
    , mStorageId(0)
    , mFormat(0)
    , mProtectionStatus(0)
    , mCompressedSize(0)
    , mThumbFormat(0)
    , mThumbCompressedSize(0)
    , mThumbPixWidth(0)
    , mThumbPixHeight(0)
    , mImagePixWidth(0)
    , mImagePixHeight(0)
    , mImagePixDepth(0)
    , mParent(0)
    , mAssociationType(0)
    , mAssociationDesc(0)
    , mSequenceNumber(0)
    , mName(NULL)
    , mDateCreated(0)
    , mDateModified(0)
    , mKeywords(NULL)
{}

CAR_INTERFACE_IMPL(CMtpObjectInfo, Object, IMtpObjectInfo)

CAR_OBJECT_IMPL(CMtpObjectInfo)

ECode CMtpObjectInfo::GetObjectHandle(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mHandle;
    return NOERROR;
}

ECode CMtpObjectInfo::GetStorageId(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mStorageId;
    return NOERROR;
}

ECode CMtpObjectInfo::GetFormat(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mFormat;
    return NOERROR;
}

ECode CMtpObjectInfo::GetProtectionStatus(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mProtectionStatus;
    return NOERROR;
}

ECode CMtpObjectInfo::GetCompressedSize(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mCompressedSize;
    return NOERROR;
}

ECode CMtpObjectInfo::GetThumbFormat(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mThumbFormat;
    return NOERROR;
}

ECode CMtpObjectInfo::GetThumbCompressedSize(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mThumbCompressedSize;
    return NOERROR;
}

ECode CMtpObjectInfo::GetThumbPixWidth(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mThumbPixWidth;
    return NOERROR;
}

ECode CMtpObjectInfo::GetThumbPixHeight(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mThumbPixHeight;
    return NOERROR;
}

ECode CMtpObjectInfo::GetImagePixWidth(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mImagePixWidth;
    return NOERROR;
}

ECode CMtpObjectInfo::GetImagePixHeight(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mImagePixHeight;
    return NOERROR;
}

ECode CMtpObjectInfo::GetImagePixDepth(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mImagePixDepth;
    return NOERROR;
}

ECode CMtpObjectInfo::GetParent(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mParent;
    return NOERROR;
}

ECode CMtpObjectInfo::GetAssociationType(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mAssociationType;
    return NOERROR;
}

ECode CMtpObjectInfo::GetAssociationDesc(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mAssociationDesc;
    return NOERROR;
}

ECode CMtpObjectInfo::GetSequenceNumber(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mSequenceNumber;
    return NOERROR;
}

ECode CMtpObjectInfo::GetName(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mName;
    return NOERROR;
}

ECode CMtpObjectInfo::GetDateCreated(
        /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mDateCreated;
    return NOERROR;
}

ECode CMtpObjectInfo::GetDateModified(
        /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mDateModified;
    return NOERROR;
}

ECode CMtpObjectInfo::GetKeywords(
        /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mKeywords;
    return NOERROR;
}

ECode CMtpObjectInfo::SetObjectHandle(
        /* [in] */ Int32 result)
{
    mHandle = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetStorageId(
        /* [in] */ Int32 result)
{
    mStorageId = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetFormat(
        /* [in] */ Int32 result)
{
    mFormat = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetProtectionStatus(
        /* [in] */ Int32 result)
{
    mProtectionStatus = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetCompressedSize(
        /* [in] */ Int32 result)
{
    mCompressedSize = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetThumbFormat(
        /* [in] */ Int32 result)
{
    mThumbFormat = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetThumbCompressedSize(
        /* [in] */ Int32 result)
{
    mThumbCompressedSize = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetThumbPixWidth(
        /* [in] */ Int32 result)
{
    mThumbPixWidth = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetThumbPixHeight(
        /* [in] */ Int32 result)
{
    mThumbPixHeight = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetImagePixWidth(
        /* [in] */ Int32 result)
{
    mImagePixWidth = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetImagePixHeight(
        /* [in] */ Int32 result)
{
    mImagePixHeight = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetImagePixDepth(
        /* [in] */ Int32 result)
{
    mImagePixDepth = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetParent(
        /* [in] */ Int32 result)
{
    mParent = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetAssociationType(
        /* [in] */ Int32 result)
{
    mAssociationType = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetAssociationDesc(
        /* [in] */ Int32 result)
{
    mAssociationDesc = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetSequenceNumber(
        /* [in] */ Int32 result)
{
    mSequenceNumber = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetName(
        /* [in] */ const String& result)
{
    mName = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetDateCreated(
        /* [in] */ Int64 result)
{
    mDateCreated = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetDateModified(
        /* [in] */ Int64 result)
{
    mDateModified = result;
    return NOERROR;
}

ECode CMtpObjectInfo::SetKeywords(
        /* [in] */ const String& result)
{
    mKeywords = result;
    return NOERROR;
}
} // namespace Mtp
} // namepsace Droid
} // namespace Elastos

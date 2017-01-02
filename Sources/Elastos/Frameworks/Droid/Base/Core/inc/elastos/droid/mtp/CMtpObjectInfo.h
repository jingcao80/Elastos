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

#ifndef __ELASTOS_DROID_MTP_CMTPOBJECTINFO_H__
#define __ELASTOS_DROID_MTP_CMTPOBJECTINFO_H__

#include "_Elastos_Droid_Mtp_CMtpObjectInfo.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Mtp {

CarClass(CMtpObjectInfo)
    , public Object
    , public IMtpObjectInfo
{
public:
    CMtpObjectInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetObjectHandle(
        /* [out] */ Int32* result);

    CARAPI GetStorageId(
        /* [out] */ Int32* result);

    CARAPI GetFormat(
        /* [out] */ Int32* result);

    CARAPI GetProtectionStatus(
        /* [out] */ Int32* result);

    CARAPI GetCompressedSize(
        /* [out] */ Int32* result);

    CARAPI GetThumbFormat(
        /* [out] */ Int32* result);

    CARAPI GetThumbCompressedSize(
        /* [out] */ Int32* result);

    CARAPI GetThumbPixWidth(
        /* [out] */ Int32* result);

    CARAPI GetThumbPixHeight(
        /* [out] */ Int32* result);

    CARAPI GetImagePixWidth(
        /* [out] */ Int32* result);

    CARAPI GetImagePixHeight(
        /* [out] */ Int32* result);

    CARAPI GetImagePixDepth(
        /* [out] */ Int32* result);

    CARAPI GetParent(
        /* [out] */ Int32* result);

    CARAPI GetAssociationType(
        /* [out] */ Int32* result);

    CARAPI GetAssociationDesc(
        /* [out] */ Int32* result);

    CARAPI GetSequenceNumber(
        /* [out] */ Int32* result);

    CARAPI GetName(
        /* [out] */ String* result);

    CARAPI GetDateCreated(
        /* [out] */ Int64* result);

    CARAPI GetDateModified(
        /* [out] */ Int64* result);

    CARAPI GetKeywords(
        /* [out] */ String* result);

    CARAPI SetObjectHandle(
        /* [in] */ Int32 result);

    CARAPI SetStorageId(
        /* [in] */ Int32 result);

    CARAPI SetFormat(
        /* [in] */ Int32 result);

    CARAPI SetProtectionStatus(
        /* [in] */ Int32 result);

    CARAPI SetCompressedSize(
        /* [in] */ Int32 result);

    CARAPI SetThumbFormat(
        /* [in] */ Int32 result);

    CARAPI SetThumbCompressedSize(
        /* [in] */ Int32 result);

    CARAPI SetThumbPixWidth(
        /* [in] */ Int32 result);

    CARAPI SetThumbPixHeight(
        /* [in] */ Int32 result);

    CARAPI SetImagePixWidth(
        /* [in] */ Int32 result);

    CARAPI SetImagePixHeight(
        /* [in] */ Int32 result);

    CARAPI SetImagePixDepth(
        /* [in] */ Int32 result);

    CARAPI SetParent(
        /* [in] */ Int32 result);

    CARAPI SetAssociationType(
        /* [in] */ Int32 result);

    CARAPI SetAssociationDesc(
        /* [in] */ Int32 result);

    CARAPI SetSequenceNumber(
        /* [in] */ Int32 result);

    CARAPI SetName(
        /* [in] */ const String& result);

    CARAPI SetDateCreated(
        /* [in] */ Int64 result);

    CARAPI SetDateModified(
        /* [in] */ Int64 result);

    CARAPI SetKeywords(
        /* [in] */ const String& result);

private:
    Int32 mHandle;
    Int32 mStorageId;
    Int32 mFormat;
    Int32 mProtectionStatus;
    Int32 mCompressedSize;
    Int32 mThumbFormat;
    Int32 mThumbCompressedSize;
    Int32 mThumbPixWidth;
    Int32 mThumbPixHeight;
    Int32 mImagePixWidth;
    Int32 mImagePixHeight;
    Int32 mImagePixDepth;
    Int32 mParent;
    Int32 mAssociationType;
    Int32 mAssociationDesc;
    Int32 mSequenceNumber;
    String mName;
    Int64 mDateCreated;
    Int64 mDateModified;
    String mKeywords;
};

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MTP_CMTPOBJECTINFO_H__

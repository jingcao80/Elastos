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

#ifndef __ELASTOS_DROID_MTP_CMTPSTORAGEINFO_H__
#define __ELASTOS_DROID_MTP_CMTPSTORAGEINFO_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Mtp_CMtpStorageInfo.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Mtp {

CarClass(CMtpStorageInfo)
    , public Object
    , public IMtpStorageInfo
{
public:
    CMtpStorageInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetStorageId(
        /* [out] */ Int32* result);

    CARAPI GetMaxCapacity(
        /* [out] */ Int64* result);

    CARAPI GetFreeSpace(
        /* [out] */ Int64* result);

    CARAPI GetDescription(
        /* [out] */ String* result);

    CARAPI GetVolumeIdentifier(
        /* [out] */ String* result);

    CARAPI SetStorageId(
        /* [in] */ Int32 result);

    CARAPI SetMaxCapacity(
        /* [in] */ Int64 result);

    CARAPI SetFreeSpace(
        /* [in] */ Int64 result);

    CARAPI SetDescription(
        /* [in] */ const String& result);

    CARAPI SetVolumeIdentifier(
        /* [in] */ const String& result);

private:
    Int32 mStorageId;
    Int64 mMaxCapacity;
    Int64 mFreeSpace;
    String mDescription;
    String mVolumeIdentifier;
};

} // namespace Mtp
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MTP_CMTPSTORAGEINFO_H__

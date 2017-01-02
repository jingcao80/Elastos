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

#ifndef __ELASTOS_DROID_IMS_CIMSREASONINFO_H__
#define __ELASTOS_DROID_IMS_CIMSREASONINFO_H__

#include "_Elastos_Droid_Ims_CImsReasonInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Ims {

CarClass(CImsReasonInfo)
    , public Object
    , public IImsReasonInfo
    , public IParcelable
{
public:
    CImsReasonInfo();

    virtual ~CImsReasonInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 code,
        /* [in] */ Int32 extraCode);

    CARAPI constructor(
        /* [in] */ Int32 code,
        /* [in] */ Int32 extraCode,
        /* [in] */ const String& extraMessage);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetCode(
        /* [out] */ Int32* result);

    CARAPI GetExtraCode(
        /* [out] */ Int32* result);

    CARAPI GetExtraMessage(
        /* [out] */ String* result);

    CARAPI GetReasonType(
        /* [out] */ Int32* result);

public:
    // For reason type
    Int32 mReasonType;
    // For main reason code
    Int32 mCode;
    // For the extra code value; it depends on the code value.
    Int32 mExtraCode;
    // For the additional message of the reason info.
    String mExtraMessage;
};

} // namespace Ims
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_IMS_CIMSREASONINFO_H__

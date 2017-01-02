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

#ifndef __ELASTOS_DROID_IMS_CIMSCALLFORWARDINFO_H__
#define __ELASTOS_DROID_IMS_CIMSCALLFORWARDINFO_H__

#include "_Elastos_Droid_Ims_CImsCallForwardInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Ims {

CarClass(CImsCallForwardInfo)
    , public Object
    , public IImsCallForwardInfo
    , public IParcelable
{
public:
    CImsCallForwardInfo();

    virtual ~CImsCallForwardInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* result);

public:
    // Refer to ImsUtInterface#CDIV_CF_XXX
    Int32 mCondition;
    // 0: disabled, 1: enabled
    Int32 mStatus;
    // 0x91: International, 0x81: Unknown
    Int32 mToA;
    // Number (it will not include the "sip" or "tel" URI scheme)
    String mNumber;
    // No reply timer for CF
    Int32 mTimeSeconds;
    // CFU start hour
    Int32 mStartHour;
    //CFU start minute
    Int32 mStartMinute;
    // CFU end hour
    Int32 mEndHour;
    //CFU end minute
    Int32 mEndMinute;
};

} // namespace Ims
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_IMS_CIMSCALLFORWARDINFO_H__

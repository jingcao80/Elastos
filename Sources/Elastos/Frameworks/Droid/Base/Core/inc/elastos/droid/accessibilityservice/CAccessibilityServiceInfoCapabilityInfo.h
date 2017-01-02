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

#ifndef __ELASTOS_DROID_ACCESSIBILITYSERVICE_CACCESSIBILITYSERVICEINFOCAPABILITYINFO_H__
#define __ELASTOS_DROID_ACCESSIBILITYSERVICE_CACCESSIBILITYSERVICEINFOCAPABILITYINFO_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_AccessibilityService_CAccessibilityServiceInfoCapabilityInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace AccessibilityService {

CarClass(CAccessibilityServiceInfoCapabilityInfo)
    , public Object
    , public IAccessibilityServiceInfoCapabilityInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAccessibilityServiceInfoCapabilityInfo();

    ~CAccessibilityServiceInfoCapabilityInfo();

    CARAPI constructor(
        /* [in] */ Int32 capability,
        /* [in] */ Int32 titleResId,
        /* [in] */ Int32 descResId);

    CARAPI GetCapability(
        /*[out]*/ Int32* capability);

    CARAPI GetTitleResId(
        /*[out]*/ Int32* titleResId);

    CARAPI GetDescResId(
        /*[out]*/ Int32* descResId);

public:
    Int32 mCapability;
    Int32 mTitleResId;
    Int32 mDescResId;
};

}
}
}

#endif // __ELASTOS_DROID_ACCESSIBILITYSERVICE_CACCESSIBILITYSERVICEINFOCAPABILITYINFO_H__

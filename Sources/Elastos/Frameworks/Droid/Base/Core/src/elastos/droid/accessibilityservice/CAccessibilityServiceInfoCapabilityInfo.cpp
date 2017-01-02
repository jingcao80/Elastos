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

#include "elastos/droid/accessibilityservice/CAccessibilityServiceInfoCapabilityInfo.h"

namespace Elastos {
namespace Droid {
namespace AccessibilityService {

CAR_INTERFACE_IMPL(CAccessibilityServiceInfoCapabilityInfo, Object, IAccessibilityServiceInfoCapabilityInfo)

CAR_OBJECT_IMPL(CAccessibilityServiceInfoCapabilityInfo)

CAccessibilityServiceInfoCapabilityInfo::CAccessibilityServiceInfoCapabilityInfo()
    : mCapability(0)
    , mTitleResId(0)
    , mDescResId(0)
{}

CAccessibilityServiceInfoCapabilityInfo::~CAccessibilityServiceInfoCapabilityInfo()
{}

ECode CAccessibilityServiceInfoCapabilityInfo::constructor(
    /* [in] */ Int32 capability,
    /* [in] */ Int32 titleResId,
    /* [in] */ Int32 descResId)
{
    mCapability = capability;
    mTitleResId = titleResId;
    mDescResId = descResId;
    return NOERROR;
}

ECode CAccessibilityServiceInfoCapabilityInfo::GetCapability(
    /*[out]*/ Int32* capability)
{
    VALIDATE_NOT_NULL(capability);
    *capability = mCapability;
    return NOERROR;
}

ECode CAccessibilityServiceInfoCapabilityInfo::GetTitleResId(
    /*[out]*/ Int32* titleResId)
{
    VALIDATE_NOT_NULL(titleResId);
    *titleResId = mTitleResId;
    return NOERROR;
}

ECode CAccessibilityServiceInfoCapabilityInfo::GetDescResId(
    /*[out]*/ Int32* descResId)
{
    VALIDATE_NOT_NULL(descResId);
    *descResId = mDescResId;
    return NOERROR;
}

}
}
}

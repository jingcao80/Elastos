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

#include "elastos/droid/accessibilityservice/CAccessibilityServiceInfoHelper.h"
#include "elastos/droid/accessibilityservice/CAccessibilityServiceInfo.h"

namespace Elastos {
namespace Droid {
namespace AccessibilityService {

CAR_INTERFACE_IMPL(CAccessibilityServiceInfoHelper, Singleton, IAccessibilityServiceInfoHelper)

CAR_SINGLETON_IMPL(CAccessibilityServiceInfoHelper)

ECode CAccessibilityServiceInfoHelper::FeedbackTypeToString(
    /* [in] */ Int32 feedbackType,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return CAccessibilityServiceInfo::FeedbackTypeToString(feedbackType, str);
}

ECode CAccessibilityServiceInfoHelper::FlagToString(
    /* [in] */ Int32 flag,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return CAccessibilityServiceInfo::FlagToString(flag, str);
}

ECode CAccessibilityServiceInfoHelper::CapabilityToString(
    /* [in] */ Int32 capability,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return CAccessibilityServiceInfo::CapabilityToString(capability, str);
}
}
}
}

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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfoRangeInfoHelper.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeInfoRangeInfo.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL(CAccessibilityNodeInfoRangeInfoHelper, Singleton, IAccessibilityNodeInfoRangeInfoHelper)

CAR_SINGLETON_IMPL(CAccessibilityNodeInfoRangeInfoHelper)

ECode CAccessibilityNodeInfoRangeInfoHelper::Obtain(
    /* [in] */ IAccessibilityNodeInfoRangeInfo* other,
    /* [out] */ IAccessibilityNodeInfoRangeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return AccessibilityNodeInfoRangeInfo::Obtain(other, info);
}

ECode CAccessibilityNodeInfoRangeInfoHelper::Obtain(
    /* [in] */ Int32 type,
    /* [in] */ Float min,
    /* [in] */ Float max,
    /* [in] */ Float current,
    /* [out] */ IAccessibilityNodeInfoRangeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return AccessibilityNodeInfoRangeInfo::Obtain(type, min, max, current, info);
}

} // Accessibility
} // View
} // Droid
} // Elastos

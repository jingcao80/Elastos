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
#include "Elastos.Droid.View.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeInfoRangeInfo.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

const Int32 AccessibilityNodeInfoRangeInfo::MAX_POOL_SIZE = 10;
AutoPtr< Pools::SynchronizedPool<IAccessibilityNodeInfoRangeInfo> > AccessibilityNodeInfoRangeInfo::sPool =
        new Pools::SynchronizedPool<IAccessibilityNodeInfoRangeInfo>(MAX_POOL_SIZE);

CAR_INTERFACE_IMPL(AccessibilityNodeInfoRangeInfo, Object, IAccessibilityNodeInfoRangeInfo)

AccessibilityNodeInfoRangeInfo::AccessibilityNodeInfoRangeInfo(
    /* [in] */ Int32 type,
    /* [in] */ Float min,
    /* [in] */ Float max,
    /* [in] */ Float current)
{
    mType = type;
    mMin = min;
    mMax = max;
    mCurrent = current;
}

ECode AccessibilityNodeInfoRangeInfo::Obtain(
    /* [in] */ IAccessibilityNodeInfoRangeInfo* other,
    /* [out] */ IAccessibilityNodeInfoRangeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    Int32 type;
    Float min, max, current;
    other->GetType(&type);
    other->GetMin(&min);
    other->GetMax(&max);
    other->GetCurrent(&current);

    return Obtain(type, min, max, current, info);
}

ECode AccessibilityNodeInfoRangeInfo::Obtain(
    /* [in] */ Int32 type,
    /* [in] */ Float min,
    /* [in] */ Float max,
    /* [in] */ Float current,
    /* [out] */ IAccessibilityNodeInfoRangeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    AutoPtr<IAccessibilityNodeInfoRangeInfo> _info = sPool->AcquireItem();

    if (_info != NULL) {
        *info = _info;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }

    AutoPtr<AccessibilityNodeInfoRangeInfo> rangeInfo = new AccessibilityNodeInfoRangeInfo(type, min, max, current);
    *info = (IAccessibilityNodeInfoRangeInfo*)rangeInfo.Get();
    REFCOUNT_ADD(*info);

    return NOERROR;
}

ECode AccessibilityNodeInfoRangeInfo::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    return NOERROR;
}

ECode AccessibilityNodeInfoRangeInfo::GetMin(
    /* [out] */ Float* min)
{
    VALIDATE_NOT_NULL(min)
    *min = mMin;
    return NOERROR;
}

ECode AccessibilityNodeInfoRangeInfo::GetMax(
    /* [out] */ Float* max)
{
    VALIDATE_NOT_NULL(max)
    *max = mMax;
    return NOERROR;
}

ECode AccessibilityNodeInfoRangeInfo::GetCurrent(
    /* [out] */ Float* current)
{
    VALIDATE_NOT_NULL(current)
    *current = mCurrent;
    return NOERROR;
}

ECode AccessibilityNodeInfoRangeInfo::Recycle()
{
    Clear();
    sPool->ReleaseItem(this);
    return NOERROR;
}

void AccessibilityNodeInfoRangeInfo::Clear()
{
    mType = 0;
    mMin = 0;
    mMax = 0;
    mCurrent = 0;
}

} // Accessibility
} // View
} // Droid
} // Elastos

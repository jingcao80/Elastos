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
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfoCollectionItemInfoHelper.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeInfoCollectionItemInfo.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL(CAccessibilityNodeInfoCollectionItemInfoHelper, Singleton, IAccessibilityNodeInfoCollectionItemInfoHelper)

CAR_SINGLETON_IMPL(CAccessibilityNodeInfoCollectionItemInfoHelper)

ECode CAccessibilityNodeInfoCollectionItemInfoHelper::Obtain(
    /* [in] */ IAccessibilityNodeInfoCollectionItemInfo* other,
    /* [out] */ IAccessibilityNodeInfoCollectionItemInfo** info)
{
    VALIDATE_NOT_NULL(info);

    return AccessibilityNodeInfoCollectionItemInfo::Obtain(other, info);
}

ECode CAccessibilityNodeInfoCollectionItemInfoHelper::Obtain(
    /* [in] */ Int32 rowIndex,
    /* [in] */ Int32 rowSpan,
    /* [in] */ Int32 columnIndex,
    /* [in] */ Int32 columnSpan,
    /* [in] */ Boolean heading,
    /* [out] */ IAccessibilityNodeInfoCollectionItemInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return AccessibilityNodeInfoCollectionItemInfo::Obtain(rowIndex, rowSpan, columnIndex, columnSpan, heading, info);
}

ECode CAccessibilityNodeInfoCollectionItemInfoHelper::Obtain(
    /* [in] */ Int32 rowIndex,
    /* [in] */ Int32 rowSpan,
    /* [in] */ Int32 columnIndex,
    /* [in] */ Int32 columnSpan,
    /* [in] */ Boolean heading,
    /* [in] */ Boolean selected,
    /* [out] */ IAccessibilityNodeInfoCollectionItemInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return AccessibilityNodeInfoCollectionItemInfo::Obtain(rowIndex, rowSpan, columnIndex, columnSpan, heading, selected, info);
}

} // Accessibility
} // View
} // Droid
} // Elastos

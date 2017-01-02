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
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeInfoCollectionInfo.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

const Int32 AccessibilityNodeInfoCollectionInfo::MAX_POOL_SIZE = 20;
AutoPtr< Pools::SynchronizedPool<AccessibilityNodeInfoCollectionInfo> > AccessibilityNodeInfoCollectionInfo::sPool =
        new Pools::SynchronizedPool<AccessibilityNodeInfoCollectionInfo>(MAX_POOL_SIZE);

CAR_INTERFACE_IMPL(AccessibilityNodeInfoCollectionInfo, Object, IAccessibilityNodeInfoCollectionInfo)

AccessibilityNodeInfoCollectionInfo::AccessibilityNodeInfoCollectionInfo(
    /* [in] */ Int32 rowCount,
    /* [in] */ Int32 columnCount,
    /* [in] */ Boolean hierarchical,
    /* [in] */ Int32 selectionMode)
{
    mRowCount = rowCount;
    mColumnCount = columnCount;
    mHierarchical = hierarchical;
    mSelectionMode = selectionMode;
}

ECode AccessibilityNodeInfoCollectionInfo::Obtain(
    /* [in] */ IAccessibilityNodeInfoCollectionInfo* other,
    /* [out] */ IAccessibilityNodeInfoCollectionInfo** info)
{
    VALIDATE_NOT_NULL(info);
    Int32 rowCount, columnCount, selectionMode;
    Boolean hierarchical;
    other->GetRowCount(&rowCount);
    other->GetColumnCount(&columnCount);
    other->IsHierarchical(&hierarchical);
    other->GetSelectionMode(&selectionMode);

    return Obtain(rowCount, columnCount, hierarchical, selectionMode, info);
}

ECode AccessibilityNodeInfoCollectionInfo::Obtain(
    /* [in] */ Int32 rowCount,
    /* [in] */ Int32 columnCount,
    /* [in] */ Boolean hierarchical,
    /* [out] */ IAccessibilityNodeInfoCollectionInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return Obtain(rowCount, columnCount, hierarchical, SELECTION_MODE_NONE, info);
}

ECode AccessibilityNodeInfoCollectionInfo::Obtain(
    /* [in] */ Int32 rowCount,
    /* [in] */ Int32 columnCount,
    /* [in] */ Boolean hierarchical,
    /* [in] */ Int32 selectionMode,
    /* [out] */ IAccessibilityNodeInfoCollectionInfo** info)
{
    VALIDATE_NOT_NULL(info);
    AutoPtr<AccessibilityNodeInfoCollectionInfo> _info = sPool->AcquireItem();

    if (_info == NULL) {
        AutoPtr<AccessibilityNodeInfoCollectionInfo> collectionInfo = new AccessibilityNodeInfoCollectionInfo(rowCount, columnCount, hierarchical, selectionMode);
        *info = (IAccessibilityNodeInfoCollectionInfo*)collectionInfo.Get();
        REFCOUNT_ADD(*info);
        return NOERROR;
    }

    _info->mRowCount = rowCount;
    _info->mColumnCount = columnCount;
    _info->mHierarchical = hierarchical;
    _info->mSelectionMode = selectionMode;

    *info = (IAccessibilityNodeInfoCollectionInfo*)_info.Get();
    REFCOUNT_ADD(*info);
    return NOERROR;
}

ECode AccessibilityNodeInfoCollectionInfo::GetRowCount(
    /* [out] */ Int32* rowCount)
{
    VALIDATE_NOT_NULL(rowCount)
    *rowCount = mRowCount;
    return NOERROR;
}

ECode AccessibilityNodeInfoCollectionInfo::GetColumnCount(
    /* [out] */ Int32* columnCount)
{
    VALIDATE_NOT_NULL(columnCount)
    *columnCount = mColumnCount;
    return NOERROR;
}

ECode AccessibilityNodeInfoCollectionInfo::IsHierarchical(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHierarchical;
    return NOERROR;
}

ECode AccessibilityNodeInfoCollectionInfo::GetSelectionMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = mSelectionMode;
    return NOERROR;
}

ECode AccessibilityNodeInfoCollectionInfo::Recycle()
{
    Clear();
    sPool->ReleaseItem(this);
    return NOERROR;
}

void AccessibilityNodeInfoCollectionInfo::Clear()
{
    mRowCount = 0;
    mColumnCount = 0;
    mHierarchical = FALSE;
    mSelectionMode = SELECTION_MODE_NONE;
}

} // Accessibility
} // View
} // Droid
} // Elastos

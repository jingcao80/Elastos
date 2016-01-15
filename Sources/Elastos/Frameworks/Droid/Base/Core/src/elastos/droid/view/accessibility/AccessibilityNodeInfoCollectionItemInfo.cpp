
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeInfoCollectionItemInfo.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

const Int32 AccessibilityNodeInfoCollectionItemInfo::MAX_POOL_SIZE = 20;
AutoPtr< Pools::SynchronizedPool<AccessibilityNodeInfoCollectionItemInfo> > AccessibilityNodeInfoCollectionItemInfo::sPool =
        new Pools::SynchronizedPool<AccessibilityNodeInfoCollectionItemInfo>(MAX_POOL_SIZE);

CAR_INTERFACE_IMPL(AccessibilityNodeInfoCollectionItemInfo, Object, IAccessibilityNodeInfoCollectionItemInfo)

AccessibilityNodeInfoCollectionItemInfo::AccessibilityNodeInfoCollectionItemInfo(
    /* [in] */ Int32 rowIndex,
    /* [in] */ Int32 rowSpan,
    /* [in] */ Int32 columnIndex,
    /* [in] */ Int32 columnSpan,
    /* [in] */ Boolean heading,
    /* [in] */ Boolean selected)
{
    mRowIndex = rowIndex;
    mRowSpan = rowSpan;
    mColumnIndex = columnIndex;
    mColumnSpan = columnSpan;
    mHeading = heading;
    mSelected = selected;
}

ECode AccessibilityNodeInfoCollectionItemInfo::Obtain(
    /* [in] */ IAccessibilityNodeInfoCollectionItemInfo* other,
    /* [out] */ IAccessibilityNodeInfoCollectionItemInfo** info)
{
    VALIDATE_NOT_NULL(info);
    Int32 rowIndex, rowSpan, columnIndex, columnSpan;
    Boolean heading, selected;
    other->GetRowIndex(&rowIndex);
    other->GetRowSpan(&rowSpan);
    other->GetColumnIndex(&columnIndex);
    other->GetColumnSpan(&columnSpan);
    other->IsHeading(&heading);
    other->IsSelected(&selected);

    return Obtain(rowIndex, rowSpan, columnIndex, columnSpan, heading, selected, info);
}

ECode AccessibilityNodeInfoCollectionItemInfo::Obtain(
    /* [in] */ Int32 rowIndex,
    /* [in] */ Int32 rowSpan,
    /* [in] */ Int32 columnIndex,
    /* [in] */ Int32 columnSpan,
    /* [in] */ Boolean heading,
    /* [out] */ IAccessibilityNodeInfoCollectionItemInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return Obtain(rowIndex, rowSpan, columnIndex, columnSpan, heading, FALSE, info);
}

ECode AccessibilityNodeInfoCollectionItemInfo::Obtain(
    /* [in] */ Int32 rowIndex,
    /* [in] */ Int32 rowSpan,
    /* [in] */ Int32 columnIndex,
    /* [in] */ Int32 columnSpan,
    /* [in] */ Boolean heading,
    /* [in] */ Boolean selected,
    /* [out] */ IAccessibilityNodeInfoCollectionItemInfo** info)
{
    VALIDATE_NOT_NULL(info);
    AutoPtr<AccessibilityNodeInfoCollectionItemInfo> _info = sPool->AcquireItem();

    if (_info == NULL) {
        AutoPtr<AccessibilityNodeInfoCollectionItemInfo> collectionItemInfo = new AccessibilityNodeInfoCollectionItemInfo(
                rowIndex, rowSpan, columnIndex, columnSpan, heading, selected);
        *info = (IAccessibilityNodeInfoCollectionItemInfo*)collectionItemInfo.Get();
        REFCOUNT_ADD(*info);
        return NOERROR;
    }

    _info->mRowIndex = rowIndex;
    _info->mRowSpan = rowSpan;
    _info->mColumnIndex = columnIndex;
    _info->mColumnSpan = columnSpan;
    _info->mHeading = heading;
    _info->mSelected = selected;

    *info = (IAccessibilityNodeInfoCollectionItemInfo*)_info.Get();
    REFCOUNT_ADD(*info);

    return NOERROR;
}

ECode AccessibilityNodeInfoCollectionItemInfo::GetColumnIndex(
    /* [out] */ Int32* columnIndex)
{
    VALIDATE_NOT_NULL(columnIndex)
    *columnIndex = mColumnIndex;
    return NOERROR;
}

ECode AccessibilityNodeInfoCollectionItemInfo::GetRowIndex(
    /* [out] */ Int32* rowIndex)
{
    VALIDATE_NOT_NULL(rowIndex)
    *rowIndex = mRowIndex;
    return NOERROR;
}

ECode AccessibilityNodeInfoCollectionItemInfo::GetColumnSpan(
    /* [out] */ Int32* columnSpan)
{
    VALIDATE_NOT_NULL(columnSpan)
    *columnSpan = mColumnSpan;
    return NOERROR;
}

ECode AccessibilityNodeInfoCollectionItemInfo::GetRowSpan(
    /* [out] */ Int32* rowSpan)
{
    VALIDATE_NOT_NULL(rowSpan)
    *rowSpan = mRowSpan;
    return NOERROR;
}

ECode AccessibilityNodeInfoCollectionItemInfo::IsHeading(
    /* [out] */ Boolean* heading)
{
    VALIDATE_NOT_NULL(heading)
    *heading = mHeading;
    return NOERROR;
}

ECode AccessibilityNodeInfoCollectionItemInfo::IsSelected(
    /* [out] */ Boolean* selected)
{
    VALIDATE_NOT_NULL(selected)
    *selected = mSelected;
    return NOERROR;
}

ECode AccessibilityNodeInfoCollectionItemInfo::Recycle()
{
    Clear();
    sPool->ReleaseItem(this);
    return NOERROR;
}

void AccessibilityNodeInfoCollectionItemInfo::Clear()
{
    mColumnIndex = 0;
    mColumnSpan = 0;
    mRowIndex = 0;
    mRowSpan = 0;
    mHeading = FALSE;
    mSelected = FALSE;
}

} // Accessibility
} // View
} // Droid
} // Elastos

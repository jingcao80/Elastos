
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfoCollectionInfoHelper.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeInfoCollectionInfo.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL(CAccessibilityNodeInfoCollectionInfoHelper, Singleton, IAccessibilityNodeInfoCollectionInfoHelper)

CAR_SINGLETON_IMPL(CAccessibilityNodeInfoCollectionInfoHelper)

ECode CAccessibilityNodeInfoCollectionInfoHelper::Obtain(
    /* [in] */ IAccessibilityNodeInfoCollectionInfo* other,
    /* [out] */ IAccessibilityNodeInfoCollectionInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return AccessibilityNodeInfoCollectionInfo::Obtain(other, info);
}

ECode CAccessibilityNodeInfoCollectionInfoHelper::Obtain(
    /* [in] */ Int32 rowCount,
    /* [in] */ Int32 columnCount,
    /* [in] */ Boolean hierarchical,
    /* [out] */ IAccessibilityNodeInfoCollectionInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return AccessibilityNodeInfoCollectionInfo::Obtain(rowCount, columnCount, hierarchical, info);
}

ECode CAccessibilityNodeInfoCollectionInfoHelper::Obtain(
    /* [in] */ Int32 rowCount,
    /* [in] */ Int32 columnCount,
    /* [in] */ Boolean hierarchical,
    /* [in] */ Int32 selectionMode,
    /* [out] */ IAccessibilityNodeInfoCollectionInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return AccessibilityNodeInfoCollectionInfo::Obtain(rowCount, columnCount, hierarchical, selectionMode, info);
}

} // Accessibility
} // View
} // Droid
} // Elastos

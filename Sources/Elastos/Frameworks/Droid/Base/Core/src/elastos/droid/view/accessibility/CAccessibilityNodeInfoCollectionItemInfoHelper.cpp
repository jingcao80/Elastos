
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

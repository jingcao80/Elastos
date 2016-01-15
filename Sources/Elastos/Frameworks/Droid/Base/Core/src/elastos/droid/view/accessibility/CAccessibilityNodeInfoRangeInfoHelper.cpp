
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

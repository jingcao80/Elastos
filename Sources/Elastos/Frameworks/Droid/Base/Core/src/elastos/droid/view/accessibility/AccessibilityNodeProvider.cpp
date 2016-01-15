
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeProvider.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL(AccessibilityNodeProvider, Object, IAccessibilityNodeProvider)

ECode AccessibilityNodeProvider::CreateAccessibilityNodeInfo(
    /* [in] */ Int32 virtualViewId,
    /* [out] */ IAccessibilityNodeInfo** nodeInfo)
{
    VALIDATE_NOT_NULL(nodeInfo);
    *nodeInfo = NULL;
    return NOERROR;
}
ECode AccessibilityNodeProvider::PerformAction(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode AccessibilityNodeProvider::FindAccessibilityNodeInfosByText(
    /* [in] */ const String& text,
    /* [in] */ Int32 virtualViewId,
    /* [out] */ IList** nodeInfos)
{
    VALIDATE_NOT_NULL(nodeInfos);
    *nodeInfos = NULL;
    return NOERROR;
}

ECode AccessibilityNodeProvider::FindFocus(
    /* [in] */ Int32 focus,
    /* [out] */  IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;
    return NOERROR;
}

} // Accessibility
} // View
} // Droid
} // Elastos

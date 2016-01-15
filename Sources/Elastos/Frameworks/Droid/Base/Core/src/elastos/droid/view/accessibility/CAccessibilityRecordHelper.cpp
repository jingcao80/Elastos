#include "elastos/droid/view/accessibility/CAccessibilityRecordHelper.h"
#include "elastos/droid/view/accessibility/AccessibilityRecord.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL(CAccessibilityRecordHelper, Singleton, IAccessibilityRecordHelper)

CAR_SINGLETON_IMPL(CAccessibilityRecordHelper)

ECode CAccessibilityRecordHelper::Obtain(
    /* [in] */ IAccessibilityRecord* record,
    /* [out] */ IAccessibilityRecord** newInstance)
{
    VALIDATE_NOT_NULL(newInstance);
    return AccessibilityRecord::Obtain(record, newInstance);
}

ECode CAccessibilityRecordHelper::Obtain(
    /* [out] */ IAccessibilityRecord** newInstance)
{
    VALIDATE_NOT_NULL(newInstance);
    return AccessibilityRecord::Obtain(newInstance);
}

} // Accessibility
} // View
} // Droid
} // Elastos

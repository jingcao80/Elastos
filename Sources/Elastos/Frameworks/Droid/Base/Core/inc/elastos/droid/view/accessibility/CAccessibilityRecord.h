#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYRECORD_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYRECORD_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_View_Accessibility_CAccessibilityRecord.h"
#include "elastos/droid/view/accessibility/AccessibilityRecord.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityRecord)
    , public AccessibilityRecord
{
public:
    CAR_OBJECT_DECL()
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYRECORD_H__

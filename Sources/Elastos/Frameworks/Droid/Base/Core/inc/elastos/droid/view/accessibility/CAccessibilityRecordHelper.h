#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYRECORDHELPER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYRECORDHELPER_H__

#include "_Elastos_Droid_View_Accessibility_CAccessibilityRecordHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityRecordHelper)
    , public Singleton
    , public IAccessibilityRecordHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Obtain(
        /* [in] */ IAccessibilityRecord* record,
        /* [out] */ IAccessibilityRecord** newInstance);

    CARAPI Obtain(
        /* [out] */ IAccessibilityRecord** newInstance);
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYRECORDHELPER_H__

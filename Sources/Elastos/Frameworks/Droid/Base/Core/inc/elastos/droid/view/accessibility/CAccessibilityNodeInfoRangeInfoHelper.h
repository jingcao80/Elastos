#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFORANGEINFOHELPER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFORANGEINFOHELPER_H__

#include "_Elastos_Droid_View_Accessibility_CAccessibilityNodeInfoRangeInfoHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityNodeInfoRangeInfoHelper)
    , public Singleton
    , public IAccessibilityNodeInfoRangeInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Obtain(
        /* [in] */ IAccessibilityNodeInfoRangeInfo* other,
        /* [out] */ IAccessibilityNodeInfoRangeInfo** info);

    CARAPI Obtain(
        /* [in] */ Int32 type,
        /* [in] */ Float min,
        /* [in] */ Float max,
        /* [in] */ Float current,
        /* [out] */ IAccessibilityNodeInfoRangeInfo** info);
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFORANGEINFOHELPER_H__

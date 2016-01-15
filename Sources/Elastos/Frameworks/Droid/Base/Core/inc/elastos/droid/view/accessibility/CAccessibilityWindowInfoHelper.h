#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYWINDOWINFOHELPER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYWINDOWINFOHELPER_H__

#include "_Elastos_Droid_View_Accessibility_CAccessibilityWindowInfoHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityWindowInfoHelper)
    , public Singleton
    , public IAccessibilityWindowInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Obtain(
        /* [out] */ IAccessibilityWindowInfo** info);

    CARAPI Obtain(
        /* [in] */ IAccessibilityWindowInfo* info,
        /* [out] */ IAccessibilityWindowInfo** cloneInfo);
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYWINDOWINFOHELPER_H__

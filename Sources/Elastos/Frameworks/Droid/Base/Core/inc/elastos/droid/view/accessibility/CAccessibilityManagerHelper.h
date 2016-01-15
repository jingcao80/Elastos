
#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CAccessibilityManager_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CAccessibilityManager_H__

#include "_Elastos_Droid_View_Accessibility_CAccessibilityManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityManagerHelper)
    , public Singleton
    , public IAccessibilityManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ IAccessibilityManager** manager);
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CAccessibilityManager_H__

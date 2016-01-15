#ifndef __ELASTOS_DROID_ACCESSIBILITYSERVICE_CACCESSIBILITYSERVICECLIENTWRAPPER_H__
#define __ELASTOS_DROID_ACCESSIBILITYSERVICE_CACCESSIBILITYSERVICECLIENTWRAPPER_H__

#include "_Elastos_Droid_AccessibilityService_CAccessibilityServiceClientWrapper.h"
#include "elastos/droid/accessibilityservice/AccessibilityService.h"

namespace Elastos {
namespace Droid {
namespace AccessibilityService {

CarClass(CAccessibilityServiceClientWrapper)
    , public IAccessibilityServiceClientWrapper
{
public:
    CAR_OBJECT_DECL()
};

}
}
}

#endif // __ELASTOS_DROID_ACCESSIBILITYSERVICE_CACCESSIBILITYSERVICECLIENTWRAPPER_H__

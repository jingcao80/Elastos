
#ifndef __ELASTOS_DROID_APP_CACCESSIBILITY_SERVICE_CLIENT_IMPL_H__
#define __ELASTOS_DROID_APP_CACCESSIBILITY_SERVICE_CLIENT_IMPL_H__

#include "_Elastos_Droid_App_CAccessibilityServiceClientImpl.h"
#include "elastos/droid/app/UiAutomation.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CAccessibilityServiceClientImpl)
    , public UiAutomation::IAccessibilityServiceClientImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACCESSIBILITY_SERVICE_CLIENT_IMPL_H__

#ifndef __ELASTOS_DROID_APP_CUIAUTOMATION_H__
#define __ELASTOS_DROID_APP_CUIAUTOMATION_H__

#include "_Elastos_Droid_App_CUiAutomation.h"
#include "elastos/droid/app/UiAutomation.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CUiAutomation)
    , public UiAutomation
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CUIAUTOMATION_H__
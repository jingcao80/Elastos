
#ifndef __ELASTOS_DROID_APP_CUIAUTOMATION_CONNECTION_H__
#define __ELASTOS_DROID_APP_CUIAUTOMATION_CONNECTION_H__

#include "_Elastos_Droid_App_CUiAutomationConnection.h"
#include "elastos/droid/app/UiAutomationConnection.h"

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CUiAutomationConnection)
    , public UiAutomationConnection
{
public:
    CAR_OBJECT_DECL()
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CUIAUTOMATION_CONNECTION_H__
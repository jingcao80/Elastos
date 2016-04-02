#ifndef __ELASTOS_DROID_APP_CSTATUSBARMANAGERHELPER_H__
#define __ELASTOS_DROID_APP_CSTATUSBARMANAGERHELPER_H__

#include "_Elastos_Droid_App_CStatusBarManagerHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CStatusBarManagerHelper)
    , public Singleton
    , public IStatusBarManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI WindowStateToString(
        /* [in] */ Int32 state,
        /* [out] */ String* value);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CSTATUSBARMANAGERHELPER_H__


#ifndef __ELASTOS_DROID_APP_ADMIN_CDEVICEPOLICYMANAGERHELPER_H__
#define __ELASTOS_DROID_APP_ADMIN_CDEVICEPOLICYMANAGERHELPER_H__

#include "_Elastos_Droid_App_Admin_CDevicePolicyManagerHelper.h"


namespace Elastos {
namespace Droid {
namespace App {
namespace Admin {

CarClass(CDevicePolicyManagerHelper)
{
public:
    /** @hide */
    CARAPI Create(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [out] */ IDevicePolicyManager** policymanager);
};

} // namespace Admin
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_ADMIN_CDEVICEPOLICYMANAGERHELPER_H__

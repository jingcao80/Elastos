
#ifndef __ELASTOS_DROID_APP_CACTIVITYMANAGER_RUNNING_APPPROCESSINFO_HELPER_H__
#define __ELASTOS_DROID_APP_CACTIVITYMANAGER_RUNNING_APPPROCESSINFO_HELPER_H__

#include "_Elastos_Droid_App_CActivityManagerRunningAppProcessInfoHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CActivityManagerRunningAppProcessInfoHelper)
    , public Singleton
    , public IActivityManagerRunningAppProcessInfoHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI ProcStateToImportance(
        /* [in] */ Int32 procState,
        /* [out] */ Int32* result);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITYMANAGER_RUNNING_APPPROCESSINFO_HELPER_H__

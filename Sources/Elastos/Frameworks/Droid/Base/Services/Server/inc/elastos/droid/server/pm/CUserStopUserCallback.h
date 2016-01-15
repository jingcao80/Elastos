
#ifndef __ELASTOS_DROID_SERVER_PM_CUSERSTOPUSERCALLBACK_H__
#define __ELASTOS_DROID_SERVER_PM_CUSERSTOPUSERCALLBACK_H__

#include "_Elastos_Droid_Server_Pm_CUserStopUserCallback.h"
#include "elastos/droid/server/pm/CUserManagerService.h"

using Elastos::Droid::App::IStopUserCallback;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CarClass(CUserStopUserCallback)
    , public Object
    , public IStopUserCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IIUserManager* mUm);

    CARAPI UserStopped(
        /* [in] */ Int32 userId);

    CARAPI UserStopAborted(
        /* [in] */ Int32 userId);

private:
    CUserManagerService* mUm;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_CUSERSTOPUSERCALLBACK_H__

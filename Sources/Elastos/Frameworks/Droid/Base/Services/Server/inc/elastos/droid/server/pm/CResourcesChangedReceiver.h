
#ifndef __ELASTOS_DROID_SERVER_PM_CRESOURCESCHANGEDRECEIVER_H__
#define __ELASTOS_DROID_SERVER_PM_CRESOURCESCHANGEDRECEIVER_H__

#include "_Elastos_Droid_Server_Pm_CResourcesChangedReceiver.h"
#include "elastos/droid/server/pm/CPackageManagerService.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CarClass(CResourcesChangedReceiver)
    , public Object
    , public IIntentReceiver
    , public IBinder
{
public:
    CResourcesChangedReceiver()
        : mReportStatus(FALSE)
    {}

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    constructor(
        /* [in] */ IIPackageManager* owner,
        /* [in] */ ISet* keys,
        /* [in] */ Boolean reportStatus);

    CARAPI PerformReceive(
       /* [in] */ IIntent* intent,
       /* [in] */ Int32 resultCode,
       /* [in] */ const String& data,
       /* [in] */ IBundle* extras,
       /* [in] */ Boolean ordered,
       /* [in] */ Boolean sticky,
       /* [in] */ Int32 sendingUser);

private:
    CPackageManagerService* mOwner;
    AutoPtr<ISet> mKeys; //Set< AutoPtr<CPackageManagerService::AsecInstallArgs> >
    Boolean mReportStatus;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_CRESOURCESCHANGEDRECEIVER_H__

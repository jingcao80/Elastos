
#ifndef __ELASTOS_DROID_SERVER_PM_CLOCALOBSERVER_H__
#define __ELASTOS_DROID_SERVER_PM_CLOCALOBSERVER_H__

#include "_Elastos_Droid_Server_Pm_CLocalObserver.h"
#include "elastos/droid/server/pm/CPackageInstallerSession.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageInstallObserver2;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CarClass(CLocalObserver)
    , public Object
    , public IIPackageInstallObserver2
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IIPackageInstallerSession* host);

    CARAPI OnUserActionRequired(
        /* [in] */ IIntent* intent);

    CARAPI OnPackageInstalled(
        /* [in] */ const String& basePackageName,
        /* [in] */ Int32 returnCode,
        /* [in] */ const String& msg,
        /* [in] */ IBundle* extras);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CPackageInstallerSession* mHost;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_CLOCALOBSERVER_H__

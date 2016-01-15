
#include "pm/CLocalObserver.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::Pm::IPackageManagerHelper;
using Elastos::Droid::Content::Pm::CPackageManagerHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CAR_INTERFACE_IMPL_2(CLocalObserver, Object, IIPackageInstallObserver2, IBinder)

CAR_OBJECT_IMPL(CLocalObserver)

ECode CLocalObserver::constructor(
    /* [in] */ IIPackageInstallerSession* host)
{
    mHost = reinterpret_cast<CPackageInstallerSession*>(host->Probe(EIID_CPackageInstallerSession));
    return NOERROR;
}

ECode CLocalObserver::OnUserActionRequired(
    /* [in] */ IIntent* intent)
{
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode CLocalObserver::OnPackageInstalled(
    /* [in] */ const String& basePackageName,
    /* [in] */ Int32 returnCode,
    /* [in] */ const String& msg,
    /* [in] */ IBundle* extras)
{
    mHost->DestroyInternal();
    mHost->DispatchSessionFinished(returnCode, msg, extras);
    return NOERROR;
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

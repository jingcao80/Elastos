
#include "pm/CPackageInstallObserver2.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::Pm::IPackageManagerHelper;
using Elastos::Droid::Content::Pm::CPackageManagerHelper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CAR_INTERFACE_IMPL_2(CPackageInstallObserver2, Object, IIPackageInstallObserver2, IBinder)

CAR_OBJECT_IMPL(CPackageInstallObserver2)

ECode CPackageInstallObserver2::constructor(
    /* [in] */ IIPackageManager* host,
    /* [in] */ IIPackageInstallObserver2* observer)
{
    mHost = reinterpret_cast<CPackageManagerService*>(host->Probe(EIID_CPackageManagerService));
    mObserver = observer;
    return NOERROR;
}

ECode CPackageInstallObserver2::OnUserActionRequired(
    /* [in] */ IIntent* intent)
{
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode CPackageInstallObserver2::OnPackageInstalled(
    /* [in] */ const String& basePackageName,
    /* [in] */ Int32 returnCode,
    /* [in] */ const String& msg,
    /* [in] */ IBundle* extras)
{
    AutoPtr<IPackageManagerHelper> helper;
    CPackageManagerHelper::AcquireSingleton((IPackageManagerHelper**)&helper);
    String str;
    helper->InstallStatusToString(returnCode, msg, &str);
    Slogger::D(TAG, "Install result for move: %s", str.string());

    // We usually have a new package now after the install, but if
    // we failed we need to clear the pending flag on the original
    // package object.
    synchronized (mHost->mPackagesLock) {
        AutoPtr<PackageParser::Package> pkg;
        HashMap<String, AutoPtr<PackageParser::Package> >::Iterator it = mHost->mPackages.Find(packageName);
        if (it != mHost->mPackages.End()) {
            pkg = it->mSecond;
        }
        if (pkg != NULL) {
            pkg->mOperationPending = FALSE;
        }
    }

    Int32 status;
    helper->InstallStatusToPublicStatus(returnCode, &status);
    switch (status) {
        case IPackageInstaller::STATUS_SUCCESS:
            mObserver->PackageMoved(packageName, IPackageManager::MOVE_SUCCEEDED);
            break;
        case IPackageInstaller::STATUS_FAILURE_STORAGE:
            mObserver->PackageMoved(packageName, IPackageManager::MOVE_FAILED_INSUFFICIENT_STORAGE);
            break;
        default:
            mObserver->PackageMoved(packageName, IPackageManager::MOVE_FAILED_INTERNAL_ERROR);
            break;
    }
    return NOERROR;
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

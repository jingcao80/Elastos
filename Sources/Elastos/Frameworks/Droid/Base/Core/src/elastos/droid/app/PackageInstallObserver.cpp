
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/PackageInstallObserver.h"
#include "elastos/droid/app/CPackageInstallObserver2.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(PackageInstallObserver, Object, IPackageInstallObserver)

ECode PackageInstallObserver::constructor()
{
    return CPackageInstallObserver2::New(this, (IIPackageInstallObserver2**)&mBinder);
}

ECode PackageInstallObserver::GetBinder(
    /* [out] */ IIPackageInstallObserver2** pio)
{
    VALIDATE_NOT_NULL(pio)
    *pio = mBinder;
    REFCOUNT_ADD(*pio)
    return NOERROR;
}

ECode PackageInstallObserver::OnUserActionRequired(
    /* [in] */ IIntent* intent)
{
    return NOERROR;
}

ECode PackageInstallObserver::OnPackageInstalled(
    /* [in] */ const String& basePackageName,
    /* [in] */ Int32 returnCode,
    /* [in] */ const String& msg,
    /* [in] */ IBundle* extras)
{
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos

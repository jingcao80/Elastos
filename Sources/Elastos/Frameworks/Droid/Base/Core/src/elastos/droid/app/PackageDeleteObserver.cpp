
#include "elastos/droid/app/PackageDeleteObserver.h"
#include "elastos/droid/app/CPackageDeleteObserver2.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(PackageDeleteObserver, Object, IPackageDeleteObserver)

ECode PackageDeleteObserver::constructor()
{
    return CPackageDeleteObserver2::New(this, (IIPackageDeleteObserver2**)&mBinder);
}

ECode PackageDeleteObserver::GetBinder(
    /* [out] */ IIPackageDeleteObserver2** observer)
{
    VALIDATE_NOT_NULL(observer)
    *observer = mBinder;
    REFCOUNT_ADD(*observer)
    return NOERROR;
}

ECode PackageDeleteObserver::OnUserActionRequired(
    /* [in] */ IIntent* intent)
{
    return NOERROR;
}

ECode PackageDeleteObserver::OnPackageDeleted(
    /* [in] */ const String& basePackageName,
    /* [in] */ Int32 returnCode,
    /* [in] */ const String& msg)
{
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos

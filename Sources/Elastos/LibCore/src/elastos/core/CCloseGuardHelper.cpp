
#include "CCloseGuardHelper.h"
#include "CCloseGuard.h"

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL(CCloseGuardHelper, Singleton, ICloseGuardHelper)

CAR_SINGLETON_IMPL(CCloseGuardHelper)

ECode CCloseGuardHelper::Get(
    /* [out] */ ICloseGuard** guard)
{
    VALIDATE_NOT_NULL(guard);

    AutoPtr<ICloseGuard> res = CCloseGuard::Get();
    *guard = res;
    REFCOUNT_ADD(*guard)
    return NOERROR;
}

ECode CCloseGuardHelper::SetEnabled(
    /* [in] */ Boolean enabled)
{
    return CCloseGuard::SetEnabled(enabled);
}

ECode CCloseGuardHelper::SetReporter(
    /* [in] */ ICloseGuardReporter* reporter)
{
    return CCloseGuard::SetReporter(reporter);
}

ECode CCloseGuardHelper::GetReporter(
    /* [out] */ ICloseGuardReporter** report)
{
    VALIDATE_NOT_NULL(report);

    AutoPtr<ICloseGuardReporter> res = CCloseGuard::GetReporter();
    *report = res;
    REFCOUNT_ADD(*report)
    return NOERROR;
}

} // namespace Core
} // namespace Elastos
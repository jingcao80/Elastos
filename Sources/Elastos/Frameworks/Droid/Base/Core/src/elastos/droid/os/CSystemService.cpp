
#include "elastos/droid/os/CSystemService.h"
#include "elastos/droid/os/SystemService.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CSystemService, Singleton, ISystemService)

CAR_SINGLETON_IMPL(CSystemService)

ECode CSystemService::Start(
    /* [in] */ const String& name)
{
    return SystemService::Start(name);
}

ECode CSystemService::Stop(
    /* [in] */ const String& name)
{
    return SystemService::Stop(name);
}

ECode CSystemService::Restart(
    /* [in] */ const String& name)
{
    return SystemService::Restart(name);
}

ECode CSystemService::GetState(
    /* [in] */ const String& service,
    /* [out] */ SystemServiceState* state)
{
    VALIDATE_NOT_NULL(state);
    *state = SystemService::GetState(service);
    return NOERROR;
}

ECode CSystemService::IsStopped(
    /* [in] */ const String& service,
    /* [out] */ Boolean* isStopped)
{
    VALIDATE_NOT_NULL(isStopped);
    *isStopped = SystemService::IsStopped(service);
    return NOERROR;
}

ECode CSystemService::IsRunning(
    /* [in] */ const String& service,
    /* [out] */ Boolean* isRunning)
{
    VALIDATE_NOT_NULL(isRunning);
    *isRunning = SystemService::IsRunning(service);
    return NOERROR;
}

ECode CSystemService::WaitForState(
    /* [in] */ const String& service,
    /* [in] */ SystemServiceState state,
    /* [in] */ Int64 timeoutMillis)
{
    return SystemService::WaitForState(service, state, timeoutMillis);
}

ECode CSystemService::WaitForAnyStopped(
    /* [in] */ ArrayOf<String>* services)
{
    return SystemService::WaitForAnyStopped(services);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos

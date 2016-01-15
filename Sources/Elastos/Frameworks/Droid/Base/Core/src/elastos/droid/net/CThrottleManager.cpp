#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/CThrottleManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

ECode CThrottleManager::constructor(
     /* [in] */ IIThrottleManager* service)
{
    if (service == NULL) {
        Logger::E("ThrottleManager", "ThrottleManager() cannot be constructed with null service");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mService = service;
    return NOERROR;
}

ECode CThrottleManager::GetResetTime(
    /* [in] */ const String& iface,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    return mService->GetResetTime(iface, result);
    //} catch (RemoteException e) {
    //    return -1;
    //}
}

ECode CThrottleManager::GetPeriodStartTime(
    /* [in] */ const String& iface,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    return mService->GetPeriodStartTime(iface, result);
    //} catch (RemoteException e) {
    //    return -1;
    //}
}

ECode CThrottleManager::GetByteCount(
    /* [in] */ const String& iface,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 period,
    /* [in] */ Int32 ago,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    return mService->GetByteCount(iface, direction, period, ago,result);
    //} catch (RemoteException e) {
    //    return -1;
    //}
}

ECode CThrottleManager::GetCliffThreshold(
    /* [in] */ const String& iface,
    /* [in] */ Int32 cliff,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    return mService->GetCliffThreshold(iface, cliff,result);
    //} catch (RemoteException e) {
    //    return -1;
    //}
}

ECode CThrottleManager::GetCliffLevel(
    /* [in] */ const String& iface,
    /* [in] */ Int32 cliff,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    return mService->GetCliffLevel(iface, cliff, result);
    //} catch (RemoteException e) {
    //    return -1;
    //}
}

ECode CThrottleManager::GetHelpUri(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    return mService->GetHelpUri(result);
    //} catch (RemoteException e) {
    //    return null;
    //}
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos
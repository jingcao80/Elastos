
#include "CPppoeManager.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Pppoe {

const String CPppoeManager::TAG("CPppoeManager");

ECode CPppoeManager::GetIIPppoeManager(
    /* [out] */ IIPppoeManager** manager)
{
    VALIDATE_NOT_NULL(manager);
    *manager = mService;
    REFCOUNT_ADD(*manager);
    return NOERROR;
}

ECode CPppoeManager::SetIIPppoeManager(
    /* [in] */ IIPppoeManager* manager)
{
    mService = manager;
    return NOERROR;
}

ECode CPppoeManager::GetHandler(
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler);
    *handler = mHandler;
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

ECode CPppoeManager::SetHandler(
    /* [in] */ IHandler* handler)
{
    mHandler = handler;
    return NOERROR;
}

ECode CPppoeManager::IsPppoeConfigured(
    /* [out] */ Boolean* result)
{
    return mService->IsPppoeConfigured(result);
}

ECode CPppoeManager::GetSavedPppoeConfig(
    /* [out] */ IPppoeDevInfo** info)
{
    return mService->GetSavedPppoeConfig(info);
}

ECode CPppoeManager::UpdatePppoeDevInfo(
    /* [in] */ IPppoeDevInfo* info)
{
    return mService->UpdatePppoeDevInfo(info);
}

ECode CPppoeManager::GetDeviceNameList(
    /* [out, callee] */ ArrayOf<String>** list)
{
    return mService->GetDeviceNameList(list);
}

ECode CPppoeManager::SetPppoeEnabled(
    /* [in] */ Boolean ebl)
{
    return mService->SetPppoeState(ebl ? IPppoeManager::PPPOE_STATE_ENABLED : IPppoeManager::PPPOE_STATE_DISABLED);
}

ECode CPppoeManager::GetPppoeState(
    /* [out] */ Int32* val)
{
    return mService->GetPppoeState(val);
}

ECode CPppoeManager::PppoeConfigured(
    /* [out] */ Boolean* result)
{
    return mService->IsPppoeConfigured(result);
}

ECode CPppoeManager::GetDhcpInfo(
    /* [out] */ IDhcpInfo** info)
{
    return mService->GetDhcpInfo(info);
}

ECode CPppoeManager::GetTotalInterface(
    /* [out] */ Int32* val)
{
    return mService->GetTotalInterface(val);
}

ECode CPppoeManager::PppoeSetDefaultConf()
{
    return mService->SetPppoeMode(IPppoeDevInfo::PPPOE_CONN_MODE_DHCP);;
}

ECode CPppoeManager::IsPppoeDeviceUp(
    /* [out] */ Boolean* result)
{
    return mService->IsPppoeDeviceUp(result);
}

ECode CPppoeManager::constructor(
    /* [in] */ IIPppoeManager* service,
    /* [in] */ IHandler* handler)
{
    Slogger::I(TAG, "Init Pppoe Manager");
    mService = service;
    mHandler = handler;
    return NOERROR;
}

} // Pppoe
} // Net
} // Droid
} // Elastos

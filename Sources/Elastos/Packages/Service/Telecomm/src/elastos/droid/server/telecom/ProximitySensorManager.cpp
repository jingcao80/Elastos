
#include "elastos/droid/server/telecom/ProximitySensorManager.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/Log.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Hardware.h>

using Elastos::Droid::Os::IPowerManager;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

const String ProximitySensorManager::TAG("ProximitySensorManager");

ProximitySensorManager::ProximitySensorManager()
    : mWasTapToWakeEnabled(FALSE)
{}

ECode ProximitySensorManager::constructor(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    obj = NULL;
    context->GetSystemService(IContext::CMHW_SERVICE, (IInterface**)&obj);
    mCmHardwareManager = ICmHardwareManager::Probe(obj);
    Boolean isWakeLockLevelSupported;
    pm->IsWakeLockLevelSupported(IPowerManager::PROXIMITY_SCREEN_OFF_WAKE_LOCK, &isWakeLockLevelSupported);
    if (isWakeLockLevelSupported) {
        pm->NewWakeLock(
                IPowerManager::PROXIMITY_SCREEN_OFF_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mProximityWakeLock);
    } else {
        mProximityWakeLock = NULL;
    }
    Log::D("ProximitySensorManager", "onCreate: mProximityWakeLock: %s", TO_CSTR(mProximityWakeLock));
    return NOERROR;
}

ECode ProximitySensorManager::OnCallRemoved(
    /* [in] */ ICall* call)
{
    AutoPtr<IList> calls;
    CallsManager::GetInstance()->GetCalls((IList**)&calls);
    Boolean isEmpty;
    calls->IsEmpty(&isEmpty);
    if (isEmpty) {
        Log::V("ProximitySensorManager", "all calls removed, resetting proximity sensor to default state");
        TurnOff(TRUE);
    }
    CallsManagerListenerBase::OnCallRemoved(call);
    return NOERROR;
}

ECode ProximitySensorManager::TurnOn()
{
    AutoPtr<IList> calls;
    CallsManager::GetInstance()->GetCalls((IList**)&calls);
    Boolean isEmpty;
    calls->IsEmpty(&isEmpty);
    if (isEmpty) {
        Log::W("ProximitySensorManager", "Asking to turn on prox sensor without a call? I don't think so.");
        return NOERROR;
    }
    if (mProximityWakeLock == NULL) {
        return NOERROR;
    }
    Boolean isHeld;
    mProximityWakeLock->IsHeld(&isHeld);
    if (!isHeld) {
        Log::I("ProximitySensorManager", "Acquiring proximity wake lock");
        mProximityWakeLock->AcquireLock();
        Boolean isSupported;
        mCmHardwareManager->IsSupported(ICmHardwareManager::FEATURE_TAP_TO_WAKE, &isSupported);
        if (isSupported) {
            mCmHardwareManager->Get(ICmHardwareManager::FEATURE_TAP_TO_WAKE, &mWasTapToWakeEnabled);
            Boolean b;
            mCmHardwareManager->Set(ICmHardwareManager::FEATURE_TAP_TO_WAKE, FALSE, &b);
        }
    } else {
        Log::I("ProximitySensorManager", "Proximity wake lock already acquired");
    }
    return NOERROR;
}

ECode ProximitySensorManager::TurnOff(
    /* [in] */ Boolean screenOnImmediately)
{
    if (mProximityWakeLock == NULL) {
        return NOERROR;
    }
    Boolean isHeld;
    mProximityWakeLock->IsHeld(&isHeld);
    if (isHeld) {
        Boolean isSupported;
        mCmHardwareManager->IsSupported(ICmHardwareManager::FEATURE_TAP_TO_WAKE, &isSupported);
        if (isSupported
                && mWasTapToWakeEnabled) {
            Boolean b;
            mCmHardwareManager->Set(ICmHardwareManager::FEATURE_TAP_TO_WAKE, TRUE, &b);
        }
        Log::I("ProximitySensorManager", "Releasing proximity wake lock");
        Int32 flags =
            (screenOnImmediately ? 0 : IPowerManager::RELEASE_FLAG_WAIT_FOR_NO_PROXIMITY);
        mProximityWakeLock->ReleaseLock(flags);
    } else {
        Log::I("ProximitySensorManager", "Proximity wake lock already released");
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos


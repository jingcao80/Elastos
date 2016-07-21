
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitorCallback.h"
#include "elastos/droid/os/SystemClock.h"

using Elastos::Droid::Os::SystemClock;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

const Int64 KeyguardUpdateMonitorCallback::VISIBILITY_CHANGED_COLLAPSE_MS = 1000;

CAR_INTERFACE_IMPL(KeyguardUpdateMonitorCallback, Object, IKeyguardUpdateMonitorCallback)

KeyguardUpdateMonitorCallback::KeyguardUpdateMonitorCallback()
    : mVisibilityChangedCalled(0)
    , mShowing(FALSE)
{
}

ECode KeyguardUpdateMonitorCallback::OnRefreshBatteryInfo(
        /* [in] */ IKeyguardUpdateMonitorBatteryStatus* status)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnTimeChanged()
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnRefreshCarrierInfo(
    /* [in] */ ICharSequence* plmn,
    /* [in] */ ICharSequence* spn)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnAirplaneModeChanged(
    /* [in] */ Boolean on)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnRingerModeChanged(
    /* [in] */ Int32 state)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnPhoneStateChanged(
    /* [in] */ Int32 phoneState)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnKeyguardVisibilityChanged(
    /* [in] */ Boolean showing)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnKeyguardVisibilityChangedRaw(
    /* [in] */ Boolean showing)
{
    Int64 now = SystemClock::GetElapsedRealtime();
    if (showing == mShowing
            && (now - mVisibilityChangedCalled) < VISIBILITY_CHANGED_COLLAPSE_MS) return NOERROR;
    OnKeyguardVisibilityChanged(showing);
    mVisibilityChangedCalled = now;
    mShowing = showing;
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnKeyguardBouncerChanged(
    /* [in] */ Boolean bouncer)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnClockVisibilityChanged()
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnDeviceProvisioned()
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnDevicePolicyManagerStateChanged()
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnUserSwitching(
    /* [in] */ Int32 userId)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnUserSwitchComplete(
    /* [in] */ Int32 userId)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnSimStateChanged(
    /* [in] */ IccCardConstantsState simState)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnUserRemoved(
    /* [in] */ Int32 userId)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnUserInfoChanged(
    /* [in] */ Int32 userId)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnBootCompleted()
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnEmergencyCallAction()
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnSetBackground(
    /* [in] */ IBitmap* bitmap)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnScreenTurnedOn()
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnScreenTurnedOff(
    /* [in] */ Int32 why)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnTrustChanged(
    /* [in] */ Int32 userId)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnTrustManagedChanged(
    /* [in] */ Int32 userId)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnTrustInitiatedByUser(
    /* [in] */ Int32 userId)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnFingerprintRecognized(
    /* [in] */ Int32 userId)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnFingerprintAcquired(
    /* [in] */ Int32 info)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnFaceUnlockStateChanged(
    /* [in] */ Boolean running,
    /* [in] */ Int32 userId)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnSubIdUpdated(
    /* [in] */ Int64 oldSubId,
    /* [in] */ Int64 newSubId)
{
    return NOERROR;
}

ECode KeyguardUpdateMonitorCallback::OnSubInfoContentChanged(
    /* [in] */ Int64 subId,
    /* [in] */ const String& column,
    /* [in] */ const String& sValue,
    /* [in] */ Int32 iValue)
{
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

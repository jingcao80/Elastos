
#include "elastos/droid/os/IPowerManager.h"
#include "elastos/droid/os/AndroidParcelUtils.h"
#include <binder/Parcel.h>
#include <hardware/power.h>

namespace Elastos {
namespace Droid {
namespace Os {

const String IPowerManagerProxy::DESCRIPTOR("android.os.IPowerManager");
const Int32 IPowerManagerProxy::TRANSACTION_powerHint = android::IBinder::FIRST_CALL_TRANSACTION + 4;

CAR_INTERFACE_IMPL(IPowerManagerProxy, Object, IIPowerManager)

IPowerManagerProxy::IPowerManagerProxy(
    /* [in] */ android::sp<android::IBinder>& pm)
    : mPm(pm)
{}

ECode IPowerManagerProxy::AcquireWakeLock(
    /* [in] */ IBinder* lock,
    /* [in] */ Int32 flags,
    /* [in] */ const String& tag,
    /* [in] */ const String& packageName,
    /* [in] */ IWorkSource* ws,
    /* [in] */ const String& historyTag)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::AcquireWakeLockWithUid(
    /* [in] */ IBinder* lock,
    /* [in] */ Int32 flags,
    /* [in] */ const String& tag,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uidtoblame)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::ReleaseWakeLock(
    /* [in] */ IBinder* lock,
    /* [in] */ Int32 flags)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::UpdateWakeLockUids(
    /* [in] */ IBinder* lock,
    /* [in] */ ArrayOf<Int32>* uids)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::PowerHint(
    /* [in] */ Int32 hintId,
    /* [in] */ Int32 value)
{
    android::Parcel data;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteInt32(data, hintId);
    AndroidParcelUtils::WriteInt32(data, value);
    mPm->transact(TRANSACTION_powerHint, data, NULL, android::IBinder::FLAG_ONEWAY);
    return NOERROR;
}

ECode IPowerManagerProxy::UpdateWakeLockWorkSource(
    /* [in] */ IBinder* lock,
    /* [in] */ IWorkSource* ws,
    /* [in] */ const String& historyTag)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::IsWakeLockLevelSupported(
    /* [in] */ Int32 level,
    /* [out] */ Boolean* res)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::UserActivity(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 event,
    /* [in] */ Int32 flags)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::WakeUp(
    /* [in] */ Int64 eventTime)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::GoToSleep(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 reason,
    /* [in] */ Int32 flags)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::Nap(
    /* [in] */ Int64 eventTime)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::IsInteractive(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::IsPowerSaveMode(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::SetPowerSaveMode(
    /* [in] */ Boolean mode,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::Reboot(
    /* [in] */ Boolean confirm,
    /* [in] */ const String& reason,
    /* [in] */ Boolean wait)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::Shutdown(
    /* [in] */ Boolean confirm,
    /* [in] */ Boolean wait)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::Crash(
    /* [in] */ const String& message)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::SetStayOnSetting(
    /* [in] */ Int32 val)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::SetMaximumScreenOffTimeoutFromDeviceAdmin(
    /* [in] */ Int32 timeMs)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::SetTemporaryScreenBrightnessSettingOverride(
    /* [in] */ Int32 brightness)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::SetTemporaryScreenAutoBrightnessAdjustmentSettingOverride(
    /* [in] */ Float adj)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::SetAttentionLight(
    /* [in] */ Boolean on,
    /* [in] */ Int32 color)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::UpdateBlockedUids(
    /* [in] */ Int32 uid,
    /* [in] */ Boolean isBlocked)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::CpuBoost(
    /* [in] */ Int32 duration)
{
    return PowerHint(POWER_HINT_INTERACTION, duration);
}

ECode IPowerManagerProxy::SetKeyboardVisibility(
    /* [in] */ Boolean visible)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::SetKeyboardLight(
    /* [in] */ Boolean on,
    /* [in] */ Int32 key)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::WakeUpWithProximityCheck(
    /* [in] */ Int64 time)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::SetPowerProfile(
    /* [in] */ const String& profile,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::GetPowerProfile(
    /* [out] */ String* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IPowerManagerProxy::ActivityResumed(
    /* [in] */ const String& componentName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
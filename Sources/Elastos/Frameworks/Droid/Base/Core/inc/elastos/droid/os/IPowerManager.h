//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_OS_IPOWERMANAGER_H__
#define __ELASTOS_DROID_OS_IPOWERMANAGER_H__

#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Os {

class IPowerManagerProxy
    : public Object
    , public IIPowerManager
{
public:
    IPowerManagerProxy(
        /* [in] */ android::sp<android::IBinder>& pm);

    CAR_INTERFACE_DECL();

    CARAPI AcquireWakeLock(
        /* [in] */ IBinder* lock,
        /* [in] */ Int32 flags,
        /* [in] */ const String& tag,
        /* [in] */ const String& packageName,
        /* [in] */ IWorkSource* ws,
        /* [in] */ const String& historyTag);

    CARAPI AcquireWakeLockWithUid(
        /* [in] */ IBinder* lock,
        /* [in] */ Int32 flags,
        /* [in] */ const String& tag,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uidtoblame);

    CARAPI ReleaseWakeLock(
        /* [in] */ IBinder* lock,
        /* [in] */ Int32 flags);

    CARAPI UpdateWakeLockUids(
        /* [in] */ IBinder* lock,
        /* [in] */ ArrayOf<Int32>* uids);

    CARAPI PowerHint(
        /* [in] */ Int32 hintId,
        /* [in] */ Int32 data);

    CARAPI UpdateWakeLockWorkSource(
        /* [in] */ IBinder* lock,
        /* [in] */ IWorkSource* ws,
        /* [in] */ const String& historyTag);

    CARAPI IsWakeLockLevelSupported(
        /* [in] */ Int32 level,
        /* [out] */ Boolean* res);

    CARAPI UserActivity(
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 event,
        /* [in] */ Int32 flags);

    CARAPI WakeUp(
        /* [in] */ Int64 eventTime);

    CARAPI GoToSleep(
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 reason,
        /* [in] */ Int32 flags);

    CARAPI Nap(
        /* [in] */ Int64 eventTime);

    CARAPI IsInteractive(
        /* [out] */ Boolean* result);

    CARAPI IsPowerSaveMode(
        /* [out] */ Boolean* result);

    CARAPI SetPowerSaveMode(
        /* [in] */ Boolean mode,
        /* [out] */ Boolean* result);

    CARAPI Reboot(
        /* [in] */ Boolean confirm,
        /* [in] */ const String& reason,
        /* [in] */ Boolean wait);

    CARAPI Shutdown(
        /* [in] */ Boolean confirm,
        /* [in] */ Boolean wait);

    CARAPI Crash(
        /* [in] */ const String& message);

    CARAPI SetStayOnSetting(
        /* [in] */ Int32 val);

    CARAPI SetMaximumScreenOffTimeoutFromDeviceAdmin(
        /* [in] */ Int32 timeMs);

    CARAPI SetTemporaryScreenBrightnessSettingOverride(
        /* [in] */ Int32 brightness);

    CARAPI SetTemporaryScreenAutoBrightnessAdjustmentSettingOverride(
        /* [in] */ Float adj);

    CARAPI SetAttentionLight(
        /* [in] */ Boolean on,
        /* [in] */ Int32 color);

    CARAPI UpdateBlockedUids(
        /* [in] */ Int32 uid,
        /* [in] */ Boolean isBlocked);

    CARAPI CpuBoost(
        /* [in] */ Int32 duration);

    CARAPI SetKeyboardVisibility(
        /* [in] */ Boolean visible);

    CARAPI SetKeyboardLight(
        /* [in] */ Boolean on,
        /* [in] */ Int32 key);

    CARAPI WakeUpWithProximityCheck(
        /* [in] */ Int64 time);

    CARAPI SetPowerProfile(
        /* [in] */ const String& profile,
        /* [out] */ Boolean* result);

    CARAPI GetPowerProfile(
        /* [out] */ String* result);

    CARAPI ActivityResumed(
        /* [in] */ const String& componentName);

private:
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_powerHint;

    android::sp<android::IBinder> mPm;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_IPOWERMANAGER_H__

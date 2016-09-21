// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/api/PowerMonitor_api.h"
#include "base/power_monitor/power_monitor_device_source_elastos.h"

#include "base/power_monitor/power_monitor.h"
#include "base/power_monitor/power_monitor_device_source.h"
#include "base/power_monitor/power_monitor_source.h"

namespace base {

// A helper function which is a friend of PowerMonitorSource.
void ProcessPowerEventHelper(PowerMonitorSource::PowerEvent event) {
  PowerMonitorSource::ProcessPowerEvent(event);
}

namespace android {

// Native implementation of PowerMonitor.java.
void OnBatteryChargingChanged() {
  ProcessPowerEventHelper(PowerMonitorSource::POWER_STATE_EVENT);
}

void OnMainActivityResumed() {
  ProcessPowerEventHelper(PowerMonitorSource::RESUME_EVENT);
}

void OnMainActivitySuspended() {
  ProcessPowerEventHelper(PowerMonitorSource::SUSPEND_EVENT);
}

}  // namespace android

bool PowerMonitorDeviceSource::IsOnBatteryPowerImpl() {
  //JNIEnv* env = base::android::AttachCurrentThread();
  //return base::android::Java_PowerMonitor_isBatteryPower(env);
  DCHECK(base::android::sElaPowerMonitorCallback);
  return base::android::sElaPowerMonitorCallback->elastos_PowerMonitor_isBatteryPower();
}

bool RegisterPowerMonitor() {
  return base::android::RegisterNativesImpl();
}

}  // namespace base

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/api/SysUtils_api.h"
#include "base/elastos/sys_utils.h"

#include "base/elastos/build_info.h"
#include "base/sys_info.h"

namespace base {
namespace android {

bool SysUtils::Register() {
  return RegisterNativesImpl();
}

bool SysUtils::IsLowEndDeviceFromJni() {
  //LLJNIEnv* env = AttachCurrentThread();
  //LLreturn Java_SysUtils_isLowEndDevice(env);
  DCHECK(sElaSysUtilsCallback);
  return sElaSysUtilsCallback->elastos_SysUtils_isLowEndDevice();
}

bool SysUtils::IsLowEndDevice() {
  static bool is_low_end = IsLowEndDeviceFromJni();
  return is_low_end;
}

size_t SysUtils::AmountOfPhysicalMemoryKBFromJni() {
  //LLJNIEnv* env = AttachCurrentThread();
  //LLreturn static_cast<size_t>(Java_SysUtils_amountOfPhysicalMemoryKB(env));
  DCHECK(sElaSysUtilsCallback);
  return static_cast<size_t>(sElaSysUtilsCallback->elastos_SysUtils_amountOfPhysicalMemoryKB());
}

size_t SysUtils::AmountOfPhysicalMemoryKB() {
  static size_t amount_of_ram = AmountOfPhysicalMemoryKBFromJni();
  return amount_of_ram;
}

SysUtils::SysUtils() { }

}  // namespace android
}  // namespace base

// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/api/CpuFeatures_api.h"
#include <cpu-features.h>

#include "base/elastos/api_elastos.h"

namespace base {
namespace android {

Elastos::Int32 GetCoreCount() {
  return android_getCpuCount();
}

Elastos::Int64 GetCpuFeatures() {
  return android_getCpuFeatures();
}

Elastos::Boolean RegisterCpuFeatures() {
  return RegisterNativesImpl();
}

}  // namespace android
}  // namespace base

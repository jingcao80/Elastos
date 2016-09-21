// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/api/MemoryPressureListener_api.h"
#include "base/elastos/memory_pressure_listener_android.h"

#include "base/memory/memory_pressure_listener.h"

// Defined and called by JNI.
static void OnMemoryPressure(Elastos::Int32 memory_pressure_level) {
  base::MemoryPressureListener::NotifyMemoryPressure(
      static_cast<base::MemoryPressureListener::MemoryPressureLevel>(
          memory_pressure_level));
}

namespace base {
namespace android {

bool MemoryPressureListenerAndroid::Register() {
  return RegisterNativesImpl();
}

void MemoryPressureListenerAndroid::RegisterSystemCallback() {
  DCHECK(sElaMemoryPressureListenerCallback);
  sElaMemoryPressureListenerCallback->elastos_MemoryPressureListener_registerSystemCallback(GetAppContext());
  //Java_MemoryPressureListener_registerSystemCallback(GetApplicationContext());
}

}  // namespace android
}  // namespace base

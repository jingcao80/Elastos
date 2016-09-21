// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/api_elastos.h"
#include "base/elastos/jni_registrar.h"

#include "base/debug/trace_event.h"
#include "base/logging.h"

namespace base {
namespace android {

bool RegisterNativeMethods( const RegistrationMethod* method,
                           size_t count) {
  TRACE_EVENT0("startup", "base_android::RegisterNativeMethods")
  const RegistrationMethod* end = method + count;
  while (method != end) {
    if (!method->func()) {
      DLOG(ERROR) << method->name << " failed registration!";
      return false;
    }
    method++;
  }
  return true;
}

}  // namespace android
}  // namespace base

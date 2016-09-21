// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/jni_utils.h"

#include "base/elastos/api_elastos.h"
//#include "base/android/scoped_java_ref.h"

#include "base/elastos/api/JNIUtils_api.h"

namespace base {
namespace android {

IInterface* GetClassLoader() {
  //return Java_JNIUtils_getClassLoader(env);
  DCHECK(sElaJNIUtilsCallback);
  return sElaJNIUtilsCallback->elastos_JNIUtils_getClassLoader();
}

bool RegisterJNIUtils() {
  return RegisterNativesImpl();
}

}  // namespace android
}  // namespace base


// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ANDROID_JNI_UTILS_H_
#define BASE_ANDROID_JNI_UTILS_H_

//#include <jni.h>

//#include "base/android/scoped_java_ref.h"
#include "ElAndroid.h"
#include "elastos.h"
#include "base/base_export.h"
namespace base {

namespace android {

// Gets a ClassLoader instance capable of loading Chromium java classes.
// This should be called either from JNI_OnLoad or from within a method called
// via JNI from Java.
//BASE_EXPORT ScopedJavaLocalRef<jobject> GetClassLoader(JNIEnv* env);
BASE_EXPORT IInterface* GetClassLoader();

bool RegisterJNIUtils();

}  // namespace android
}  // namespace base

#endif  // BASE_ANDROID_JNI_UTILS_H_


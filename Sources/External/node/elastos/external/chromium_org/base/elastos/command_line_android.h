// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ANDROID_COMMAND_LINE_ANDROID_H_
#define BASE_ANDROID_COMMAND_LINE_ANDROID_H_

//#include <jni.h>
#include "ElAndroid.h"
#include "elastos.h"
#include "base/base_export.h"

namespace base {
namespace android {

// Appends all strings in the given array as flags to the Chrome command line.
//LLvoid BASE_EXPORT InitNativeCommandLineFromJavaArray(
//LL    JNIEnv* env,
//LL    jobjectArray init_command_line);
void BASE_EXPORT InitNativeCommandLineFromJavaArray(Elastos::ArrayOf<Elastos::String>* init_command_line);

// JNI registration boilerplate.
bool RegisterCommandLine();

}  // namespace android
}  // namespace base

#endif  // BASE_ANDROID_COMMAND_LINE_ANDROID_H_

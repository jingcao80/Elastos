// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/api/CommandLine_api.h"
#include "base/elastos/jni_array.h"
#include "base/elastos/jni_string.h"
#include "base/elastos/command_line_android.h"

#include "base/command_line.h"
#include "base/logging.h"

using base::android::ConvertUTF8ToJavaString;
using base::android::ConvertJavaStringToUTF8;

namespace {

void AppendJavaStringArrayToCommandLine(Elastos::ArrayOf<Elastos::String>* array,
                                        bool includes_program) {
  std::vector<std::string> vec;
  if (array)
    base::android::AppendJavaStringArrayToStringVector(array, &vec);
  if (!includes_program)
    vec.insert(vec.begin(), "");
  CommandLine extra_command_line(vec);
  CommandLine::ForCurrentProcess()->AppendArguments(extra_command_line,
                                                    includes_program);
}

}  // namespace

static void Reset() {
  CommandLine::Reset();
}

static Elastos::Boolean HasSwitch(const Elastos::String& jswitch) {
  std::string switch_string(ConvertJavaStringToUTF8(jswitch));
  return CommandLine::ForCurrentProcess()->HasSwitch(switch_string);
}

static Elastos::String GetSwitchValue(const Elastos::String& jswitch) {
  std::string switch_string(ConvertJavaStringToUTF8(jswitch));
  std::string value(CommandLine::ForCurrentProcess()->GetSwitchValueNative(
      switch_string));
  if (value.empty())
    return Elastos::String();
  // OK to release, JNI binding.
  return ConvertUTF8ToJavaString(value);
}

static void AppendSwitch(const Elastos::String& jswitch) {
  std::string switch_string(ConvertJavaStringToUTF8(jswitch));
  CommandLine::ForCurrentProcess()->AppendSwitch(switch_string);
}

static void AppendSwitchWithValue(const Elastos::String& jswitch, const Elastos::String& jvalue) {
  std::string switch_string(ConvertJavaStringToUTF8(jswitch));
  std::string value_string (ConvertJavaStringToUTF8(jvalue));
  CommandLine::ForCurrentProcess()->AppendSwitchASCII(switch_string,
                                                      value_string);
}

static void AppendSwitchesAndArguments(Elastos::ArrayOf<Elastos::String>* array) {
  Elastos::AutoPtr<Elastos::ArrayOf<Elastos::String> > array_ptr(array);
  AppendJavaStringArrayToCommandLine(array_ptr, false);
}

namespace base {
namespace android {

void InitNativeCommandLineFromJavaArray(Elastos::ArrayOf<Elastos::String>* array) {
  // TODO(port): Make an overload of Init() that takes StringVector rather than
  // have to round-trip via AppendArguments.
  CommandLine::Init(0, NULL);
  Elastos::AutoPtr<Elastos::ArrayOf<Elastos::String> > array_ptr(array);
  AppendJavaStringArrayToCommandLine(array_ptr, true);
}

bool RegisterCommandLine() {
  return RegisterNativesImpl();
}

}  // namespace android
}  // namespace base

// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/api/BuildInfo_api.h"
#include "base/elastos/build_info.h"

#include <string>

#include "base/elastos/api_elastos.h"
//#include "base/android/jni_string.h"
//#include "base/android/scoped_java_ref.h"
#include "base/logging.h"
#include "base/memory/singleton.h"
//#include "jni/BuildInfo_jni.h"

namespace {

// The caller takes ownership of the returned const char*.
//LLconst char* StrDupJString(const base::android::JavaRef<jstring>& java_string) {
const char* StrDupJString(const Elastos::String& ela_string) {
  //LLstd::string str = ConvertJavaStringToUTF8(java_string);
  //LLreturn strdup(str.c_str());
  return strdup(ela_string.string());
}

}  // namespace

namespace base {
namespace android {

struct BuildInfoSingletonTraits {
  static BuildInfo* New() {
    return new BuildInfo();
  }

  static void Delete(BuildInfo* x) {
    // We're leaking this type, see kRegisterAtExit.
    NOTREACHED();
  }

  static const bool kRegisterAtExit = false;
#ifndef NDEBUG
  static const bool kAllowedToAccessOnNonjoinableThread = true;
#endif
};

//TODO DCHECK(sElaBuildInfoCallback)
BuildInfo::BuildInfo()
    : device_(StrDupJString(sElaBuildInfoCallback->elastos_BuildInfo_getDevice())),
      model_(StrDupJString(sElaBuildInfoCallback->elastos_BuildInfo_getDeviceModel())),
      brand_(StrDupJString(sElaBuildInfoCallback->elastos_BuildInfo_getBrand())),
      android_build_id_(StrDupJString(sElaBuildInfoCallback->elastos_BuildInfo_getAndroidBuildId())),
      android_build_fp_(StrDupJString(sElaBuildInfoCallback->elastos_BuildInfo_getAndroidBuildFingerprint())),
      package_version_code_(StrDupJString(sElaBuildInfoCallback->elastos_BuildInfo_getPackageVersionCode(
          GetAppContext()))),
      package_version_name_(StrDupJString(sElaBuildInfoCallback->elastos_BuildInfo_getPackageVersionName(
          GetAppContext()))),
      package_label_(StrDupJString(sElaBuildInfoCallback->elastos_BuildInfo_getPackageLabel(
          GetAppContext()))),
      package_name_(StrDupJString(sElaBuildInfoCallback->elastos_BuildInfo_getPackageName(
          GetAppContext()))),
      build_type_(StrDupJString(sElaBuildInfoCallback->elastos_BuildInfo_getBuildType())),
      sdk_int_(sElaBuildInfoCallback->elastos_BuildInfo_getSdkInt()),
      java_exception_info_(NULL) {
}

// static
BuildInfo* BuildInfo::GetInstance() {
  return Singleton<BuildInfo, BuildInfoSingletonTraits >::get();
}

void BuildInfo::set_java_exception_info(const std::string& info) {
  DCHECK(!java_exception_info_) << "info should be set only once.";
  java_exception_info_ = strndup(info.c_str(), 1024);
}

// static
bool BuildInfo::RegisterBindings() {
  return RegisterNativesImpl();
}

}  // namespace android
}  // namespace base

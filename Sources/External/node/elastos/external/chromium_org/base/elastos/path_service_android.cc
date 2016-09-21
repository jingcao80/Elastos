// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/api/PathService_api.h"
#include "base/elastos/path_service_android.h"

#include "base/elastos/api_elastos.h"
#include "base/elastos/jni_string.h"
#include "base/files/file_path.h"
#include "base/path_service.h"

namespace base {
namespace android {

void Override(Elastos::Int32 what, const Elastos::String& path) {
  FilePath file_path(ConvertJavaStringToUTF8(path));
  PathService::Override(what, file_path);
}

bool RegisterPathService() {
  return RegisterNativesImpl();
}

}  // namespace android
}  // namespace base

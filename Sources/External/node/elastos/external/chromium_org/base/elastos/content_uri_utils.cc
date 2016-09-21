// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/api/ContentUriUtils_api.h"
#include "base/elastos/content_uri_utils.h"

#include "base/elastos/api_elastos.h"
#include "base/elastos/jni_string.h"

using base::android::ConvertUTF8ToJavaString;

namespace base {

bool RegisterContentUriUtils() {
  return RegisterNativesImpl();
}

bool ContentUriExists(const FilePath& content_uri) {
  //LLJNIEnv* env = base::android::AttachCurrentThread();
  //LLScopedJavaLocalRef<jstring> j_uri =
  //LL    ConvertUTF8ToJavaString(env, content_uri.value());
  Elastos::String j_uri = ConvertUTF8ToJavaString(content_uri.value());
  //return Java_ContentUriUtils_contentUriExists(
  //    env, base::android::GetApplicationContext(), j_uri.obj());
  DCHECK(sElaContentUriUtilsCallback);
  return sElaContentUriUtilsCallback->elastos_ContentUriUtils_contentUriExists(
          base::android::GetAppContext(), j_uri);
}

File OpenContentUriForRead(const FilePath& content_uri) {
  //LLJNIEnv* env = base::android::AttachCurrentThread();
  //LLScopedJavaLocalRef<jstring> j_uri =
  //LL    ConvertUTF8ToJavaString(env, content_uri.value());
  //LLjint fd = Java_ContentUriUtils_openContentUriForRead(
  //LL    env, base::android::GetApplicationContext(), j_uri.obj());
  Elastos::String j_uri = ConvertUTF8ToJavaString(content_uri.value());
  DCHECK(sElaContentUriUtilsCallback);
  Elastos::Int32 fd = sElaContentUriUtilsCallback->elastos_ContentUriUtils_openContentUriForRead(
          base::android::GetAppContext(), j_uri);
  if (fd < 0)
    return File();
  return File(fd);
}

}  // namespace base

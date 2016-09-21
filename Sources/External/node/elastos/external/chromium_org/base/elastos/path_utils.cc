// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/api/PathUtils_api.h"
#include "base/elastos/path_utils.h"

#include "base/elastos/api_elastos.h"
#include "base/elastos/jni_string.h"
//#include "base/elastos/scoped_java_ref.h"
#include "base/files/file_path.h"


namespace base {
namespace android {

bool GetDataDirectory(FilePath* result) {
  //LLJNIEnv* env = AttachCurrentThread();
  //LLScopedJavaLocalRef<jstring> path =
  //LL    Java_PathUtils_getDataDirectory(env, GetApplicationContext());
  //LLFilePath data_path(ConvertJavaStringToUTF8(path));
  DCHECK(sElaPathUtilsCallback);
  Elastos::String path = sElaPathUtilsCallback->elastos_PathUtils_getDataDirectory(GetAppContext());
  FilePath data_path(ConvertJavaStringToUTF8(path));
  *result = data_path;
  return true;
}

bool GetDatabaseDirectory(FilePath* result) {
  //LLJNIEnv* env = AttachCurrentThread();
  //LLScopedJavaLocalRef<jstring> path =
  //LL    Java_PathUtils_getDatabaseDirectory(env, GetApplicationContext());
  //LLFilePath data_path(ConvertJavaStringToUTF8(path));
  DCHECK(sElaPathUtilsCallback);
  Elastos::String path = sElaPathUtilsCallback->elastos_PathUtils_getDatabaseDirectory(GetAppContext());
  FilePath data_path(ConvertJavaStringToUTF8(path));
  *result = data_path;
  return true;
}

bool GetCacheDirectory(FilePath* result) {
  //LLJNIEnv* env = AttachCurrentThread();
  //LLScopedJavaLocalRef<jstring> path =
  //LL    Java_PathUtils_getCacheDirectory(env, GetApplicationContext());
  //LLFilePath cache_path(ConvertJavaStringToUTF8(path));
  DCHECK(sElaPathUtilsCallback);
  Elastos::String path = sElaPathUtilsCallback->elastos_PathUtils_getCacheDirectory(GetAppContext());
  FilePath cache_path(ConvertJavaStringToUTF8(path));
  *result = cache_path;
  return true;
}

bool GetDownloadsDirectory(FilePath* result) {
  //LLJNIEnv* env = AttachCurrentThread();
  //LLScopedJavaLocalRef<jstring> path =
  //LL    Java_PathUtils_getDownloadsDirectory(env, GetApplicationContext());
  //LLFilePath downloads_path(ConvertJavaStringToUTF8(path));
  DCHECK(sElaPathUtilsCallback);
  Elastos::String path = sElaPathUtilsCallback->elastos_PathUtils_getDownloadsDirectory(GetAppContext());
  FilePath downloads_path(ConvertJavaStringToUTF8(path));
  *result = downloads_path;
  return true;
}

bool GetNativeLibraryDirectory(FilePath* result) {
  //JNIEnv* env = AttachCurrentThread();
  //ScopedJavaLocalRef<jstring> path =
  //    Java_PathUtils_getNativeLibraryDirectory(env, GetApplicationContext());
  //FilePath library_path(ConvertJavaStringToUTF8(path));
  DCHECK(sElaPathUtilsCallback);
  Elastos::String path = sElaPathUtilsCallback->elastos_PathUtils_getNativeLibraryDirectory(GetAppContext());
  FilePath library_path(ConvertJavaStringToUTF8(path));
  *result = library_path;
  return true;
}

bool GetExternalStorageDirectory(FilePath* result) {
  //JNIEnv* env = AttachCurrentThread();
  //ScopedJavaLocalRef<jstring> path =
  //    Java_PathUtils_getExternalStorageDirectory(env);
  //FilePath storage_path(ConvertJavaStringToUTF8(path));
  DCHECK(sElaPathUtilsCallback);
  Elastos::String path = sElaPathUtilsCallback->elastos_PathUtils_getExternalStorageDirectory();
  FilePath storage_path(ConvertJavaStringToUTF8(path));
  *result = storage_path;
  return true;
}

bool RegisterPathUtils() {
  return RegisterNativesImpl();
}

}  // namespace android
}  // namespace base

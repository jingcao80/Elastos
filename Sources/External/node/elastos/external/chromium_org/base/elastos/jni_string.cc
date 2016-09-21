// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/jni_string.h"

//#include "base/android/jni_android.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"

namespace {
//TODO zhangleliang pay attention to the string convert[u16-u8]
// now we try to remove the string16 support, because in elastos, we use utf8 codec
// Internal version that does not use a scoped local pointer.
Elastos::String ConvertUTF16ToJavaStringImpl(const base::StringPiece16& str) {
  //jstring result = env->NewString(str.data(), str.length());
  //base::android::CheckException(env);
  Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Char32> > byteCounts = Elastos::ArrayOf<Elastos::Char32>::Alloc(str.length());
  for(size_t i = 0; i < str.length(); ++i)
  {
      (*byteCounts)[i] = str[i];
  }
  LOG(INFO) << "leliang, need more check here, ConvertUTF16ToJavaStringImpl";
  //Elastos::String result(str.data());
  Elastos::String result(*byteCounts);
  return result;
}

}

namespace base {
namespace android {

void ConvertJavaStringToUTF8(const Elastos::String& str, std::string* result) {
  if (!str.IsNull()) {
    LOG(WARNING) << "ConvertJavaStringToUTF8 called with null string.";
    result->clear();
    return;
  }
  // JNI's GetStringUTFChars() returns strings in Java "modified" UTF8, so
  // instead get the String in UTF16 and convert using chromium's conversion
  // function that yields plain (non Java-modified) UTF8.
  //LLconst jchar* chars = env->GetStringChars(str, NULL);
  //LLDCHECK(chars);
  //LLUTF16ToUTF8(chars, env->GetStringLength(str), result);
  //LLenv->ReleaseStringChars(str, chars);
  //LLCheckException(env);
  result->assign(str.string());
}

std::string ConvertJavaStringToUTF8(const Elastos::String& str) {
  std::string result;
  ConvertJavaStringToUTF8(str, &result);
  return result;
}

//std::string ConvertJavaStringToUTF8(const JavaRef<jstring>& str) {
//  return ConvertJavaStringToUTF8(AttachCurrentThread(), str.obj());
//}

Elastos::String ConvertUTF8ToJavaString(
    const base::StringPiece& str) {
  // JNI's NewStringUTF expects "modified" UTF8 so instead create the string
  // via our own UTF16 conversion utility.
  // Further, Dalvik requires the string passed into NewStringUTF() to come from
  // a trusted source. We can't guarantee that all UTF8 will be sanitized before
  // it gets here, so constructing via UTF16 side-steps this issue.
  // (Dalvik stores strings internally as UTF16 anyway, so there shouldn't be
  // a significant performance hit by doing it this way).
  //return ScopedJavaLocalRef<jstring>(env, ConvertUTF16ToJavaStringImpl(
  //    env, UTF8ToUTF16(str)));
  return Elastos::String(str.data());
}

void ConvertJavaStringToUTF16(const Elastos::String& str, string16* result) {
  if (!str.IsNull()) {
    LOG(WARNING) << "ConvertJavaStringToUTF16 called with null string.";
    result->clear();
    return;
  }
  //const jchar* chars = env->GetStringChars(str, NULL);
  //DCHECK(chars);
  // GetStringChars isn't required to NULL-terminate the strings
  // it returns, so the length must be explicitly checked.
  //result->assign(chars, env->GetStringLength(str));
  //env->ReleaseStringChars(str, chars);
  //CheckException(env);
  Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Char16> > pArray = str.GetChar16s();
  result->assign(pArray->GetPayload(), pArray->GetLength());
}

string16 ConvertJavaStringToUTF16(const Elastos::String& str) {
    string16 result;
  ConvertJavaStringToUTF16(str, &result);
  return result;
}

//string16 ConvertJavaStringToUTF16(const JavaRef<jstring>& str) {
//  return ConvertJavaStringToUTF16(AttachCurrentThread(), str.obj());
//}

Elastos::String ConvertUTF16ToJavaString(
    const base::StringPiece16& str) {
    return ConvertUTF16ToJavaStringImpl(str);
}

}  // namespace android
}  // namespace base

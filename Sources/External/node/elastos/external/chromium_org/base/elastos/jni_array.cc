// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/jni_array.h"

#include "base/elastos/api_elastos.h"
#include "base/elastos/jni_string.h"
#include "base/logging.h"

namespace base {
namespace android {

Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Byte> > ToJavaByteArray(
    const uint8* bytes, size_t len) {
  Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Byte> > result = Elastos::ArrayOf<Elastos::Byte>::Alloc(len);
        /*
    const uint8* bytes, size_t len) {
  jbyteArray byte_array = env->NewByteArray(len);
  CheckException(env);
  DCHECK(byte_array);

  env->SetByteArrayRegion(
      byte_array, 0, len, reinterpret_cast<const jbyte*>(bytes));
  CheckException(env);

  return ScopedJavaLocalRef<jbyteArray>(env, byte_array);
  */
  Elastos::Byte* byte_array = result->GetPayload();
  memcpy(byte_array, bytes, len);
  return result;
}

Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Int32> > ToJavaIntArray(
    const int* ints, size_t len) {
  Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Int32> > result = Elastos::ArrayOf<Elastos::Int32>::Alloc(len);
  /*
  jintArray int_array = env->NewIntArray(len);
  CheckException(env);
  DCHECK(int_array);

  env->SetIntArrayRegion(
      int_array, 0, len, reinterpret_cast<const jint*>(ints));
  CheckException(env);

  return ScopedJavaLocalRef<jintArray>(env, int_array);
  */
  for(size_t i = 0; i < len; ++i)
  {
      (*result)[i] = ints[i];
  }
  return result;
}

Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Int32> > ToJavaIntArray(
    const std::vector<int>& ints) {
  return ToJavaIntArray(ints.begin(), ints.size());
}

Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Int64> > ToJavaLongArray(
    const int64* longs, size_t len) {
  Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Int64> > result = Elastos::ArrayOf<Elastos::Int64>::Alloc(len);
  /*
  jlongArray long_array = env->NewLongArray(len);
  CheckException(env);
  DCHECK(long_array);

  env->SetLongArrayRegion(
      long_array, 0, len, reinterpret_cast<const jlong*>(longs));
  CheckException(env);

  return ScopedJavaLocalRef<jlongArray>(env, long_array);
  */
  for(size_t i = 0; i < len; ++i)
  {
      (*result)[i] = longs[i];
  }
  return result;
}

// Returns a new Java long array converted from the given int64 array.
BASE_EXPORT Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Int64> > ToJavaLongArray(
    const std::vector<int64>& longs) {
  return ToJavaLongArray(longs.begin(), longs.size());
}

Elastos::AutoPtr<Elastos::ArrayOf<Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Byte> > > > ToJavaArrayOfByteArray(
    const std::vector<std::string>& v) {
  Elastos::AutoPtr<Elastos::ArrayOf<Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Byte> > > > result;
    /*
  ScopedJavaLocalRef<jclass> byte_array_clazz = GetClass(env, "[B");
  jobjectArray joa = env->NewObjectArray(v.size(),
                                         byte_array_clazz.obj(), NULL);
  CheckException(env);

  for (size_t i = 0; i < v.size(); ++i) {
    ScopedJavaLocalRef<jbyteArray> byte_array = ToJavaByteArray(env,
        reinterpret_cast<const uint8*>(v[i].data()), v[i].length());
    env->SetObjectArrayElement(joa, i, byte_array.obj());
  }
  return ScopedJavaLocalRef<jobjectArray>(env, joa);
  */
  result = Elastos::ArrayOf<Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Byte> > >::Alloc(v.size());
  for (size_t i = 0; i < v.size(); ++i) {
      Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Byte> > byte_array = ToJavaByteArray(reinterpret_cast<const uint8*>(v[i].data()), v[i].length());
      //byte_array->Copy(&((*result)[i]));
      result->Set(i, byte_array);
  }
  return result;
}

Elastos::AutoPtr<Elastos::ArrayOf<Elastos::String> > ToJavaArrayOfStrings(
    const std::vector<std::string>& v) {
  Elastos::AutoPtr<Elastos::ArrayOf<Elastos::String> > result = Elastos::ArrayOf<Elastos::String>::Alloc(v.size());
  /*
  ScopedJavaLocalRef<jclass> string_clazz = GetClass(env, "java/lang/String");
  jobjectArray joa = env->NewObjectArray(v.size(), string_clazz.obj(), NULL);
  CheckException(env);

  for (size_t i = 0; i < v.size(); ++i) {
    ScopedJavaLocalRef<jstring> item = ConvertUTF8ToJavaString(env, v[i]);
    env->SetObjectArrayElement(joa, i, item.obj());
  }
  return ScopedJavaLocalRef<jobjectArray>(env, joa);
  */
  for(size_t i = 0; i < v.size(); ++i)
  {
      (*result)[i] = v[i].c_str();
  }
  return result;
}

Elastos::AutoPtr<Elastos::ArrayOf<Elastos::String> > ToJavaArrayOfStrings(
    const std::vector<string16>& v) {
  Elastos::AutoPtr<Elastos::ArrayOf<Elastos::String> > result;
  /*TODO
  ScopedJavaLocalRef<jclass> string_clazz = GetClass(env, "java/lang/String");
  jobjectArray joa = env->NewObjectArray(v.size(), string_clazz.obj(), NULL);
  CheckException(env);

  for (size_t i = 0; i < v.size(); ++i) {
    ScopedJavaLocalRef<jstring> item = ConvertUTF16ToJavaString(env, v[i]);
    env->SetObjectArrayElement(joa, i, item.obj());
  }
  return ScopedJavaLocalRef<jobjectArray>(env, joa);
  */
  LOG(INFO) << "leliang, NO IMPL, ToJavaArrayOfStrings in elastos/jni_array.cc";
  return result;
}

void AppendJavaStringArrayToStringVector(Elastos::ArrayOf<Elastos::String>*  array,
                                         std::vector<string16>* out) {
  /*TODO
  DCHECK(out);
  if (!array)
    return;
  jsize len = env->GetArrayLength(array);
  size_t back = out->size();
  out->resize(back + len);
  for (jsize i = 0; i < len; ++i) {
    ScopedJavaLocalRef<jstring> str(env,
        static_cast<jstring>(env->GetObjectArrayElement(array, i)));
    ConvertJavaStringToUTF16(env, str.obj(), &((*out)[back + i]));
  }
  */
  LOG(INFO) << "leliang, NO IMPL, ToJavaArrayOfStrings in elastos/jni_array.cc";
}

void AppendJavaStringArrayToStringVector(Elastos::ArrayOf<Elastos::String>* array,
                                         std::vector<std::string>* out) {
  DCHECK(out);
  if (!array)
    return;
  /*
  jsize len = env->GetArrayLength(array);
  size_t back = out->size();
  out->resize(back + len);
  for (jsize i = 0; i < len; ++i) {
    ScopedJavaLocalRef<jstring> str(env,
        static_cast<jstring>(env->GetObjectArrayElement(array, i)));
    ConvertJavaStringToUTF8(env, str.obj(), &((*out)[back + i]));
  }
  */
  int len = array->GetLength();
  size_t back = out->size();
  out->resize(back + len);
  for (int i = 0; i < len; ++i)
  {
      ConvertJavaStringToUTF8((*array)[i], &((*out)[back + i]));
  }
}

void AppendJavaByteArrayToByteVector(Elastos::ArrayOf<Elastos::Byte>* byte_array,
                                     std::vector<uint8>* out) {
  DCHECK(out);
  if (!byte_array)
    return;
  /*
  jsize len = env->GetArrayLength(byte_array);
  jbyte* bytes = env->GetByteArrayElements(byte_array, NULL);
  out->insert(out->end(), bytes, bytes + len);
  env->ReleaseByteArrayElements(byte_array, bytes, JNI_ABORT);
  */
  int len = byte_array->GetLength();
  Elastos::Byte* bytes = byte_array->GetPayload();
  out->insert(out->end(), bytes, bytes + len);
}

void JavaByteArrayToByteVector(Elastos::ArrayOf<Elastos::Byte>* byte_array,
                               std::vector<uint8>* out) {
  DCHECK(out);
  out->clear();
  AppendJavaByteArrayToByteVector(byte_array, out);
}

void JavaIntArrayToIntVector(Elastos::ArrayOf<Elastos::Int32>* int_array,
                             std::vector<int>* out) {
  DCHECK(out);
  out->clear();
  /*
  jsize len = env->GetArrayLength(int_array);
  jint* ints = env->GetIntArrayElements(int_array, NULL);
  for (jsize i = 0; i < len; ++i) {
    out->push_back(static_cast<int>(ints[i]));
  }
  env->ReleaseIntArrayElements(int_array, ints, JNI_ABORT);
  */
  int len = int_array->GetLength();
  Elastos::Int32* ints = int_array->GetPayload();
  for (int i = 0; i < len; ++i)
  {
    out->push_back(static_cast<int>(ints[i]));
  }
}

void JavaLongArrayToLongVector(Elastos::ArrayOf<Elastos::Int64>* long_array,
                               std::vector<long>* out) {
  DCHECK(out);
  out->clear();
  /*TODO: the long is not equal Elastos::Int64 on 32bit system
  jsize len = env->GetArrayLength(long_array);
  jlong* longs = env->GetLongArrayElements(long_array, NULL);
  for (jsize i = 0; i < len; ++i) {
    out->push_back(static_cast<long>(longs[i]));
  }
  env->ReleaseLongArrayElements(long_array, longs, JNI_ABORT);
  */
  int len = long_array->GetLength();
  Elastos::Int64* longs = long_array->GetPayload();
  for (int i = 0; i < len; ++i)
  {
    out->push_back(static_cast<long>(longs[i]));
  }
}

void JavaFloatArrayToFloatVector(Elastos::ArrayOf<Elastos::Float>* float_array,
                                 std::vector<float>* out) {
  DCHECK(out);
  out->clear();
  /*
  jsize len = env->GetArrayLength(float_array);
  jfloat* floats = env->GetFloatArrayElements(float_array, NULL);
  for (jsize i = 0; i < len; ++i) {
    out->push_back(static_cast<float>(floats[i]));
  }
  env->ReleaseFloatArrayElements(float_array, floats, JNI_ABORT);
  */
  int len = float_array->GetLength();
  Elastos::Float* floats = float_array->GetPayload();
  for (int i = 0; i < len; ++i)
  {
    out->push_back(static_cast<float>(floats[i]));
  }
}

void JavaArrayOfByteArrayToStringVector(
    Elastos::ArrayOf<Elastos::AutoPtr<Elastos::ArrayOf<Elastos::Byte> > >* array,
    std::vector<std::string>* out) {
  DCHECK(out);
  out->clear();
  /*
  jsize len = env->GetArrayLength(array);
  out->resize(len);
  for (jsize i = 0; i < len; ++i) {
    ScopedJavaLocalRef<jbyteArray> bytes_array(
        env, static_cast<jbyteArray>(
            env->GetObjectArrayElement(array, i)));
    jsize bytes_len = env->GetArrayLength(bytes_array.obj());
    jbyte* bytes = env->GetByteArrayElements(bytes_array.obj(), NULL);
    (*out)[i].assign(reinterpret_cast<const char*>(bytes), bytes_len);
    env->ReleaseByteArrayElements(bytes_array.obj(), bytes, JNI_ABORT);
  }
  */
  int len = array->GetLength();
  out->resize(len);
  for (int i = 0; i < len; ++i)
  {
    int bytes_len = ((*array)[i])->GetLength();
    Elastos::Byte* bytes = ((*array)[i])->GetPayload();
    (*out)[i].assign(reinterpret_cast<const char*>(bytes), bytes_len);
  }
}

}  // namespace android
}  // namespace base

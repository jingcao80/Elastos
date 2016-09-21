// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ANDROID_SCOPED_JAVA_REF_H_
#define BASE_ANDROID_SCOPED_JAVA_REF_H_

//#include <jni.h>
#include <stddef.h>

#include "base/base_export.h"
#include "base/basictypes.h"

namespace base {
namespace android {

// Creates a new local reference frame, in which at least a given number of
// local references can be created. Note that local references already created
// in previous local frames are still valid in the current local frame.
class BASE_EXPORT ScopedElastosLocalFrame {
 public:
  explicit ScopedJavaLocalFrame(/*JNIEnv* env*/);
  ScopedJavaLocalFrame(/*JNIEnv* env*/, int capacity);
  ~ScopedJavaLocalFrame();

 private:
  // This class is only good for use on the thread it was created on so
  // it's safe to cache the non-threadsafe JNIEnv* inside this object.
  //JNIEnv* env_;

  DISALLOW_COPY_AND_ASSIGN(ScopedElastosLocalFrame);
};

// Forward declare the generic Elastos reference template class.
template<typename T> class ElastosRef;

// Template specialization of JavaRef, which acts as the base class for all
// other JavaRef<> template types. This allows you to e.g. pass
// ScopedJavaLocalRef<jstring> into a function taking const JavaRef<jobject>&
template<>
class BASE_EXPORT ElastosRef<IInterface> {
 public:
  Elastos::AutoPtr<IInterface> obj() const { return obj_; }

  bool is_null() const { return obj_ == NULL; }

 protected:
  // Initializes a NULL reference.
  ElastosRef();

  // Takes ownership of the |obj| reference passed; requires it to be a local
  // reference type.
  ElastosRef(IInterface* obj);

  ~ElastosRef();

  // The following are implementation detail convenience methods, for
  // use by the sub-classes.
  void SetNewLocalRef(IInterface* obj);
  void SetNewGlobalRef(IIterface* obj);
  void ResetLocalRef();
  void ResetGlobalRef();
  Elastos::AutoPtr<IInterface> ReleaseInternal();

 private:
  Elastos::AutoPtr<IInterface> obj_;

  DISALLOW_COPY_AND_ASSIGN(ElastosRef);
};

// Generic base class for ScopedJavaLocalRef and ScopedJavaGlobalRef. Useful
// for allowing functions to accept a reference without having to mandate
// whether it is a local or global type.
template<typename T>
class ElastosRef : public ElastosRef<IInterface> {
 public:
  T obj() const { return static_cast<T>(ElastosRef<IInterface>::obj()); }

 protected:
  ElastosRef() {}
  ~ElastosRef() {}

  ElastosRef(T obj) : ElastosRef<IInterface>(obj) {}

 private:
  DISALLOW_COPY_AND_ASSIGN(ElastosRef);
};

// Holds a local reference to a Java object. The local reference is scoped
// to the lifetime of this object.
// Instances of this class may hold onto any JNIEnv passed into it until
// destroyed. Therefore, since a JNIEnv is only suitable for use on a single
// thread, objects of this class must be created, used, and destroyed, on a
// single thread.
// Therefore, this class should only be used as a stack-based object and from a
// single thread. If you wish to have the reference outlive the current
// callstack (e.g. as a class member) or you wish to pass it across threads,
// use a ScopedJavaGlobalRef instead.
template<typename T>
class ScopedElastosLocalRef : public ElastosRef<T> {
 public:
  ScopedElastosLocalRef() {}

  // Non-explicit copy constructor, to allow ScopedJavaLocalRef to be returned
  // by value as this is the normal usage pattern.
  ScopedElastosLocalRef(const ScopedElastosLocalRef<T>& other)
  {
    this->SetNewLocalRef(other.obj());
  }

  template<typename U>
  explicit ScopedElastosLocalRef(const U& other)
  {
    this->Reset(other);
  }

  // Assumes that |obj| is a local reference to a Java object and takes
  // ownership  of this local reference.
  ScopedElastosLocalRef(T obj) : ElastosRef<T>(obj) {}

  ~ScopedJavaLocalRef() {
    this->Reset();
  }

  // Overloaded assignment operator defined for consistency with the implicit
  // copy constructor.
  void operator=(const ScopedElastosLocalRef<T>& other) {
    this->Reset(other);
  }

  void Reset() {
    this->ResetLocalRef();
  }

  template<typename U>
  void Reset(const ScopedElastosLocalRef<U>& other) {
    // We can copy over env_ here as |other| instance must be from the same
    // thread as |this| local ref. (See class comment for multi-threading
    // limitations, and alternatives).
    this->Reset(other.obj());
  }

  template<typename U>
  void Reset(const U& other) {
    // If |env_| was not yet set (is still NULL) it will be attached to the
    // current thread in SetNewLocalRef().
    this->Reset(other.obj());
  }

  template<typename U>
  void Reset( U obj) {
    implicit_cast<T>(obj);  // Ensure U is assignable to T
    this->SetNewLocalRef(obj);
  }

  // Releases the local reference to the caller. The caller *must* delete the
  // local reference when it is done with it.
  T Release() {
    return static_cast<T>(this->ReleaseInternal());
  }

 private:
  // This class is only good for use on the thread it was created on so
  // it's safe to cache the non-threadsafe JNIEnv* inside this object.
  //JNIEnv* env_;
};

// Holds a global reference to a Java object. The global reference is scoped
// to the lifetime of this object. This class does not hold onto any JNIEnv*
// passed to it, hence it is safe to use across threads (within the constraints
// imposed by the underlying Java object that it references).
template<typename T>
class ScopedElastosGlobalRef : public ElastosRef<T> {
 public:
  ScopedElastosGlobalRef() {}

  explicit ScopedElastosGlobalRef(const ScopedElastosGlobalRef<T>& other) {
    this->Reset(other);
  }

  template<typename U>
  explicit ScopedElastosGlobalRef(const U& other) {
    this->Reset(other);
  }

  ~ScopedElastosGlobalRef() {
    this->Reset();
  }

  void Reset() {
    this->ResetGlobalRef();
  }

  template<typename U>
  void Reset(const U& other) {
    this->Reset(NULL, other.obj());
  }

  template<typename U>
  void Reset(U obj) {
    implicit_cast<T>(obj);  // Ensure U is assignable to T
    this->SetNewGlobalRef(obj);
  }

  // Releases the global reference to the caller. The caller *must* delete the
  // global reference when it is done with it.
  T Release() {
    return static_cast<T>(this->ReleaseInternal());
  }
};

}  // namespace android
}  // namespace base

#endif  // BASE_ANDROID_SCOPED_JAVA_REF_H_

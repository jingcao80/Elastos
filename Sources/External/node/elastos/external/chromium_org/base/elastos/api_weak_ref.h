// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ELASTOS_API_WEAK_REF_H_
#define BASE_ELASTOS_API_WEAK_REF_H_

//#include <jni.h>

//#include "base/android/scoped_java_ref.h"
#include "ElAndroid.h"
#include "elastos.h"
#include "base/base_export.h"

// Manages WeakGlobalRef lifecycle.
// This class is not thread-safe w.r.t. get() and reset(). Multiple threads may
// safely use get() concurrently, but if the user calls reset() (or of course,
// calls the destructor) they'll need to provide their own synchronization.
class BASE_EXPORT ObjectWeakGlobalRef {
 public:
  ObjectWeakGlobalRef();
  ObjectWeakGlobalRef(const ObjectWeakGlobalRef& orig);
  ObjectWeakGlobalRef(IInterface* obj);
  virtual ~ObjectWeakGlobalRef();

  void operator=(const ObjectWeakGlobalRef& rhs);

  //base::android::ScopedJavaLocalRef<jobject> get(JNIEnv* env) const;
  Elastos::AutoPtr<IInterface> get() const;

  bool is_empty() const { return obj_ == NULL; }

  void reset();

 private:
  void Assign(const ObjectWeakGlobalRef& rhs);

  //jweak obj_;
  Elastos::AutoPtr<IWeakReference> obj_;
};

// Get the real object stored in the weak reference returned as a
// ScopedJavaLocalRef.
//BASE_EXPORT base::android::ScopedJavaLocalRef<jobject> GetRealObject(
//    JNIEnv* env, jweak obj);
Elastos::AutoPtr<IInterface> GetRealObject(Elastos::AutoPtr<IWeakReference> obj);

#endif  // BASE_ANDROID_JNI_WEAK_REF_H_

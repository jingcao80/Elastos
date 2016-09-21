// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/api_weak_ref.h"

#include "base/elastos/api_elastos.h"
#include "base/logging.h"

//using base::android::AttachCurrentThread;

ObjectWeakGlobalRef::ObjectWeakGlobalRef()
  : obj_(NULL) {
}

ObjectWeakGlobalRef::ObjectWeakGlobalRef(
    const ObjectWeakGlobalRef& orig)
    : obj_(NULL) {
  Assign(orig);
}

ObjectWeakGlobalRef::ObjectWeakGlobalRef(IInterface* obj) {
    //LL: obj_(env->NewWeakGlobalRef(obj)) {
  DCHECK(obj);
  IWeakReferenceSource* source = IWeakReferenceSource::Probe((IWeakReferenceSource *)obj);
  if (source)
      source->GetWeakReference((IWeakReference**)&obj_);
  DCHECK(obj_);
}

ObjectWeakGlobalRef::~ObjectWeakGlobalRef() {
  reset();
}

void ObjectWeakGlobalRef::operator=(const ObjectWeakGlobalRef& rhs) {
  Assign(rhs);
}

void ObjectWeakGlobalRef::reset() {
  if (obj_) {
    //LLAttachCurrentThread()->DeleteWeakGlobalRef(obj_);
    obj_ = NULL;
  }
}

//LLbase::android::ScopedJavaLocalRef<jobject>
//    ObjectWeakGlobalRef::get(JNIEnv* env) const {
Elastos::AutoPtr<IInterface> ObjectWeakGlobalRef::get() const {
  return GetRealObject(obj_);
}

/*
base::android::ScopedJavaLocalRef<jobject> GetRealObject(
    JNIEnv* env, jweak obj) {
  jobject real = NULL;
  if (obj) {
    real = env->NewLocalRef(obj);
    if (!real)
      DLOG(ERROR) << "The real object has been deleted!";
  }
  return base::android::ScopedJavaLocalRef<jobject>(env, real);
}
*/
Elastos::AutoPtr<IInterface> GetRealObject(Elastos::AutoPtr<IWeakReference> obj) {
    Elastos::AutoPtr<IInterface> ws;
    if (obj)
    {
        obj->Resolve(EIID_IInterface, (IInterface**)&ws);
        if (!ws)
            DLOG(ERROR) << "The real object has been deleted!";
    }
    return ws;
}

void ObjectWeakGlobalRef::Assign(const ObjectWeakGlobalRef& other) {
  if (&other == this)
    return;

  //JNIEnv* env = AttachCurrentThread();
  //if (obj_)
  //  env->DeleteWeakGlobalRef(obj_);

  //obj_ = other.obj_ ? env->NewWeakGlobalRef(other.obj_) : NULL;
  obj_ = other.obj_ ? other.obj_ : NULL;
}

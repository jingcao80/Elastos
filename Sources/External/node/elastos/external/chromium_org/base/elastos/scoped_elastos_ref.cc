// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/scoped_elastos_ref.h"

#include "base/elastos/api_elastos.h"
#include "base/logging.h"

namespace base {
namespace android {
namespace {

const int kDefaultLocalFrameCapacity = 16;

}  // namespace

ScopedElastosLocalFrame::ScopedElastosLocalFrame() {
  //int failed = env_->PushLocalFrame(kDefaultLocalFrameCapacity);
  //DCHECK(!failed);
}

ScopedElastosLocalFrame::ScopedElastosLocalFrame(int capacity)
{
  //int failed = env_->PushLocalFrame(capacity);
  //DCHECK(!failed);
}

ScopedElastosLocalFrame::~ScopedElastosLocalFrame() { /*env_->PopLocalFrame(NULL);*/ }

ElastosRef<IInterface>::ElastosRef() : obj_(NULL) {}

ElastosRef<IInterface>::ElastosRef(IInterface* obj) : obj_(obj) {
  /*
  if (obj) {
    DCHECK(env && env->GetObjectRefType(obj) == JNILocalRefType);
  }
  */
}

ElastosRef<IInterface>::~ElastosRef() {
}

void ElastosRef<IInterface>::SetNewLocalRef(IInterface* obj) {
  /*
  if (!env) {
    env = AttachCurrentThread();
  } else {
    DCHECK_EQ(env, AttachCurrentThread());  // Is |env| on correct thread.
  }
  if (obj)
    obj = env->NewLocalRef(obj);
  if (obj_)
    env->DeleteLocalRef(obj_);
  */
  obj_ = obj;
}

void ElastosRef<IInterface>::SetNewGlobalRef(IInterface* obj) {
  /*
  if (!env) {
    env = AttachCurrentThread();
  } else {
    DCHECK_EQ(env, AttachCurrentThread());  // Is |env| on correct thread.
  }
  */
  //if (obj)
  //  obj = env->NewGlobalRef(obj);
  //if (obj_)
  //  env->DeleteGlobalRef(obj_);
  obj_ = obj;
}

void ElastosRef<IInterface>::ResetLocalRef() {
  if (obj_) {
    //DCHECK_EQ(env, AttachCurrentThread());  // Is |env| on correct thread.
    //env->DeleteLocalRef(obj_);
    obj_ = NULL;
  }
}

void ElastosRef<IInterface>::ResetGlobalRef() {
  if (obj_) {
    //AttachCurrentThread()->DeleteGlobalRef(obj_);
    obj_ = NULL;
  }
}

Elastos::AutoPtr<IInterface> ElastosRef<IInterface>::ReleaseInternal() {
  Elastos::AutoPtr<IInterface> obj = obj_;
  obj_ = NULL;
  return obj;
}

}  // namespace android
}  // namespace base

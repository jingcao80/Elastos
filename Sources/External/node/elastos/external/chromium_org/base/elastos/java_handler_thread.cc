// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/elastos/java_handler_thread.h"
#include "base/elastos/api/JavaHandlerThread_api.h"

//#include <jni.h>

#include "base/elastos/api_elastos.h"
#include "base/elastos/jni_string.h"
#include "base/message_loop/message_loop.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/thread_restrictions.h"

namespace base {

namespace android {

JavaHandlerThread::JavaHandlerThread(const char* name) {
  //LLJNIEnv* env = base::android::AttachCurrentThread();

  //LLjava_thread_.Reset(Java_JavaHandlerThread_create(
  //LL    env, ConvertUTF8ToJavaString(env, name).Release()));
  DCHECK(sElaJavaHandlerThreadCallback);
  java_thread_ = sElaJavaHandlerThreadCallback->elastos_JavaHandlerThread_create(Elastos::String(name));
}

JavaHandlerThread::~JavaHandlerThread() {
}

void JavaHandlerThread::Start() {
  // Check the thread has not already been started.
  DCHECK(!message_loop_);

  //LLJNIEnv* env = base::android::AttachCurrentThread();
  base::WaitableEvent initialize_event(false, false);
  //Java_JavaHandlerThread_start(env,
  //                             java_thread_.obj(),
  //                             reinterpret_cast<intptr_t>(this),
  //                             reinterpret_cast<intptr_t>(&initialize_event));
  DCHECK(sElaJavaHandlerThreadCallback);
  sElaJavaHandlerThreadCallback->elastos_JavaHandlerThread_start(java_thread_.Get(),
          reinterpret_cast<intptr_t>(this), reinterpret_cast<intptr_t>(&initialize_event));
  // Wait for thread to be initialized so it is ready to be used when Start
  // returns.
  base::ThreadRestrictions::ScopedAllowWait wait_allowed;
  initialize_event.Wait();
}

void JavaHandlerThread::Stop() {
}

void JavaHandlerThread::InitializeThread(IInterface* caller, Elastos::Int64 event) {
  // TYPE_JAVA to get the Android java style message loop.
  message_loop_.reset(new base::MessageLoop(base::MessageLoop::TYPE_JAVA));
  static_cast<MessageLoopForUI*>(message_loop_.get())->Start();
  reinterpret_cast<base::WaitableEvent*>(event)->Signal();
}

// static
bool JavaHandlerThread::RegisterBindings() {
  return RegisterNativesImpl();
}

} // namespace android
} // namespace base

//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/javaproxy/CIProcessObserverNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::App::EIID_IIProcessObserver;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIProcessObserverNative::TAG("CIProcessObserverNative");

CAR_INTERFACE_IMPL_2(CIProcessObserverNative, Object, IIProcessObserver, IBinder)

CAR_OBJECT_IMPL(CIProcessObserverNative)

CIProcessObserverNative::~CIProcessObserverNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIProcessObserverNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIProcessObserverNative::OnForegroundActivitiesChanged(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean foregroundActivities)
{
    // LOGGERD(TAG, "+ CIProcessObserverNative::OnForegroundActivitiesChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IProcessObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IProcessObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onForegroundActivitiesChanged", "(IIZ)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onForegroundActivitiesChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)pid, (jint)uid, (jboolean)foregroundActivities);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onForegroundActivitiesChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIProcessObserverNative::OnForegroundActivitiesChanged()");
    return NOERROR;
}

ECode CIProcessObserverNative::OnProcessStateChanged(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 importance)
{
    // LOGGERD(TAG, "+ CIProcessObserverNative::OnProcessStateChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IProcessObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IProcessObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onProcessStateChanged", "(III)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onProcessStateChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)pid, (jint)uid, (jint)importance);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onProcessStateChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIProcessObserverNative::OnProcessStateChanged()");
    return NOERROR;
}

ECode CIProcessObserverNative::OnProcessDied(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid)
{
    // LOGGERD(TAG, "+ CIProcessObserverNative::OnProcessDied()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IProcessObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IProcessObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onProcessDied", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onProcessDied %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)pid, (jint)uid);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onProcessDied %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIProcessObserverNative::OnProcessDied()");
    return NOERROR;
}

ECode CIProcessObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIProcessObserverNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, TAG, "FindClass: Object %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: toString %d", __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: toString %d", __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, "- CIProcessObserverNative::ToString()");
    return NOERROR;
}

}
}
}


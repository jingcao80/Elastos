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

#include "elastos/droid/javaproxy/CIUserSwitchObserverNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::App::EIID_IIUserSwitchObserver;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIUserSwitchObserverNative::TAG("CIUserSwitchObserverNative");

CAR_INTERFACE_IMPL_2(CIUserSwitchObserverNative, Object, IIUserSwitchObserver, IBinder)

CAR_OBJECT_IMPL(CIUserSwitchObserverNative)

CIUserSwitchObserverNative::~CIUserSwitchObserverNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIUserSwitchObserverNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIUserSwitchObserverNative::OnUserSwitching(
    /* [in] */ Int32 newUserId,
    /* [in] */ IIRemoteCallback* reply)
{
    // LOGGERD(TAG, "+ CIUserSwitchObserverNative::OnUserSwitching()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jreply = NULL;
    if (reply) {
        LOGGERE(TAG, "TODO: OnUserSwitching reply != NULL");
    }
    jclass c = env->FindClass("android/app/IUserSwitchObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IUserSwitchObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onUserSwitching", "(ILandroid/os/IRemoteCallback;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onUserSwitching %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, newUserId, jreply);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onUserSwitching %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jreply);

    // LOGGERD(TAG, "- CIUserSwitchObserverNative::OnUserSwitching()");
    return NOERROR;
}

ECode CIUserSwitchObserverNative::OnUserSwitchComplete(
    /* [in] */ Int32 newUserId)
{
    // LOGGERD(TAG, "+ CIUserSwitchObserverNative::OnUserSwitchComplete()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IUserSwitchObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IUserSwitchObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onUserSwitchComplete", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onUserSwitchComplete %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, newUserId);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onUserSwitchComplete %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIUserSwitchObserverNative::OnUserSwitchComplete()");
    return NOERROR;
}

ECode CIUserSwitchObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIUserSwitchObserverNative::ToString()");

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

    // LOGGERD(TAG, "- CIUserSwitchObserverNative::ToString()");
    return NOERROR;
}


}
}
}


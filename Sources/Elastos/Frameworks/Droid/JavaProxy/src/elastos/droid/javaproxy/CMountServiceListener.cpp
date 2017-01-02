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

#include "elastos/droid/javaproxy/CMountServiceListener.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Storage::EIID_IIMountServiceListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CMountServiceListener::TAG("CMountServiceListener");

CAR_INTERFACE_IMPL_2(CMountServiceListener, Object, IIMountServiceListener, IBinder)

CAR_OBJECT_IMPL(CMountServiceListener)

CMountServiceListener::~CMountServiceListener()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CMountServiceListener::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CMountServiceListener::OnUsbMassStorageConnectionChanged(
    /* [in] */ Boolean connected)
{
    // LOGGERD(TAG, "+ CMountServiceListener::OnUsbMassStorageConnectionChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/os/storage/MountServiceListener");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: MountServiceListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onUsbMassStorageConnectionChanged", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onUsbMassStorageConnectionChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, connected);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onUsbMassStorageConnectionChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CMountServiceListener::OnUsbMassStorageConnectionChanged()");
    return NOERROR;
}

ECode CMountServiceListener::OnStorageStateChanged(
    /* [in] */ const String& path,
    /* [in] */ const String& oldState,
    /* [in] */ const String& newState)
{
    // LOGGERD(TAG, "+ CMountServiceListener::OnUsbMassStorageConnectionChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jpath = Util::ToJavaString(env, path);
    jstring joldState = Util::ToJavaString(env, oldState);
    jstring jnewState = Util::ToJavaString(env, newState);

    jclass c = env->FindClass("android/os/storage/MountServiceListener");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: MountServiceListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onStorageStateChange", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onStorageStateChange %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jpath, joldState, jnewState);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onStorageStateChange %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpath);
    env->DeleteLocalRef(joldState);
    env->DeleteLocalRef(jnewState);

    // LOGGERD(TAG, "- CMountServiceListener::OnUsbMassStorageConnectionChanged()");
    return NOERROR;
}

ECode CMountServiceListener::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CMountServiceListener::ToString()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, "ToString", "FindClass: Object", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: toString", __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: toString", __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, "- CMountServiceListener::ToString()");
    return NOERROR;
}

}
}
}


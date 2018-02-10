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

#include "elastos/droid/javaproxy/CIRotationWatcherNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::View::EIID_IRotationWatcher;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIRotationWatcherNative::TAG("CIRotationWatcherNative");

CAR_INTERFACE_IMPL_2(CIRotationWatcherNative, Object, IRotationWatcher, IBinder)

CAR_OBJECT_IMPL(CIRotationWatcherNative)

CIRotationWatcherNative::~CIRotationWatcherNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIRotationWatcherNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIRotationWatcherNative::OnRotationChanged(
    /* [in] */ Int32 rotation)
{
    // LOGGERD(TAG, "+ CIRotationWatcherNative::OnRotationChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IRotationWatcher");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRotationWatcher %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onRotationChanged", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onRotationChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)rotation);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onRotationChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIRotationWatcherNative::OnRotationChanged()");
    return NOERROR;
}

ECode CIRotationWatcherNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIRotationWatcherNative::ToString()");

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

    // LOGGERD(TAG, "- CIRotationWatcherNative::ToString()");
    return NOERROR;
}


}
}
}


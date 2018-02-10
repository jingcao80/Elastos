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

#include "elastos/droid/javaproxy/CIStopUserCallbackNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::App::EIID_IStopUserCallback;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIStopUserCallbackNative::TAG("CIStopUserCallbackNative");

CAR_INTERFACE_IMPL_2(CIStopUserCallbackNative, Object, IStopUserCallback, IBinder)

CAR_OBJECT_IMPL(CIStopUserCallbackNative)

CIStopUserCallbackNative::~CIStopUserCallbackNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIStopUserCallbackNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIStopUserCallbackNative::UserStopped(
    /* [in] */ Int32 userId)
{
    // LOGGERD(TAG, "+ CIStopUserCallbackNative::UserStopped()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IStopUserCallback");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IStopUserCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "userStopped", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: userStopped %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)userId);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: userStopped %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIStopUserCallbackNative::UserStopped()");
    return NOERROR;
}

ECode CIStopUserCallbackNative::UserStopAborted(
    /* [in] */ Int32 userId)
{
    // LOGGERD(TAG, "+ CIStopUserCallbackNative::UserStopAborted()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IStopUserCallback");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IStopUserCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "userStopAborted", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: userStopAborted %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)userId);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: userStopAborted %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIStopUserCallbackNative::UserStopAborted()");
    return NOERROR;
}

ECode CIStopUserCallbackNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIStopUserCallbackNative::ToString()");
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

    // LOGGERD(TAG, "- CIStopUserCallbackNative::ToString()");
    return NOERROR;
}

}
}
}


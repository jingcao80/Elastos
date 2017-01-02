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

#include "elastos/droid/javaproxy/CIApplicationTokenNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::View::EIID_IApplicationToken;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIApplicationTokenNative::TAG("CIApplicationTokenNative");

CAR_INTERFACE_IMPL_2(CIApplicationTokenNative, Object, IApplicationToken, IBinder)

CAR_OBJECT_IMPL(CIApplicationTokenNative)

CIApplicationTokenNative::~CIApplicationTokenNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIApplicationTokenNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIApplicationTokenNative::WindowsDrawn()
{
    // LOGGERD(TAG, "+ CIApplicationTokenNative::WindowsDrawn()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IApplicationToken");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IApplicationToken %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "windowsDrawn", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: windowsDrawn %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: windowsDrawn %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIApplicationTokenNative::WindowsDrawn()");
    return NOERROR;
}

ECode CIApplicationTokenNative::WindowsVisible()
{
    // LOGGERD(TAG, "+ CIApplicationTokenNative::WindowsVisible()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IApplicationToken");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IApplicationToken %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "windowsVisible", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: windowsVisible %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: windowsVisible %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIApplicationTokenNative::WindowsVisible()");
    return NOERROR;
}

ECode CIApplicationTokenNative::WindowsGone()
{
    // LOGGERD(TAG, "+ CIApplicationTokenNative::WindowsGone()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IApplicationToken");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IApplicationToken %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "windowsGone", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: windowsGone %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: windowsGone %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIApplicationTokenNative::WindowsGone()");
    return NOERROR;
}

ECode CIApplicationTokenNative::KeyDispatchingTimedOut(
    /* [in] */ const String& reason,
    /* [out] */ Boolean* res)
{
    // LOGGERD(TAG, "+ CIApplicationTokenNative::KeyDispatchingTimedOut()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jreason = Util::ToJavaString(env, reason);

    jclass c = env->FindClass("android/view/IApplicationToken");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IApplicationToken %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "keyDispatchingTimedOut", "(Ljava/lang/String;)Z");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: keyDispatchingTimedOut %d", __LINE__);

    *res = env->CallBooleanMethod(mJInstance, m, jreason);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: keyDispatchingTimedOut %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIApplicationTokenNative::KeyDispatchingTimedOut()");
    return NOERROR;
}

ECode CIApplicationTokenNative::GetKeyDispatchingTimeout(
    /* [out] */ Int64* timeOut)
{
    // LOGGERD(TAG, "+ CIApplicationTokenNative::GetKeyDispatchingTimeout()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IApplicationToken");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IApplicationToken %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "getKeyDispatchingTimeout", "(V)J");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: getKeyDispatchingTimeout %d", __LINE__);

    *timeOut = env->CallLongMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: getKeyDispatchingTimeout %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIApplicationTokenNative::GetKeyDispatchingTimeout()");
    return NOERROR;
}

ECode CIApplicationTokenNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIApplicationTokenNative::ToString()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, "ToString", "FindClass: Object", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: toString"), __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: toString"), __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, String("- CIApplicationTokenNative::ToString()"));
    return NOERROR;
}

}
}
}


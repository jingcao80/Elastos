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

#include "elastos/droid/javaproxy/CITransientNotificationNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::App::EIID_IITransientNotification;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CITransientNotificationNative::TAG("CITransientNotificationNative");

CAR_INTERFACE_IMPL_2(CITransientNotificationNative, Object, IITransientNotification, IBinder)

CAR_OBJECT_IMPL(CITransientNotificationNative)

CITransientNotificationNative::~CITransientNotificationNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CITransientNotificationNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CITransientNotificationNative::Show()
{
    // LOGGERD(TAG, "+ CITransientNotificationNative::Show()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/ITransientNotification");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ITransientNotification %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "show", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: show %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: show %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CITransientNotificationNative::Show()");
    return NOERROR;
}

ECode CITransientNotificationNative::Hide()
{
    // LOGGERD(TAG, "+ CITransientNotificationNative::Hide()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/ITransientNotification");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ITransientNotification %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "hide", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: hide %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: hide %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CITransientNotificationNative::Hide()");
    return NOERROR;
}

ECode CITransientNotificationNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CITransientNotificationNative::ToString()");
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

    // LOGGERD(TAG, "- CITransientNotificationNative::ToString()");
    return NOERROR;
}

}
}
}


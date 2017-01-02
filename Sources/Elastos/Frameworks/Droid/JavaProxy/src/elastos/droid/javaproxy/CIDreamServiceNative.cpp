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

#include "elastos/droid/javaproxy/CIDreamServiceNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Service::Dreams::EIID_IIDreamService;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIDreamServiceNative::TAG("CIDreamServiceNative");

CAR_INTERFACE_IMPL_2(CIDreamServiceNative, Object, IIDreamService, IBinder)

CAR_OBJECT_IMPL(CIDreamServiceNative)

CIDreamServiceNative::~CIDreamServiceNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIDreamServiceNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIDreamServiceNative::Attach(
    /* [in] */ IBinder* windowToken,
    /* [in] */ Boolean canDoze)
{
    // LOGGERD(TAG, "+ CIDreamServiceNative::Attach()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jwindowToken = NULL;
    if (windowToken != NULL) {
        jclass c = env->FindClass("android/os/ElBinderProxy");
        Util::CheckErrorAndLog(env, TAG, "FindClass: ElBinderProxy", __LINE__);

        jmethodID m = env->GetMethodID(c, "<init>", "(J)V");
        Util::CheckErrorAndLog(env, TAG, "GetMethodID: ElBinderProxy", __LINE__);

        jwindowToken = env->NewObject(c, m, (jlong)windowToken);
        Util::CheckErrorAndLog(env, TAG, "NewObject: ElBinderProxy", __LINE__);
        windowToken->AddRef();

        env->DeleteLocalRef(c);
    }

    jclass c = env->FindClass("android/service/dreams/IDreamService");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IDreamService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "attach", "(Landroid/os/IBinder;Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: attach Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jwindowToken, canDoze);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: attach Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jwindowToken);

    // LOGGERD(TAG, "- CIDreamServiceNative::Attach()");
    return NOERROR;
}

ECode CIDreamServiceNative::Detach()
{
    // LOGGERD(TAG, "+ CIDreamServiceNative::Detach()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/service/dreams/IDreamService");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IDreamService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "detach", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: detach Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: detach Line: %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIDreamServiceNative::Detach()");
    return NOERROR;
}

ECode CIDreamServiceNative::WakeUp()
{
    // LOGGERD(TAG, "+ CIDreamServiceNative::WakeUp()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/service/dreams/IDreamService");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IDreamService %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "wakeUp", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: wakeUp Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: wakeUp Line: %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIDreamServiceNative::WakeUp()");
    return NOERROR;
}

ECode CIDreamServiceNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIDreamServiceNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIDreamServiceNative::ToString()");
    return NOERROR;
}

}
}
}


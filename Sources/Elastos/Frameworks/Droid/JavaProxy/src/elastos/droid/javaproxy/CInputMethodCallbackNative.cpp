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

#include "elastos/droid/javaproxy/CInputMethodCallbackNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::View::EIID_IInputMethodCallback;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CInputMethodCallbackNative::TAG("CInputMethodCallbackNative");

CAR_INTERFACE_IMPL_2(CInputMethodCallbackNative, Object, IInputMethodCallback, IBinder)

CAR_OBJECT_IMPL(CInputMethodCallbackNative)

CInputMethodCallbackNative::~CInputMethodCallbackNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CInputMethodCallbackNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CInputMethodCallbackNative::FinishedEvent(
    /* [in] */ Int32 seq,
    /* [in] */ Boolean handled)
{
    // LOGGERD(TAG, "+ CInputMethodCallbackNative::FinishedEvent()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethodCallback");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IInputMethodCallback", __LINE__);

    jmethodID m = env->GetMethodID(c, "finishedEvent", "(IZ)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: finishedEvent", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)seq, (jboolean)handled);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: finishedEvent", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CInputMethodCallbackNative::FinishedEvent()");
    return NOERROR;
}

ECode CInputMethodCallbackNative::SessionCreated(
    /* [in] */ IIInputMethodSession* session)
{
    LOGGERD(TAG, "CInputMethodCallbackNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CInputMethodCallbackNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CInputMethodCallbackNative::ToString()");

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

    // LOGGERD(TAG, "- CInputMethodCallbackNative::ToString()");
    return NOERROR;
}

}
}
}


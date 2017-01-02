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

#include "elastos/droid/javaproxy/CIObbActionListenerNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Storage::EIID_IIObbActionListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIObbActionListenerNative::TAG("CIObbActionListenerNative");

CAR_INTERFACE_IMPL_2(CIObbActionListenerNative, Object, IIObbActionListener, IBinder)

CAR_OBJECT_IMPL(CIObbActionListenerNative)

CIObbActionListenerNative::~CIObbActionListenerNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIObbActionListenerNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIObbActionListenerNative::OnObbResult(
    /* [in] */ const String& filename,
    /* [in] */ Int32 nonce,
    /* [in] */ Int32 status)
{
    // LOGGERD(TAG, "+ CIObbActionListenerNative::OnObbResult()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jfilename = Util::ToJavaString(env, filename);
    jclass c = env->FindClass("android/os/storage/IIObbActionListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IIObbActionListener", __LINE__);

    jmethodID m = env->GetMethodID(c, "onObbResult", "(Ljava/lang/String;II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onObbResult", __LINE__);

    env->CallVoidMethod(mJInstance, m, jfilename, nonce, status);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onObbResult", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIObbActionListenerNative::OnObbResult()");
    return NOERROR;
}

ECode CIObbActionListenerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIObbActionListenerNative::ToString()");

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

    // LOGGERD(TAG, "- CIObbActionListenerNative::ToString()");
    return NOERROR;
}

}
}
}


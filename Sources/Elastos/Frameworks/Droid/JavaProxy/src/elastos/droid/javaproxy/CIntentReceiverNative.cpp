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

#include "elastos/droid/javaproxy/CIntentReceiverNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::EIID_IIntentReceiver;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIntentReceiverNative::TAG("CIntentReceiverNative");

CAR_INTERFACE_IMPL_2(CIntentReceiverNative, Object, IIntentReceiver, IBinder)

CAR_OBJECT_IMPL(CIntentReceiverNative)

CIntentReceiverNative::~CIntentReceiverNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIntentReceiverNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIntentReceiverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIntentReceiverNative::ToString()");
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

    // LOGGERD(TAG, "- CIntentReceiverNative::ToString()");
    return NOERROR;
}

ECode CIntentReceiverNative::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    // LOGGERD(TAG, "+ CIntentReceiverNative::PerformReceive()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/content/IIntentReceiver");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIntentReceiver", __LINE__);

    jobject jintent = NULL;
    if (intent != NULL) {
        jintent = Util::ToJavaIntent(env, intent);
    }

    jstring jdata = Util::ToJavaString(env, data);

    jobject jextras = NULL;
    if (extras != NULL) {
        jextras = Util::ToJavaBundle(env, extras);
    }

    jmethodID m = env->GetMethodID(c, "performReceive", "(Landroid/content/Intent;ILjava/lang/String;Landroid/os/Bundle;ZZI)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: performReceive", __LINE__);

    env->CallVoidMethod(mJInstance, m, jintent, (jint)resultCode, jdata, jextras, (jboolean)ordered, (jboolean)sticky, (jint)sendingUser);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: performReceive", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jintent);
    env->DeleteLocalRef(jdata);
    env->DeleteLocalRef(jextras);

    // LOGGERD(TAG, "- CIntentReceiverNative::PerformReceive()");
    return NOERROR;
}

}
}
}


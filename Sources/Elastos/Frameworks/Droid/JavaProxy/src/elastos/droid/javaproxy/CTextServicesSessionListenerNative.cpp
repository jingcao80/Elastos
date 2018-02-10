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

#include "elastos/droid/javaproxy/CTextServicesSessionListenerNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::TextService::EIID_IITextServicesSessionListener;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CTextServicesSessionListenerNative::TAG("CTextServicesSessionListenerNative");

CAR_INTERFACE_IMPL_2(CTextServicesSessionListenerNative, Object, IITextServicesSessionListener, IBinder)

CAR_OBJECT_IMPL(CTextServicesSessionListenerNative)

CTextServicesSessionListenerNative::~CTextServicesSessionListenerNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CTextServicesSessionListenerNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    mJInstance = env->NewGlobalRef((jobject)jInstance);

    return NOERROR;
}

ECode CTextServicesSessionListenerNative::OnServiceConnected(
    /* [in] */ IISpellCheckerSession* spellCheckerSession)
{
    // LOGGERD(TAG, String("+ CTextServicesSessionListenerNative::OnServiceConnected()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jspellCheckerSession = NULL;
    if (spellCheckerSession != NULL) {
        LOGGERE(TAG, "OnServiceConnected() spellCheckerSession NOT NULL!");
    }

    jclass c = env->FindClass("com/android/internal/textservice/ITextServicesSessionListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ITextServicesSessionListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onServiceConnected", "(Lcom/android/internal/textservice/ISpellCheckerSession;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onServiceConnected %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jspellCheckerSession);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onServiceConnected %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jspellCheckerSession);

    // LOGGERD(TAG, "- CTextServicesSessionListenerNative::OnServiceConnected()");
    return NOERROR;
}

ECode CTextServicesSessionListenerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CTextServicesSessionListenerNative::ToString()");
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

    // LOGGERD(TAG, "- CTextServicesSessionListenerNative::ToString()");
    return NOERROR;
}

}
}
}


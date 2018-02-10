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

#include "elastos/droid/javaproxy/CIInputFilterNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::View::EIID_IIInputFilter;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIInputFilterNative::TAG("CIInputFilterNative");

CAR_INTERFACE_IMPL_2(CIInputFilterNative, Object, IIInputFilter, IBinder)

CAR_OBJECT_IMPL(CIInputFilterNative)

CIInputFilterNative::~CIInputFilterNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIInputFilterNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIInputFilterNative::Install(
    /* [in] */ IIInputFilterHost* host)
{
    LOGGERD(TAG, "CIInputFilterNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputFilterNative::Uninstall()
{
    LOGGERD(TAG, "CIInputFilterNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputFilterNative::FilterInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 policyFlags)
{
    LOGGERD(TAG, "CIInputFilterNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputFilterNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIInputFilterNative::ToString()");

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

    // LOGGERD(TAG, "- CIInputFilterNative::ToString()");
    return NOERROR;
}

}
}
}


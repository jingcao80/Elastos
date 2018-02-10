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

#include "elastos/droid/javaproxy/CICountryListenerNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Location::EIID_IICountryListener;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CICountryListenerNative::TAG("CICountryListenerNative");

CAR_INTERFACE_IMPL_2(CICountryListenerNative, Object, IICountryListener, IBinder)

CAR_OBJECT_IMPL(CICountryListenerNative)

CICountryListenerNative::~CICountryListenerNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CICountryListenerNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CICountryListenerNative::OnCountryDetected(
    /* [in] */ ICountry* country)
{
    // LOGGERD(TAG, "+ CICountryListenerNative::OnCountryDetected()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jcountry = NULL;
    if (country != NULL) {
        jcountry = Util::ToJavaCountry(env, country);
    }

    jclass c = env->FindClass("android/location/ICountryListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ICountryListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onCountryDetected", "(Landroid/location/Country;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onCountryDetected Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jcountry);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onCountryDetected Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jcountry);
    // LOGGERD(TAG, "- CICountryListenerNative::OnCountryDetected()");
    return NOERROR;
}

ECode CICountryListenerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CICountryListenerNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CICountryListenerNative::ToString()");
    return NOERROR;
}

}
}
}


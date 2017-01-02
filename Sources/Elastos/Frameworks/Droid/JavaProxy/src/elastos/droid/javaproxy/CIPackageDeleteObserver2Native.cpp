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

#include "elastos/droid/javaproxy/CIPackageDeleteObserver2Native.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Content::Pm::EIID_IIPackageDeleteObserver2;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIPackageDeleteObserver2Native::TAG("CIPackageDeleteObserver2Native");

CAR_INTERFACE_IMPL_2(CIPackageDeleteObserver2Native, Object, IIPackageDeleteObserver2, IBinder)

CAR_OBJECT_IMPL(CIPackageDeleteObserver2Native)

CIPackageDeleteObserver2Native::~CIPackageDeleteObserver2Native()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIPackageDeleteObserver2Native::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIPackageDeleteObserver2Native::OnUserActionRequired(
        /* [in] */ IIntent* intent)
{
    assert(0);
    return NOERROR;
}

ECode CIPackageDeleteObserver2Native::OnPackageDeleted(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 returnCode,
        /* [in] */ const String& msg)
{
    // LOGGERD(TAG, "+ CIPackageDeleteObserver2Native::OnPackageDeleted()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jpackageName = Util::ToJavaString(env, packageName);
    jstring jmsg = Util::ToJavaString(env, msg);

    jclass c = env->FindClass("android/content/pm/IPackageDeleteObserver2");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IPackageDeleteObserver2 %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onPackageDeleted", "(Ljava/lang/String;ILjava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onPackageDeleted %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jpackageName, (jint)returnCode, jmsg);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onPackageDeleted %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpackageName);
    env->DeleteLocalRef(jmsg);

    // LOGGERD(TAG, "- CIPackageDeleteObserver2Native::OnPackageDeleted()");
    return NOERROR;
}

ECode CIPackageDeleteObserver2Native::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIPackageDeleteObserver2Native::ToString()");
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

    // LOGGERD(TAG, "- CIPackageDeleteObserver2Native::ToString()");
    return NOERROR;
}

}
}
}

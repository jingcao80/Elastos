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

#include "elastos/droid/javaproxy/CPackageStatusObserverNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::EIID_IIPackageStatsObserver;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CPackageStatusObserverNative::TAG("CPackageStatusObserverNative");

CAR_INTERFACE_IMPL_2(CPackageStatusObserverNative, Object, IIPackageStatsObserver, IBinder)

CAR_OBJECT_IMPL(CPackageStatusObserverNative)

CPackageStatusObserverNative::~CPackageStatusObserverNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CPackageStatusObserverNative::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CPackageStatusObserverNative::OnGetStatsCompleted(
    /* [in] */ IPackageStats* pStats,
    /* [in] */ Boolean succeeded)
{
    // LOGGERD(TAG, "+ CPackageStatusObserverNative::OnGetStatsCompleted()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jpStats = NULL;
    if (pStats != NULL) {
        jpStats = Util::ToJavaPackageStats(env, pStats);
    }

    jclass c = env->FindClass("android/content/pm/IPackageStatsObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IPackageStatsObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onGetStatsCompleted", "(Landroid/content/pm/PackageStats;Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onGetStatsCompleted %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jpStats, (jboolean)succeeded);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onGetStatsCompleted %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpStats);

    // LOGGERD(TAG, "- CPackageStatusObserverNative::OnGetStatsCompleted()");
    return NOERROR;
}

ECode CPackageStatusObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CPackageStatusObserverNative::ToString()");
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

    // LOGGERD(TAG, "- CPackageStatusObserverNative::ToString()");
    return NOERROR;
}

}
}
}


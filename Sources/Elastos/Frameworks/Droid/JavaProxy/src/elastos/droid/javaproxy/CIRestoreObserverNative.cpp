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

#include "elastos/droid/javaproxy/CIRestoreObserverNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::Backup::EIID_IIRestoreObserver;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIRestoreObserverNative::TAG("CIRestoreObserverNative");

CAR_INTERFACE_IMPL_2(CIRestoreObserverNative, Object, IIRestoreObserver, IBinder)

CAR_OBJECT_IMPL(CIRestoreObserverNative)

CIRestoreObserverNative::~CIRestoreObserverNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIRestoreObserverNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIRestoreObserverNative::RestoreSetsAvailable(
    /* [in] */ ArrayOf<IRestoreSet*>* results)
{
    // LOGGERD(TAG, "+ CIRestoreObserverNative::RestoreSetsAvailable()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobjectArray jresults = NULL;
    if(results != NULL) {
        jclass rsKlass = env->FindClass("android/app/backup/RestoreSet");
        Util::CheckErrorAndLog(env, TAG, "FindClass: RestoreSet %d", __LINE__);

        Int32 count = results->GetLength();
        jresults = env->NewObjectArray((jsize)count, rsKlass, NULL);
        Util::CheckErrorAndLog(env, TAG, "NewObjectArray: RestoreSet %d", __LINE__);

        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IRestoreSet> result = (*results)[i];
            if (result != NULL) {
                jobject jresult = Util::ToJavaRestoreSet(env, result);

                env->SetObjectArrayElement(jresults, i, jresult);
                Util::CheckErrorAndLog(env, TAG, "SetObjectArrayElement: RestoreSet %d", __LINE__);
                env->DeleteLocalRef(jresult);
            }
        }

        env->DeleteLocalRef(rsKlass);
    }

    jclass c = env->FindClass("android/app/backup/IRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "restoreSetsAvailable", "([Landroid/app/backup/RestoreSet;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: restoreSetsAvailable Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jresults);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: restoreSetsAvailable Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jresults);
    // LOGGERD(TAG, "- CIRestoreObserverNative::RestoreSetsAvailable()");
    return NOERROR;
}

ECode CIRestoreObserverNative::RestoreStarting(
    /* [in] */ Int32 numPackages)
{
    // LOGGERD(TAG, "+ CIRestoreObserverNative::RestoreStarting()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/backup/IRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "restoreStarting", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: restoreStarting Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)numPackages);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: restoreStarting Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    // LOGGERD(TAG, "- CIRestoreObserverNative::RestoreStarting()");
    return NOERROR;
}

ECode CIRestoreObserverNative::OnUpdate(
    /* [in] */ Int32 nowBeingRestored,
    /* [in] */ const String& curentPackage)
{
    LOGGERD(TAG, "+ CIRestoreObserverNative::OnUpdate()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jcurentPackage = Util::ToJavaString(env, curentPackage);

    jclass c = env->FindClass("android/app/backup/IRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onUpdate", "(ILjava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onUpdate Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)nowBeingRestored, jcurentPackage);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onUpdate Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jcurentPackage);
    LOGGERD(TAG, "- CIRestoreObserverNative::OnUpdate()");
    return NOERROR;
}

ECode CIRestoreObserverNative::RestoreFinished(
    /* [in] */ Int32 error)
{
    // LOGGERD(TAG, "+ CIRestoreObserverNative::RestoreFinished()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/backup/IRestoreObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRestoreObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "restoreFinished", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: restoreFinished Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)error);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: restoreFinished Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    // LOGGERD(TAG, "- CIRestoreObserverNative::RestoreFinished()");
    return NOERROR;
}

ECode CIRestoreObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIRestoreObserverNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIRestoreObserverNative::ToString()");
    return NOERROR;
}

}
}
}


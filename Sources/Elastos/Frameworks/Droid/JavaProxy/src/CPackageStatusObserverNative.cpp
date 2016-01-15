
#include "CPackageStatusObserverNative.h"
#include "Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CPackageStatusObserverNative::TAG("CPackageStatusObserverNative");

CPackageStatusObserverNative::~CPackageStatusObserverNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CPackageStatusObserverNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CPackageStatusObserverNative::OnGetStatsCompleted(
    /* [in] */ IPackageStats* pStats,
    /* [in] */ Boolean succeeded)
{
    // LOGGERD(TAG, String("+ CPackageStatusObserverNative::OnGetStatsCompleted()"));

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

    // LOGGERD(TAG, String("- CPackageStatusObserverNative::OnGetStatsCompleted()"));
    return NOERROR;
}

ECode CPackageStatusObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CPackageStatusObserverNative::ToString()"));
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, "ToString", "FindClass: Object", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: toString"), __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: toString"), __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, String("- CPackageStatusObserverNative::ToString()"));
    return NOERROR;
}

}
}
}



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
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
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


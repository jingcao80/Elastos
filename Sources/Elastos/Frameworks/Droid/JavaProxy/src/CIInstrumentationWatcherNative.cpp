
#include "CIInstrumentationWatcherNative.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIInstrumentationWatcherNative::TAG("CIInstrumentationWatcherNative");

CIInstrumentationWatcherNative::~CIInstrumentationWatcherNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIInstrumentationWatcherNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIInstrumentationWatcherNative::InstrumentationStatus(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    LOGGERD(TAG, String("+ CIInstrumentationWatcherNative::InstrumentationStatus()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jname = NULL;
    if (name != NULL) {
        jname = Util::ToJavaComponentName(env, name);
    }

    jobject jresults = NULL;
    if (results != NULL) {
        jresults = Util::ToJavaBundle(env, results);
    }

    jclass c = env->FindClass("android/app/IInstrumentationWatcher");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInstrumentationWatcher %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "instrumentationStatus", "(Landroid/content/ComponentName;ILandroid/os/Bundle;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: instrumentationStatus %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jname, (jint)resultCode, jresults);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: instrumentationStatus %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jname);
    env->DeleteLocalRef(jresults);
    LOGGERD(TAG, String("- CIInstrumentationWatcherNative::InstrumentationStatus()"));
    return NOERROR;
}

ECode CIInstrumentationWatcherNative::InstrumentationFinished(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    LOGGERD(TAG, String("+ CIInstrumentationWatcherNative::InstrumentationFinished()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jname = NULL;
    if (name != NULL) {
        jname = Util::ToJavaComponentName(env, name);
    }

    jobject jresults = NULL;
    if (results != NULL) {
        jresults = Util::ToJavaBundle(env, results);
    }

    jclass c = env->FindClass("android/app/IInstrumentationWatcher");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInstrumentationWatcher %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "instrumentationFinished", "(Landroid/content/ComponentName;ILandroid/os/Bundle;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: instrumentationFinished %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jname, (jint)resultCode, jresults);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: instrumentationFinished %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jname);
    env->DeleteLocalRef(jresults);
    LOGGERD(TAG, String("- CIInstrumentationWatcherNative::InstrumentationFinished()"));
    return NOERROR;
}

ECode CIInstrumentationWatcherNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIIWallpaperServiceNative::ToString()"));

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

    // LOGGERD(TAG, String("- CIIWallpaperServiceNative::ToString()"));
    return NOERROR;
}

}
}
}


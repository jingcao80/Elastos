
#include "elastos/droid/javaproxy/CIProcessObserverNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIProcessObserverNative::TAG("CIProcessObserverNative");

CIProcessObserverNative::~CIProcessObserverNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIProcessObserverNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIProcessObserverNative::OnForegroundActivitiesChanged(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean foregroundActivities)
{
    LOGGERD(TAG, String("+ CIProcessObserverNative::OnForegroundActivitiesChanged()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IProcessObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IProcessObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onForegroundActivitiesChanged", "(IIZ)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onForegroundActivitiesChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)pid, (jint)uid, (jboolean)foregroundActivities);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onForegroundActivitiesChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CIProcessObserverNative::OnForegroundActivitiesChanged()"));
    return NOERROR;
}

ECode CIProcessObserverNative::OnImportanceChanged(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 importance)
{
    LOGGERD(TAG, String("+ CIProcessObserverNative::OnImportanceChanged()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IProcessObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IProcessObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onImportanceChanged", "(III)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onImportanceChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)pid, (jint)uid, (jint)importance);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onImportanceChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CIProcessObserverNative::OnImportanceChanged()"));
    return NOERROR;
}

ECode CIProcessObserverNative::OnProcessDied(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid)
{
    // LOGGERD(TAG, String("+ CIProcessObserverNative::OnProcessDied()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IProcessObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IProcessObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onProcessDied", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onProcessDied %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)pid, (jint)uid);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onProcessDied %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CIProcessObserverNative::OnProcessDied()"));
    return NOERROR;
}

ECode CIProcessObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIProcessObserverNative::ToString()"));

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

    // LOGGERD(TAG, String("- CIProcessObserverNative::ToString()"));
    return NOERROR;
}

}
}
}


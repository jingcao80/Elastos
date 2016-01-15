
#include "CIRotationWatcherNative.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIRotationWatcherNative::TAG("CIRotationWatcherNative");

CIRotationWatcherNative::~CIRotationWatcherNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIRotationWatcherNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIRotationWatcherNative::OnRotationChanged(
    /* [in] */ Int32 rotation)
{
    LOGGERD(TAG, String("+ CIRotationWatcherNative::OnRotationChanged()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IRotationWatcher");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IRotationWatcher %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onRotationChanged", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onRotationChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)rotation);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onRotationChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CIRotationWatcherNative::OnRotationChanged()"));
    return NOERROR;
}

ECode CIRotationWatcherNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIRotationWatcherNative::ToString()"));

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

    // LOGGERD(TAG, String("- CIRotationWatcherNative::ToString()"));
    return NOERROR;
}


}
}
}


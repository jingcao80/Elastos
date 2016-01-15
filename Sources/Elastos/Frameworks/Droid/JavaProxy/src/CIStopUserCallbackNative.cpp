
#include "CIStopUserCallbackNative.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIStopUserCallbackNative::TAG("CIStopUserCallbackNative");

CIStopUserCallbackNative::~CIStopUserCallbackNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIStopUserCallbackNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIStopUserCallbackNative::UserStopped(
    /* [in] */ Int32 userId)
{
    LOGGERD(TAG, String("+ CIStopUserCallbackNative::UserStopped()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IStopUserCallback");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IStopUserCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "userStopped", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: userStopped %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)userId);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: userStopped %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CIStopUserCallbackNative::UserStopped()"));
    return NOERROR;
}

ECode CIStopUserCallbackNative::UserStopAborted(
    /* [in] */ Int32 userId)
{
    LOGGERD(TAG, String("+ CIStopUserCallbackNative::UserStopAborted()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/app/IStopUserCallback");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IStopUserCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "userStopAborted", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: userStopAborted %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)userId);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: userStopAborted %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CIStopUserCallbackNative::UserStopAborted()"));
    return NOERROR;
}

ECode CIStopUserCallbackNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIStopUserCallbackNative::ToString()"));
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

    // LOGGERD(TAG, String("- CIStopUserCallbackNative::ToString()"));
    return NOERROR;
}

}
}
}



#include "CInputMethodClientNative.h"
#include "Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CInputMethodClientNative::TAG("CInputMethodClientNative");

CInputMethodClientNative::~CInputMethodClientNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CInputMethodClientNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CInputMethodClientNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CInputMethodClientNative::ToString()"));

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

    // LOGGERD(TAG, String("- CInputMethodClientNative::ToString()"));
    return NOERROR;
}

ECode CInputMethodClientNative::SetUsingInputMethod(
    /* [in] */ Boolean state)
{
    // LOGGERD(TAG, String("+ CInputMethodClientNative::SetUsingInputMethod()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethodClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IInputMethodClient", __LINE__);

    jmethodID m = env->GetMethodID(c, "setUsingInputMethod", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: SetUsingInputMethod"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)state);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: SetUsingInputMethod"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputMethodClientNative::SetUsingInputMethod()"));
    return NOERROR;
}

ECode CInputMethodClientNative::OnBindMethod(
    /* [in] */ IInputBindResult* res)
{
    // LOGGERD(TAG, String("+ CInputMethodClientNative::OnBindMethod()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethodClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IInputMethodClient", __LINE__);

    jmethodID m = env->GetMethodID(c, "onBindMethod", "(Lcom/android/internal/view/InputBindResult;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: OnBindMethod"), __LINE__);

    jobject jres = NULL;
    if (res != NULL) {
        jres = Util::ToJavaInputBindResult(env, res);
    }

    env->CallVoidMethod(mJInstance, m, jres);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: OnBindMethod"), __LINE__);

    env->DeleteLocalRef(c);
    if(jres){
        env->DeleteLocalRef(jres);
    }

    // LOGGERD(TAG, String("- CInputMethodClientNative::OnBindMethod()"));
    return NOERROR;
}

ECode CInputMethodClientNative::OnUnbindMethod(
    /* [in] */ Int32 sequence)
{
    // LOGGERD(TAG, String("+ CInputMethodClientNative::OnUnbindMethod()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethodClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IInputMethodClient", __LINE__);

    jmethodID m = env->GetMethodID(c, "onUnbindMethod", "(I)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: OnUnbindMethod"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)sequence);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: OnUnbindMethod"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputMethodClientNative::OnUnbindMethod()"));
    return NOERROR;
}

ECode CInputMethodClientNative::SetActive(
    /* [in] */ Boolean active)
{
    // LOGGERD(TAG, String("+ CInputMethodClientNative::SetActive()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethodClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IInputMethodClient", __LINE__);

    jmethodID m = env->GetMethodID(c, "setActive", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: SetActive"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jboolean)active);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: SetActive"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputMethodClientNative::SetActive()"));
    return NOERROR;
}

}
}
}


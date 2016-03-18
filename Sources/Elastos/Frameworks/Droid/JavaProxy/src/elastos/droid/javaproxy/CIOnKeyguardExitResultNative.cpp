
#include "elastos/droid/javaproxy/CIOnKeyguardExitResultNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::View::EIID_IOnKeyguardExitResult;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIOnKeyguardExitResultNative::TAG("CIOnKeyguardExitResultNative");

CAR_INTERFACE_IMPL_2(CIOnKeyguardExitResultNative, Object, IOnKeyguardExitResult, IBinder)

CAR_OBJECT_IMPL(CIOnKeyguardExitResultNative)

CIOnKeyguardExitResultNative::~CIOnKeyguardExitResultNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIOnKeyguardExitResultNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIOnKeyguardExitResultNative::OnKeyguardExitResult(
    /* [in] */ Boolean success)
{
     // LOGGERD(TAG, "+ CIOnKeyguardExitResultNative::OnKeyguardExitResult()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IOnKeyguardExitResult");
    Util::CheckErrorAndLog(env, "FindClass: IOnKeyguardExitResult", __LINE__);

    jmethodID m = env->GetMethodID(c, "onKeyguardExitResult", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onKeyguardExitResult", __LINE__);

    env->CallVoidMethod(mJInstance, m, success);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onKeyguardExitResult", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIOnKeyguardExitResultNative::OnKeyguardExitResult()");
    return NOERROR;
}

ECode CIOnKeyguardExitResultNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIOnKeyguardExitResultNative::ToString()");

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

    // LOGGERD(TAG, "- CIOnKeyguardExitResultNative::ToString()");
    return NOERROR;
}

}
}
}


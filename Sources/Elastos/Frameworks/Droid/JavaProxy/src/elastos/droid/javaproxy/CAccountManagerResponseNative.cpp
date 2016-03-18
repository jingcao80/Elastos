
#include "elastos/droid/javaproxy/CAccountManagerResponseNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Accounts::EIID_IAccountManagerResponse;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CAccountManagerResponseNative::TAG("CAccountManagerResponseNative");

CAR_INTERFACE_IMPL_2(CAccountManagerResponseNative, Object, IAccountManagerResponse, IBinder)

CAR_OBJECT_IMPL(CAccountManagerResponseNative)

CAccountManagerResponseNative::~CAccountManagerResponseNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CAccountManagerResponseNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CAccountManagerResponseNative::OnResult(
    /* [in] */ IBundle* value)
{
    // LOGGERD(TAG, "+ CAccountManagerResponseNative::OnResult()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/accounts/IAccountManagerResponse");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IAccountManagerResponse %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onResult", "(Landroid/os/Bundle;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onError %d", __LINE__);

    jobject jvalue = NULL;
    if (value != NULL) {
        jvalue = Util::ToJavaBundle(env, value);
    }

    env->CallVoidMethod(mJInstance, m, jvalue);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onError %d", __LINE__);

    env->DeleteLocalRef(c);

    if(jvalue){
        env->DeleteLocalRef(jvalue);
    }

    // LOGGERD(TAG, "- CAccountManagerResponseNative::OnResult()");
    return NOERROR;
}

ECode CAccountManagerResponseNative::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& errorMessage)
{
    // LOGGERD(TAG, "+ CAccountManagerResponseNative::OnError()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/accounts/IAccountManagerResponse");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IAccountManagerResponse %d", __LINE__);

    jobject jerrorMessage = Util::ToJavaString(env, errorMessage);

    jmethodID m = env->GetMethodID(c, "onError", "(ILjava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onError %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)errorCode, jerrorMessage);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onError %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jerrorMessage);

    // LOGGERD(TAG, "- CAccountManagerResponseNative::OnError()");
    return NOERROR;
}

ECode CAccountManagerResponseNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CAccountManagerResponseNative::ToString()");
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

    // LOGGERD(TAG, "- CAccountManagerResponseNative::ToString()");
    return NOERROR;
}

}
}
}


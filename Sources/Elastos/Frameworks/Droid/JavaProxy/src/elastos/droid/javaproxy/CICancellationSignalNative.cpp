
#include "elastos/droid/javaproxy/CICancellationSignalNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::JavaProxy::Util;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CICancellationSignalNative::TAG("CICancellationSignalNative");

CICancellationSignalNative::~CICancellationSignalNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CICancellationSignalNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CICancellationSignalNative::Cancel()
{
    LOGGERD(TAG, String("+ CICancellationSignalNative::Cancel()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/os/ICancellationSignal");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ICancellationSignal %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "cancel", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: cancel Line: %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: cancel Line: %d", __LINE__);

    env->DeleteLocalRef(c);
    LOGGERD(TAG, String("- CICancellationSignalNative::Cancel()"));
    return NOERROR;
}

ECode CICancellationSignalNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CICancellationSignalNative::ToString()"));

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

    // LOGGERD(TAG, String("- CICancellationSignalNative::ToString()"));
    return NOERROR;
}

}
}
}


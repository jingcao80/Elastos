
#include "CIMountShutdownObserverNative.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIMountShutdownObserverNative::TAG("CIMountShutdownObserverNative");

CIMountShutdownObserverNative::~CIMountShutdownObserverNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIMountShutdownObserverNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIMountShutdownObserverNative::OnShutDownComplete(
    /* [in] */ Int32 statusCode)
{
    LOGGERD(TAG, String("CIMountShutdownObserverNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIMountShutdownObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIMountShutdownObserverNative::ToString()"));

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

    // LOGGERD(TAG, String("- CIMountShutdownObserverNative::ToString()"));
    return NOERROR;
}

}
}
}


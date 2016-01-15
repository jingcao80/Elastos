
#include "CIApplicationTokenNative.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIApplicationTokenNative::TAG("CIApplicationTokenNative");

CIApplicationTokenNative::~CIApplicationTokenNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIApplicationTokenNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIApplicationTokenNative::WindowsDrawn()
{
    LOGGERD(TAG, String("CIApplicationTokenNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIApplicationTokenNative::WindowsVisible()
{
    LOGGERD(TAG, String("CIApplicationTokenNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIApplicationTokenNative::WindowsGone()
{
    LOGGERD(TAG, String("CIApplicationTokenNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIApplicationTokenNative::KeyDispatchingTimedOut(
    /* [out] */ Boolean* res)
{
    LOGGERD(TAG, String("CIApplicationTokenNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIApplicationTokenNative::GetKeyDispatchingTimeout(
    /* [out] */ Int64* timeOut)
{
    LOGGERD(TAG, String("CIApplicationTokenNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIApplicationTokenNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIApplicationTokenNative::ToString()"));
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

    // LOGGERD(TAG, String("- CIApplicationTokenNative::ToString()"));
    return NOERROR;
}

}
}
}


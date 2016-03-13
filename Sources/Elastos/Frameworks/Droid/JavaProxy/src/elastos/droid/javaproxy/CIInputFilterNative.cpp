
#include "elastos/droid/javaproxy/CIInputFilterNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIInputFilterNative::TAG("CIInputFilterNative");

CIInputFilterNative::~CIInputFilterNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIInputFilterNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIInputFilterNative::Install(
    /* [in] */ IInputFilterHost* host)
{
    LOGGERD(TAG, String("CIInputFilterNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputFilterNative::Uninstall()
{
    LOGGERD(TAG, String("CIInputFilterNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputFilterNative::FilterInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 policyFlags)
{
    LOGGERD(TAG, String("CIInputFilterNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    return E_NOT_IMPLEMENTED;
}

ECode CIInputFilterNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIInputFilterNative::ToString()"));

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

    // LOGGERD(TAG, String("- CIInputFilterNative::ToString()"));
    return NOERROR;
}

}
}
}


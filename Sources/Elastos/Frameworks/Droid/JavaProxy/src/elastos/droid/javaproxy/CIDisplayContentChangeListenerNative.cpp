
#include "elastos/droid/javaproxy/CIDisplayContentChangeListenerNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIDisplayContentChangeListenerNative::TAG("CIDisplayContentChangeListenerNative");

CIDisplayContentChangeListenerNative::~CIDisplayContentChangeListenerNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIDisplayContentChangeListenerNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIDisplayContentChangeListenerNative::OnWindowTransition(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 transition,
    /* [in] */ IWindowInfo* info)
{
    LOGGERD(TAG, String("CIDisplayContentChangeListenerNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    return E_NOT_IMPLEMENTED;
}

ECode CIDisplayContentChangeListenerNative::OnRectangleOnScreenRequested(
    /* [in] */ Int32 displayId,
    /* [in] */ IRect* rectangle,
    /* [in] */ Boolean immediate)
{
    LOGGERD(TAG, String("CIDisplayContentChangeListenerNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    return E_NOT_IMPLEMENTED;
}

ECode CIDisplayContentChangeListenerNative::OnWindowLayersChanged(
    /* [in] */ Int32 displayId)
{
    LOGGERD(TAG, String("CIDisplayContentChangeListenerNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    return E_NOT_IMPLEMENTED;
}

ECode CIDisplayContentChangeListenerNative::OnRotationChanged(
    /* [in] */ Int32 rotation)
{
    LOGGERD(TAG, String("CIDisplayContentChangeListenerNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    return E_NOT_IMPLEMENTED;
}

ECode CIDisplayContentChangeListenerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIDisplayContentChangeListenerNative::ToString()"));
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

    // LOGGERD(TAG, String("- CIDisplayContentChangeListenerNative::ToString()"));
    return NOERROR;
}

}
}
}


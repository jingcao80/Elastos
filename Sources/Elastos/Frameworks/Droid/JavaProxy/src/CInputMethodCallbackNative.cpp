
#include "CInputMethodCallbackNative.h"
#include "Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CInputMethodCallbackNative::TAG("CInputMethodCallbackNative");

CInputMethodCallbackNative::~CInputMethodCallbackNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CInputMethodCallbackNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CInputMethodCallbackNative::FinishedEvent(
    /* [in] */ Int32 seq,
    /* [in] */ Boolean handled)
{
    // LOGGERD(TAG, String("+ CInputMethodCallbackNative::FinishedEvent()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("com/android/internal/view/IInputMethodCallback");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IInputMethodCallback", __LINE__);

    jmethodID m = env->GetMethodID(c, "finishedEvent", "(IZ)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: finishedEvent"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)seq, (jboolean)handled);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: finishedEvent"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CInputMethodCallbackNative::FinishedEvent()"));
    return NOERROR;
}

ECode CInputMethodCallbackNative::SessionCreated(
    /* [in] */ IIInputMethodSession* session)
{
    LOGGERD(TAG, String("CInputMethodCallbackNative E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

}
}
}


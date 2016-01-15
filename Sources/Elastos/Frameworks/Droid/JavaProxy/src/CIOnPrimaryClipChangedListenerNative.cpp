
#include "CIOnPrimaryClipChangedListenerNative.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIOnPrimaryClipChangedListenerNative::TAG("CIOnPrimaryClipChangedListenerNative");

CIOnPrimaryClipChangedListenerNative::~CIOnPrimaryClipChangedListenerNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIOnPrimaryClipChangedListenerNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIOnPrimaryClipChangedListenerNative::DispatchPrimaryClipChanged()
{
    // LOGGERD(TAG, String("+ CIOnPrimaryClipChangedListenerNative::DispatchPrimaryClipChanged()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/content/IOnPrimaryClipChangedListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IOnPrimaryClipChangedListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchPrimaryClipChanged", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchPrimaryClipChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchPrimaryClipChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CIOnPrimaryClipChangedListenerNative::DispatchPrimaryClipChanged()"));
    return NOERROR;
}

}
}
}



#include "CDisplayManagerCallbackNative.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Droid::JavaProxy::Util;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CDisplayManagerCallbackNative::TAG("CDisplayManagerCallbackNative");

CDisplayManagerCallbackNative::~CDisplayManagerCallbackNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CDisplayManagerCallbackNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CDisplayManagerCallbackNative::OnDisplayEvent(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 event)
{
    // LOGGERD(TAG, String("+ CDisplayManagerCallbackNative::OnDisplayEvent()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/hardware/display/IDisplayManagerCallback");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IDisplayManagerCallback", __LINE__);

    jmethodID m = env->GetMethodID(c, "onDisplayEvent", "(II)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: onDisplayEvent"), __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)displayId, (jint)event);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: onDisplayEvent"), __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CDisplayManagerCallbackNative::OnDisplayEvent()"));
    return NOERROR;
}

} // namespace JavaProxy
} // namepsace Droid
} // namespace Elastos
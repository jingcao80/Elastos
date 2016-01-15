
#include "CInputDevicesChangedListenerNative.h"
#include "Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CInputDevicesChangedListenerNative::TAG("CInputDevicesChangedListenerNative");

CInputDevicesChangedListenerNative::~CInputDevicesChangedListenerNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CInputDevicesChangedListenerNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CInputDevicesChangedListenerNative::OnInputDevicesChanged(
    /* [in] */ ArrayOf<Int32>* deviceIdAndGeneration)
{
    // LOGGERD(TAG, String("+ CInputDevicesChangedListenerNative::OnInputDevicesChanged()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/hardware/input/IInputDevicesChangedListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IInputDevicesChangedListener", __LINE__);

    jmethodID m = env->GetMethodID(c, "onInputDevicesChanged", "([I)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: onInputDevicesChanged"), __LINE__);

    jintArray jdeviceIdAndGeneration = NULL;
    if (deviceIdAndGeneration != NULL) {
        jdeviceIdAndGeneration = Util::ToJavaIntArray(env, deviceIdAndGeneration);
    }

    env->CallVoidMethod(mJInstance, m, jdeviceIdAndGeneration);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: onInputDevicesChanged"), __LINE__);

    env->DeleteLocalRef(c);
    if(jdeviceIdAndGeneration){
        env->DeleteLocalRef(jdeviceIdAndGeneration);
    }

    // LOGGERD(TAG, String("- CInputDevicesChangedListenerNative::OnInputDevicesChanged()"));
    return NOERROR;
}

}
}
}


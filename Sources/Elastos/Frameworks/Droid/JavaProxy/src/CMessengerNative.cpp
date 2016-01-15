
#include "CMessengerNative.h"
#include "Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CMessengerNative::TAG("CMessengerNative");

CMessengerNative::~CMessengerNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CMessengerNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CMessengerNative::Send(
    /* [in] */ IMessage* message)
{
    // LOGGERD(TAG, String("+ CMessengerNative::Send()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/os/IMessenger");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Messenger", __LINE__);

    jmethodID m = env->GetMethodID(c, "send", "(Landroid/os/Message;)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: send"), __LINE__);

    jobject jmessage = NULL;
    if (message != NULL) {
        jmessage = Util::ToJavaMessage(env, message);
    }

    env->CallVoidMethod(mJInstance, m, jmessage);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: send"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jmessage);

    // LOGGERD(TAG, String("- CMessengerNative::Send()"));
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

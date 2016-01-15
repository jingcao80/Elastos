
#include "CMountServiceListener.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CMountServiceListener::TAG("CMountServiceListener");

CMountServiceListener::~CMountServiceListener()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CMountServiceListener::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CMountServiceListener::OnUsbMassStorageConnectionChanged(
    /* [in] */ Boolean connected)
{
    // LOGGERD(TAG, String("+ CMountServiceListener::OnUsbMassStorageConnectionChanged()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/os/storage/MountServiceListener");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: MountServiceListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onUsbMassStorageConnectionChanged", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onUsbMassStorageConnectionChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, connected);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onUsbMassStorageConnectionChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, String("- CMountServiceListener::OnUsbMassStorageConnectionChanged()"));
    return NOERROR;
}

ECode CMountServiceListener::OnStorageStateChanged(
    /* [in] */ const String& path,
    /* [in] */ const String& oldState,
    /* [in] */ const String& newState)
{
    // LOGGERD(TAG, String("+ CMountServiceListener::OnUsbMassStorageConnectionChanged()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jpath = Util::ToJavaString(env, path);
    jstring joldState = Util::ToJavaString(env, oldState);
    jstring jnewState = Util::ToJavaString(env, newState);

    jclass c = env->FindClass("android/os/storage/MountServiceListener");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: MountServiceListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onStorageStateChange", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onStorageStateChange %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jpath, joldState, jnewState);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onStorageStateChange %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpath);
    env->DeleteLocalRef(joldState);
    env->DeleteLocalRef(jnewState);

    // LOGGERD(TAG, String("- CMountServiceListener::OnUsbMassStorageConnectionChanged()"));
    return NOERROR;
}

}
}
}


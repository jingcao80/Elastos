
#include "CIAudioRoutesObserver.h"
#include "Util.h"

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIAudioRoutesObserver::TAG("CIAudioRoutesObserver");

CIAudioRoutesObserver::~CIAudioRoutesObserver()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIAudioRoutesObserver::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIAudioRoutesObserver::DispatchAudioRoutesChanged(
    /* [in] */ IAudioRoutesInfo* newRoutes)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jnewRoutes = Util::ToJavaAudioRoutesInfo(env, newRoutes);

    jclass c = env->FindClass("android/media/IAudioRoutesObserver");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteVolumeObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchAudioRoutesChanged", "(Landroid/media/AudioRoutesInfo;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchAudioRoutesChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jnewRoutes);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchAudioRoutesChanged %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jnewRoutes);
    return NOERROR;
}

ECode CIAudioRoutesObserver::ToString(
    /* [out] */ String* str)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);
    return NOERROR;
}

}
}
}


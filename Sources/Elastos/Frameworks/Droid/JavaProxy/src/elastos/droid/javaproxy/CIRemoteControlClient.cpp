
#include "elastos/droid/javaproxy/CIRemoteControlClient.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIRemoteControlClient::TAG("CIRemoteControlClient");

CIRemoteControlClient::~CIRemoteControlClient()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIRemoteControlClient::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIRemoteControlClient::OnInformationRequested(
    /* [in] */ Int32 generationId,
    /* [in] */ Int32 infoFlags,
    /* [in] */ Int32 artWidth,
    /* [in] */ Int32 artHeight)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlClient %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onInformationRequested", "(IIII)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onInformationRequested %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, generationId, infoFlags, artWidth, artHeight);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onInformationRequested %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteControlClient::SetCurrentClientGenerationId(
    /* [in] */ Int32 clientGeneration)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlClient %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setCurrentClientGenerationId", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setCurrentClientGenerationId %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, clientGeneration);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setCurrentClientGenerationId %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteControlClient::PlugRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd)
{
    AutoPtr<IJavaNativeProxy> crcd = IJavaNativeProxy::Probe(rcd);

    if(crcd == NULL) {
        Logger::W(TAG, "CIRemoteControlClient::PlugRemoteControlDisplay(), unknown param type!");
        return NOERROR;
    }

    Handle32 jrcd;
    crcd->GetJavaInstance(&jrcd);

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlClient %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "plugRemoteControlDisplay", "(Landroid/media/IRemoteControlDisplay;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: plugRemoteControlDisplay %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jobject)jrcd);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: plugRemoteControlDisplay %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteControlClient::UnplugRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd)
{
    AutoPtr<IJavaNativeProxy> crcd = IJavaNativeProxy::Probe(rcd);

    if(crcd == NULL) {
        Logger::W(TAG, "CIRemoteControlClient::UnplugRemoteControlDisplay(), unknown param type!");
        return NOERROR;
    }

    Handle32 jrcd;
    crcd->GetJavaInstance(&jrcd);

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlClient %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "unplugRemoteControlDisplay", "(Landroid/media/IRemoteControlDisplay;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: unplugRemoteControlDisplay %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jobject)jrcd);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: unplugRemoteControlDisplay %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteControlClient::ToString(
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


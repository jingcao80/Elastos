
#include "elastos/droid/javaproxy/CIRemoteControlClient.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Media::EIID_IIRemoteControlClient;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIRemoteControlClient::TAG("CIRemoteControlClient");

CAR_INTERFACE_IMPL_2(CIRemoteControlClient, Object, IIRemoteControlClient, IBinder)

CAR_OBJECT_IMPL(CIRemoteControlClient)

CIRemoteControlClient::~CIRemoteControlClient()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIRemoteControlClient::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIRemoteControlClient::OnInformationRequested(
    /* [in] */ Int32 generationId,
    /* [in] */ Int32 infoFlags)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlClient %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onInformationRequested", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onInformationRequested %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, generationId, infoFlags);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onInformationRequested %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteControlClient::InformationRequestForDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    AutoPtr<IJavaNativeProxy> crcd = IJavaNativeProxy::Probe(rcd);
    if(crcd == NULL) {
        Logger::W(TAG, "CIRemoteControlClient::InformationRequestForDisplay(), unknown param type!");
        return NOERROR;
    }

    Handle64 jrcd;
    crcd->GetJavaInstance(&jrcd);

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlClient %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "informationRequestForDisplay", "(Landroid/media/IRemoteControlDisplay;II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: informationRequestForDisplay %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jobject)jrcd, w, h);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: informationRequestForDisplay %d", __LINE__);

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
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    AutoPtr<IJavaNativeProxy> crcd = IJavaNativeProxy::Probe(rcd);

    if(crcd == NULL) {
        Logger::W(TAG, "CIRemoteControlClient::PlugRemoteControlDisplay(), unknown param type!");
        return NOERROR;
    }

    Handle64 jrcd;
    crcd->GetJavaInstance(&jrcd);

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlClient %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "plugRemoteControlDisplay", "(Landroid/media/IRemoteControlDisplay;II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: plugRemoteControlDisplay %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jobject)jrcd, w, h);
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

    Handle64 jrcd;
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

ECode CIRemoteControlClient::SetBitmapSizeForDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    AutoPtr<IJavaNativeProxy> crcd = IJavaNativeProxy::Probe(rcd);

    if(crcd == NULL) {
        Logger::W(TAG, "CIRemoteControlClient::SetBitmapSizeForDisplay(), unknown param type!");
        return NOERROR;
    }

    Handle64 jrcd;
    crcd->GetJavaInstance(&jrcd);

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlClient %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setBitmapSizeForDisplay", "(Landroid/media/IRemoteControlDisplay;II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setBitmapSizeForDisplay %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jobject)jrcd, w, h);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setBitmapSizeForDisplay %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteControlClient::SetWantsSyncForDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Boolean wantsSync)
{
    AutoPtr<IJavaNativeProxy> crcd = IJavaNativeProxy::Probe(rcd);

    if(crcd == NULL) {
        Logger::W(TAG, "CIRemoteControlClient::SetWantsSyncForDisplay(), unknown param type!");
        return NOERROR;
    }

    Handle64 jrcd;
    crcd->GetJavaInstance(&jrcd);

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlClient %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setWantsSyncForDisplay", "(Landroid/media/IRemoteControlDisplay;Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setWantsSyncForDisplay %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jobject)jrcd, wantsSync);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setWantsSyncForDisplay %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteControlClient::EnableRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Boolean enabled)
{
    AutoPtr<IJavaNativeProxy> crcd = IJavaNativeProxy::Probe(rcd);

    if(crcd == NULL) {
        Logger::W(TAG, "CIRemoteControlClient::EnableRemoteControlDisplay(), unknown param type!");
        return NOERROR;
    }

    Handle64 jrcd;
    crcd->GetJavaInstance(&jrcd);

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlClient %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "enableRemoteControlDisplay", "(Landroid/media/IRemoteControlDisplay;Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: enableRemoteControlDisplay %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jobject)jrcd, enabled);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: enableRemoteControlDisplay %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteControlClient::SeekTo(
    /* [in] */ Int32 clientGeneration,
    /* [in] */ Int64 timeMs)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlClient %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setCurrentClientGenerationId", "(IJ)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setCurrentClientGenerationId %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, clientGeneration, timeMs);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setCurrentClientGenerationId %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteControlClient::UpdateMetadata(
    /* [in] */ Int32 clientGeneration,
    /* [in] */ Int32 key,
    /* [in] */ IRating* value)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jvalue = NULL;
    if (value != NULL) {
        jvalue = Util::ToJavaRating(env, value);
    }
    jclass c = env->FindClass("android/media/IRemoteControlClient");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlClient %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setCurrentClientGenerationId", "(IILandroid/media/Rating;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setCurrentClientGenerationId %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, clientGeneration, key, jvalue);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setCurrentClientGenerationId %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jvalue);
    return NOERROR;
}

ECode CIRemoteControlClient::SetPlayItem(
    /* [in] */ Int32 scope,
    /* [in] */ Int64 uid)
{
    assert(0);
    return NOERROR;
}

ECode CIRemoteControlClient::SetBrowsedPlayer()
{
    assert(0);
    return NOERROR;
}

ECode CIRemoteControlClient::GetNowPlayingEntries()
{
    assert(0);
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


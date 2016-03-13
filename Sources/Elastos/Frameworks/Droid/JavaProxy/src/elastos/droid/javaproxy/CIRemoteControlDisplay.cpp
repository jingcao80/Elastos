
#include "elastos/droid/javaproxy/CIRemoteControlDisplay.h"
#include "elastos/droid/javaproxy/Util.h"

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIRemoteControlDisplay::TAG("CIRemoteControlDisplay");

CIRemoteControlDisplay::~CIRemoteControlDisplay()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIRemoteControlDisplay::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIRemoteControlDisplay::GetJavaInstance(Handle32* obj)
{
    *obj = (Handle32)mJInstance;
    return NOERROR;
}

ECode CIRemoteControlDisplay::SetCurrentClientId(
    /* [in] */ Int32 clientGeneration,
    /* [in] */ IPendingIntent* clientMediaIntent,
    /* [in] */ Boolean clearing)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jclientMediaIntent =  Util::ToJavaPendingIntent(env, clientMediaIntent);

    jclass c = env->FindClass("android/media/IRemoteControlDisplay");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlDisplay %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setCurrentClientId", "(ILandroid/app/PendingIntent;Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setCurrentClientId %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, clientGeneration, jclientMediaIntent, clearing);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setCurrentClientId %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jclientMediaIntent);
    return NOERROR;
}

ECode CIRemoteControlDisplay::SetPlaybackState(
    /* [in] */ Int32 generationId,
    /* [in] */ Int32 state,
    /* [in] */ Int64 stateChangeTimeMs)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlDisplay");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlDisplay %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setPlaybackState", "(IIJ)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setPlaybackState %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, generationId, state, stateChangeTimeMs);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setPlaybackState %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteControlDisplay::SetTransportControlFlags(
    /* [in] */ Int32 generationId,
    /* [in] */ Int32 transportControlFlags)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlDisplay");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlDisplay %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setTransportControlFlags", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setTransportControlFlags %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, generationId, transportControlFlags);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setTransportControlFlags %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteControlDisplay::SetMetadata(
    /* [in] */ Int32 generationId,
    /* [in] */ IBundle* metadata)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jmetadata = Util::ToJavaBundle(env, metadata);

    jclass c = env->FindClass("android/media/IRemoteControlDisplay");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlDisplay %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setMetadata", "(ILandroid/os/Bundle;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setMetadata %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, generationId, jmetadata);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setMetadata %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jmetadata);
    return NOERROR;
}

ECode CIRemoteControlDisplay::SetArtwork(
    /* [in] */ Int32 generationId,
    /* [in] */ IBitmap* artwork)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jartwork = Util::ToJavaBitmap(env, artwork);

    jclass c = env->FindClass("android/media/IRemoteControlDisplay");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlDisplay %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setArtwork", "(ILandroid/graphics/Bitmap;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setArtwork %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, generationId, jartwork);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setArtwork %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jartwork);
    return NOERROR;
}

ECode CIRemoteControlDisplay::SetAllMetadata(
    /* [in] */ Int32 generationId,
    /* [in] */ IBundle* metadata,
    /* [in] */ IBitmap* artwork)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jmetadata = Util::ToJavaBundle(env, metadata);
    jobject jartwork = Util::ToJavaBitmap(env, artwork);

    jclass c = env->FindClass("android/media/IRemoteControlDisplay");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlDisplay %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setAllMetadata", "(ILandroid/os/Bundle;Landroid/graphics/Bitmap;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setAllMetadata %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, generationId, jmetadata, jartwork);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setAllMetadata %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jmetadata);
    env->DeleteLocalRef(jartwork);
    return NOERROR;
}

ECode CIRemoteControlDisplay::ToString(
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


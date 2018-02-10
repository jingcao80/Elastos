//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/javaproxy/CIRemoteControlDisplay.h"
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Media::EIID_IIRemoteControlDisplay;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIRemoteControlDisplay::TAG("CIRemoteControlDisplay");

CAR_INTERFACE_IMPL_3(CIRemoteControlDisplay, Object, IIRemoteControlDisplay, IJavaNativeProxy, IBinder)

CAR_OBJECT_IMPL(CIRemoteControlDisplay)

CIRemoteControlDisplay::~CIRemoteControlDisplay()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIRemoteControlDisplay::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIRemoteControlDisplay::GetJavaInstance(
    /* [out] */ HANDLE* obj)
{
    *obj = (HANDLE)mJInstance;
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

ECode CIRemoteControlDisplay::SetEnabled(
        /* [in] */ Boolean enabled)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlDisplay");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlDisplay %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setEnabled", "(Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setEnabled %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, enabled);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setEnabled %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteControlDisplay::SetPlaybackState(
    /* [in] */ Int32 generationId,
    /* [in] */ Int32 state,
    /* [in] */ Int64 stateChangeTimeMs,
    /* [in] */ Int64 currentPosMs,
    /* [in] */ Float speed)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlDisplay");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlDisplay %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setPlaybackState", "(IIJJF)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setPlaybackState %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, generationId, state, stateChangeTimeMs, currentPosMs, speed);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setPlaybackState %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRemoteControlDisplay::SetTransportControlInfo(
    /* [in] */ Int32 generationId,
    /* [in] */ Int32 transportControlFlags,
    /* [in] */ Int32 posCapabilities)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRemoteControlDisplay");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRemoteControlDisplay %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "setTransportControlInfo", "(III)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: setTransportControlInfo %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, generationId, transportControlFlags, posCapabilities);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: setTransportControlInfo %d", __LINE__);

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


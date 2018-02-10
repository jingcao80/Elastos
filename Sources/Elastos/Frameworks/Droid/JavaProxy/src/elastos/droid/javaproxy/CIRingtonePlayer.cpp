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

#include "elastos/droid/javaproxy/CIRingtonePlayer.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Media::EIID_IIRingtonePlayer;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIRingtonePlayer::TAG("CIRingtonePlayer");

CAR_INTERFACE_IMPL_2(CIRingtonePlayer, Object, IIRingtonePlayer, IBinder)

CAR_OBJECT_IMPL(CIRingtonePlayer)

CIRingtonePlayer::~CIRingtonePlayer()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIRingtonePlayer::constructor(
    /* [in] */ HANDLE jVM,
    /* [in] */ HANDLE jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIRingtonePlayer::Play(
    /* [in] */ IBinder* token,
    /* [in] */ IUri* uri,
    /* [in] */ IAudioAttributes* aa)
{
    LOGGERD(TAG, String("CIRingtonePlayer E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIRingtonePlayer::Stop(
    /* [in] */ IBinder* token)
{
    LOGGERD(TAG, String("CIRingtonePlayer E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIRingtonePlayer::IsPlaying(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    LOGGERD(TAG, String("CIRingtonePlayer E_NOT_IMPLEMENTED Line:%d"), __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIRingtonePlayer::SetVolume(
    /* [in] */ IBinder* token,
    /* [in] */ Float volume)
{
    assert(0);
    return NOERROR;
}

ECode CIRingtonePlayer::PlayAsync(
    /* [in] */ IUri* uri,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean looping,
    /* [in] */ IAudioAttributes* aa)
{
    //Util::ToJavaUserHandle
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject juri = Util::ToJavaUri(env, uri);
    jobject juser = Util::ToJavaUserHandle(env, user);
    jobject jaa = Util::ToJavaAudioAttributes(env, aa);

    jclass c = env->FindClass("android/media/IRingtonePlayer");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRingtonePlayer %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "playAsync", "(Landroid/net/Uri;Landroid/os/UserHandle;ZLandroid/media/AudioAttributes)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: PlayAsync %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, juri, juser, looping, jaa);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: PlayAsync %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(juri);
    env->DeleteLocalRef(juser);
    env->DeleteLocalRef(jaa);
    return NOERROR;
}

ECode CIRingtonePlayer::StopAsync()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/media/IRingtonePlayer");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRingtonePlayer %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "stopAsync", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: stopAsync %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: stopAsync %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CIRingtonePlayer::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIRingtonePlayer::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, TAG, "FindClass: Object", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: toString", __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: toString", __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, "- CIRingtonePlayer::ToString()");
    return NOERROR;
}

}
}
}


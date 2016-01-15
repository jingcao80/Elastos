
#include "CIRingtonePlayer.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIRingtonePlayer::TAG("CIRingtonePlayer");

CIRingtonePlayer::~CIRingtonePlayer()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIRingtonePlayer::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIRingtonePlayer::Play(
    /* [in] */ IBinder* token,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 streamType)
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

ECode CIRingtonePlayer::PlayAsync(
    /* [in] */ IUri* uri,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean looping,
    /* [in] */ Int32 streamType)
{
    //Util::ToJavaUserHandle
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject juri = Util::ToJavaUri(env, uri);
    jobject juser = Util::ToJavaUserHandle(env, user);

    jclass c = env->FindClass("android/media/IRingtonePlayer");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IRingtonePlayer %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "playAsync", "(Landroid/net/Uri;Landroid/os/UserHandle;ZI)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: PlayAsync %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, juri, juser, looping, streamType);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: PlayAsync %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(juri);
    env->DeleteLocalRef(juser);
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
    // LOGGERD(TAG, String("+ CIRingtonePlayer::ToString()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, "ToString", "FindClass: Object", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: toString"), __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: toString"), __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, String("- CIRingtonePlayer::ToString()"));
    return NOERROR;
}

}
}
}


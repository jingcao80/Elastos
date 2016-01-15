
#include "CIAudioFocusDispatcher.h"

#include "Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIAudioFocusDispatcher::TAG("CIAudioFocusDispatcher");

CIAudioFocusDispatcher::~CIAudioFocusDispatcher()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIAudioFocusDispatcher::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIAudioFocusDispatcher::DispatchAudioFocusChange(
    /* [in] */ Int32 focusChange,
    /* [in] */ const String& clientId)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jclientId = Util::ToJavaString(env, clientId);

    jclass c = env->FindClass("android/media/IAudioFocusDispatcher");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IAudioFocusDispatcher %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchAudioFocusChange", "(ILjava/lang/String;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchAudioFocusChange %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, focusChange, jclientId);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchAudioFocusChange %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jclientId);
    return NOERROR;
}

ECode CIAudioFocusDispatcher::ToString(
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


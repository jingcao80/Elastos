
#include "elastos/droid/javaproxy/CIAudioFocusDispatcher.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Media::EIID_IIAudioFocusDispatcher;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIAudioFocusDispatcher::TAG("CIAudioFocusDispatcher");

CAR_INTERFACE_IMPL_2(CIAudioFocusDispatcher, Object, IIAudioFocusDispatcher, IBinder)

CAR_OBJECT_IMPL(CIAudioFocusDispatcher)

CIAudioFocusDispatcher::~CIAudioFocusDispatcher()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIAudioFocusDispatcher::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIAudioFocusDispatcher::DispatchAudioFocusChange(
    /* [in] */ Int32 focusChange,
    /* [in] */ const String& clientId)
{
     // LOGGERD(TAG, "+ CIAudioFocusDispatcher::DispatchAudioFocusChange");
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
     // LOGGERD(TAG, "+ CIAudioFocusDispatcher::DispatchAudioFocusChange");
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


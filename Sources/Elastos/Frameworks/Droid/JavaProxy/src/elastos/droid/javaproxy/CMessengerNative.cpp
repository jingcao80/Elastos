
#include "elastos/droid/javaproxy/CMessengerNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IIMessenger;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CMessengerNative::TAG("CMessengerNative");

CAR_INTERFACE_IMPL_2(CMessengerNative, Object, IIMessenger, IBinder)

CAR_OBJECT_IMPL(CMessengerNative)

CMessengerNative::~CMessengerNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CMessengerNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CMessengerNative::Send(
    /* [in] */ IMessage* message)
{
    // LOGGERD(TAG, "+ CMessengerNative::Send()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/os/IMessenger");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: Messenger", __LINE__);

    jmethodID m = env->GetMethodID(c, "send", "(Landroid/os/Message;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: send", __LINE__);

    jobject jmessage = NULL;
    if (message != NULL) {
        jmessage = Util::ToJavaMessage(env, message);
    }

    env->CallVoidMethod(mJInstance, m, jmessage);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: send", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jmessage);

    // LOGGERD(TAG, "- CMessengerNative::Send()");
    return NOERROR;
}

ECode CMessengerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CMessengerNative::ToString()");
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, "ToString", "FindClass: Object", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: toString", __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: toString", __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, "- CMessengerNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

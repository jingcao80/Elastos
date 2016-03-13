
#include "elastos/droid/javaproxy/CIntentReceiverNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::JavaProxy::Util;
using Elastos::Utility::Logging::Logger;

using Elastos::Droid::Content::IIntentReceiver;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIntentReceiverNative::TAG("CIntentReceiverNative");

CIntentReceiverNative::~CIntentReceiverNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIntentReceiverNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIntentReceiverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIntentReceiverNative::ToString()"));
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

    // LOGGERD(TAG, String("- CIntentReceiverNative::ToString()"));
    return NOERROR;
}

ECode CIntentReceiverNative::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    // LOGGERD(TAG, String("+ CIntentReceiverNative::PerformReceive()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/content/IIntentReceiver");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IIntentReceiver", __LINE__);

    jobject jintent = NULL;
    if (intent != NULL) {
        jintent = Util::ToJavaIntent(env, intent);
    }

    jstring jdata = Util::ToJavaString(env, data);

    jobject jextras = NULL;
    if (extras != NULL) {
        jextras = Util::ToJavaBundle(env, extras);
    }

    jmethodID m = env->GetMethodID(c, "performReceive", "(Landroid/content/Intent;ILjava/lang/String;Landroid/os/Bundle;ZZI)V");
    Util::CheckErrorAndLog(env, TAG, String("GetMethodID: performReceive"), __LINE__);

    env->CallVoidMethod(mJInstance, m, jintent, (jint)resultCode, jdata, jextras, (jboolean)ordered, (jboolean)sticky, (jint)sendingUser);
    Util::CheckErrorAndLog(env, TAG, String("CallVoidMethod: performReceive"), __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jintent);
    env->DeleteLocalRef(jdata);
    env->DeleteLocalRef(jextras);

    // LOGGERD(TAG, String("- CIntentReceiverNative::PerformReceive()"));
    return NOERROR;
}

}
}
}


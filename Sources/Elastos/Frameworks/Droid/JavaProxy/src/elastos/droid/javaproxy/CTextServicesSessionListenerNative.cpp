
#include "elastos/droid/javaproxy/CTextServicesSessionListenerNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CTextServicesSessionListenerNative::TAG("CTextServicesSessionListenerNative");

CTextServicesSessionListenerNative::~CTextServicesSessionListenerNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CTextServicesSessionListenerNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    mJInstance = env->NewGlobalRef((jobject)jInstance);

    return NOERROR;
}

ECode CTextServicesSessionListenerNative::OnServiceConnected(
    /* [in] */ IISpellCheckerSession* spellCheckerSession)
{
    LOGGERD(TAG, String("+ CTextServicesSessionListenerNative::OnServiceConnected()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobject jspellCheckerSession = NULL;
    if (spellCheckerSession != NULL) {
        LOGGERE(TAG, "OnServiceConnected() spellCheckerSession NOT NULL!");
    }

    jclass c = env->FindClass("com/android/internal/textservice/ITextServicesSessionListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ITextServicesSessionListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onServiceConnected", "(Lcom/android/internal/textservice/ISpellCheckerSession;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onServiceConnected %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jspellCheckerSession);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onServiceConnected %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CTextServicesSessionListenerNative::OnServiceConnected()"));
    return NOERROR;
}

ECode CTextServicesSessionListenerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CTextServicesSessionListenerNative::ToString()"));
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

    // LOGGERD(TAG, String("- CTextServicesSessionListenerNative::ToString()"));
    return NOERROR;
}

}
}
}


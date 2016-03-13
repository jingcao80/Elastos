
#include "elastos/droid/javaproxy/CSpellCheckerSessionListenerNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CSpellCheckerSessionListenerNative::TAG("CSpellCheckerSessionListenerNative");

CSpellCheckerSessionListenerNative::~CSpellCheckerSessionListenerNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CSpellCheckerSessionListenerNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    mJInstance = env->NewGlobalRef((jobject)jInstance);

    return NOERROR;
}

ECode CSpellCheckerSessionListenerNative::OnGetSuggestions(
    /* [in] */ const ArrayOf<ISuggestionsInfo *>& results)
{
    LOGGERD(TAG, String("+ CSpellCheckerSessionListenerNative::OnGetSuggestions()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobjectArray jresults = NULL;
    Int32 count = results.GetLength();
    if (count > 0) {
        jclass siKlass = env->FindClass("android/view/textservice/SuggestionsInfo");
        Util::CheckErrorAndLog(env, TAG, "FindClass: SuggestionsInfo %d", __LINE__);

        jresults = env->NewObjectArray((jsize)count, siKlass, 0);
        Util::CheckErrorAndLog(env, TAG, "NewObjectArray: SuggestionsInfo %d", __LINE__);

        for (Int32 i = 0; i < count; i++) {
            AutoPtr<ISuggestionsInfo> info = results[i];
            jobject jinfo = Util::ToJavaSuggestionsInfo(env, info);

            env->SetObjectArrayElement(jresults, i, jinfo);
            Util::CheckErrorAndLog(env, TAG, "SetObjectArrayElement: SuggestionsInfo %d", __LINE__);

            env->DeleteLocalRef(jinfo);
        }

        env->DeleteLocalRef(siKlass);
    }

    jclass c = env->FindClass("com/android/internal/textservice/ISpellCheckerSessionListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ISpellCheckerSessionListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onGetSuggestions", "([Landroid/view/textservice/SuggestionsInfo;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onGetSuggestions %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jresults);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onGetSuggestions %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jresults);

    LOGGERD(TAG, String("- CSpellCheckerSessionListenerNative::OnGetSuggestions()"));
    return NOERROR;
}

ECode CSpellCheckerSessionListenerNative::OnGetSentenceSuggestions(
    /* [in] */ const ArrayOf<ISentenceSuggestionsInfo *>& results)
{
    LOGGERD(TAG, String("+ CSpellCheckerSessionListenerNative::OnGetSentenceSuggestions()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jobjectArray jresults = NULL;
    Int32 count = results.GetLength();
    if (count > 0) {
        jclass ssiKlass = env->FindClass("android/view/textservice/SentenceSuggestionsInfo");
        Util::CheckErrorAndLog(env, TAG, "FindClass: SentenceSuggestionsInfo %d", __LINE__);

        jresults = env->NewObjectArray((jsize)count, ssiKlass, 0);
        Util::CheckErrorAndLog(env, TAG, "NewObjectArray: SentenceSuggestionsInfo %d", __LINE__);

        for (Int32 i = 0; i < count; i++) {
            AutoPtr<ISentenceSuggestionsInfo> info = results[i];
            jobject jinfo = Util::ToJavaSentenceSuggestionsInfo(env, info);

            env->SetObjectArrayElement(jresults, i, jinfo);
            Util::CheckErrorAndLog(env, TAG, "SetObjectArrayElement: SentenceSuggestionsInfo %d", __LINE__);

            env->DeleteLocalRef(jinfo);
        }

        env->DeleteLocalRef(ssiKlass);
    }

    jclass c = env->FindClass("com/android/internal/textservice/ISpellCheckerSessionListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: ISpellCheckerSessionListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onGetSentenceSuggestions", "([Landroid/view/textservice/SentenceSuggestionsInfo;)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onGetSentenceSuggestions %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jresults);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onGetSentenceSuggestions %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CSpellCheckerSessionListenerNative::OnGetSentenceSuggestions()"));
    return NOERROR;
}

ECode CSpellCheckerSessionListenerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CSpellCheckerSessionListenerNative::ToString()"));
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

    // LOGGERD(TAG, String("- CSpellCheckerSessionListenerNative::ToString()"));
    return NOERROR;
}

}
}
}


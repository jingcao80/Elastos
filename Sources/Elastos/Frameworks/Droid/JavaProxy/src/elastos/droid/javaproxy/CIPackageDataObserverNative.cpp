
#include "elastos/droid/javaproxy/CIPackageDataObserverNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIPackageDataObserverNative::TAG("CIPackageDataObserverNative");

CIPackageDataObserverNative::~CIPackageDataObserverNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIPackageDataObserverNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIPackageDataObserverNative::OnRemoveCompleted(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean succeeded)
{
    // LOGGERD(TAG, String("+ CIPackageDataObserverNative::OnRemoveCompleted()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jpackageName = Util::ToJavaString(env, packageName);

    jclass c = env->FindClass("android/content/pm/IPackageDataObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IPackageDataObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onRemoveCompleted", "(Ljava/lang/String;Z)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onRemoveCompleted %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jpackageName, (jboolean)succeeded);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onRemoveCompleted %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpackageName);

    // LOGGERD(TAG, String("- CIPackageDataObserverNative::OnRemoveCompleted()"));
    return NOERROR;
}

ECode CIPackageDataObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIPackageDataObserverNative::ToString()"));
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

    // LOGGERD(TAG, String("- CIPackageDataObserverNative::ToString()"));
    return NOERROR;
}

}
}
}


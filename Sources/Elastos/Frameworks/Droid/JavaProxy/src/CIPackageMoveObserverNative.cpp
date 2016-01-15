
#include "CIPackageMoveObserverNative.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIPackageMoveObserverNative::TAG("CIPackageMoveObserverNative");

CIPackageMoveObserverNative::~CIPackageMoveObserverNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIPackageMoveObserverNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIPackageMoveObserverNative::PackageMoved(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 returnCode)
{
    LOGGERD(TAG, String("+ CIPackageMoveObserverNative::PackageMoved()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jpackageName = Util::ToJavaString(env, packageName);

    jclass c = env->FindClass("android/content/pm/IPackageMoveObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IPackageMoveObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "packageMoved", "(Ljava/lang/String;I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: packageMoved %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jpackageName, (jint)returnCode);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: packageMoved %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpackageName);

    LOGGERD(TAG, String("- CIPackageMoveObserverNative::PackageMoved()"));
    return NOERROR;
}

ECode CIPackageMoveObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIPackageMoveObserverNative::ToString()"));
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

    // LOGGERD(TAG, String("- CIPackageMoveObserverNative::ToString()"));
    return NOERROR;
}

}
}
}


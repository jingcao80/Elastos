
#include "CIPackageDeleteObserverNative.h"
#include <elastos/utility/logging/Logger.h>
#include "Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIPackageDeleteObserverNative::TAG("CIPackageDeleteObserverNative");

CIPackageDeleteObserverNative::~CIPackageDeleteObserverNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIPackageDeleteObserverNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIPackageDeleteObserverNative::PackageDeleted(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 returnCode)
{
    // LOGGERD(TAG, String("+ CIPackageDeleteObserverNative::PackageDeleted()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jpackageName = Util::ToJavaString(env, packageName);

    jclass c = env->FindClass("android/content/pm/IPackageDeleteObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IPackageDeleteObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "packageDeleted", "(Ljava/lang/String;I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: packageDeleted %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jpackageName, (jint)returnCode);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: packageDeleted %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jpackageName);

    // LOGGERD(TAG, String("- CIPackageDeleteObserverNative::PackageDeleted()"));
    return NOERROR;
}

ECode CIPackageDeleteObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CIPackageDeleteObserverNative::ToString()"));
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

    // LOGGERD(TAG, String("- CIPackageDeleteObserverNative::ToString()"));
    return NOERROR;
}


}
}
}


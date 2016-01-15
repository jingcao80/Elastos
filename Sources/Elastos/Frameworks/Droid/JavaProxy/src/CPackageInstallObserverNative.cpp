
#include "CPackageInstallObserverNative.h"
#include "Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CPackageInstallObserverNative::TAG("CPackageInstallObserverNative");

CPackageInstallObserverNative::~CPackageInstallObserverNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CPackageInstallObserverNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CPackageInstallObserverNative::PackageInstalled(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 returnCode)
{
    // LOGGERD(TAG, String("+ CPackageInstallObserverNative::PackageInstalled()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jstring jstr = Util::ToJavaString(env, packageName);

    jclass c = env->FindClass("android/content/pm/IPackageInstallObserver");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IPackageInstallObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "packageInstalled", "(Ljava/lang/String;I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: packageInstalled %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, jstr, (jint)returnCode);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: packageInstalled %d", __LINE__);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, String("- CPackageInstallObserverNative::PackageInstalled()"));
    return NOERROR;
}

ECode CPackageInstallObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CPackageInstallObserverNative::ToString()"));
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

    // LOGGERD(TAG, String("- CPackageInstallObserverNative::ToString()"));
    return NOERROR;
}


}
}
}



#include "elastos/droid/javaproxy/CIPackageDeleteObserverNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Content::Pm::EIID_IIPackageDeleteObserver;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIPackageDeleteObserverNative::TAG("CIPackageDeleteObserverNative");

CAR_INTERFACE_IMPL_2(CIPackageDeleteObserverNative, Object, IIPackageDeleteObserver, IBinder)

CAR_OBJECT_IMPL(CIPackageDeleteObserverNative)

CIPackageDeleteObserverNative::~CIPackageDeleteObserverNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIPackageDeleteObserverNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIPackageDeleteObserverNative::PackageDeleted(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 returnCode)
{
    // LOGGERD(TAG, "+ CIPackageDeleteObserverNative::PackageDeleted()");

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

    // LOGGERD(TAG, "- CIPackageDeleteObserverNative::PackageDeleted()");
    return NOERROR;
}

ECode CIPackageDeleteObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIPackageDeleteObserverNative::ToString()");
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

    // LOGGERD(TAG, "- CIPackageDeleteObserverNative::ToString()");
    return NOERROR;
}

}
}
}

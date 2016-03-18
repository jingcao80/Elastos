
#include "elastos/droid/javaproxy/CIPackageDataObserverNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Content::Pm::EIID_IIPackageDataObserver;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIPackageDataObserverNative::TAG("CIPackageDataObserverNative");

CAR_INTERFACE_IMPL_2(CIPackageDataObserverNative, Object, IIPackageDataObserver, IBinder)

CAR_OBJECT_IMPL(CIPackageDataObserverNative)

CIPackageDataObserverNative::~CIPackageDataObserverNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIPackageDataObserverNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIPackageDataObserverNative::OnRemoveCompleted(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean succeeded)
{
    // LOGGERD(TAG, "+ CIPackageDataObserverNative::OnRemoveCompleted()");

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

    // LOGGERD(TAG, "- CIPackageDataObserverNative::OnRemoveCompleted()");
    return NOERROR;
}

ECode CIPackageDataObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIPackageDataObserverNative::ToString()");
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

    // LOGGERD(TAG, "- CIPackageDataObserverNative::ToString()");
    return NOERROR;
}

}
}
}


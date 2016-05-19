
#include "elastos/droid/javaproxy/CPackageInstallObserver2Native.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::EIID_IIPackageInstallObserver2;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CPackageInstallObserver2Native::TAG("CPackageInstallObserver2Native");

CAR_INTERFACE_IMPL_2(CPackageInstallObserver2Native, Object, IIPackageInstallObserver2, IBinder)

CAR_OBJECT_IMPL(CPackageInstallObserver2Native)

CPackageInstallObserver2Native::~CPackageInstallObserver2Native()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CPackageInstallObserver2Native::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CPackageInstallObserver2Native::OnUserActionRequired(
        /* [in] */ IIntent* intent)
{
    assert(0);
    return NOERROR;
}

ECode CPackageInstallObserver2Native::OnPackageInstalled(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 returnCode,
    /* [in] */ const String& msg,
    /* [in] */ IBundle* extras)
{
    assert(0);
    return NOERROR;
}

ECode CPackageInstallObserver2Native::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CPackageInstallObserver2Native::ToString()");
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

    // LOGGERD(TAG, "- CPackageInstallObserver2Native::ToString()");
    return NOERROR;
}

}
}
}


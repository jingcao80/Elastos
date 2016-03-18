
#include "elastos/droid/javaproxy/CIActivityControllerNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::App::EIID_IIActivityController;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIActivityControllerNative::TAG("CIActivityControllerNative");

CAR_INTERFACE_IMPL_2(CIActivityControllerNative, Object, IIActivityController, IBinder)

CAR_OBJECT_IMPL(CIActivityControllerNative)

CIActivityControllerNative::~CIActivityControllerNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIActivityControllerNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIActivityControllerNative::ActivityStarting(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& pkg,
    /* [out] */ Boolean * pRes)
{
    LOGGERD(TAG, "CIActivityControllerNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIActivityControllerNative::ActivityResuming(
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* res)
{
    LOGGERD(TAG, "CIActivityControllerNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIActivityControllerNative::AppCrashed(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& shortMsg,
    /* [in] */ const String& longMsg,
    /* [in] */ Int64 timeMillis,
    /* [in] */ const String& stackTrace,
    /* [out] */ Boolean* res)
{
    LOGGERD(TAG, "CIActivityControllerNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIActivityControllerNative::AppEarlyNotResponding(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& annotation,
    /* [out] */ Int32* res)
{
    LOGGERD(TAG, "CIActivityControllerNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIActivityControllerNative::AppNotResponding(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& processStats,
    /* [out] */ Int32* res)
{
    LOGGERD(TAG, "CIActivityControllerNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIActivityControllerNative::SystemNotResponding(
    /* [in] */ const String& msg,
    /* [out] */ Int32* res)
{
    LOGGERD(TAG, "CIActivityControllerNative E_NOT_IMPLEMENTED Line:%d", __LINE__);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CIActivityControllerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIActivityControllerNative::ToString()");

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

    // LOGGERD(TAG, "- CIActivityControllerNative::ToString()");
    return NOERROR;
}

}
}
}


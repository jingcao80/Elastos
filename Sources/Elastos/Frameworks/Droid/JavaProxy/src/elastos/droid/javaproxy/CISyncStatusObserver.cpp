
#include "elastos/droid/javaproxy/CISyncStatusObserver.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Content::EIID_IISyncStatusObserver;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CISyncStatusObserver::TAG("CISyncStatusObserver");

CAR_INTERFACE_IMPL_2(CISyncStatusObserver, Object, IISyncStatusObserver, IBinder)

CAR_OBJECT_IMPL(CISyncStatusObserver)

CISyncStatusObserver::~CISyncStatusObserver()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CISyncStatusObserver::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CISyncStatusObserver::OnStatusChanged(
    /* [in] */ Int32 which)
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/content/ISyncStatusObserver");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: ISyncStatusObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onStatusChanged", "(I)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onStatusChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)which);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onStatusChanged %d", __LINE__);

    env->DeleteLocalRef(c);
    return NOERROR;
}

ECode CISyncStatusObserver::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CISyncStatusObserver::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("java/lang/Object");
    Util::CheckErrorAndLog(env, TAG, "FindClass: Object %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "toString", "()Ljava/lang/String;");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: toString %d", __LINE__);

    jstring jstr = (jstring)env->CallObjectMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: toString %d", __LINE__);

    *str = Util::GetElString(env, jstr);

    env->DeleteLocalRef(c);
    env->DeleteLocalRef(jstr);

    // LOGGERD(TAG, "- CISyncStatusObserver::ToString()");
    return NOERROR;
}

}
}
}


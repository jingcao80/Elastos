
#include "elastos/droid/javaproxy/CISyncStatusObserver.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CISyncStatusObserver::TAG("CISyncStatusObserver");

CISyncStatusObserver::~CISyncStatusObserver()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CISyncStatusObserver::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
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

}
}
}


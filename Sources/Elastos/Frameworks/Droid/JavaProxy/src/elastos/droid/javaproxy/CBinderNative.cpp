
#include "elastos/droid/javaproxy/CBinderNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CBinderNative::TAG("CBinderNative");

CAR_INTERFACE_IMPL(CBinderNative, Object, IBinder)

CAR_OBJECT_IMPL(CBinderNative)

CBinderNative::~CBinderNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CBinderNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CBinderNative::ToString(
    /* [out] */ String* str)
{
    LOGGERD(TAG, "+ CBinderNative::ToString()");

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

    LOGGERD(TAG, "- CBinderNative::ToString()");
    return NOERROR;
}

}
}
}


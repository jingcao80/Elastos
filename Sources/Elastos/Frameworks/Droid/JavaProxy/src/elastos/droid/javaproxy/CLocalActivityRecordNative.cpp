
#include "elastos/droid/javaproxy/CLocalActivityRecordNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::App::EIID_ILocalActivityRecord;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CLocalActivityRecordNative::TAG("CIntentReceiverNative");

CAR_INTERFACE_IMPL_2(CLocalActivityRecordNative, Object, ILocalActivityRecord, IBinder)

CAR_OBJECT_IMPL(CLocalActivityRecordNative)

CLocalActivityRecordNative::~CLocalActivityRecordNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CLocalActivityRecordNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CLocalActivityRecordNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CLocalActivityRecordNative::ToString()");
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

    // LOGGERD(TAG, "- CLocalActivityRecordNative::ToString()");
    return NOERROR;
}

}
}
}


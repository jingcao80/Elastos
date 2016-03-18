
#include "elastos/droid/javaproxy/CDataSetObserverNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CDataSetObserverNative::TAG("CDataSetObserverNative");

CAR_INTERFACE_IMPL_2(CDataSetObserverNative, Object, IDataSetObserver, IBinder)

CAR_OBJECT_IMPL(CDataSetObserverNative)

CDataSetObserverNative::~CDataSetObserverNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CDataSetObserverNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CDataSetObserverNative::OnChanged()
{
    // LOGGERD(TAG, "+ CDataSetObserverNative::OnChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/DataSetObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: DataSetObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onChanged", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CDataSetObserverNative::OnChanged()");
    return NOERROR;
}

ECode CDataSetObserverNative::OnInvalidated()
{
    // LOGGERD(TAG, "+ CDataSetObserverNative::OnInvalidated()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/DataSetObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: DataSetObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onInvalidated", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onInvalidated %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onInvalidated %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CDataSetObserverNative::OnInvalidated()");
    return NOERROR;
}

ECode CDataSetObserverNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CDataSetObserverNative::ToString()");

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

    // LOGGERD(TAG, "- CDataSetObserverNative::ToString()");
    return NOERROR;
}

}
}
}


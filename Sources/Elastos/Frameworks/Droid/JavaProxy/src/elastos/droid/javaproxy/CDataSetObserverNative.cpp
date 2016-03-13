
#include "elastos/droid/javaproxy/CDataSetObserverNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::JavaProxy::Util;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CDataSetObserverNative::TAG("CDataSetObserverNative");


CDataSetObserverNative::~CDataSetObserverNative(){
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CDataSetObserverNative::constructor(
    /* [in] */ Handle32 jVM,
    /* [in] */ Handle32 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CDataSetObserverNative::OnChanged()
{
    LOGGERD(TAG, String("+ CDataSetObserverNative::OnChanged()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/DataSetObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: DataSetObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onChanged", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CDataSetObserverNative::OnChanged()"));
    return NOERROR;
}

ECode CDataSetObserverNative::OnInvalidated()
{
    LOGGERD(TAG, String("+ CDataSetObserverNative::OnInvalidated()"));

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/database/DataSetObserver");
    Util::CheckErrorAndLog(env, TAG, "FindClass: DataSetObserver %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onInvalidated", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onInvalidated %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onInvalidated %d", __LINE__);

    env->DeleteLocalRef(c);

    LOGGERD(TAG, String("- CDataSetObserverNative::OnInvalidated()"));
    return NOERROR;
}

}
}
}


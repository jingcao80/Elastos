
#include "elastos/droid/javaproxy/CIOnPrimaryClipChangedListenerNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Content::EIID_IOnPrimaryClipChangedListener;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIOnPrimaryClipChangedListenerNative::TAG("CIOnPrimaryClipChangedListenerNative");

CAR_INTERFACE_IMPL_2(CIOnPrimaryClipChangedListenerNative, Object, IOnPrimaryClipChangedListener, IBinder)

CAR_OBJECT_IMPL(CIOnPrimaryClipChangedListenerNative)

CIOnPrimaryClipChangedListenerNative::~CIOnPrimaryClipChangedListenerNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIOnPrimaryClipChangedListenerNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIOnPrimaryClipChangedListenerNative::DispatchPrimaryClipChanged()
{
    // LOGGERD(TAG, "+ CIOnPrimaryClipChangedListenerNative::DispatchPrimaryClipChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/content/IOnPrimaryClipChangedListener");
    Util::CheckErrorAndLog(env, TAG, "FindClass: IOnPrimaryClipChangedListener %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "dispatchPrimaryClipChanged", "()V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: dispatchPrimaryClipChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: dispatchPrimaryClipChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIOnPrimaryClipChangedListenerNative::DispatchPrimaryClipChanged()");
    return NOERROR;
}

ECode CIOnPrimaryClipChangedListenerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIOnPrimaryClipChangedListenerNative::ToString()");

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

    // LOGGERD(TAG, "- CIOnPrimaryClipChangedListenerNative::ToString()");
    return NOERROR;
}

}
}
}


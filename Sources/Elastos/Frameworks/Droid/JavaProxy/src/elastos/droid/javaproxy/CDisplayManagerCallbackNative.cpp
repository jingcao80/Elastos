
#include "elastos/droid/javaproxy/CDisplayManagerCallbackNative.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/javaproxy/Util.h"

using Elastos::Droid::Hardware::Display::EIID_IIDisplayManagerCallback;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CDisplayManagerCallbackNative::TAG("CDisplayManagerCallbackNative");

CAR_INTERFACE_IMPL_2(CDisplayManagerCallbackNative, Object, IIDisplayManagerCallback, IBinder)

CAR_OBJECT_IMPL(CDisplayManagerCallbackNative)

CDisplayManagerCallbackNative::~CDisplayManagerCallbackNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CDisplayManagerCallbackNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CDisplayManagerCallbackNative::OnDisplayEvent(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 event)
{
    // LOGGERD(TAG, "+ CDisplayManagerCallbackNative::OnDisplayEvent()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/hardware/display/IDisplayManagerCallback");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IDisplayManagerCallback", __LINE__);

    jmethodID m = env->GetMethodID(c, "onDisplayEvent", "(II)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onDisplayEvent", __LINE__);

    env->CallVoidMethod(mJInstance, m, (jint)displayId, (jint)event);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onDisplayEvent", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CDisplayManagerCallbackNative::OnDisplayEvent()");
    return NOERROR;
}

ECode CDisplayManagerCallbackNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, String("+ CDisplayManagerCallbackNative::ToString()"));

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

    // LOGGERD(TAG, String("- CDisplayManagerCallbackNative::ToString()"));
    return NOERROR;
}

} // namespace JavaProxy
} // namepsace Droid
} // namespace Elastos
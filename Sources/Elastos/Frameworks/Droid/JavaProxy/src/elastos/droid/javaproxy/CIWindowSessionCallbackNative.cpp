
#include "elastos/droid/javaproxy/CIWindowSessionCallbackNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::View::EIID_IIWindowSessionCallback;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIWindowSessionCallbackNative::TAG("CIWindowSessionCallbackNative");

CAR_INTERFACE_IMPL_2(CIWindowSessionCallbackNative, Object, IIWindowSessionCallback, IBinder)

CAR_OBJECT_IMPL(CIWindowSessionCallbackNative)

CIWindowSessionCallbackNative::~CIWindowSessionCallbackNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIWindowSessionCallbackNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIWindowSessionCallbackNative::OnAnimatorScaleChanged(
    /* [in] */ Float scale)
{
    // LOGGERD(TAG, "+ CIWindowSessionCallbackNative::OnAnimatorScaleChanged()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);

    jclass c = env->FindClass("android/view/IWindowSessionCallback");
    Util::CheckErrorAndLog(env, TAG, "Fail FindClass: IWindowSessionCallback %d", __LINE__);

    jmethodID m = env->GetMethodID(c, "onAnimatorScaleChanged", "(F)V");
    Util::CheckErrorAndLog(env, TAG, "GetMethodID: onAnimatorScaleChanged %d", __LINE__);

    env->CallVoidMethod(mJInstance, m, scale);
    Util::CheckErrorAndLog(env, TAG, "CallVoidMethod: onAnimatorScaleChanged %d", __LINE__);

    env->DeleteLocalRef(c);

    // LOGGERD(TAG, "- CIWindowSessionCallbackNative::OnAnimatorScaleChanged()");
    return NOERROR;
}

ECode CIWindowSessionCallbackNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIWindowSessionCallbackNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIWindowSessionCallbackNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

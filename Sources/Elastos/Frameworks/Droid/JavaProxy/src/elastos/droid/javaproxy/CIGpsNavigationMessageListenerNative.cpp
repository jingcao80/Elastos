
#include "elastos/droid/javaproxy/CIGpsNavigationMessageListenerNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Location::EIID_IIGpsNavigationMessageListener;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIGpsNavigationMessageListenerNative::TAG("CIGpsNavigationMessageListenerNative");

CAR_INTERFACE_IMPL_2(CIGpsNavigationMessageListenerNative, Object, IIGpsNavigationMessageListener, IBinder)

CAR_OBJECT_IMPL(CIGpsNavigationMessageListenerNative)

CIGpsNavigationMessageListenerNative::~CIGpsNavigationMessageListenerNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIGpsNavigationMessageListenerNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIGpsNavigationMessageListenerNative::OnGpsNavigationMessageReceived(
    /* [in] */ IGpsNavigationMessageEvent* event)
{
    assert(0);
    return NOERROR;
}

ECode CIGpsNavigationMessageListenerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIGpsNavigationMessageListenerNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIGpsNavigationMessageListenerNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

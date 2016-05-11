
#include "elastos/droid/javaproxy/CIConditionListenerNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Service::Notification::EIID_IIConditionListener;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIConditionListenerNative::TAG("CIConditionListenerNative");

CAR_INTERFACE_IMPL_2(CIConditionListenerNative, Object, IIConditionListener, IBinder)

CAR_OBJECT_IMPL(CIConditionListenerNative)

CIConditionListenerNative::~CIConditionListenerNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIConditionListenerNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIConditionListenerNative::OnConditionsReceived(
    /* [in] */ ArrayOf<ICondition*>* conditions)
{
    assert(0);
    return NOERROR;
}

ECode CIConditionListenerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIConditionListenerNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIConditionListenerNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos


#include "elastos/droid/javaproxy/CIConditionProviderNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Service::Notification::EIID_IIConditionProvider;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CIConditionProviderNative::TAG("CIConditionProviderNative");

CAR_INTERFACE_IMPL_2(CIConditionProviderNative, Object, IIConditionProvider, IBinder)

CAR_OBJECT_IMPL(CIConditionProviderNative)

CIConditionProviderNative::~CIConditionProviderNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CIConditionProviderNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CIConditionProviderNative::OnConnected()
{
    assert(0);
    return NOERROR;
}

ECode CIConditionProviderNative::OnRequestConditions(
    /* [in] */ Int32 relevance)
{
    assert(0);
    return NOERROR;
}

ECode CIConditionProviderNative::OnSubscribe(
    /* [in] */ IUri* conditionId)
{
    assert(0);
    return NOERROR;
}

ECode CIConditionProviderNative::OnUnsubscribe(
    /* [in] */ IUri* conditionId)
{
    assert(0);
    return NOERROR;
}

ECode CIConditionProviderNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CIConditionProviderNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CIConditionProviderNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

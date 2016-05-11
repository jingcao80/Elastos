
#include "elastos/droid/javaproxy/CINotificationListenerNative.h"
#include "elastos/droid/javaproxy/Util.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Service::Notification::EIID_IINotificationListener;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

const String CINotificationListenerNative::TAG("CINotificationListenerNative");

CAR_INTERFACE_IMPL_2(CINotificationListenerNative, Object, IINotificationListener, IBinder)

CAR_OBJECT_IMPL(CINotificationListenerNative)

CINotificationListenerNative::~CINotificationListenerNative()
{
    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    env->DeleteGlobalRef(mJInstance);
}

ECode CINotificationListenerNative::constructor(
    /* [in] */ Handle64 jVM,
    /* [in] */ Handle64 jInstance)
{
    mJVM = (JavaVM*)jVM;
    mJInstance = (jobject)jInstance;
    return NOERROR;
}

ECode CINotificationListenerNative::OnListenerConnected(
    /* [in] */ INotificationRankingUpdate* update)
{
    assert(0);
    return NOERROR;
}

ECode CINotificationListenerNative::OnNotificationPosted(
    /* [in] */ IIStatusBarNotificationHolder* notificationHolder,
    /* [in] */ INotificationRankingUpdate* update)
{
    assert(0);
    return NOERROR;
}

ECode CINotificationListenerNative::OnNotificationRemoved(
    /* [in] */ IIStatusBarNotificationHolder* notificationHolder,
    /* [in] */ INotificationRankingUpdate* update)
{
    assert(0);
    return NOERROR;
}

ECode CINotificationListenerNative::OnNotificationRankingUpdate(
    /* [in] */ INotificationRankingUpdate* update)
{
    assert(0);
    return NOERROR;
}

ECode CINotificationListenerNative::OnListenerHintsChanged(
    /* [in] */ Int32 hints)
{
    assert(0);
    return NOERROR;
}

ECode CINotificationListenerNative::OnInterruptionFilterChanged(
    /* [in] */ Int32 interruptionFilter)
{
    assert(0);
    return NOERROR;
}

ECode CINotificationListenerNative::ToString(
    /* [out] */ String* str)
{
    // LOGGERD(TAG, "+ CINotificationListenerNative::ToString()");

    JNIEnv* env;
    mJVM->AttachCurrentThread(&env, NULL);
    *str = Util::GetJavaToStringResult(env, mJInstance);

    // LOGGERD(TAG, "- CINotificationListenerNative::ToString()");
    return NOERROR;
}

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

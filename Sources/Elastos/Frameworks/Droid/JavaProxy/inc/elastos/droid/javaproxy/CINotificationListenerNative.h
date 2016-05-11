
#ifndef __ELASTOS_DROID_JAVAPROXY_CINOTIFICATIONLISTENERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CINOTIFICATIONLISTENERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CINotificationListenerNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Service::Notification::IINotificationListener;
using Elastos::Droid::Service::Notification::INotificationRankingUpdate;
using Elastos::Droid::Service::Notification::IIStatusBarNotificationHolder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CINotificationListenerNative)
    , public Object
    , public IINotificationListener
    , public IBinder
{
public:
    ~CINotificationListenerNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle64 jVM,
        /* [in] */ Handle64 jInstance);

    CARAPI OnListenerConnected(
        /* [in] */ INotificationRankingUpdate* update);

    CARAPI OnNotificationPosted(
        /* [in] */ IIStatusBarNotificationHolder* notificationHolder,
        /* [in] */ INotificationRankingUpdate* update);

    CARAPI OnNotificationRemoved(
        /* [in] */ IIStatusBarNotificationHolder* notificationHolder,
        /* [in] */ INotificationRankingUpdate* update);

    CARAPI OnNotificationRankingUpdate(
        /* [in] */ INotificationRankingUpdate* update);

    CARAPI OnListenerHintsChanged(
        /* [in] */ Int32 hints);

    CARAPI OnInterruptionFilterChanged(
        /* [in] */ Int32 interruptionFilter);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_JAVAPROXY_CINOTIFICATIONLISTENERNATIVE_H__

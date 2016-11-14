
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONLISTENERSERVICE_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONLISTENERSERVICE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CNotificationListenerService.h"
#include <elastos/core/Runnable.h>
#include <elastos/droid/service/notification/NotificationListenerService.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Service::Notification::INotificationListenerServiceRankingMap;
using Elastos::Droid::Service::Notification::IStatusBarNotification;
using Elastos::Droid::Service::Notification::NotificationListenerService;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class BaseStatusBar;

/**
 * The view representing the separation between important and less important notifications
 */
CarClass(CNotificationListenerService)
    , public NotificationListenerService
{
private:
    class AddNotificationRunnable : public Runnable
    {
    public:
        AddNotificationRunnable(
            /* [in] */ CNotificationListenerService* service,
            /* [in] */ ArrayOf<IStatusBarNotification*>* notifications,
            /* [in] */ INotificationListenerServiceRankingMap* currentRanking);

        // @Override
        CARAPI Run();

    private:
        CNotificationListenerService* mService;
        AutoPtr<ArrayOf<IStatusBarNotification*> > mNotifications;
        AutoPtr<INotificationListenerServiceRankingMap> mCurrentRanking;
    };

    class UpdateNotificationRunnable : public Runnable
    {
    public:
        UpdateNotificationRunnable(
            /* [in] */ CNotificationListenerService* service,
            /* [in] */ IStatusBarNotification* sbn,
            /* [in] */ INotificationListenerServiceRankingMap* rankingMap);

        // @Override
        CARAPI Run();

    private:
        CNotificationListenerService* mService;
        AutoPtr<IStatusBarNotification> mSbn;
        AutoPtr<INotificationListenerServiceRankingMap> mRankingMap;
    };

    class RemoveNotificationRunnable : public Runnable
    {
    public:
        RemoveNotificationRunnable(
            /* [in] */ CNotificationListenerService* service,
            /* [in] */ IStatusBarNotification* sbn,
            /* [in] */ INotificationListenerServiceRankingMap* rankingMap);

        // @Override
        CARAPI Run();

    private:
        CNotificationListenerService* mService;
        AutoPtr<IStatusBarNotification> mSbn;
        AutoPtr<INotificationListenerServiceRankingMap> mRankingMap;
    };

    class UpdateNotificationRankingRunnable : public Runnable
    {
    public:
        UpdateNotificationRankingRunnable(
            /* [in] */ CNotificationListenerService* service,
            /* [in] */ INotificationListenerServiceRankingMap* rankingMap);

        // @Override
        CARAPI Run();

    private:
        CNotificationListenerService* mService;
        AutoPtr<INotificationListenerServiceRankingMap> mRankingMap;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBaseStatusBar* host);

    // @Override
    CARAPI OnListenerConnected();

    // @Override
    CARAPI OnNotificationPosted(
        /* [in] */ IStatusBarNotification* sbn,
        /* [in] */ INotificationListenerServiceRankingMap* rankingMap);

    // @Override
    CARAPI OnNotificationRemoved(
        /* [in] */ IStatusBarNotification* sbn,
        /* [in] */ INotificationListenerServiceRankingMap* rankingMap);

    // @Override
    CARAPI OnNotificationRankingUpdate(
        /* [in] */ INotificationListenerServiceRankingMap* rankingMap);

private:
    BaseStatusBar* mHost;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONLISTENERSERVICE_H__

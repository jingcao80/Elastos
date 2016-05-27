
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONLISTENERSERVICE_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CNOTIFICATIONLISTENERSERVICE_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CNotificationListenerService.h"
#include <elastos/droid/os/Runnable.h>
#include <elastos/droid/service/notification/NotificationListenerService.h>

using Elastos::Droid::Os::Runnable;
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
    class Runnable1 : public Runnable
    {
    public:
        Runnable1(
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

    class Runnable2 : public Runnable
    {
    public:
        Runnable2(
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

    class Runnable3 : public Runnable
    {
    public:
        Runnable3(
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

    class Runnable4 : public Runnable
    {
    public:
        Runnable4(
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

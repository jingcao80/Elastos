//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

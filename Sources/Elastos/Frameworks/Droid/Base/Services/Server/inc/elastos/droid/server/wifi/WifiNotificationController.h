#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFINOTIFICATIONCONTROLLER_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFINOTIFICATIONCONTROLLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/server/wifi/WifiStateMachine.h"

using Elastos::Droid::App::INotification;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

/* Takes care of handling the "open wi-fi network available" notification @hide */
class WifiNotificationController
    : public Object
{
private:
    class InnerBroadcastReceiver1
        : public BroadcastReceiver
    {
    public:
        InnerBroadcastReceiver1(
            /* [in] */ WifiNotificationController* owner);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiNotificationController* mOwner;
    };

    class NotificationEnabledSettingObserver
        : public ContentObserver
    {
    public:
        NotificationEnabledSettingObserver(
            /* [in] */ WifiNotificationController* owner,
            /* [in] */ IHandler* handler);

        virtual CARAPI Register();

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CARAPI_(Boolean) GetValue();
    private:
        WifiNotificationController* mOwner;
    };

public:
    WifiNotificationController(
        /* [in] */ IContext* context,
        /* [in] */ WifiStateMachine* wsm);

    virtual CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    // synchronized
    CARAPI_(void) CheckAndSetNotification(
        /* [in] */ INetworkInfo* networkInfo,
        /* [in] */ IList* scanResults);//IScanResult

    /**
      * Clears variables related to tracking whether a notification has been
      * shown recently and clears the current notification.
      */
    // synchronized
    CARAPI_(void) ResetNotification();

    /**
      * Display or don't display a notification that there are open Wi-Fi networks.
      * @param visible {@code true} if notification should be visible, {@code false} otherwise
      * @param numNetworks the number networks seen
      * @param force {@code true} to force notification to be shown/not-shown,
      * even if it is already shown/not-shown.
      * @param delay time in milliseconds after which the notification should be made
      * visible or invisible.
      */
    CARAPI_(void) SetNotificationVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 numNetworks,
        /* [in] */ Boolean force,
        /* [in] */ Int32 delay);

private:
    /**
      * The icon to show in the 'available networks' notification. This will also
      * be the ID of the Notification given to the NotificationManager.
      */
    static const Int32 ICON_NETWORKS_AVAILABLE;// = com.android.internal.R.drawable.stat_notify_wifi_in_range;
    /**
      * When a notification is shown, we wait this amount before possibly showing it again.
      */
    Int64 NOTIFICATION_REPEAT_DELAY_MS;
    /**
      * Whether the user has set the setting to show the 'available networks' notification.
      */
    Boolean mNotificationEnabled;
    /**
      * Observes the user setting to keep {@link #mNotificationEnabled} in sync.
      */
    AutoPtr<NotificationEnabledSettingObserver> mNotificationEnabledSettingObserver;
    /**
      * The {@link System#currentTimeMillis()} must be at least this value for us
      * to show the notification again.
      */
    Int64 mNotificationRepeatTime;
    /**
      * The Notification object given to the NotificationManager.
      */
    AutoPtr<INotification> mNotification;
    /**
      * Whether the notification is being shown, as set by us. That is, if the
      * user cancels the notification, we will not receive the callback so this
      * will still be true. We only guarantee if this is false, then the
      * notification is not showing.
      */
    Boolean mNotificationShown;
    /**
      * The number of continuous scans that must occur before consider the
      * supplicant in a scanning state. This allows supplicant to associate with
      * remembered networks that are in the scan results.
      */
    static const Int32 NUM_SCANS_BEFORE_ACTUALLY_SCANNING = 3;
    /**
      * The number of scans since the last network state change. When this
      * exceeds {@link #NUM_SCANS_BEFORE_ACTUALLY_SCANNING}, we consider the
      * supplicant to actually be scanning. When the network state changes to
      * something other than scanning, we reset this to 0.
      */
    Int32 mNumScansSinceNetworkStateChange;
    AutoPtr<IContext> mContext;
    AutoPtr<WifiStateMachine> mWifiStateMachine;
    AutoPtr<INetworkInfo> mNetworkInfo;
    volatile Int32 mWifiState;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFINOTIFICATIONCONTROLLER_H__


#include "Elastos.Droid.App.h"
#include "elastos/droid/server/wifi/WifiNotificationController.h"
#include "elastos/droid/R.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//         WifiNotificationController::InnerBroadcastReceiver1
//=====================================================================
WifiNotificationController::InnerBroadcastReceiver1::InnerBroadcastReceiver1(
    /* [in] */ WifiNotificationController* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode WifiNotificationController::InnerBroadcastReceiver1::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // if (intent.getAction().equals(WifiManager.WIFI_STATE_CHANGED_ACTION)) {
    //     mWifiState = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE,
    //             WifiManager.WIFI_STATE_UNKNOWN);
    //     resetNotification();
    // } else if (intent.getAction().equals(
    //         WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
    //     mNetworkInfo = (NetworkInfo) intent.getParcelableExtra(
    //             WifiManager.EXTRA_NETWORK_INFO);
    //     // reset & clear notification on a network connect & disconnect
    //     switch(mNetworkInfo.getDetailedState()) {
    //         case CONNECTED:
    //         case DISCONNECTED:
    //         case CAPTIVE_PORTAL_CHECK:
    //             resetNotification();
    //             break;
    //     }
    // } else if (intent.getAction().equals(
    //         WifiManager.SCAN_RESULTS_AVAILABLE_ACTION)) {
    //     checkAndSetNotification(mNetworkInfo,
    //             mWifiStateMachine.syncGetScanResultsList());
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//    WifiNotificationController::NotificationEnabledSettingObserver
//=====================================================================
WifiNotificationController::NotificationEnabledSettingObserver::NotificationEnabledSettingObserver(
    /* [in] */ IHandler* handler)
{
    // ==================before translated======================
    // super(handler);
}

ECode WifiNotificationController::NotificationEnabledSettingObserver::Register()
{
    // ==================before translated======================
    // ContentResolver cr = mContext.getContentResolver();
    // cr.registerContentObserver(Settings.Global.getUriFor(
    //         Settings.Global.WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON), true, this);
    // synchronized (WifiNotificationController.this) {
    //     mNotificationEnabled = getValue();
    // }
    assert(0);
    return NOERROR;
}

ECode WifiNotificationController::NotificationEnabledSettingObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    // ==================before translated======================
    // super.onChange(selfChange);
    //
    // synchronized (WifiNotificationController.this) {
    //     mNotificationEnabled = getValue();
    //     resetNotification();
    // }
    assert(0);
    return NOERROR;
}

Boolean WifiNotificationController::NotificationEnabledSettingObserver::GetValue()
{
    // ==================before translated======================
    // return Settings.Global.getInt(mContext.getContentResolver(),
    //         Settings.Global.WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON, 1) == 1;
    assert(0);
    return FALSE;
}

//=====================================================================
//                      WifiNotificationController
//=====================================================================
const Int32 WifiNotificationController::ICON_NETWORKS_AVAILABLE = Elastos::Droid::R::drawable::stat_notify_wifi_in_range;
const Int32 WifiNotificationController::NUM_SCANS_BEFORE_ACTUALLY_SCANNING;

WifiNotificationController::WifiNotificationController(
    /* [in] */ IContext* context,
    /* [in] */ WifiStateMachine* wsm)
{
    // ==================before translated======================
    // mContext = context;
    // mWifiStateMachine = wsm;
    // mWifiState = WifiManager.WIFI_STATE_UNKNOWN;
    //
    // IntentFilter filter = new IntentFilter();
    // filter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
    // filter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
    // filter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
    //
    // mContext.registerReceiver(
    //         new BroadcastReceiver() {
    //             @Override
    //             public void onReceive(Context context, Intent intent) {
    //                 if (intent.getAction().equals(WifiManager.WIFI_STATE_CHANGED_ACTION)) {
    //                     mWifiState = intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE,
    //                             WifiManager.WIFI_STATE_UNKNOWN);
    //                     resetNotification();
    //                 } else if (intent.getAction().equals(
    //                         WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
    //                     mNetworkInfo = (NetworkInfo) intent.getParcelableExtra(
    //                             WifiManager.EXTRA_NETWORK_INFO);
    //                     // reset & clear notification on a network connect & disconnect
    //                     switch(mNetworkInfo.getDetailedState()) {
    //                         case CONNECTED:
    //                         case DISCONNECTED:
    //                         case CAPTIVE_PORTAL_CHECK:
    //                             resetNotification();
    //                             break;
    //                     }
    //                 } else if (intent.getAction().equals(
    //                         WifiManager.SCAN_RESULTS_AVAILABLE_ACTION)) {
    //                     checkAndSetNotification(mNetworkInfo,
    //                             mWifiStateMachine.syncGetScanResultsList());
    //                 }
    //             }
    //         }, filter);
    //
    // // Setting is in seconds
    // NOTIFICATION_REPEAT_DELAY_MS = Settings.Global.getInt(context.getContentResolver(),
    //         Settings.Global.WIFI_NETWORKS_AVAILABLE_REPEAT_DELAY, 900) * 1000l;
    // mNotificationEnabledSettingObserver = new NotificationEnabledSettingObserver(new Handler());
    // mNotificationEnabledSettingObserver.register();
}

ECode WifiNotificationController::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    VALIDATE_NOT_NULL(fd);
    VALIDATE_NOT_NULL(pw);
    VALIDATE_NOT_NULL(args);
    // ==================before translated======================
    // pw.println("mNotificationEnabled " + mNotificationEnabled);
    // pw.println("mNotificationRepeatTime " + mNotificationRepeatTime);
    // pw.println("mNotificationShown " + mNotificationShown);
    // pw.println("mNumScansSinceNetworkStateChange " + mNumScansSinceNetworkStateChange);
    assert(0);
    return NOERROR;
}

// synchronized
void WifiNotificationController::CheckAndSetNotification(
    /* [in] */ INetworkInfo* networkInfo,
    /* [in] */ IList* scanResults)//IScanResult
{
    // ==================before translated======================
    //
    // // TODO: unregister broadcast so we do not have to check here
    // // If we shouldn't place a notification on available networks, then
    // // don't bother doing any of the following
    // if (!mNotificationEnabled) return;
    // if (mWifiState != WifiManager.WIFI_STATE_ENABLED) return;
    //
    // NetworkInfo.State state = NetworkInfo.State.DISCONNECTED;
    // if (networkInfo != null)
    //     state = networkInfo.getState();
    //
    // if ((state == NetworkInfo.State.DISCONNECTED)
    //         || (state == NetworkInfo.State.UNKNOWN)) {
    //     if (scanResults != null) {
    //         int numOpenNetworks = 0;
    //         for (int i = scanResults.size() - 1; i >= 0; i--) {
    //             ScanResult scanResult = scanResults.get(i);
    //
    //             //A capability of [ESS] represents an open access point
    //             //that is available for an STA to connect
    //             if (scanResult.capabilities != null &&
    //                     scanResult.capabilities.equals("[ESS]")) {
    //                 numOpenNetworks++;
    //             }
    //         }
    //
    //         if (numOpenNetworks > 0) {
    //             if (++mNumScansSinceNetworkStateChange >= NUM_SCANS_BEFORE_ACTUALLY_SCANNING) {
    //                 /*
    //                  * We've scanned continuously at least
    //                  * NUM_SCANS_BEFORE_NOTIFICATION times. The user
    //                  * probably does not have a remembered network in range,
    //                  * since otherwise supplicant would have tried to
    //                  * associate and thus resetting this counter.
    //                  */
    //                 setNotificationVisible(true, numOpenNetworks, false, 0);
    //             }
    //             return;
    //         }
    //     }
    // }
    //
    // // No open networks in range, remove the notification
    // setNotificationVisible(false, 0, false, 0);
    assert(0);
}

// synchronized
void WifiNotificationController::ResetNotification()
{
    // ==================before translated======================
    // mNotificationRepeatTime = 0;
    // mNumScansSinceNetworkStateChange = 0;
    // setNotificationVisible(false, 0, false, 0);
    assert(0);
}

void WifiNotificationController::SetNotificationVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 numNetworks,
    /* [in] */ Boolean force,
    /* [in] */ Int32 delay)
{
    // ==================before translated======================
    //
    // // Since we use auto cancel on the notification, when the
    // // mNetworksAvailableNotificationShown is true, the notification may
    // // have actually been canceled.  However, when it is false we know
    // // for sure that it is not being shown (it will not be shown any other
    // // place than here)
    //
    // // If it should be hidden and it is already hidden, then noop
    // if (!visible && !mNotificationShown && !force) {
    //     return;
    // }
    //
    // NotificationManager notificationManager = (NotificationManager) mContext
    //         .getSystemService(Context.NOTIFICATION_SERVICE);
    //
    // Message message;
    // if (visible) {
    //
    //     // Not enough time has passed to show the notification again
    //     if (System.currentTimeMillis() < mNotificationRepeatTime) {
    //         return;
    //     }
    //
    //     if (mNotification == null) {
    //         // Cache the Notification object.
    //         mNotification = new Notification();
    //         mNotification.when = 0;
    //         mNotification.icon = ICON_NETWORKS_AVAILABLE;
    //         mNotification.flags = Notification.FLAG_AUTO_CANCEL;
    //         mNotification.contentIntent = TaskStackBuilder.create(mContext)
    //                 .addNextIntentWithParentStack(
    //                         new Intent(WifiManager.ACTION_PICK_WIFI_NETWORK))
    //                 .getPendingIntent(0, 0, null, UserHandle.CURRENT);
    //     }
    //
    //     CharSequence title = mContext.getResources().getQuantityText(
    //             com.android.internal.R.plurals.wifi_available, numNetworks);
    //     CharSequence details = mContext.getResources().getQuantityText(
    //             com.android.internal.R.plurals.wifi_available_detailed, numNetworks);
    //     mNotification.tickerText = title;
    //     mNotification.color = mContext.getResources().getColor(
    //             com.android.internal.R.color.system_notification_accent_color);
    //     mNotification.setLatestEventInfo(mContext, title, details, mNotification.contentIntent);
    //
    //     mNotificationRepeatTime = System.currentTimeMillis() + NOTIFICATION_REPEAT_DELAY_MS;
    //
    //     notificationManager.notifyAsUser(null, ICON_NETWORKS_AVAILABLE, mNotification,
    //             UserHandle.ALL);
    // } else {
    //     notificationManager.cancelAsUser(null, ICON_NETWORKS_AVAILABLE, UserHandle.ALL);
    // }
    //
    // mNotificationShown = visible;
    assert(0);
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


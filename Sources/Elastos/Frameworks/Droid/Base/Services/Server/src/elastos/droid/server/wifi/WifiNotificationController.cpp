#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Wifi.h"
#include "elastos/droid/server/wifi/WifiNotificationController.h"
#include "elastos/droid/R.h"
#include "elastos/droid/provider/Settings.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::ITaskStackBuilder;
using Elastos::Droid::App::ITaskStackBuilderHelper;
using Elastos::Droid::App::CTaskStackBuilderHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Net::NetworkInfoState;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;

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
}

ECode WifiNotificationController::InnerBroadcastReceiver1::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IWifiManager::WIFI_STATE_CHANGED_ACTION)) {
        Int32 wifiState;
        intent->GetInt32Extra(IWifiManager::EXTRA_WIFI_STATE,
                IWifiManager::WIFI_STATE_UNKNOWN, &wifiState);
        mOwner->mWifiState = wifiState;
        mOwner->ResetNotification();
    } else if (action.Equals(
            IWifiManager::NETWORK_STATE_CHANGED_ACTION)) {
        AutoPtr<IParcelable> value;
        intent->GetParcelableExtra(IWifiManager::EXTRA_NETWORK_INFO,
                (IParcelable**)&value);
        mOwner->mNetworkInfo = INetworkInfo::Probe(value);
        // reset & clear notification on a network connect & disconnect
        NetworkInfoDetailedState detailedState;
        mOwner->mNetworkInfo->GetDetailedState(&detailedState);
        switch(detailedState) {
            case Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED:
            case Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED:
            case Elastos::Droid::Net::NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK:
                mOwner->ResetNotification();
                break;
        }
    } else if (action.Equals(
            IWifiManager::SCAN_RESULTS_AVAILABLE_ACTION)) {
        AutoPtr<IList> list;
        mOwner->mWifiStateMachine->SyncGetScanResultsList((IList**)&list);
        mOwner->CheckAndSetNotification(mOwner->mNetworkInfo, list);
    }
    return NOERROR;
}

//=====================================================================
//    WifiNotificationController::NotificationEnabledSettingObserver
//=====================================================================
ECode WifiNotificationController::NotificationEnabledSettingObserver::constructor(
    /* [in] */ WifiNotificationController* owner,
    /* [in] */ IHandler* handler)
{
    mOwner = owner;
    return ContentObserver::constructor(handler);
}

ECode WifiNotificationController::NotificationEnabledSettingObserver::Register()
{
    AutoPtr<IContentResolver> cr;
    mOwner->mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> uri;
    Settings::Global::GetUriFor(ISettingsGlobal::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON, (IUri**)&uri);
    cr->RegisterContentObserver(uri, TRUE, this);
    {    AutoLock syncLock(mOwner);
        mOwner->mNotificationEnabled = GetValue();
    }
    return NOERROR;
}

ECode WifiNotificationController::NotificationEnabledSettingObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    ContentObserver::OnChange(selfChange);

    {    AutoLock syncLock(mOwner);
        mOwner->mNotificationEnabled = GetValue();
        mOwner->ResetNotification();
    }
    return NOERROR;
}

Boolean WifiNotificationController::NotificationEnabledSettingObserver::GetValue()
{
    AutoPtr<IContentResolver> cr;
    mOwner->mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 value;
    Settings::Global::GetInt32(cr, ISettingsGlobal::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON, 1, &value);
    return value == 1;
}

//=====================================================================
//                      WifiNotificationController
//=====================================================================
const Int32 WifiNotificationController::ICON_NETWORKS_AVAILABLE = Elastos::Droid::R::drawable::stat_notify_wifi_in_range;
const Int32 WifiNotificationController::NUM_SCANS_BEFORE_ACTUALLY_SCANNING;

WifiNotificationController::WifiNotificationController(
    /* [in] */ IContext* context,
    /* [in] */ WifiStateMachine* wsm)
    : NOTIFICATION_REPEAT_DELAY_MS(0)
    , mNotificationEnabled(FALSE)
    , mNotificationRepeatTime(0)
    , mNotificationShown(FALSE)
    , mNumScansSinceNetworkStateChange(0)
{
    mContext = context;
    mWifiStateMachine = wsm;
    mWifiState = IWifiManager::WIFI_STATE_UNKNOWN;

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IWifiManager::WIFI_STATE_CHANGED_ACTION);
    filter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);
    filter->AddAction(IWifiManager::SCAN_RESULTS_AVAILABLE_ACTION);

    AutoPtr<IBroadcastReceiver> broadcastReceiver = new InnerBroadcastReceiver1(this);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(broadcastReceiver, filter, (IIntent**)&intent);

    // Setting is in seconds
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 value;
    Settings::Global::GetInt32(cr, ISettingsGlobal::WIFI_NETWORKS_AVAILABLE_REPEAT_DELAY, 900, &value);
    NOTIFICATION_REPEAT_DELAY_MS = value * 1000l;
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    mNotificationEnabledSettingObserver = new NotificationEnabledSettingObserver();
    mNotificationEnabledSettingObserver->constructor(this, handler);
    mNotificationEnabledSettingObserver->Register();
}

ECode WifiNotificationController::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
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
    {    AutoLock syncLock(this);
        // TODO: unregister broadcast so we do not have to check here
        // If we shouldn't place a notification on available networks, then
        // don't bother doing any of the following
        if (!mNotificationEnabled) return;
        if (mWifiState != IWifiManager::WIFI_STATE_ENABLED) return;

        NetworkInfoState state = Elastos::Droid::Net::NetworkInfoState_DISCONNECTED;
        if (networkInfo != NULL)
            networkInfo->GetState(&state);

        if ((state == Elastos::Droid::Net::NetworkInfoState_DISCONNECTED)
                || (state == Elastos::Droid::Net::NetworkInfoState_UNKNOWN)) {
            if (scanResults != NULL) {
                Int32 numOpenNetworks = 0;
                Int32 size;
                scanResults->GetSize(&size);
                for (Int32 i = size - 1; i >= 0; i--) {
                    AutoPtr<IInterface> obj;
                    scanResults->Get(i, (IInterface**)&obj);
                    IScanResult* scanResult = IScanResult::Probe(obj);

                    //A capability of [ESS] represents an open access point
                    //that is available for an STA to connect
                    String capabilities;
                    scanResult->GetCapabilities(&capabilities);
                    if (capabilities.Equals(String("[ESS]"))) {
                        numOpenNetworks++;
                    }
                }

                if (numOpenNetworks > 0) {
                    if (++mNumScansSinceNetworkStateChange >= NUM_SCANS_BEFORE_ACTUALLY_SCANNING) {
                        /*
                         * We've scanned continuously at least
                         * NUM_SCANS_BEFORE_NOTIFICATION times. The user
                         * probably does not have a remembered network in range,
                         * since otherwise supplicant would have tried to
                         * associate and thus resetting this counter.
                         */
                        SetNotificationVisible(TRUE, numOpenNetworks, FALSE, 0);
                    }
                    return;
                }
            }
        }

        // No open networks in range, remove the notification
        SetNotificationVisible(FALSE, 0, FALSE, 0);
    }
}

// synchronized
void WifiNotificationController::ResetNotification()
{
    {    AutoLock syncLock(this);
        mNotificationRepeatTime = 0;
        mNumScansSinceNetworkStateChange = 0;
        SetNotificationVisible(FALSE, 0, FALSE, 0);
    }
}

void WifiNotificationController::SetNotificationVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 numNetworks,
    /* [in] */ Boolean force,
    /* [in] */ Int32 delay)
{
    // Since we use auto cancel on the notification, when the
    // mNetworksAvailableNotificationShown is true, the notification may
    // have actually been canceled.  However, when it is false we know
    // for sure that it is not being shown (it will not be shown any other
    // place than here)

    // If it should be hidden and it is already hidden, then noop
    if (!visible && !mNotificationShown && !force) {
        return;
    }

    AutoPtr<IInterface> nmObj;
    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&nmObj);
    AutoPtr<INotificationManager> notificationManager = INotificationManager::Probe(nmObj);

    AutoPtr<IMessage> message;
    if (visible) {

        // Not enough time has passed to show the notification again
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 currentTimeMillis;
        system->GetCurrentTimeMillis(&currentTimeMillis);
        if (currentTimeMillis < mNotificationRepeatTime) {
            return;
        }

        if (mNotification == NULL) {
            // Cache the Notification object.
            CNotification::New((INotification**)&mNotification);
            mNotification->SetWhen(0);
            mNotification->SetIcon(ICON_NETWORKS_AVAILABLE);
            mNotification->SetFlags(INotification::FLAG_AUTO_CANCEL);
            AutoPtr<ITaskStackBuilderHelper> tsbHelper;
            CTaskStackBuilderHelper::AcquireSingleton((ITaskStackBuilderHelper**)&tsbHelper);

            AutoPtr<ITaskStackBuilder> builder;
            tsbHelper->Create(mContext, (ITaskStackBuilder**)&builder);
            AutoPtr<IIntent> intent;
            CIntent::New(IWifiManager::ACTION_PICK_WIFI_NETWORK, (IIntent**)&intent);
            builder->AddNextIntentWithParentStack(intent);
            AutoPtr<IUserHandleHelper> uhHelper;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
            AutoPtr<IUserHandle> uh;
            uhHelper->GetCURRENT((IUserHandle**)&uh);
            AutoPtr<IPendingIntent> pendingIntent;
            builder->GetPendingIntent(0, 0, NULL, uh, (IPendingIntent**)&pendingIntent);
            mNotification->SetContentIntent(pendingIntent);
        }

        AutoPtr<IResources> resources;
        mContext->GetResources((IResources**)&resources);
        AutoPtr<ICharSequence> title;
        resources->GetQuantityText(R::plurals::wifi_available, numNetworks, (ICharSequence**)&title);
        AutoPtr<ICharSequence> details;
        resources->GetQuantityText(R::plurals::wifi_available_detailed, numNetworks, (ICharSequence**)&details);
        mNotification->SetTickerText(title);
        Int32 color;
        resources->GetColor(R::color::system_notification_accent_color, &color);
        mNotification->SetColor(color);
        AutoPtr<IPendingIntent> pendingIntent;
        mNotification->GetContentIntent((IPendingIntent**)&pendingIntent);
        mNotification->SetLatestEventInfo(mContext, title, details, pendingIntent);

        system->GetCurrentTimeMillis(&currentTimeMillis);
        mNotificationRepeatTime = currentTimeMillis + NOTIFICATION_REPEAT_DELAY_MS;

        AutoPtr<IUserHandleHelper> uhHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
        AutoPtr<IUserHandle> uh;
        uhHelper->GetALL((IUserHandle**)&uh);
        notificationManager->NotifyAsUser(String(NULL), ICON_NETWORKS_AVAILABLE, mNotification, uh);
    } else {
        AutoPtr<IUserHandleHelper> uhHelper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
        AutoPtr<IUserHandle> uh;
        uhHelper->GetALL((IUserHandle**)&uh);
        notificationManager->CancelAsUser(String(NULL), ICON_NETWORKS_AVAILABLE, uh);
    }

    mNotificationShown = visible;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


#include "elastos/droid/systemui/statusbar/policy/CNetworkControllerBroadcastReceiver.h"
#include "elastos/droid/systemui/statusbar/policy/NetworkControllerImpl.h"
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::Wimax::IWimaxManagerConstants;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_OBJECT_IMPL(CNetworkControllerBroadcastReceiver);
ECode CNetworkControllerBroadcastReceiver::constructor(
    /* [in] */ INetworkControllerImpl* controller)
{
    mHost = (NetworkControllerImpl*)controller;
    return BroadcastReceiver::constructor();
}

ECode CNetworkControllerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IWifiManager::RSSI_CHANGED_ACTION)
            || action.Equals(IWifiManager::WIFI_STATE_CHANGED_ACTION)
            || action.Equals(IWifiManager::NETWORK_STATE_CHANGED_ACTION)) {
        mHost->UpdateWifiState(intent);
        mHost->RefreshViews();
    }
    else if (action.Equals(ITelephonyIntents::ACTION_SIM_STATE_CHANGED)) {
        mHost->UpdateSimState(intent);
        mHost->UpdateDataIcon();
        mHost->RefreshViews();
    }
    else if (action.Equals(ITelephonyIntents::SPN_STRINGS_UPDATED_ACTION)) {
        Boolean b1 = FALSE, b2 = FALSE;
        intent->GetBooleanExtra(ITelephonyIntents::EXTRA_SHOW_SPN, FALSE, &b1);
        intent->GetBooleanExtra(ITelephonyIntents::EXTRA_SHOW_PLMN, FALSE, &b2);
        String s1, s2;
        intent->GetStringExtra(ITelephonyIntents::EXTRA_SPN, &s1);
        intent->GetStringExtra(ITelephonyIntents::EXTRA_PLMN, &s2);
        mHost->UpdateNetworkName(b1, s1, b2, s2);
        mHost->RefreshViews();
    }
    else if (action.Equals(IConnectivityManager::CONNECTIVITY_ACTION_IMMEDIATE) ||
             action.Equals(IConnectivityManager::INET_CONDITION_ACTION)) {
        mHost->UpdateConnectivity(intent);
        mHost->RefreshViews();
    }
    else if (action.Equals(IIntent::ACTION_CONFIGURATION_CHANGED)) {
        mHost->RefreshLocale();
        mHost->RefreshViews();
    }
    else if (action.Equals(IIntent::ACTION_AIRPLANE_MODE_CHANGED)) {
        mHost->RefreshLocale();
        mHost->UpdateAirplaneMode();
        mHost->RefreshViews();
    }
    else if (action.Equals(IWimaxManagerConstants::NET_4G_STATE_CHANGED_ACTION) ||
            action.Equals(IWimaxManagerConstants::SIGNAL_LEVEL_CHANGED_ACTION) ||
            action.Equals(IWimaxManagerConstants::WIMAX_NETWORK_STATE_CHANGED_ACTION)) {
        mHost->UpdateWimaxState(intent);
        mHost->RefreshViews();
    }
    return NOERROR;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

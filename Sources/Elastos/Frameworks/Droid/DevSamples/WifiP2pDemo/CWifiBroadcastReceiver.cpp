#include "CWifiBroadcastReceiver.h"
#include "CActivityOne.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiP2pDemo {

CWifiBroadcastReceiver::CWifiBroadcastReceiver(
    /* [in] */ IWifiP2pManager* wpm,
    /* [in] */ IWifiP2pManagerChannel* wpmc,
    /* [in] */ CActivityOne* activity)
    : mP2pManager(wpm)
    , mChannel(wpmc)
    , mActivity(activity)
{
}

ECode CWifiBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    Slogger::D("CWifiBroadcastReceiver", "OnReceive: [%s]", action.string());

    if (IWifiP2pManager::WIFI_P2P_STATE_CHANGED_ACTION.Equals(action)) {
        // Determine if Wifi P2P mode is enabled or not, alert the Activity.
        Int32 state;
        intent->GetInt32Extra(IWifiP2pManager::EXTRA_WIFI_STATE, -1, &state);
        if (state == IWifiP2pManager::WIFI_P2P_STATE_ENABLED) {
            mActivity->SetIsWifiP2pEnabled(TRUE);
        }
        else {
            mActivity->SetIsWifiP2pEnabled(FALSE);
        }
        Slogger::D("CWifiBroadcastReceiver", "P2P state changed - %d", state);
    }
    else if (IWifiP2pManager::WIFI_P2P_PEERS_CHANGED_ACTION.Equals(action)) {
        mActivity->OnPeersChanged();
    }
    else if (IWifiP2pManager::WIFI_P2P_CONNECTION_CHANGED_ACTION.Equals(action)) {
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(IWifiP2pManager::EXTRA_NETWORK_INFO, (IParcelable**)&parcelable);
        AutoPtr<INetworkInfo> networkInfo = INetworkInfo::Probe(parcelable);
        assert(networkInfo != NULL);
        mActivity->OnConnectionChanged(networkInfo);
    }
    else if (IWifiP2pManager::WIFI_P2P_THIS_DEVICE_CHANGED_ACTION.Equals(action)) {
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(IWifiP2pManager::EXTRA_WIFI_P2P_DEVICE, (IParcelable**)&parcelable);
        AutoPtr<IWifiP2pDevice> deviceInfo = IWifiP2pDevice::Probe(parcelable);
        assert(deviceInfo != NULL);
        mActivity->OnThisDeviceChanged(deviceInfo);
    }

    return NOERROR;
}


} // namespace WifiP2pDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

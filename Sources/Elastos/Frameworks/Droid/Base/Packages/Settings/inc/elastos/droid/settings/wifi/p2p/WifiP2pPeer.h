

package com.android.settings.wifi.p2p;

using Elastos::Droid::Settings::IR;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Net::Wifi::P2p::IWifiP2pDevice;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;

public class WifiP2pPeer extends Preference {

    private static const Int32[] STATE_SECURED = {R.attr.state_encrypted};
    public WifiP2pDevice device;

    private final Int32 mRssi;
    private ImageView mSignal;

    private static const Int32 SIGNAL_LEVELS = 4;

    public WifiP2pPeer(Context context, WifiP2pDevice dev) {
        Super(context);
        device = dev;
        SetWidgetLayoutResource(R.layout.preference_widget_wifi_signal);
        mRssi = 60; //TODO: fix
    }

    //@Override
    protected void OnBindView(View view) {
        if (TextUtils->IsEmpty(device.deviceName)) {
            SetTitle(device.deviceAddress);
        } else {
            SetTitle(device.deviceName);
        }
        mSignal = (ImageView) view->FindViewById(R.id.signal);
        if (mRssi == Integer.MAX_VALUE) {
            mSignal->SetImageDrawable(NULL);
        } else {
            mSignal->SetImageResource(R.drawable.wifi_signal_dark);
            mSignal->SetImageState(STATE_SECURED,  TRUE);
        }
        Refresh();
        super->OnBindView(view);
    }

    //@Override
    public Int32 CompareTo(Preference preference) {
        if (!(preference instanceof WifiP2pPeer)) {
            return 1;
        }
        WifiP2pPeer other = (WifiP2pPeer) preference;

        // devices go in the order of the status
        if (device.status != other.device.status) {
            return device.status < other.device.status ? -1 : 1;
        }

        // Sort by name/address
        if (device.deviceName != NULL) {
            return device.deviceName->CompareToIgnoreCase(other.device.deviceName);
        }

        return device.deviceAddress->CompareToIgnoreCase(other.device.deviceAddress);
    }

    Int32 GetLevel() {
        if (mRssi == Integer.MAX_VALUE) {
            return -1;
        }
        return WifiManager->CalculateSignalLevel(mRssi, SIGNAL_LEVELS);
    }

    private void Refresh() {
        if (mSignal == NULL) {
            return;
        }
        Context context = GetContext();
        mSignal->SetImageLevel(GetLevel());
        String[] statusArray = context->GetResources()->GetStringArray(R.array.wifi_p2p_status);
        SetSummary(statusArray[device.status]);
    }
}

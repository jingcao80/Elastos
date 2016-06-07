
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Net.h>
#include "elastos/droid/settings/wifi/WifiEnabler.h"
#include "elastos/droid/settings/search/Index.h"
#include "elastos/droid/settings/WirelessSettings.h"
#include "../R.h"

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Settings::Search::Index;
using Elastos::Droid::Settings::Widget::EIID_ISwitchBarOnSwitchChangeListener;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Wifi::ISupplicantState;
using Elastos::Droid::Wifi::CWifiInfoHelper;
using Elastos::Droid::Wifi::IWifiInfoHelper;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

const String WifiEnabler::EVENT_DATA_IS_WIFI_ON("is_wifi_on");
const Int32 WifiEnabler::EVENT_UPDATE_INDEX;

//===============================================================================
//                  WifiEnabler::InitBroadcastReceiver
//===============================================================================

WifiEnabler::InitBroadcastReceiver::InitBroadcastReceiver(
    /* [in] */ WifiEnabler* host)
    : mHost(host)
{}

WifiEnabler::InitBroadcastReceiver::~InitBroadcastReceiver()
{}

ECode WifiEnabler::InitBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IWifiManager::WIFI_STATE_CHANGED_ACTION.Equals(action)) {
        Int32 value;
        intent->GetInt32Extra(
                IWifiManager::EXTRA_WIFI_STATE,
                IWifiManager::WIFI_STATE_UNKNOWN, &value);
        mHost->HandleWifiStateChanged(value);
    }
    else if (IWifiManager::SUPPLICANT_STATE_CHANGED_ACTION.Equals(action)) {
        Boolean res;
        if (mHost->mConnected->Get(&res), !res) {
            AutoPtr<IParcelable> parcel;
            intent->GetParcelableExtra(IWifiManager::EXTRA_NEW_STATE, (IParcelable**)&parcel);
            AutoPtr<IWifiInfoHelper> helper;
            CWifiInfoHelper::AcquireSingleton((IWifiInfoHelper**)&helper);
            NetworkInfoDetailedState state;
            helper->GetDetailedStateOf(ISupplicantState::Probe(parcel), &state);
            mHost->HandleStateChanged(state);
        }
    }
    else if (IWifiManager::NETWORK_STATE_CHANGED_ACTION.Equals(action)) {
        AutoPtr<IParcelable> parcel;
        intent->GetParcelableExtra(IWifiManager::EXTRA_NETWORK_INFO, (IParcelable**)&parcel);
        AutoPtr<INetworkInfo> info = INetworkInfo::Probe(intent);
        Boolean isConnected;
        info->IsConnected(&isConnected);
        mHost->mConnected->Set(isConnected);
        NetworkInfoDetailedState state;
        info->GetDetailedState(&state);
        mHost->HandleStateChanged(state);
    }
    return NOERROR;
}

//===============================================================================
//                  WifiEnabler::InitHandler
//===============================================================================

WifiEnabler::InitHandler::InitHandler(
    /* [in] */ WifiEnabler* host)
    : mHost(host)
{}

WifiEnabler::InitHandler::~InitHandler()
{}

ECode WifiEnabler::InitHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_UPDATE_INDEX:
            AutoPtr<IBundle> data;
            msg->GetData((IBundle**)&data);
            Boolean isWiFiOn;
            data->GetBoolean(EVENT_DATA_IS_WIFI_ON, &isWiFiOn);
            Index::GetInstance(mHost->mContext)->UpdateFromClassNameResource(
                    String("Elastos.Droid.Settings.Wifi.CWifiSettings"),
                    TRUE, isWiFiOn);
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  WifiEnabler
//===============================================================================

CAR_INTERFACE_IMPL(WifiEnabler, Object, ISwitchBarOnSwitchChangeListener);

WifiEnabler::WifiEnabler(
    /* [in] */ IContext* context,
    /* [in] */ SwitchBar* switchBar)
    : mListeningToOnSwitchChange(FALSE)
    , mStateMachineEvent(FALSE)
{
    mContext = context;
    mSwitchBar = switchBar;

    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mConnected);
    mReceiver = new InitBroadcastReceiver(this);
    mHandler = new InitHandler(this);
    mHandler->constructor();

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    mWifiManager = IWifiManager::Probe(obj);

    CIntentFilter::New(IWifiManager::WIFI_STATE_CHANGED_ACTION, (IIntentFilter**)&mIntentFilter);
    // The order matters! We really should not depend on this. :(
    mIntentFilter->AddAction(IWifiManager::SUPPLICANT_STATE_CHANGED_ACTION);
    mIntentFilter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);

    SetupSwitchBar();
}

WifiEnabler::~WifiEnabler()
{}

ECode WifiEnabler::SetupSwitchBar()
{
    Int32 state;
    mWifiManager->GetWifiState(&state);
    HandleWifiStateChanged(state);
    if (!mListeningToOnSwitchChange) {
        mSwitchBar->AddOnSwitchChangeListener((ISwitchBarOnSwitchChangeListener*)this);
        mListeningToOnSwitchChange = TRUE;
    }
    mSwitchBar->Show();
    return NOERROR;
}

ECode WifiEnabler::TeardownSwitchBar()
{
    if (mListeningToOnSwitchChange) {
        mSwitchBar->RemoveOnSwitchChangeListener((ISwitchBarOnSwitchChangeListener*)this);
        mListeningToOnSwitchChange = FALSE;
    }
    mSwitchBar->Hide();
    return NOERROR;
}

ECode WifiEnabler::Resume(
    /* [in] */ IContext* context)
{
    mContext = context;
    // Wi-Fi state is sticky, so just let the receiver update UI
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mReceiver, mIntentFilter, (IIntent**)&intent);
    if (!mListeningToOnSwitchChange) {
        mSwitchBar->AddOnSwitchChangeListener((ISwitchBarOnSwitchChangeListener*)this);
        mListeningToOnSwitchChange = TRUE;
    }
    return NOERROR;
}

ECode WifiEnabler::Pause()
{
    mContext->UnregisterReceiver(mReceiver);
    if (mListeningToOnSwitchChange) {
        mSwitchBar->RemoveOnSwitchChangeListener((ISwitchBarOnSwitchChangeListener*)this);
        mListeningToOnSwitchChange = FALSE;
    }
    return NOERROR;
}

void WifiEnabler::HandleWifiStateChanged(
    /* [in] */ Int32 state)
{
    switch (state) {
        case IWifiManager::WIFI_STATE_ENABLING:
            mSwitchBar->SetEnabled(FALSE);
            break;
        case IWifiManager::WIFI_STATE_ENABLED:
            SetSwitchBarChecked(TRUE);
            mSwitchBar->SetEnabled(TRUE);
            UpdateSearchIndex(TRUE);
            break;
        case IWifiManager::WIFI_STATE_DISABLING:
            mSwitchBar->SetEnabled(FALSE);
            break;
        case IWifiManager::WIFI_STATE_DISABLED:
            SetSwitchBarChecked(FALSE);
            mSwitchBar->SetEnabled(TRUE);
            UpdateSearchIndex(FALSE);
            break;
        default:
            SetSwitchBarChecked(FALSE);
            mSwitchBar->SetEnabled(TRUE);
            UpdateSearchIndex(FALSE);
    }
}

void WifiEnabler::UpdateSearchIndex(
    /* [in] */ Boolean isWiFiOn)
{
    mHandler->RemoveMessages(EVENT_UPDATE_INDEX);

    AutoPtr<IMessage> msg;
    CMessage::New((IMessage**)&msg);
    msg->SetWhat(EVENT_UPDATE_INDEX);
    AutoPtr<IBundle> data;
    msg->GetData((IBundle**)&data);
    data->PutBoolean(EVENT_DATA_IS_WIFI_ON, isWiFiOn);
    Boolean res;
    mHandler->SendMessage(msg, &res);
}

void WifiEnabler::SetSwitchBarChecked(
    /* [in] */ Boolean checked)
{
    mStateMachineEvent = TRUE;
    mSwitchBar->SetChecked(checked);
    mStateMachineEvent = FALSE;
}

void WifiEnabler::HandleStateChanged(
    /* [in] */ NetworkInfoDetailedState state)
{
    // After the refactoring from a CheckBoxPreference to a Switch, this method is useless since
    // there is nowhere to display a summary.
    // This code is kept in case a future change re-introduces an associated text.
    /*
    // WifiInfo is valid if and only if Wi-Fi is enabled.
    // Here we use the state of the switch as an optimization.
    if (state != NULL && mSwitch->IsChecked()) {
        WifiInfo info = mWifiManager->GetConnectionInfo();
        if (info != NULL) {
            //SetSummary(Summary->Get(mContext, info->GetSSID(), state));
        }
    }
    */
}

ECode WifiEnabler::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    //Do nothing if called as a result of a state machine event
    if (mStateMachineEvent) {
        return NOERROR;
    }

    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);

    // Show toast message if Wi-Fi is not allowed in airplane mode
    if (isChecked && !WirelessSettings::IsRadioAllowed(mContext, ISettingsGlobal::RADIO_WIFI)) {
        AutoPtr<IToast> toast;
        helper->MakeText(mContext, R::string::wifi_in_airplane_mode,
                IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
        // Reset switch to off. No infinite check/listenenr loop.
        mSwitchBar->SetChecked(FALSE);
        return NOERROR;
    }

    // Disable tethering if enabling Wifi
    Boolean res;
    Int32 wifiApState;
    mWifiManager->GetWifiApState(&wifiApState);
    if (isChecked && ((wifiApState == IWifiManager::WIFI_AP_STATE_ENABLING) ||
            (wifiApState == IWifiManager::WIFI_AP_STATE_ENABLED))) {
        mWifiManager->SetWifiApEnabled(NULL, FALSE, &res);
    }

    if (mWifiManager->SetWifiEnabled(isChecked, &res), !res) {
        // Error
        mSwitchBar->SetEnabled(TRUE);
        AutoPtr<IToast> toast;
        helper->MakeText(mContext, R::string::wifi_error,
                IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
    }
    return NOERROR;
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

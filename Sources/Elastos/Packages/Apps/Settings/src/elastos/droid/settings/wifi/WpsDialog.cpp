
#include "Elastos.Droid.Net.h"
#include "elastos/droid/settings/wifi/WpsDialog.h"
#include "elastos/droid/settings/wifi/CWpsDialogHelper.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Wifi::EIID_IWifiManagerWpsCallback;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

const String WpsDialog::TAG("WpsDialog");
const String WpsDialog::DIALOG_STATE("android:dialogState");
const String WpsDialog::DIALOG_MSG_STRING("android:dialogMsg");

const Int32 WpsDialog::WPS_TIMEOUT_S = 120;

//===============================================================================
//                  WpsDialog::WpsListener
//===============================================================================

CAR_INTERFACE_IMPL(WpsDialog::WpsListener, Object, IWifiManagerWpsCallback);

WpsDialog::WpsListener::WpsListener(
    /* [in] */ WpsDialog* host)
    : mHost(host)
{}

WpsDialog::WpsListener::~WpsListener()
{}

ECode WpsDialog::WpsListener::OnStarted(
    /* [in] */ const String& pin)
{
    if (!pin.IsNull()) {
        String str;
        mHost->mContext->GetString(R::string::wifi_wps_onstart_pin, &str);
        String form("");
        form.AppendFormat(str, pin.string());
        mHost->UpdateDialog(WpsDialogState_WPS_START, form);
    }
    else {
        String str;
        mHost->mContext->GetString(R::string::wifi_wps_onstart_pbc, &str);
        mHost->UpdateDialog(WpsDialogState_WPS_START, str);
    }
    return NOERROR;
}

ECode WpsDialog::WpsListener::OnSucceeded()
{
    String str;
    mHost->mContext->GetString(R::string::wifi_wps_complete, &str);
    mHost->UpdateDialog(WpsDialogState_WPS_COMPLETE, str);
    return NOERROR;
}

ECode WpsDialog::WpsListener::OnFailed(
    /* [in] */ Int32 reason)
{
    String msg;
    switch (reason) {
        case IWifiManager::WPS_OVERLAP_ERROR:
            mHost->mContext->GetString(R::string::wifi_wps_failed_overlap, &msg);
            break;
        case IWifiManager::WPS_WEP_PROHIBITED:
            mHost->mContext->GetString(R::string::wifi_wps_failed_wep, &msg);
            break;
        case IWifiManager::WPS_TKIP_ONLY_PROHIBITED:
            mHost->mContext->GetString(R::string::wifi_wps_failed_tkip, &msg);
            break;
        case IWifiManager::IN_PROGRESS:
            mHost->mContext->GetString(R::string::wifi_wps_in_progress, &msg);
            break;
        default:
            mHost->mContext->GetString(R::string::wifi_wps_failed_generic, &msg);
            break;
    }
    mHost->UpdateDialog(WpsDialogState_WPS_FAILED, msg);
    return NOERROR;
}

//===============================================================================
//                  WpsDialog::InitBroadcastReceiver
//===============================================================================

WpsDialog::InitBroadcastReceiver::InitBroadcastReceiver(
    /* [in] */ WpsDialog* host)
    : mHost(host)
{}

WpsDialog::InitBroadcastReceiver::~InitBroadcastReceiver()
{}

ECode WpsDialog::InitBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->HandleEvent(context, intent);
    return NOERROR;
}

//===============================================================================
//                  WpsDialog::UpdateDialogRunnable
//===============================================================================

WpsDialog::UpdateDialogRunnable::UpdateDialogRunnable(
    /* [in] */ WpsDialog* host,
    /* [in] */ WpsDialogState state,
    /* [in] */ const String& msg)
    : mHost(host)
    , mState(state)
    , mMsg(msg)
{}

WpsDialog::UpdateDialogRunnable::~UpdateDialogRunnable()
{}

ECode WpsDialog::UpdateDialogRunnable::Run()
{
    switch(mState) {
        case WpsDialogState_WPS_COMPLETE:
            IView::Probe(mHost->mTimeoutBar)->SetVisibility(IView::GONE);
            IView::Probe(mHost->mProgressBar)->SetVisibility(IView::VISIBLE);
            break;
        case WpsDialogState_CONNECTED:
        case WpsDialogState_WPS_FAILED: {
            String str;
            mHost->mContext->GetString(R::string::dlg_ok, &str);
            ITextView::Probe(mHost->mButton)->SetText(CoreUtils::Convert(str));
            IView::Probe(mHost->mTimeoutBar)->SetVisibility(IView::GONE);
            IView::Probe(mHost->mProgressBar)->SetVisibility(IView::GONE);
            if (mHost->mReceiver != NULL) {
                mHost->mContext->UnregisterReceiver((IBroadcastReceiver*)mHost->mReceiver);
                mHost->mReceiver = NULL;
            }
            break;
        }
    }
    return mHost->mTextView->SetText(CoreUtils::Convert(mMsg));
}

//===============================================================================
//                  WpsDialog
//===============================================================================

WpsDialog::WpsDialog(
    /* [in] */ IContext* context,
    /* [in] */ Int32 wpsSetup)
    : mWpsSetup(0)
    , mMsgString(String(""))
    , mDialogState(WpsDialogState_WPS_INIT)
{
    AlertDialog::constructor(context);
    mContext = context;
    mWpsSetup = wpsSetup;

    CHandler::New((IHandler**)&mHandler);

    mWpsListener = new WpsListener(this);

    CIntentFilter::New((IIntentFilter**)&mFilter);
    mFilter->AddAction(IWifiManager::NETWORK_STATE_CHANGED_ACTION);
    mReceiver = new InitBroadcastReceiver(this);
    SetCanceledOnTouchOutside(FALSE);
}

WpsDialog::~WpsDialog()
{}

void WpsDialog::UpdateDialog(
    /* [in] */ WpsDialogState state,
    /* [in] */ const String& msg)
{
    AutoPtr<IWpsDialogHelper> helper;
    CWpsDialogHelper::AcquireSingleton((IWpsDialogHelper**)&helper);
    Int32 index1, index2;
    helper->Index(mDialogState, &index1);
    helper->Index(state, &index2);
    if (index1 >= index2) {
        //ignore.
        return;
    }
    mDialogState = state;
    mMsgString = msg;

    AutoPtr<UpdateDialogRunnable> runnable = new UpdateDialogRunnable(this, state, msg);

    Boolean res;
    mHandler->Post((IRunnable*)runnable, &res);
}

void WpsDialog::HandleEvent(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IWifiManager::NETWORK_STATE_CHANGED_ACTION.Equals(action)) {
        AutoPtr<IParcelable> parcel;
        intent->GetParcelableExtra(
                IWifiManager::EXTRA_NETWORK_INFO, (IParcelable**)&parcel);
        AutoPtr<INetworkInfo> info = INetworkInfo::Probe(parcel);
        NetworkInfoDetailedState state;
        info->GetDetailedState(&state);
        if (state == NetworkInfoDetailedState_CONNECTED &&
                mDialogState == WpsDialogState_WPS_COMPLETE) {
            AutoPtr<IWifiInfo> wifiInfo;
            mWifiManager->GetConnectionInfo((IWifiInfo**)&wifiInfo);
            if (wifiInfo != NULL) {
                String str;
                mContext->GetString(
                        R::string::wifi_wps_connected, &str);
                String ssid;
                wifiInfo->GetSSID(&ssid);
                String msg("");
                msg.AppendFormat(str, ssid.string());
                UpdateDialog(WpsDialogState_CONNECTED, msg);
            }
        }
    }
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

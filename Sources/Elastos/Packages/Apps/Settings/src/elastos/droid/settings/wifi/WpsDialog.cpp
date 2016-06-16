
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

ECode WpsDialog::OnSaveInstanceState(
    /* [out] */ IBundle** result)
{
    assert(0 && "TODO");
    // Bundle bundle = AlertDialog::OnSaveInstanceState();
    // bundle->PutString(DIALOG_STATE, mDialogState->ToString());
    // bundle->PutString(DIALOG_MSG_STRING, mMsgString->ToString());
    // return bundle;
    return NOERROR;
}

ECode WpsDialog::OnRestoreInstanceState(
    /* [in] */ IBundle* savedInstanceState)
{
    assert(0 && "TODO");
    // if (savedInstanceState != NULL) {
    //     AlertDialog::OnRestoreInstanceState(savedInstanceState);
    //     WpsDialogState dialogState = mDialogState->ValueOf(savedInstanceState->GetString(DIALOG_STATE));
    //     String msg = savedInstanceState->GetString(DIALOG_MSG_STRING);
    //     UpdateDialog(dialogState, msg);
    // }
    return NOERROR;
}

ECode WpsDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    assert(0 && "TODO");
    // mView = GetLayoutInflater()->Inflate(R::layout::wifi_wps_dialog, NULL);

    // mTextView = (TextView) mView->FindViewById(R::id::wps_dialog_txt);
    // mTextView->SetText(R::string::wifi_wps_setup_msg);

    // mTimeoutBar = ((ProgressBar) mView->FindViewById(R::id::wps_timeout_bar));
    // mTimeoutBar->SetMax(WPS_TIMEOUT_S);
    // mTimeoutBar->SetProgress(0);

    // mProgressBar = ((ProgressBar) mView->FindViewById(R::id::wps_progress_bar));
    // mProgressBar->SetVisibility(IView::GONE);

    // mButton = ((Button) mView->FindViewById(R::id::wps_dialog_btn));
    // mButton->SetText(R::string::wifi_cancel);
    // mButton->SetOnClickListener(new View->OnClickListener() {
    //     //@Override
    //     CARAPI OnClick(View v) {
    //         Dismiss();
    //     }
    // });

    // mWifiManager = (WifiManager) mContext->GetSystemService(IContext::WIFI_SERVICE);

    // SetView(mView);
    // AlertDialog::OnCreate(savedInstanceState);
    return NOERROR;
}

ECode WpsDialog::OnStart()
{
    assert(0 && "TODO");
    // /*
    //  * increment timeout bar per second.
    //  */
    // mTimer = new Timer(FALSE);
    // mTimer->Schedule(new TimerTask() {
    //     //@Override
    //     CARAPI Run() {
    //         mHandler->Post(new Runnable() {

    //             //@Override
    //             CARAPI Run() {
    //                 mTimeoutBar->IncrementProgressBy(1);
    //             }
    //         });
    //     }
    // }, 1000, 1000);

    // mContext->RegisterReceiver((IBroadcastReceiver*)mReceiver, mFilter);

    // WpsInfo wpsConfig = new WpsInfo();
    // wpsConfig.setup = mWpsSetup;
    // mWifiManager->StartWps(wpsConfig, mWpsListener);
}

ECode WpsDialog::OnStop()
{
    assert(0 && "TODO");
    // if (mDialogState != WpsDialogState_WPS_COMPLETE) {
    //     mWifiManager->CancelWps(NULL);
    // }

    // if (mReceiver != NULL) {
    //     mContext->UnregisterReceiver((IBroadcastReceiver*)mReceiver);
    //     mReceiver = NULL;
    // }

    // if (mTimer != NULL) {
    //     mTimer->Cancel();
    // }
}

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

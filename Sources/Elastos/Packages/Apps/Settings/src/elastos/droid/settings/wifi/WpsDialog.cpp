
#include "Elastos.Droid.Net.h"
#include "elastos/droid/settings/wifi/WpsDialog.h"
#include "elastos/droid/settings/wifi/CWpsDialogHelper.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Wifi::IWpsInfo;
using Elastos::Droid::Wifi::CWpsInfo;
using Elastos::Droid::Wifi::EIID_IWifiManagerWpsCallback;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CTimer;

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
//                  WpsDialog::OnCreateViewOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(WpsDialog::OnCreateViewOnClickListener, Object, IViewOnClickListener);

WpsDialog::OnCreateViewOnClickListener::OnCreateViewOnClickListener(
    /* [in] */ WpsDialog* host)
    : mHost(host)
{}

WpsDialog::OnCreateViewOnClickListener::~OnCreateViewOnClickListener()
{}

ECode WpsDialog::OnCreateViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->Dismiss();
}

//===============================================================================
//                  WpsDialog::OnStartRunnable
//===============================================================================

WpsDialog::OnStartRunnable::OnStartRunnable(
    /* [in] */ WpsDialog* host)
    : mHost(host)
{}

WpsDialog::OnStartRunnable::~OnStartRunnable()
{}

ECode WpsDialog::OnStartRunnable::Run()
{
    return mHost->mTimeoutBar->IncrementProgressBy(1);
}

//===============================================================================
//                  WpsDialog::OnStartTimerTask
//===============================================================================

WpsDialog::OnStartTimerTask::OnStartTimerTask(
    /* [in] */ WpsDialog* host)
    : mHost(host)
{}

WpsDialog::OnStartTimerTask::~OnStartTimerTask()
{}

ECode WpsDialog::OnStartTimerTask::Run()
{
    AutoPtr<IRunnable> runnable = new OnStartRunnable(mHost);
    Boolean res;
    return mHost->mHandler->Post(runnable, &res);
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
    AutoPtr<IBundle> bundle;
    AlertDialog::OnSaveInstanceState((IBundle**)&bundle);

    AutoPtr<IWpsDialogHelper> helper;
    CWpsDialogHelper::AcquireSingleton((IWpsDialogHelper**)&helper);
    String str;
    helper->WpsDialogStateToString(mDialogState, &str);
    bundle->PutString(DIALOG_STATE, str);
    bundle->PutString(DIALOG_MSG_STRING, mMsgString);
    *result = bundle;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode WpsDialog::OnRestoreInstanceState(
    /* [in] */ IBundle* savedInstanceState)
{
    if (savedInstanceState != NULL) {
        AlertDialog::OnRestoreInstanceState(savedInstanceState);

        AutoPtr<IWpsDialogHelper> helper;
        CWpsDialogHelper::AcquireSingleton((IWpsDialogHelper**)&helper);

        String dialogStateStr;
        savedInstanceState->GetString(DIALOG_STATE, &dialogStateStr);

        WpsDialogState dialogState;
        helper->WpsDialogStateFromString(dialogStateStr, &dialogState);

        String msg;
        savedInstanceState->GetString(DIALOG_MSG_STRING, &msg);
        UpdateDialog(dialogState, msg);
    }
    return NOERROR;
}

ECode WpsDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    AutoPtr<ILayoutInflater> inflater;
    GetLayoutInflater((ILayoutInflater**)&inflater);
    inflater->Inflate(R::layout::wifi_wps_dialog, NULL, (IView**)&mView);

    AutoPtr<IView> view;
    mView->FindViewById(R::id::wps_dialog_txt, (IView**)&view);
    mTextView = ITextView::Probe(view);
    mTextView->SetText(R::string::wifi_wps_setup_msg);

    view = NULL;
    mView->FindViewById(R::id::wps_timeout_bar, (IView**)&view);
    mTimeoutBar = IProgressBar::Probe(view);
    mTimeoutBar->SetMax(WPS_TIMEOUT_S);
    mTimeoutBar->SetProgress(0);

    view = NULL;
    mView->FindViewById(R::id::wps_progress_bar, (IView**)&view);
    mProgressBar = IProgressBar::Probe(view);
    view->SetVisibility(IView::GONE);

    view = NULL;
    mView->FindViewById(R::id::wps_dialog_btn, (IView**)&view);
    mButton = IButton::Probe(view);
    ITextView::Probe(mButton)->SetText(R::string::wifi_cancel);
    AutoPtr<IViewOnClickListener> listener = new OnCreateViewOnClickListener(this);
    view->SetOnClickListener(listener);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    mWifiManager = IWifiManager::Probe(obj);

    SetView(mView);
    return AlertDialog::OnCreate(savedInstanceState);
}

ECode WpsDialog::OnStart()
{
    /*
     * increment timeout bar per second.
     */
    CTimer::New(FALSE, (ITimer**)&mTimer);
    AutoPtr<OnStartTimerTask> task = new OnStartTimerTask(this);
    mTimer->Schedule(task, 1000, 1000);

    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver((IBroadcastReceiver*)mReceiver, mFilter, (IIntent**)&intent);

    AutoPtr<IWpsInfo> wpsConfig;
    CWpsInfo::New((IWpsInfo**)&wpsConfig);
    wpsConfig->SetSetup(mWpsSetup);
    mWifiManager->StartWps(wpsConfig, mWpsListener);
    return NOERROR;
}

ECode WpsDialog::OnStop()
{
    if (mDialogState != WpsDialogState_WPS_COMPLETE) {
        mWifiManager->CancelWps(NULL);
    }

    if (mReceiver != NULL) {
        mContext->UnregisterReceiver((IBroadcastReceiver*)mReceiver);
        mReceiver = NULL;
    }

    if (mTimer != NULL) {
        mTimer->Cancel();
    }
    return NOERROR;
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

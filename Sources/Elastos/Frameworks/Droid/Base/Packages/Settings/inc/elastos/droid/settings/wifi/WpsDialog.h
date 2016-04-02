#ifndef __ELASTOS_DROID_SETTINGS_WIFI_WPSDIALOG_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_WPSDIALOG_H__

#include "Elastos.Droid.Wifi.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/AlertDialog.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "_Settings.h"

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Wifi::IWifiManagerWpsCallback;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::IWpsInfo;

// using Elastos::Utility::ITimer;
// using Elastos::Utility::ITimerTask;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

/**
 * Dialog to show WPS progress.
 */
class WpsDialog
    : public AlertDialog
{
private:
    class WpsListener
        : public Object
        , public IWifiManagerWpsCallback
    {
    public:
        CAR_INTERFACE_DECL();

        WpsListener(
            /* [in] */ WpsDialog* host);

        ~WpsListener();

        CARAPI OnStarted(
            /* [in] */ const String& pin);

        CARAPI OnSucceeded();

        CARAPI OnFailed(
            /* [in] */ Int32 reason);

    private:
        WpsDialog* mHost;
    };

    class InitBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        InitBroadcastReceiver(
            /* [in] */ WpsDialog* host);

        ~InitBroadcastReceiver();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WpsDialog* mHost;
    };

    class UpdateDialogRunnable
        : public Runnable
    {
    public:
        UpdateDialogRunnable(
            /* [in] */ WpsDialog* host,
            /* [in] */ WpsDialogState state,
            /* [in] */ const String& msg);

        ~UpdateDialogRunnable();

        //@Override
        CARAPI Run();

    private:
        WpsDialog* mHost;
        WpsDialogState mState;
        String mMsg;
    };

public:
    WpsDialog(
        /* [in] */ IContext* context,
        /* [in] */ Int32 wpsSetup);

    ~WpsDialog();

    // //@Override
    // public Bundle OnSaveInstanceState () {
    //     Bundle bundle  = super->OnSaveInstanceState();
    //     bundle->PutString(DIALOG_STATE, mDialogState->ToString());
    //     bundle->PutString(DIALOG_MSG_STRING, mMsgString->ToString());
    //     return bundle;
    // }

    // //@Override
    // CARAPI OnRestoreInstanceState(Bundle savedInstanceState) {
    //     if (savedInstanceState != NULL) {
    //         super->OnRestoreInstanceState(savedInstanceState);
    //         WpsDialogState dialogState = mDialogState->ValueOf(savedInstanceState->GetString(DIALOG_STATE));
    //         String msg = savedInstanceState->GetString(DIALOG_MSG_STRING);
    //         UpdateDialog(dialogState, msg);
    //     }
    // }

    // //@Override
    // protected void OnCreate(Bundle savedInstanceState) {
    //     mView = GetLayoutInflater()->Inflate(R::layout::wifi_wps_dialog, NULL);

    //     mTextView = (TextView) mView->FindViewById(R::id::wps_dialog_txt);
    //     mTextView->SetText(R::string::wifi_wps_setup_msg);

    //     mTimeoutBar = ((ProgressBar) mView->FindViewById(R::id::wps_timeout_bar));
    //     mTimeoutBar->SetMax(WPS_TIMEOUT_S);
    //     mTimeoutBar->SetProgress(0);

    //     mProgressBar = ((ProgressBar) mView->FindViewById(R::id::wps_progress_bar));
    //     mProgressBar->SetVisibility(IView::GONE);

    //     mButton = ((Button) mView->FindViewById(R::id::wps_dialog_btn));
    //     mButton->SetText(R::string::wifi_cancel);
    //     mButton->SetOnClickListener(new View->OnClickListener() {
    //         //@Override
    //         CARAPI OnClick(View v) {
    //             Dismiss();
    //         }
    //     });

    //     mWifiManager = (WifiManager) mContext->GetSystemService(IContext::WIFI_SERVICE);

    //     SetView(mView);
    //     super->OnCreate(savedInstanceState);
    // }

    // //@Override
    // protected void OnStart() {
    //     /*
    //      * increment timeout bar per second.
    //      */
    //     mTimer = new Timer(FALSE);
    //     mTimer->Schedule(new TimerTask() {
    //         //@Override
    //         CARAPI Run() {
    //             mHandler->Post(new Runnable() {

    //                 //@Override
    //                 CARAPI Run() {
    //                     mTimeoutBar->IncrementProgressBy(1);
    //                 }
    //             });
    //         }
    //     }, 1000, 1000);

    //     mContext->RegisterReceiver((IBroadcastReceiver*)mReceiver, mFilter);

    //     WpsInfo wpsConfig = new WpsInfo();
    //     wpsConfig.setup = mWpsSetup;
    //     mWifiManager->StartWps(wpsConfig, mWpsListener);
    // }

    // //@Override
    // protected void OnStop() {
    //     if (mDialogState != WpsDialogState_WPS_COMPLETE) {
    //         mWifiManager->CancelWps(NULL);
    //     }

    //     if (mReceiver != NULL) {
    //         mContext->UnregisterReceiver((IBroadcastReceiver*)mReceiver);
    //         mReceiver = NULL;
    //     }

    //     if (mTimer != NULL) {
    //         mTimer->Cancel();
    //     }
    // }

private:
    CARAPI_(void) UpdateDialog(
        /* [in] */ WpsDialogState state,
        /* [in] */ const String& msg);

    CARAPI_(void) HandleEvent(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    static const String TAG;
    static const String DIALOG_STATE;
    static const String DIALOG_MSG_STRING;

    // View mView;
    AutoPtr<ITextView> mTextView;
    AutoPtr<IProgressBar> mTimeoutBar;
    AutoPtr<IProgressBar> mProgressBar;
    AutoPtr<IButton> mButton;
    // Timer mTimer;

    static const Int32 WPS_TIMEOUT_S;

    AutoPtr<IWifiManager> mWifiManager;
    AutoPtr<IWifiManagerWpsCallback> mWpsListener;
    Int32 mWpsSetup;

    AutoPtr<IIntentFilter> mFilter;
    AutoPtr<BroadcastReceiver> mReceiver;

    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;
    String mMsgString;

    WpsDialogState mDialogState;
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_WPSDIALOG_H__

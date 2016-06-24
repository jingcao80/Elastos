#ifndef __ELASTOS_DROID_SETTINGS_WIFI_WPSDIALOG_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_WPSDIALOG_H__

#include "Elastos.Droid.Wifi.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/AlertDialog.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "_Elastos.Droid.Settings.h"
#include <elastos/utility/TimerTask.h>

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
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Wifi::IWifiManagerWpsCallback;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Utility::ITimer;
using Elastos::Utility::TimerTask;

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

    class OnCreateViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        OnCreateViewOnClickListener(
            /* [in] */ WpsDialog* host);

        ~OnCreateViewOnClickListener();

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        WpsDialog* mHost;
    };

    class OnStartRunnable
        : public Runnable
    {
        friend class OnStartTimerTask;
    public:
        OnStartRunnable(
            /* [in] */ WpsDialog* host);

        ~OnStartRunnable();

        //@Override
        CARAPI Run();

    private:
        WpsDialog* mHost;
    };

    class OnStartTimerTask
        : public TimerTask
    {
    public:
        OnStartTimerTask(
            /* [in] */ WpsDialog* host);

        ~OnStartTimerTask();

        //@Override
        CARAPI Run();

    private:
        WpsDialog* mHost;
    };

public:
    WpsDialog(
        /* [in] */ IContext* context,
        /* [in] */ Int32 wpsSetup);

    ~WpsDialog();

    //@Override
    CARAPI OnSaveInstanceState(
        /* [out] */ IBundle** result);

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* savedInstanceState);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI OnStop();

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

    AutoPtr<IView> mView;
    AutoPtr<ITextView> mTextView;
    AutoPtr<IProgressBar> mTimeoutBar;
    AutoPtr<IProgressBar> mProgressBar;
    AutoPtr<IButton> mButton;
    AutoPtr<ITimer> mTimer;

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

//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SETTINGS_WIFI_CWPSDIALOG_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_CWPSDIALOG_H__

#include "Elastos.Droid.Wifi.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_Droid_Settings_Wifi_CWpsDialog.h"
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
CarClass(CWpsDialog)
    , public AlertDialog
{
private:
    class WpsListener
        : public Object
        , public IWifiManagerWpsCallback
    {
    public:
        TO_STRING_IMPL("CWpsDialog::WpsListener")

        CAR_INTERFACE_DECL();

        WpsListener(
            /* [in] */ CWpsDialog* host);

        ~WpsListener();

        CARAPI OnStarted(
            /* [in] */ const String& pin);

        CARAPI OnSucceeded();

        CARAPI OnFailed(
            /* [in] */ Int32 reason);

    private:
        CWpsDialog* mHost;
    };

    class InitBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CWpsDialog::InitBroadcastReceiver")

        InitBroadcastReceiver(
            /* [in] */ CWpsDialog* host);

        ~InitBroadcastReceiver();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CWpsDialog* mHost;
    };

    class UpdateDialogRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CWpsDialog::UpdateDialogRunnable")

        UpdateDialogRunnable(
            /* [in] */ CWpsDialog* host,
            /* [in] */ WpsDialogState state,
            /* [in] */ const String& msg);

        ~UpdateDialogRunnable();

        //@Override
        CARAPI Run();

    private:
        CWpsDialog* mHost;
        WpsDialogState mState;
        String mMsg;
    };

    class OnCreateViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CWpsDialog::OnCreateViewOnClickListener")

        CAR_INTERFACE_DECL();

        OnCreateViewOnClickListener(
            /* [in] */ CWpsDialog* host);

        ~OnCreateViewOnClickListener();

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CWpsDialog* mHost;
    };

    class OnStartRunnable
        : public Runnable
    {
        friend class OnStartTimerTask;
    public:
        TO_STRING_IMPL("CWpsDialog::OnStartRunnable")

        OnStartRunnable(
            /* [in] */ CWpsDialog* host);

        ~OnStartRunnable();

        //@Override
        CARAPI Run();

    private:
        CWpsDialog* mHost;
    };

    class OnStartTimerTask
        : public TimerTask
    {
    public:
        TO_STRING_IMPL("CWpsDialog::OnStartTimerTask")

        OnStartTimerTask(
            /* [in] */ CWpsDialog* host);

        ~OnStartTimerTask();

        //@Override
        CARAPI Run();

    private:
        CWpsDialog* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CWpsDialog();

    ~CWpsDialog();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 wpsSetup);

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

#endif //__ELASTOS_DROID_SETTINGS_WIFI_CWPSDIALOG_H__

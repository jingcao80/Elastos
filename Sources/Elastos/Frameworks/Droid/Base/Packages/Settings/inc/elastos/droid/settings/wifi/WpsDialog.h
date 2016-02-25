/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.android.settings.wifi;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::NetworkInfo::IDetailedState;
using Elastos::Droid::Net::Wifi::IWifiInfo;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Net::Wifi::IWpsInfo;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;

using Elastos::Utility::ITimer;
using Elastos::Utility::ITimerTask;

using Elastos::Droid::Settings::IR;


/**
 * Dialog to show WPS progress.
 */
public class WpsDialog extends AlertDialog {

    private static const String TAG = "WpsDialog";
    private static const String DIALOG_STATE = "android:dialogState";
    private static const String DIALOG_MSG_STRING = "android:dialogMsg";

    private View mView;
    private TextView mTextView;
    private ProgressBar mTimeoutBar;
    private ProgressBar mProgressBar;
    private Button mButton;
    private Timer mTimer;

    private static const Int32 WPS_TIMEOUT_S = 120;

    private WifiManager mWifiManager;
    private WifiManager.WpsCallback mWpsListener;
    private Int32 mWpsSetup;

    private final IntentFilter mFilter;
    private BroadcastReceiver mReceiver;

    private Context mContext;
    private Handler mHandler = new Handler();
    private String mMsgString = "";

    private enum DialogState {
        WPS_INIT,
        WPS_START,
        WPS_COMPLETE,
        CONNECTED, //WPS + IP config is done
        WPS_FAILED
    }
    DialogState mDialogState = DialogState.WPS_INIT;

    public WpsDialog(Context context, Int32 wpsSetup) {
        Super(context);
        mContext = context;
        mWpsSetup = wpsSetup;

        class WpsListener extends WifiManager.WpsCallback {

            CARAPI OnStarted(String pin) {
                if (pin != NULL) {
                    UpdateDialog(DialogState.WPS_START, String->Format(
                            mContext->GetString(R::string::wifi_wps_onstart_pin), pin));
                } else {
                    UpdateDialog(DialogState.WPS_START, mContext->GetString(
                            R::string::wifi_wps_onstart_pbc));
                }
            }

            CARAPI OnSucceeded() {
                UpdateDialog(DialogState.WPS_COMPLETE,
                        mContext->GetString(R::string::wifi_wps_complete));
            }

            CARAPI OnFailed(Int32 reason) {
                String msg;
                switch (reason) {
                    case WifiManager.WPS_OVERLAP_ERROR:
                        msg = mContext->GetString(R::string::wifi_wps_failed_overlap);
                        break;
                    case WifiManager.WPS_WEP_PROHIBITED:
                        msg = mContext->GetString(R::string::wifi_wps_failed_wep);
                        break;
                    case WifiManager.WPS_TKIP_ONLY_PROHIBITED:
                        msg = mContext->GetString(R::string::wifi_wps_failed_tkip);
                        break;
                    case WifiManager.IN_PROGRESS:
                        msg = mContext->GetString(R::string::wifi_wps_in_progress);
                        break;
                    default:
                        msg = mContext->GetString(R::string::wifi_wps_failed_generic);
                        break;
                }
                UpdateDialog(DialogState.WPS_FAILED, msg);
            }
        }

        mWpsListener = new WpsListener();


        mFilter = new IntentFilter();
        mFilter->AddAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
        mReceiver = new BroadcastReceiver() {
            //@Override
            CARAPI OnReceive(Context context, Intent intent) {
                HandleEvent(context, intent);
            }
        };
        SetCanceledOnTouchOutside(FALSE);
    }

    //@Override
    public Bundle onSaveInstanceState () {
        Bundle bundle  = super->OnSaveInstanceState();
        bundle->PutString(DIALOG_STATE, mDialogState->ToString());
        bundle->PutString(DIALOG_MSG_STRING, mMsgString->ToString());
        return bundle;
    }

    //@Override
    CARAPI OnRestoreInstanceState(Bundle savedInstanceState) {
        if (savedInstanceState != NULL) {
            super->OnRestoreInstanceState(savedInstanceState);
            DialogState dialogState = mDialogState->ValueOf(savedInstanceState->GetString(DIALOG_STATE));
            String msg = savedInstanceState->GetString(DIALOG_MSG_STRING);
            UpdateDialog(dialogState, msg);
        }
    }

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        mView = GetLayoutInflater()->Inflate(R.layout.wifi_wps_dialog, NULL);

        mTextView = (TextView) mView->FindViewById(R.id.wps_dialog_txt);
        mTextView->SetText(R::string::wifi_wps_setup_msg);

        mTimeoutBar = ((ProgressBar) mView->FindViewById(R.id.wps_timeout_bar));
        mTimeoutBar->SetMax(WPS_TIMEOUT_S);
        mTimeoutBar->SetProgress(0);

        mProgressBar = ((ProgressBar) mView->FindViewById(R.id.wps_progress_bar));
        mProgressBar->SetVisibility(View.GONE);

        mButton = ((Button) mView->FindViewById(R.id.wps_dialog_btn));
        mButton->SetText(R::string::wifi_cancel);
        mButton->SetOnClickListener(new View->OnClickListener() {
            //@Override
            CARAPI OnClick(View v) {
                Dismiss();
            }
        });

        mWifiManager = (WifiManager) mContext->GetSystemService(Context.WIFI_SERVICE);

        SetView(mView);
        super->OnCreate(savedInstanceState);
    }

    //@Override
    protected void OnStart() {
        /*
         * increment timeout bar per second.
         */
        mTimer = new Timer(FALSE);
        mTimer->Schedule(new TimerTask() {
            //@Override
            CARAPI Run() {
                mHandler->Post(new Runnable() {

                    //@Override
                    CARAPI Run() {
                        mTimeoutBar->IncrementProgressBy(1);
                    }
                });
            }
        }, 1000, 1000);

        mContext->RegisterReceiver(mReceiver, mFilter);

        WpsInfo wpsConfig = new WpsInfo();
        wpsConfig.setup = mWpsSetup;
        mWifiManager->StartWps(wpsConfig, mWpsListener);
    }

    //@Override
    protected void OnStop() {
        if (mDialogState != DialogState.WPS_COMPLETE) {
            mWifiManager->CancelWps(NULL);
        }

        if (mReceiver != NULL) {
            mContext->UnregisterReceiver(mReceiver);
            mReceiver = NULL;
        }

        if (mTimer != NULL) {
            mTimer->Cancel();
        }
    }

    private void UpdateDialog(final DialogState state, final String msg) {
        if (mDialogState->Ordinal() >= state->Ordinal()) {
            //ignore.
            return;
        }
        mDialogState = state;
        mMsgString = msg;

        mHandler->Post(new Runnable() {
                //@Override
                CARAPI Run() {
                    Switch(state) {
                        case WPS_COMPLETE:
                            mTimeoutBar->SetVisibility(View.GONE);
                            mProgressBar->SetVisibility(View.VISIBLE);
                            break;
                        case CONNECTED:
                        case WPS_FAILED:
                            mButton->SetText(mContext->GetString(R::string::dlg_ok));
                            mTimeoutBar->SetVisibility(View.GONE);
                            mProgressBar->SetVisibility(View.GONE);
                            if (mReceiver != NULL) {
                                mContext->UnregisterReceiver(mReceiver);
                                mReceiver = NULL;
                            }
                            break;
                    }
                    mTextView->SetText(msg);
                }
            });
   }

    private void HandleEvent(Context context, Intent intent) {
        String action = intent->GetAction();
        if (WifiManager.NETWORK_STATE_CHANGED_ACTION->Equals(action)) {
            NetworkInfo info = (NetworkInfo) intent->GetParcelableExtra(
                    WifiManager.EXTRA_NETWORK_INFO);
            final NetworkInfo.DetailedState state = info->GetDetailedState();
            if (state == DetailedState.CONNECTED &&
                    mDialogState == DialogState.WPS_COMPLETE) {
                WifiInfo wifiInfo = mWifiManager->GetConnectionInfo();
                if (wifiInfo != NULL) {
                    String msg = String->Format(mContext->GetString(
                            R::string::wifi_wps_connected), wifiInfo->GetSSID());
                    UpdateDialog(DialogState.CONNECTED, msg);
                }
            }
        }
    }

}

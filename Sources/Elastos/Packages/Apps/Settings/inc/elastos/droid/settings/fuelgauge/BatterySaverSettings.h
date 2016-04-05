

package com.android.settings.fuelgauge;

using static android::Os::PowerManager::IACTION_POWER_SAVE_MODE_CHANGING;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Provider::Settings::IGlobal;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Widget::ISwitch;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::Notification::ISettingPref;
using Elastos::Droid::Settings::Widget::ISwitchBar;

public class BatterySaverSettings extends SettingsPreferenceFragment
        implements SwitchBar.OnSwitchChangeListener {
    private static const String TAG = "BatterySaverSettings";
    private static const Boolean DEBUG = Log->IsLoggable(TAG, Log.DEBUG);
    private static const String KEY_TURN_ON_AUTOMATICALLY = "turn_on_automatically";
    private static const Int64 WAIT_FOR_SWITCH_ANIM = 500;

    private final Handler mHandler = new Handler();
    private final SettingsObserver mSettingsObserver = new SettingsObserver(mHandler);
    private final Receiver mReceiver = new Receiver();

    private Context mContext;
    private Boolean mCreated;
    private SettingPref mTriggerPref;
    private SwitchBar mSwitchBar;
    private Switch mSwitch;
    private Boolean mValidListener;
    private PowerManager mPowerManager;

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);
        if (mCreated) return;
        mCreated = TRUE;
        AddPreferencesFromResource(R.xml.battery_saver_settings);

        mContext = GetActivity();
        mSwitchBar = ((SettingsActivity) mContext).GetSwitchBar();
        mSwitch = mSwitchBar->GetSwitch();
        mSwitchBar->Show();

        mTriggerPref = new SettingPref(SettingPref.TYPE_GLOBAL, KEY_TURN_ON_AUTOMATICALLY,
                Global.LOW_POWER_MODE_TRIGGER_LEVEL,
                0, /*default*/
                GetResources()->GetIntArray(R.array.battery_saver_trigger_values)) {
            //@Override
            protected String GetCaption(Resources res, Int32 value) {
                if (value > 0 && value < 100) {
                    return res->GetString(R::string::battery_saver_turn_on_automatically_pct, value);
                }
                return res->GetString(R::string::battery_saver_turn_on_automatically_never);
            }
        };
        mTriggerPref->Init(this);

        mPowerManager = (PowerManager) mContext->GetSystemService(Context.POWER_SERVICE);
    }

    //@Override
    CARAPI OnDestroyView() {
        super->OnDestroyView();
        mSwitchBar->Hide();
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        mSettingsObserver->SetListening(TRUE);
        mReceiver->SetListening(TRUE);
        if (!mValidListener) {
            mSwitchBar->AddOnSwitchChangeListener(this);
            mValidListener = TRUE;
        }
        UpdateSwitch();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        mSettingsObserver->SetListening(FALSE);
        mReceiver->SetListening(FALSE);
        if (mValidListener) {
            mSwitchBar->RemoveOnSwitchChangeListener(this);
            mValidListener = FALSE;
        }
    }

    //@Override
    CARAPI OnSwitchChanged(Switch switchView, Boolean isChecked) {
        mHandler->RemoveCallbacks(mStartMode);
        if (isChecked) {
            mHandler->PostDelayed(mStartMode, WAIT_FOR_SWITCH_ANIM);
        } else {
            if (DEBUG) Logger::D(TAG, "Stopping low power mode from settings");
            TrySetPowerSaveMode(FALSE);
        }
    }

    private void TrySetPowerSaveMode(Boolean mode) {
        if (!mPowerManager->SetPowerSaveMode(mode)) {
            if (DEBUG) Logger::D(TAG, "Setting mode failed, fallback to current value");
            mHandler->Post(mUpdateSwitch);
        }
    }

    private void UpdateSwitch() {
        final Boolean mode = mPowerManager->IsPowerSaveMode();
        if (DEBUG) Logger::D(TAG, "updateSwitch: isChecked=" + mSwitch->IsChecked() + " mode=" + mode);
        if (mode == mSwitch->IsChecked()) return;

        // set listener to NULL so that that code below doesn't trigger OnCheckedChanged()
        if (mValidListener) {
            mSwitchBar->RemoveOnSwitchChangeListener(this);
        }
        mSwitch->SetChecked(mode);
        if (mValidListener) {
            mSwitchBar->AddOnSwitchChangeListener(this);
        }
    }

    private final Runnable mUpdateSwitch = new Runnable() {
        //@Override
        CARAPI Run() {
            UpdateSwitch();
        }
    };

    private final Runnable mStartMode = new Runnable() {
        //@Override
        CARAPI Run() {
            AsyncTask->Execute(new Runnable() {
                //@Override
                CARAPI Run() {
                    if (DEBUG) Logger::D(TAG, "Starting low power mode from settings");
                    TrySetPowerSaveMode(TRUE);
                }
            });
        }
    };

    private final class Receiver extends BroadcastReceiver {
        private Boolean mRegistered;

        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            if (DEBUG) Logger::D(TAG, "Received " + intent->GetAction());
            mHandler->Post(mUpdateSwitch);
        }

        CARAPI SetListening(Boolean listening) {
            if (listening && !mRegistered) {
                mContext->RegisterReceiver(this, new IntentFilter(ACTION_POWER_SAVE_MODE_CHANGING));
                mRegistered = TRUE;
            } else if (!listening && mRegistered) {
                mContext->UnregisterReceiver(this);
                mRegistered = FALSE;
            }
        }
    }

    private final class SettingsObserver extends ContentObserver {
        private final Uri LOW_POWER_MODE_TRIGGER_LEVEL_URI
                = Global->GetUriFor(Global.LOW_POWER_MODE_TRIGGER_LEVEL);

        public SettingsObserver(Handler handler) {
            Super(handler);
        }

        //@Override
        CARAPI OnChange(Boolean selfChange, Uri uri) {
            if (LOW_POWER_MODE_TRIGGER_LEVEL_URI->Equals(uri)) {
                mTriggerPref->Update(mContext);
            }
        }

        CARAPI SetListening(Boolean listening) {
            final ContentResolver cr = GetContentResolver();
            if (listening) {
                cr->RegisterContentObserver(LOW_POWER_MODE_TRIGGER_LEVEL_URI, FALSE, this);
            } else {
                cr->UnregisterContentObserver(this);
            }
        }
    }
}

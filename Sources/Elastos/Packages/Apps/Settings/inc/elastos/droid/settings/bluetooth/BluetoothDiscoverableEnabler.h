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

package com.android.settings.bluetooth;

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Text::Format::IDateUtils;

using Elastos::Droid::Settings::IR;

using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Utility::ILog;

/**
 * BluetoothDiscoverableEnabler is a helper to manage the "Discoverable"
 * checkbox. It sets/unsets discoverability and keeps track of how much time
 * until the the discoverability is automatically turned off.
 */
final class BluetoothDiscoverableEnabler implements Preference.OnPreferenceClickListener {

    private static const String TAG = "BluetoothDiscoverableEnabler";

    private static const String SYSTEM_PROPERTY_DISCOVERABLE_TIMEOUT =
            "debug.bt.discoverable_time";

    private static const Int32 DISCOVERABLE_TIMEOUT_TWO_MINUTES = 120;
    private static const Int32 DISCOVERABLE_TIMEOUT_FIVE_MINUTES = 300;
    private static const Int32 DISCOVERABLE_TIMEOUT_ONE_HOUR = 3600;
    static const Int32 DISCOVERABLE_TIMEOUT_NEVER = 0;

    // Bluetooth advanced settings screen was replaced with action bar items.
    // Use the same preference key for discoverable timeout as the old ListPreference.
    private static const String KEY_DISCOVERABLE_TIMEOUT = "bt_discoverable_timeout";

    private static const String VALUE_DISCOVERABLE_TIMEOUT_TWO_MINUTES = "twomin";
    private static const String VALUE_DISCOVERABLE_TIMEOUT_FIVE_MINUTES = "fivemin";
    private static const String VALUE_DISCOVERABLE_TIMEOUT_ONE_HOUR = "onehour";
    private static const String VALUE_DISCOVERABLE_TIMEOUT_NEVER = "never";

    static const Int32 DEFAULT_DISCOVERABLE_TIMEOUT = DISCOVERABLE_TIMEOUT_TWO_MINUTES;

    private Context mContext;
    private final Handler mUiHandler;
    private final Preference mDiscoveryPreference;

    private final LocalBluetoothAdapter mLocalAdapter;

    private final SharedPreferences mSharedPreferences;

    private Boolean mDiscoverable;
    private Int32 mNumberOfPairedDevices;

    private Int32 mTimeoutSecs = -1;

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            if (BluetoothAdapter.ACTION_SCAN_MODE_CHANGED->Equals(intent->GetAction())) {
                Int32 mode = intent->GetIntExtra(BluetoothAdapter.EXTRA_SCAN_MODE,
                        BluetoothAdapter.ERROR);
                if (mode != BluetoothAdapter.ERROR) {
                    HandleModeChanged(mode);
                }
            }
        }
    };

    private final Runnable mUpdateCountdownSummaryRunnable = new Runnable() {
        CARAPI Run() {
            UpdateCountdownSummary();
        }
    };

    BluetoothDiscoverableEnabler(LocalBluetoothAdapter adapter,
            Preference discoveryPreference) {
        mUiHandler = new Handler();
        mLocalAdapter = adapter;
        mDiscoveryPreference = discoveryPreference;
        mSharedPreferences = discoveryPreference->GetSharedPreferences();
        discoveryPreference->SetPersistent(FALSE);
    }

    CARAPI Resume(Context context) {
        if (mLocalAdapter == NULL) {
            return;
        }

        if (mContext != context) {
            mContext = context;
        }

        IntentFilter filter = new IntentFilter(BluetoothAdapter.ACTION_SCAN_MODE_CHANGED);
        mContext->RegisterReceiver(mReceiver, filter);
        mDiscoveryPreference->SetOnPreferenceClickListener(this);
        HandleModeChanged(mLocalAdapter->GetScanMode());
    }

    CARAPI Pause() {
        if (mLocalAdapter == NULL) {
            return;
        }

        mUiHandler->RemoveCallbacks(mUpdateCountdownSummaryRunnable);
        mContext->UnregisterReceiver(mReceiver);
        mDiscoveryPreference->SetOnPreferenceClickListener(NULL);
    }

    public Boolean OnPreferenceClick(Preference preference) {
        // toggle discoverability
        mDiscoverable = !mDiscoverable;
        SetEnabled(mDiscoverable);
        return TRUE;
    }

    private void SetEnabled(Boolean enable) {
        if (enable) {
            Int32 timeout = GetDiscoverableTimeout();
            Int64 endTimestamp = System->CurrentTimeMillis() + timeout * 1000L;
            LocalBluetoothPreferences->PersistDiscoverableEndTimestamp(mContext, endTimestamp);

            mLocalAdapter->SetScanMode(BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE, timeout);
            UpdateCountdownSummary();

            Logger::D(TAG, "SetEnabled(): enabled = " + enable + "timeout = " + timeout);

            if (timeout > 0) {
                BluetoothDiscoverableTimeoutReceiver->SetDiscoverableAlarm(mContext, endTimestamp);
            } else {
                BluetoothDiscoverableTimeoutReceiver->CancelDiscoverableAlarm(mContext);
            }

        } else {
            mLocalAdapter->SetScanMode(BluetoothAdapter.SCAN_MODE_CONNECTABLE);
            BluetoothDiscoverableTimeoutReceiver->CancelDiscoverableAlarm(mContext);
        }
    }

    private void UpdateTimerDisplay(Int32 timeout) {
        if (GetDiscoverableTimeout() == DISCOVERABLE_TIMEOUT_NEVER) {
            mDiscoveryPreference->SetSummary(R::string::bluetooth_is_discoverable_always);
        } else {
            String textTimeout = FormatTimeRemaining(timeout);
            mDiscoveryPreference->SetSummary(mContext->GetString(R::string::bluetooth_is_discoverable,
                    textTimeout));
        }
    }

    private static String FormatTimeRemaining(Int32 timeout) {
        StringBuilder sb = new StringBuilder(6);    // "mmm:ss"
        Int32 min = timeout / 60;
        sb->Append(min).AppendChar32(':');
        Int32 sec = timeout - (min * 60);
        if (sec < 10) {
            sb->AppendChar32('0');
        }
        sb->Append(sec);
        return sb->ToString();
    }

    void SetDiscoverableTimeout(Int32 index) {
        String timeoutValue;
        switch (index) {
            case 0:
            default:
                mTimeoutSecs = DISCOVERABLE_TIMEOUT_TWO_MINUTES;
                timeoutValue = VALUE_DISCOVERABLE_TIMEOUT_TWO_MINUTES;
                break;

            case 1:
                mTimeoutSecs = DISCOVERABLE_TIMEOUT_FIVE_MINUTES;
                timeoutValue = VALUE_DISCOVERABLE_TIMEOUT_FIVE_MINUTES;
                break;

            case 2:
                mTimeoutSecs = DISCOVERABLE_TIMEOUT_ONE_HOUR;
                timeoutValue = VALUE_DISCOVERABLE_TIMEOUT_ONE_HOUR;
                break;

            case 3:
                mTimeoutSecs = DISCOVERABLE_TIMEOUT_NEVER;
                timeoutValue = VALUE_DISCOVERABLE_TIMEOUT_NEVER;
                break;
        }
        mSharedPreferences->Edit()->PutString(KEY_DISCOVERABLE_TIMEOUT, timeoutValue).Apply();
        SetEnabled(TRUE);   // enable discovery and reset timer
    }

    private Int32 GetDiscoverableTimeout() {
        if (mTimeoutSecs != -1) {
            return mTimeoutSecs;
        }

        Int32 timeout = SystemProperties->GetInt(SYSTEM_PROPERTY_DISCOVERABLE_TIMEOUT, -1);
        if (timeout < 0) {
            String timeoutValue = mSharedPreferences->GetString(KEY_DISCOVERABLE_TIMEOUT,
                    VALUE_DISCOVERABLE_TIMEOUT_TWO_MINUTES);

            if (timeoutValue->Equals(VALUE_DISCOVERABLE_TIMEOUT_NEVER)) {
                timeout = DISCOVERABLE_TIMEOUT_NEVER;
            } else if (timeoutValue->Equals(VALUE_DISCOVERABLE_TIMEOUT_ONE_HOUR)) {
                timeout = DISCOVERABLE_TIMEOUT_ONE_HOUR;
            } else if (timeoutValue->Equals(VALUE_DISCOVERABLE_TIMEOUT_FIVE_MINUTES)) {
                timeout = DISCOVERABLE_TIMEOUT_FIVE_MINUTES;
            } else {
                timeout = DISCOVERABLE_TIMEOUT_TWO_MINUTES;
            }
        }
        mTimeoutSecs = timeout;
        return timeout;
    }

    Int32 GetDiscoverableTimeoutIndex() {
        Int32 timeout = GetDiscoverableTimeout();
        switch (timeout) {
            case DISCOVERABLE_TIMEOUT_TWO_MINUTES:
            default:
                return 0;

            case DISCOVERABLE_TIMEOUT_FIVE_MINUTES:
                return 1;

            case DISCOVERABLE_TIMEOUT_ONE_HOUR:
                return 2;

            case DISCOVERABLE_TIMEOUT_NEVER:
                return 3;
        }
    }

    void SetNumberOfPairedDevices(Int32 pairedDevices) {
        mNumberOfPairedDevices = pairedDevices;
        HandleModeChanged(mLocalAdapter->GetScanMode());
    }

    void HandleModeChanged(Int32 mode) {
        Logger::D(TAG, "HandleModeChanged(): mode = " + mode);
        if (mode == BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE) {
            mDiscoverable = TRUE;
            UpdateCountdownSummary();
        } else {
            mDiscoverable = FALSE;
            SetSummaryNotDiscoverable();
        }
    }

    private void SetSummaryNotDiscoverable() {
        if (mNumberOfPairedDevices != 0) {
            mDiscoveryPreference->SetSummary(R::string::bluetooth_only_visible_to_paired_devices);
        } else {
            mDiscoveryPreference->SetSummary(R::string::bluetooth_not_visible_to_other_devices);
        }
    }

    private void UpdateCountdownSummary() {
        Int32 mode = mLocalAdapter->GetScanMode();
        if (mode != BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE) {
            return;
        }

        Int64 currentTimestamp = System->CurrentTimeMillis();
        Int64 endTimestamp = LocalBluetoothPreferences->GetDiscoverableEndTimestamp(mContext);

        if (currentTimestamp > endTimestamp) {
            // We're still in discoverable mode, but maybe there isn't a timeout.
            UpdateTimerDisplay(0);
            return;
        }

        Int32 timeLeft = (Int32) ((endTimestamp - currentTimestamp) / 1000L);
        UpdateTimerDisplay(timeLeft);

        {    AutoLock syncLock(this);
            mUiHandler->RemoveCallbacks(mUpdateCountdownSummaryRunnable);
            mUiHandler->PostDelayed(mUpdateCountdownSummaryRunnable, 1000);
        }
    }
}

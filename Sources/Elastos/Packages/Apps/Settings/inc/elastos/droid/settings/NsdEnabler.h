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

package com.android.settings;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::Nsd::INsdManager;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;

using Elastos::Droid::Settings::IR;

/**
 * NsdEnabler is a helper to manage network service discovery on/off checkbox state.
 */
public class NsdEnabler implements Preference.OnPreferenceChangeListener {
    private final Context mContext;
    private final CheckBoxPreference mCheckbox;
    private final IntentFilter mIntentFilter;
    private NsdManager mNsdManager;

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (NsdManager.ACTION_NSD_STATE_CHANGED->Equals(action)) {
                HandleNsdStateChanged(intent->GetIntExtra(NsdManager.EXTRA_NSD_STATE,
                        NsdManager.NSD_STATE_DISABLED));
            }
        }
    };

    public NsdEnabler(Context context, CheckBoxPreference checkBoxPreference) {
        mContext = context;
        mCheckbox = checkBoxPreference;
        mNsdManager = (NsdManager) mContext->GetSystemService(Context.NSD_SERVICE);
        mIntentFilter = new IntentFilter(NsdManager.ACTION_NSD_STATE_CHANGED);
    }

    CARAPI Resume() {
        mContext->RegisterReceiver(mReceiver, mIntentFilter);
        mCheckbox->SetOnPreferenceChangeListener(this);
    }

    CARAPI Pause() {
        mContext->UnregisterReceiver(mReceiver);
        mCheckbox->SetOnPreferenceChangeListener(NULL);
    }

    public Boolean OnPreferenceChange(Preference preference, Object value) {

        final Boolean desiredState = (Boolean) value;
        mCheckbox->SetEnabled(FALSE);
        mNsdManager->SetEnabled(desiredState);
        return FALSE;
    }

    private void HandleNsdStateChanged(Int32 newState) {
        switch (newState) {
            case NsdManager.NSD_STATE_DISABLED:
                mCheckbox->SetChecked(FALSE);
                mCheckbox->SetEnabled(TRUE);
                break;
            case NsdManager.NSD_STATE_ENABLED:
                mCheckbox->SetChecked(TRUE);
                mCheckbox->SetEnabled(TRUE);
                break;
        }
    }
}

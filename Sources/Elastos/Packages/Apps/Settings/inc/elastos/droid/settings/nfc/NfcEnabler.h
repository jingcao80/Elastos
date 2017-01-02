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

/*
 * Copyright (C) 2010 The Android Open Source Project
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

package com.android.settings.nfc;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Nfc::INfcAdapter;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ISwitchPreference;

using Elastos::Droid::Settings::IR;

/**
 * NfcEnabler is a helper to manage the Nfc on/off checkbox preference. It is
 * turns on/off Nfc and ensures the summary of the preference reflects the
 * current state.
 */
public class NfcEnabler implements Preference.OnPreferenceChangeListener {
    private final Context mContext;
    private final SwitchPreference mSwitch;
    private final PreferenceScreen mAndroidBeam;
    private final NfcAdapter mNfcAdapter;
    private final IntentFilter mIntentFilter;
    private Boolean mBeamDisallowed;

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (NfcAdapter.ACTION_ADAPTER_STATE_CHANGED->Equals(action)) {
                HandleNfcStateChanged(intent->GetIntExtra(NfcAdapter.EXTRA_ADAPTER_STATE,
                        NfcAdapter.STATE_OFF));
            }
        }
    };

    public NfcEnabler(Context context, SwitchPreference switchPreference,
            PreferenceScreen androidBeam) {
        mContext = context;
        mSwitch = switchPreference;
        mAndroidBeam = androidBeam;
        mNfcAdapter = NfcAdapter->GetDefaultAdapter(context);
        mBeamDisallowed = ((UserManager) mContext->GetSystemService(Context.USER_SERVICE))
                .HasUserRestriction(UserManager.DISALLOW_OUTGOING_BEAM);

        if (mNfcAdapter == NULL) {
            // NFC is not supported
            mSwitch->SetEnabled(FALSE);
            mAndroidBeam->SetEnabled(FALSE);
            mIntentFilter = NULL;
            return;
        }
        if (mBeamDisallowed) {
            mAndroidBeam->SetEnabled(FALSE);
        }
        mIntentFilter = new IntentFilter(NfcAdapter.ACTION_ADAPTER_STATE_CHANGED);
    }

    CARAPI Resume() {
        if (mNfcAdapter == NULL) {
            return;
        }
        HandleNfcStateChanged(mNfcAdapter->GetAdapterState());
        mContext->RegisterReceiver(mReceiver, mIntentFilter);
        mSwitch->SetOnPreferenceChangeListener(this);
    }

    CARAPI Pause() {
        if (mNfcAdapter == NULL) {
            return;
        }
        mContext->UnregisterReceiver(mReceiver);
        mSwitch->SetOnPreferenceChangeListener(NULL);
    }

    public Boolean OnPreferenceChange(Preference preference, Object value) {
        // Turn NFC on/off

        final Boolean desiredState = (Boolean) value;
        mSwitch->SetEnabled(FALSE);

        if (desiredState) {
            mNfcAdapter->Enable();
        } else {
            mNfcAdapter->Disable();
        }

        return FALSE;
    }

    private void HandleNfcStateChanged(Int32 newState) {
        switch (newState) {
        case NfcAdapter.STATE_OFF:
            mSwitch->SetChecked(FALSE);
            mSwitch->SetEnabled(TRUE);
            mAndroidBeam->SetEnabled(FALSE);
            mAndroidBeam->SetSummary(R::string::android_beam_disabled_summary);
            break;
        case NfcAdapter.STATE_ON:
            mSwitch->SetChecked(TRUE);
            mSwitch->SetEnabled(TRUE);
            mAndroidBeam->SetEnabled(!mBeamDisallowed);
            if (mNfcAdapter->IsNdefPushEnabled() && !mBeamDisallowed) {
                mAndroidBeam->SetSummary(R::string::android_beam_on_summary);
            } else {
                mAndroidBeam->SetSummary(R::string::android_beam_off_summary);
            }
            break;
        case NfcAdapter.STATE_TURNING_ON:
            mSwitch->SetChecked(TRUE);
            mSwitch->SetEnabled(FALSE);
            mAndroidBeam->SetEnabled(FALSE);
            break;
        case NfcAdapter.STATE_TURNING_OFF:
            mSwitch->SetChecked(FALSE);
            mSwitch->SetEnabled(FALSE);
            mAndroidBeam->SetEnabled(FALSE);
            break;
        }
    }
}

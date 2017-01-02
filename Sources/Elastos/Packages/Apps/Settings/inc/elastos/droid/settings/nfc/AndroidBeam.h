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

package com.android.settings.nfc;

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Nfc::INfcAdapter;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ISwitch;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::Widget::ISwitchBar;

public class AndroidBeam extends Fragment
        implements SwitchBar.OnSwitchChangeListener {
    private View mView;
    private NfcAdapter mNfcAdapter;
    private SwitchBar mSwitchBar;
    private CharSequence mOldActivityTitle;
    private Boolean mBeamDisallowed;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        final ActionBar actionBar = GetActivity()->GetActionBar();

        mOldActivityTitle = actionBar->GetTitle();
        actionBar->SetTitle(R::string::android_beam_settings_title);

        mNfcAdapter = NfcAdapter->GetDefaultAdapter(GetActivity());
        mBeamDisallowed = ((UserManager) GetActivity()->GetSystemService(Context.USER_SERVICE))
                .HasUserRestriction(UserManager.DISALLOW_OUTGOING_BEAM);
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        mView = inflater->Inflate(R.layout.android_beam, container, FALSE);

        return mView;
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        SettingsActivity activity = (SettingsActivity) GetActivity();

        mSwitchBar = activity->GetSwitchBar();
        mSwitchBar->SetChecked(!mBeamDisallowed && mNfcAdapter->IsNdefPushEnabled());
        mSwitchBar->AddOnSwitchChangeListener(this);
        mSwitchBar->SetEnabled(!mBeamDisallowed);
        mSwitchBar->Show();
    }

    //@Override
    CARAPI OnDestroyView() {
        super->OnDestroyView();
        if (mOldActivityTitle != NULL) {
            GetActivity()->GetActionBar().SetTitle(mOldActivityTitle);
        }
        mSwitchBar->RemoveOnSwitchChangeListener(this);
        mSwitchBar->Hide();
    }

    //@Override
    CARAPI OnSwitchChanged(Switch switchView, Boolean desiredState) {
        Boolean success = FALSE;
        mSwitchBar->SetEnabled(FALSE);
        if (desiredState) {
            success = mNfcAdapter->EnableNdefPush();
        } else {
            success = mNfcAdapter->DisableNdefPush();
        }
        if (success) {
            mSwitchBar->SetChecked(desiredState);
        }
        mSwitchBar->SetEnabled(TRUE);
    }
}

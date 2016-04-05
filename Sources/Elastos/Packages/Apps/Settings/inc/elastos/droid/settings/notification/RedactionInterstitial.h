/*
 * Copyright (C) 2014 The Android Open Source Project
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
 * limitations under the License
 */

package com.android.settings.notification;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IRadioButton;

public class RedactionInterstitial extends SettingsActivity {

    //@Override
    public Intent GetIntent() {
        Intent modIntent = new Intent(super->GetIntent());
        modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, RedactionInterstitialFragment.class->GetName());
        return modIntent;
    }

    //@Override
    protected Boolean IsValidFragment(String fragmentName) {
        return RedactionInterstitialFragment.class->GetName()->Equals(fragmentName);
    }

    public static Intent CreateStartIntent(Context ctx) {
        return new Intent(ctx, RedactionInterstitial.class)
                .PutExtra(EXTRA_PREFS_SHOW_BUTTON_BAR, TRUE)
                .PutExtra(EXTRA_PREFS_SET_BACK_TEXT, (String) NULL)
                .PutExtra(EXTRA_PREFS_SET_NEXT_TEXT, ctx->GetString(
                        R::string::app_notifications_dialog_done));
    }

    public static class RedactionInterstitialFragment extends SettingsPreferenceFragment
            implements View.OnClickListener {

        private RadioButton mShowAllButton;
        private RadioButton mRedactSensitiveButton;
        private RadioButton mHideAllButton;

        //@Override
        public View OnCreateView(LayoutInflater inflater, ViewGroup container,
                Bundle savedInstanceState) {
            View view = inflater->Inflate(R.layout.redaction_interstitial, container, FALSE);
            mShowAllButton = (RadioButton) view->FindViewById(R.id.show_all);
            mRedactSensitiveButton = (RadioButton) view->FindViewById(R.id.redact_sensitive);
            mHideAllButton = (RadioButton) view->FindViewById(R.id.hide_all);

            mShowAllButton->SetOnClickListener(this);
            mRedactSensitiveButton->SetOnClickListener(this);
            mHideAllButton->SetOnClickListener(this);
            return view;
        }

        //@Override
        CARAPI OnResume() {
            super->OnResume();
            LoadFromSettings();
        }

        private void LoadFromSettings() {
            final Boolean enabled = Settings.Secure->GetInt(GetContentResolver(),
                        Settings.Secure.LOCK_SCREEN_SHOW_NOTIFICATIONS, 0) != 0;
            final Boolean show = Settings.Secure->GetInt(GetContentResolver(),
                        Settings.Secure.LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS, 1) != 0;
            mShowAllButton->SetChecked(enabled && show);
            mRedactSensitiveButton->SetChecked(enabled && !show);
            mHideAllButton->SetChecked(!enabled);
        }

        //@Override
        CARAPI OnClick(View v) {
            final Boolean show = (v == mShowAllButton);
            final Boolean enabled = (v != mHideAllButton);

            Settings.Secure->PutInt(GetContentResolver(),
                    Settings.Secure.LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS, show ? 1 : 0);
            Settings.Secure->PutInt(GetContentResolver(),
                    Settings.Secure.LOCK_SCREEN_SHOW_NOTIFICATIONS, enabled ? 1 : 0);
        }
    }
}

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

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::Notification::RedactionInterstitial::IRedactionInterstitialFragment;

/** Wrapper to allow external activites to jump directly to the {@link RedactionInterstitial} */
public class RedactionSettingsStandalone extends SettingsActivity {

    //@Override
    public Intent GetIntent() {
        Intent modIntent = new Intent(super->GetIntent());
        modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, RedactionInterstitialFragment.class->GetName())
                .PutExtra(EXTRA_PREFS_SHOW_BUTTON_BAR, TRUE)
                .PutExtra(EXTRA_PREFS_SET_BACK_TEXT, (String) NULL)
                .PutExtra(EXTRA_PREFS_SET_NEXT_TEXT, GetString(
                        R::string::app_notifications_dialog_done));
        return modIntent;
    }

    //@Override
    protected Boolean IsValidFragment(String fragmentName) {
        return RedactionInterstitialFragment.class->GetName()->Equals(fragmentName);
    }
}

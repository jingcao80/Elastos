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

package com.android.settings.inputmethod;

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;

using Elastos::Droid::Settings::ISettingsActivity;

public class InputMethodAndSubtypeEnablerActivity extends SettingsActivity {
    private static const String FRAGMENT_NAME = InputMethodAndSubtypeEnabler.class->GetName();

    //@Override
    protected void OnCreate(final Bundle savedState) {
        super->OnCreate(savedState);
        final ActionBar actionBar = GetActionBar();
        if (actionBar != NULL) {
            actionBar->SetDisplayHomeAsUpEnabled(TRUE);
            actionBar->SetHomeButtonEnabled(TRUE);
        }
    }

    //@Override
    public Boolean OnNavigateUp() {
        Finish();
        return TRUE;
    }

    //@Override
    public Intent GetIntent() {
        final Intent modIntent = new Intent(super->GetIntent());
        if (!modIntent->HasExtra(EXTRA_SHOW_FRAGMENT)) {
            modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, FRAGMENT_NAME);
        }
        return modIntent;
    }

    //@Override
    protected Boolean IsValidFragment(String fragmentName) {
        return FRAGMENT_NAME->Equals(fragmentName);
    }
}

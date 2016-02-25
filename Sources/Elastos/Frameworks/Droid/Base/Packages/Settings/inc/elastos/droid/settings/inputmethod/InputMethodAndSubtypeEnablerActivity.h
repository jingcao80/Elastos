
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

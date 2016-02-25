

package com.android.settings;

using Elastos::Droid::Utility::ILog;

/**
 * Stub class for showing sub-settings; we can't use the main Settings class
 * since for our app it is a special singleTask class.
 */
public class SubSettings extends SettingsActivity {

    //@Override
    public Boolean OnNavigateUp() {
        Finish();
        return TRUE;
    }

    //@Override
    protected Boolean IsValidFragment(String fragmentName) {
        Logger::D("SubSettings", "Launching fragment " + fragmentName);
        return TRUE;
    }
}

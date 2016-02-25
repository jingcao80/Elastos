

package com.android.settings;

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreferenceActivity;

public class TestingSettings extends PreferenceActivity {

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        AddPreferencesFromResource(R.xml.testing_settings);
    }

}

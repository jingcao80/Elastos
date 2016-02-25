

package com.android.settings.inputmethod;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::ISwitchPreference;

class SwitchWithNoTextPreference extends SwitchPreference {
    private static const String EMPTY_TEXT = "";

    SwitchWithNoTextPreference(final Context context) {
        Super(context);
        SetSwitchTextOn(EMPTY_TEXT);
        SetSwitchTextOff(EMPTY_TEXT);
    }
}

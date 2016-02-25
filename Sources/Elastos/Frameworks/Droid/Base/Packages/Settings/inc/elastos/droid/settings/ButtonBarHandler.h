
package com.android.settings;

using Elastos::Droid::Widget::IButton;

/**
 * Interface letting {@link SettingsPreferenceFragment} access to bottom bar inside
 * {@link SettingsActivity}.
 */
public interface ButtonBarHandler {
    public Boolean HasNextButton();
    public Button GetNextButton();
}
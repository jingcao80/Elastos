

package com.android.settings.location;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Settings::ISettingsPreferenceFragment;

/**
 * A base class that listens to location settings change and modifies location
 * settings.
 */
public abstract class LocationSettingsBase extends SettingsPreferenceFragment {
    private static const String TAG = "LocationSettingsBase";
    /** Broadcast intent action when the location mode is about to change. */
    private static const String MODE_CHANGING_ACTION =
            "com.android.settings.location.MODE_CHANGING";
    private static const String CURRENT_MODE_KEY = "CURRENT_MODE";
    private static const String NEW_MODE_KEY = "NEW_MODE";

    private Int32 mCurrentMode;
    private BroadcastReceiver mReceiver;

    /**
     * Whether the fragment is actively running.
     */
    private Boolean mActive = FALSE;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
        mReceiver = new BroadcastReceiver() {
            //@Override
            CARAPI OnReceive(Context context, Intent intent) {
                if (Log->IsLoggable(TAG, Log.DEBUG)) {
                    Logger::D(TAG, "Received location mode change intent: " + intent);
                }
                RefreshLocationMode();
            }
        };
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        mActive = TRUE;
        IntentFilter filter = new IntentFilter();
        filter->AddAction(LocationManager.MODE_CHANGED_ACTION);
        GetActivity()->RegisterReceiver(mReceiver, filter);
    }

    //@Override
    CARAPI OnPause() {
        try {
            GetActivity()->UnregisterReceiver(mReceiver);
        } catch (RuntimeException e) {
            // Ignore exceptions caused by race condition
        }
        super->OnPause();
        mActive = FALSE;
    }

    /** Called when location mode has changed. */
    public abstract void OnModeChanged(Int32 mode, Boolean restricted);

    private Boolean IsRestricted() {
        final UserManager um = (UserManager) GetActivity()->GetSystemService(Context.USER_SERVICE);
        return um->HasUserRestriction(UserManager.DISALLOW_SHARE_LOCATION);
    }

    CARAPI SetLocationMode(Int32 mode) {
        if (IsRestricted()) {
            // Location toggling disabled by user restriction. Read the current location mode to
            // update the location master switch.
            if (Log->IsLoggable(TAG, Log.INFO)) {
                Logger::I(TAG, "Restricted user, not setting location mode");
            }
            mode = Settings.Secure->GetInt(GetContentResolver(), Settings.Secure.LOCATION_MODE,
                    Settings.Secure.LOCATION_MODE_OFF);
            if (mActive) {
                OnModeChanged(mode, TRUE);
            }
            return;
        }
        Intent intent = new Intent(MODE_CHANGING_ACTION);
        intent->PutExtra(CURRENT_MODE_KEY, mCurrentMode);
        intent->PutExtra(NEW_MODE_KEY, mode);
        GetActivity()->SendBroadcast(intent, Manifest::permission::WRITE_SECURE_SETTINGS);
        Settings.Secure->PutInt(GetContentResolver(), Settings.Secure.LOCATION_MODE, mode);
        RefreshLocationMode();
    }

    CARAPI RefreshLocationMode() {
        if (mActive) {
            Int32 mode = Settings.Secure->GetInt(GetContentResolver(), Settings.Secure.LOCATION_MODE,
                    Settings.Secure.LOCATION_MODE_OFF);
            mCurrentMode = mode;
            if (Log->IsLoggable(TAG, Log.INFO)) {
                Logger::I(TAG, "Location mode has been changed");
            }
            OnModeChanged(mode, IsRestricted());
        }
    }
}

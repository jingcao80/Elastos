/*
 * Copyright (C) 2009 The Android Open Source Project
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
 * limitations under the License.
 */

package com.android.settings.widget;

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Appwidget::IAppWidgetManager;
using Elastos::Droid::Appwidget::IAppWidgetProvider;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Widget::IRemoteViews;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::Bluetooth::ILocalBluetoothAdapter;
using Elastos::Droid::Settings::Bluetooth::ILocalBluetoothManager;

/**
 * Provides control of power-related settings from a widget.
 */
public class SettingsAppWidgetProvider extends AppWidgetProvider {
    static const String TAG = "SettingsAppWidgetProvider";

    static const ComponentName THIS_APPWIDGET =
            new ComponentName("com.android.settings",
                    "com.android.settings.widget.SettingsAppWidgetProvider");

    private static LocalBluetoothAdapter sLocalBluetoothAdapter = NULL;

    private static const Int32 BUTTON_WIFI = 0;
    private static const Int32 BUTTON_BRIGHTNESS = 1;
    private static const Int32 BUTTON_SYNC = 2;
    private static const Int32 BUTTON_LOCATION = 3;
    private static const Int32 BUTTON_BLUETOOTH = 4;

    // This widget keeps track of two sets of states:
    // "3-state": STATE_DISABLED, STATE_ENABLED, STATE_INTERMEDIATE
    // "5-state": STATE_DISABLED, STATE_ENABLED, STATE_TURNING_ON, STATE_TURNING_OFF, STATE_UNKNOWN
    private static const Int32 STATE_DISABLED = 0;
    private static const Int32 STATE_ENABLED = 1;
    private static const Int32 STATE_TURNING_ON = 2;
    private static const Int32 STATE_TURNING_OFF = 3;
    private static const Int32 STATE_UNKNOWN = 4;
    private static const Int32 STATE_INTERMEDIATE = 5;

    // Position in the widget bar, to enable different graphics for left, center and right buttons
    private static const Int32 POS_LEFT = 0;
    private static const Int32 POS_CENTER = 1;
    private static const Int32 POS_RIGHT = 2;

    private static const Int32[] IND_DRAWABLE_OFF = {
        R.drawable.appwidget_settings_ind_off_l_holo,
        R.drawable.appwidget_settings_ind_off_c_holo,
        R.drawable.appwidget_settings_ind_off_r_holo
    };

    private static const Int32[] IND_DRAWABLE_MID = {
        R.drawable.appwidget_settings_ind_mid_l_holo,
        R.drawable.appwidget_settings_ind_mid_c_holo,
        R.drawable.appwidget_settings_ind_mid_r_holo
    };

    private static const Int32[] IND_DRAWABLE_ON = {
        R.drawable.appwidget_settings_ind_on_l_holo,
        R.drawable.appwidget_settings_ind_on_c_holo,
        R.drawable.appwidget_settings_ind_on_r_holo
    };

    /** Minimum brightness at which the indicator is shown at half-full and ON */
    private static const Float HALF_BRIGHTNESS_THRESHOLD = 0.3f;
    /** Minimum brightness at which the indicator is shown at full */
    private static const Float FULL_BRIGHTNESS_THRESHOLD = 0.8f;

    private static const StateTracker sWifiState = new WifiStateTracker();
    private static const StateTracker sBluetoothState = new BluetoothStateTracker();
    private static const StateTracker sLocationState = new LocationStateTracker();
    private static const StateTracker sSyncState = new SyncStateTracker();
    private static SettingsObserver sSettingsObserver;

    /**
     * The state machine for a setting's toggling, tracking reality
     * versus the user's intent.
     *
     * This is necessary because reality moves relatively slowly
     * (turning on &amp; off radio drivers), compared to user's
     * expectations.
     */
    private abstract static class StateTracker {
        // Is the state in the process of changing?
        private Boolean mInTransition = FALSE;
        private Boolean mActualState = NULL;  // initially not set
        private Boolean mIntendedState = NULL;  // initially not set

        // Did a toggle request arrive while a state update was
        // already in-flight?  If so, the mIntendedState needs to be
        // requested when the other one is done, unless we happened to
        // arrive at that state already.
        private Boolean mDeferredStateChangeRequestNeeded = FALSE;

        /**
         * User pressed a button to change the state.  Something
         * should immediately appear to the user afterwards, even if
         * we effectively do nothing.  Their press must be heard.
         */
        public final void ToggleState(Context context) {
            Int32 currentState = GetTriState(context);
            Boolean newState = FALSE;
            switch (currentState) {
                case STATE_ENABLED:
                    newState = FALSE;
                    break;
                case STATE_DISABLED:
                    newState = TRUE;
                    break;
                case STATE_INTERMEDIATE:
                    if (mIntendedState != NULL) {
                        newState = !mIntendedState;
                    }
                    break;
            }
            mIntendedState = newState;
            if (mInTransition) {
                // We don't send off a transition request if we're
                // already transitioning.  Makes our state tracking
                // easier, and is probably nicer on lower levels.
                // (even though they should be able to take it...)
                mDeferredStateChangeRequestNeeded = TRUE;
            } else {
                mInTransition = TRUE;
                RequestStateChange(context, newState);
            }
        }

        /**
         * Return the ID of the clickable container for the setting.
         */
        public abstract Int32 GetContainerId();

        /**
         * Return the ID of the main large image button for the setting.
         */
        public abstract Int32 GetButtonId();

        /**
         * Returns the small indicator image ID underneath the setting.
         */
        public abstract Int32 GetIndicatorId();

        /**
         * Returns the resource ID of the setting's content description.
         */
        public abstract Int32 GetButtonDescription();

        /**
         * Returns the resource ID of the image to show as a function of
         * the on-vs-off state.
         */
        public abstract Int32 GetButtonImageId(Boolean on);

        /**
         * Returns the position in the button bar - either POS_LEFT, POS_RIGHT or POS_CENTER.
         */
        public Int32 GetPosition() { return POS_CENTER; }

        /**
         * Updates the remote views depending on the state (off, on,
         * turning off, turning on) of the setting.
         */
        public final void SetImageViewResources(Context context, RemoteViews views) {
            Int32 containerId = GetContainerId();
            Int32 buttonId = GetButtonId();
            Int32 indicatorId = GetIndicatorId();
            Int32 pos = GetPosition();
            switch (GetTriState(context)) {
                case STATE_DISABLED:
                    views->SetContentDescription(containerId,
                        GetContentDescription(context, R::string::gadget_state_off));
                    views->SetImageViewResource(buttonId, GetButtonImageId(FALSE));
                    views->SetImageViewResource(
                        indicatorId, IND_DRAWABLE_OFF[pos]);
                    break;
                case STATE_ENABLED:
                    views->SetContentDescription(containerId,
                        GetContentDescription(context, R::string::gadget_state_on));
                    views->SetImageViewResource(buttonId, GetButtonImageId(TRUE));
                    views->SetImageViewResource(
                        indicatorId, IND_DRAWABLE_ON[pos]);
                    break;
                case STATE_INTERMEDIATE:
                    // In the transitional state, the bottom green bar
                    // shows the tri-state (on, off, transitioning), but
                    // the top dark-gray-or-bright-white logo shows the
                    // user's intent.  This is much easier to see in
                    // sunlight.
                    if (IsTurningOn()) {
                        views->SetContentDescription(containerId,
                            GetContentDescription(context, R::string::gadget_state_turning_on));
                        views->SetImageViewResource(buttonId, GetButtonImageId(TRUE));
                        views->SetImageViewResource(
                            indicatorId, IND_DRAWABLE_MID[pos]);
                    } else {
                        views->SetContentDescription(containerId,
                            GetContentDescription(context, R::string::gadget_state_turning_off));
                        views->SetImageViewResource(buttonId, GetButtonImageId(FALSE));
                        views->SetImageViewResource(
                            indicatorId, IND_DRAWABLE_OFF[pos]);
                    }
                    break;
            }
        }

        /**
         * Returns the gadget state template populated with the gadget
         * description and state.
         */
        private final String GetContentDescription(Context context, Int32 stateResId) {
            final String gadget = context->GetString(GetButtonDescription());
            final String state = context->GetString(stateResId);
            return context->GetString(R::string::gadget_state_template, gadget, state);
        }

        /**
         * Update internal state from a broadcast state change.
         */
        public abstract void OnActualStateChange(Context context, Intent intent);

        /**
         * Sets the value that we're now in.  To be called from onActualStateChange.
         *
         * @param newState one of STATE_DISABLED, STATE_ENABLED, STATE_TURNING_ON,
         *                 STATE_TURNING_OFF, STATE_UNKNOWN
         */
        protected final void SetCurrentState(Context context, Int32 newState) {
            final Boolean wasInTransition = mInTransition;
            switch (newState) {
                case STATE_DISABLED:
                    mInTransition = FALSE;
                    mActualState = FALSE;
                    break;
                case STATE_ENABLED:
                    mInTransition = FALSE;
                    mActualState = TRUE;
                    break;
                case STATE_TURNING_ON:
                    mInTransition = TRUE;
                    mActualState = FALSE;
                    break;
                case STATE_TURNING_OFF:
                    mInTransition = TRUE;
                    mActualState = TRUE;
                    break;
            }

            if (wasInTransition && !mInTransition) {
                if (mDeferredStateChangeRequestNeeded) {
                    Logger::V(TAG, "processing deferred state change");
                    if (mActualState != NULL && mIntendedState != NULL &&
                        mIntendedState->Equals(mActualState)) {
                        Logger::V(TAG, "... but intended state matches, so no changes.");
                    } else if (mIntendedState != NULL) {
                        mInTransition = TRUE;
                        RequestStateChange(context, mIntendedState);
                    }
                    mDeferredStateChangeRequestNeeded = FALSE;
                }
            }
        }


        /**
         * If we're in a transition mode, this returns TRUE if we're
         * transitioning towards being enabled.
         */
        public final Boolean IsTurningOn() {
            return mIntendedState != NULL && mIntendedState;
        }

        /**
         * Returns simplified 3-state value from underlying 5-state.
         *
         * @param context
         * @return STATE_ENABLED, STATE_DISABLED, or STATE_INTERMEDIATE
         */
        public final Int32 GetTriState(Context context) {
            if (mInTransition) {
                // If we know we just got a toggle request recently
                // (which set mInTransition), don't even ask the
                // underlying interface for its state.  We know we're
                // changing.  This avoids blocking the UI thread
                // during UI refresh post-toggle if the underlying
                // service state accessor has coarse locking on its
                // state (to be fixed separately).
                return STATE_INTERMEDIATE;
            }
            switch (GetActualState(context)) {
                case STATE_DISABLED:
                    return STATE_DISABLED;
                case STATE_ENABLED:
                    return STATE_ENABLED;
                default:
                    return STATE_INTERMEDIATE;
            }
        }

        /**
         * Gets underlying actual state.
         *
         * @param context
         * @return STATE_ENABLED, STATE_DISABLED, STATE_ENABLING, STATE_DISABLING,
         *         or or STATE_UNKNOWN.
         */
        public abstract Int32 GetActualState(Context context);

        /**
         * Actually make the desired change to the underlying radio
         * API.
         */
        protected abstract void RequestStateChange(Context context, Boolean desiredState);
    }

    /**
     * Subclass of StateTracker to get/set Wifi state.
     */
    private static const class WifiStateTracker extends StateTracker {
        public Int32 GetContainerId() { return R.id.btn_wifi; }
        public Int32 GetButtonId() { return R.id.img_wifi; }
        public Int32 GetIndicatorId() { return R.id.ind_wifi; }
        public Int32 GetButtonDescription() { return R::string::gadget_wifi; }
        public Int32 GetButtonImageId(Boolean on) {
            return on ? R.drawable.ic_appwidget_settings_wifi_on_holo
                    : R.drawable.ic_appwidget_settings_wifi_off_holo;
        }

        //@Override
        public Int32 GetPosition() { return POS_LEFT; }

        //@Override
        public Int32 GetActualState(Context context) {
            WifiManager wifiManager = (WifiManager) context->GetSystemService(Context.WIFI_SERVICE);
            if (wifiManager != NULL) {
                return WifiStateToFiveState(wifiManager->GetWifiState());
            }
            return STATE_UNKNOWN;
        }

        //@Override
        protected void RequestStateChange(Context context, final Boolean desiredState) {
            final WifiManager wifiManager =
                    (WifiManager) context->GetSystemService(Context.WIFI_SERVICE);
            if (wifiManager == NULL) {
                Logger::D(TAG, "No wifiManager.");
                return;
            }

            // Actually request the wifi change and persistent
            // settings write off the UI thread, as it can take a
            // user-noticeable amount of time, especially if there's
            // disk contention.
            new AsyncTask<Void, Void, Void>() {
                //@Override
                protected Void DoInBackground(Void... args) {
                    /**
                     * Disable tethering if enabling Wifi
                     */
                    Int32 wifiApState = wifiManager->GetWifiApState();
                    if (desiredState && ((wifiApState == WifiManager.WIFI_AP_STATE_ENABLING) ||
                                         (wifiApState == WifiManager.WIFI_AP_STATE_ENABLED))) {
                        wifiManager->SetWifiApEnabled(NULL, FALSE);
                    }

                    wifiManager->SetWifiEnabled(desiredState);
                    return NULL;
                }
            }.Execute();
        }

        //@Override
        CARAPI OnActualStateChange(Context context, Intent intent) {
            if (!WifiManager.WIFI_STATE_CHANGED_ACTION->Equals(intent->GetAction())) {
                return;
            }
            Int32 wifiState = intent->GetIntExtra(WifiManager.EXTRA_WIFI_STATE, -1);
            SetCurrentState(context, WifiStateToFiveState(wifiState));
        }

        /**
         * Converts WifiManager's state values into our
         * Wifi/Bluetooth-common state values.
         */
        private static Int32 WifiStateToFiveState(Int32 wifiState) {
            switch (wifiState) {
                case WifiManager.WIFI_STATE_DISABLED:
                    return STATE_DISABLED;
                case WifiManager.WIFI_STATE_ENABLED:
                    return STATE_ENABLED;
                case WifiManager.WIFI_STATE_DISABLING:
                    return STATE_TURNING_OFF;
                case WifiManager.WIFI_STATE_ENABLING:
                    return STATE_TURNING_ON;
                default:
                    return STATE_UNKNOWN;
            }
        }
    }

    /**
     * Subclass of StateTracker to get/set Bluetooth state.
     */
    private static const class BluetoothStateTracker extends StateTracker {
        public Int32 GetContainerId() { return R.id.btn_bluetooth; }
        public Int32 GetButtonId() { return R.id.img_bluetooth; }
        public Int32 GetIndicatorId() { return R.id.ind_bluetooth; }
        public Int32 GetButtonDescription() { return R::string::gadget_bluetooth; }
        public Int32 GetButtonImageId(Boolean on) {
            return on ? R.drawable.ic_appwidget_settings_bluetooth_on_holo
                    : R.drawable.ic_appwidget_settings_bluetooth_off_holo;
        }

        //@Override
        public Int32 GetActualState(Context context) {
            if (sLocalBluetoothAdapter == NULL) {
                LocalBluetoothManager manager = LocalBluetoothManager->GetInstance(context);
                if (manager == NULL) {
                    return STATE_UNKNOWN;  // On emulator?
                }
                sLocalBluetoothAdapter = manager->GetBluetoothAdapter();
            }
            return BluetoothStateToFiveState(sLocalBluetoothAdapter->GetBluetoothState());
        }

        //@Override
        protected void RequestStateChange(Context context, final Boolean desiredState) {
            if (sLocalBluetoothAdapter == NULL) {
                Logger::D(TAG, "No LocalBluetoothManager");
                return;
            }
            // Actually request the Bluetooth change and persistent
            // settings write off the UI thread, as it can take a
            // user-noticeable amount of time, especially if there's
            // disk contention.
            new AsyncTask<Void, Void, Void>() {
                //@Override
                protected Void DoInBackground(Void... args) {
                    sLocalBluetoothAdapter->SetBluetoothEnabled(desiredState);
                    return NULL;
                }
            }.Execute();
        }

        //@Override
        CARAPI OnActualStateChange(Context context, Intent intent) {
            if (!BluetoothAdapter.ACTION_STATE_CHANGED->Equals(intent->GetAction())) {
                return;
            }
            Int32 bluetoothState = intent->GetIntExtra(BluetoothAdapter.EXTRA_STATE, -1);
            SetCurrentState(context, BluetoothStateToFiveState(bluetoothState));
        }

        /**
         * Converts BluetoothAdapter's state values into our
         * Wifi/Bluetooth-common state values.
         */
        private static Int32 BluetoothStateToFiveState(Int32 bluetoothState) {
            switch (bluetoothState) {
                case BluetoothAdapter.STATE_OFF:
                    return STATE_DISABLED;
                case BluetoothAdapter.STATE_ON:
                    return STATE_ENABLED;
                case BluetoothAdapter.STATE_TURNING_ON:
                    return STATE_TURNING_ON;
                case BluetoothAdapter.STATE_TURNING_OFF:
                    return STATE_TURNING_OFF;
                default:
                    return STATE_UNKNOWN;
            }
        }
    }

    /**
     * Subclass of StateTracker for location state.
     */
    private static const class LocationStateTracker extends StateTracker {
        private Int32 mCurrentLocationMode = Settings.Secure.LOCATION_MODE_OFF;

        public Int32 GetContainerId() { return R.id.btn_location; }
        public Int32 GetButtonId() { return R.id.img_location; }
        public Int32 GetIndicatorId() { return R.id.ind_location; }
        public Int32 GetButtonDescription() { return R::string::gadget_location; }
        public Int32 GetButtonImageId(Boolean on) {
            if (on) {
                switch (mCurrentLocationMode) {
                    case Settings.Secure.LOCATION_MODE_HIGH_ACCURACY:
                    case Settings.Secure.LOCATION_MODE_SENSORS_ONLY:
                        return R.drawable.ic_appwidget_settings_location_on_holo;
                    default:
                        return R.drawable.ic_appwidget_settings_location_saving_holo;
                }
            }

            return R.drawable.ic_appwidget_settings_location_off_holo;
        }

        //@Override
        public Int32 GetActualState(Context context) {
            ContentResolver resolver = context->GetContentResolver();
            mCurrentLocationMode = Settings.Secure->GetInt(resolver,
                    Settings.Secure.LOCATION_MODE, Settings.Secure.LOCATION_MODE_OFF);
            return (mCurrentLocationMode == Settings.Secure.LOCATION_MODE_OFF)
                    ? STATE_DISABLED : STATE_ENABLED;
        }

        //@Override
        CARAPI OnActualStateChange(Context context, Intent unused) {
            // Note: the broadcast location providers changed intent
            // doesn't include an extras bundles saying what the new value is.
            SetCurrentState(context, GetActualState(context));
        }

        //@Override
        CARAPI RequestStateChange(final Context context, final Boolean desiredState) {
            final ContentResolver resolver = context->GetContentResolver();
            new AsyncTask<Void, Void, Boolean>() {
                //@Override
                protected Boolean DoInBackground(Void... args) {
                    final UserManager um =
                            (UserManager) context->GetSystemService(Context.USER_SERVICE);
                    if (!um->HasUserRestriction(UserManager.DISALLOW_SHARE_LOCATION)) {
                        Int32 currentMode = Settings.Secure->GetInt(resolver,
                                Settings.Secure.LOCATION_MODE, Settings.Secure.LOCATION_MODE_OFF);
                        Int32 mode = Settings.Secure.LOCATION_MODE_HIGH_ACCURACY;
                        switch (currentMode) {
                            case Settings.Secure.LOCATION_MODE_HIGH_ACCURACY:
                                mode = Settings.Secure.LOCATION_MODE_BATTERY_SAVING;
                                break;
                            case Settings.Secure.LOCATION_MODE_BATTERY_SAVING:
                                mode = Settings.Secure.LOCATION_MODE_HIGH_ACCURACY;
                                break;
                            case Settings.Secure.LOCATION_MODE_SENSORS_ONLY:
                                mode = Settings.Secure.LOCATION_MODE_OFF;
                                break;
                            case Settings.Secure.LOCATION_MODE_OFF:
                                mode = Settings.Secure.LOCATION_MODE_HIGH_ACCURACY;
                                break;
                        }
                        Settings.Secure->PutInt(resolver, Settings.Secure.LOCATION_MODE, mode);
                        return mode != Settings.Secure.LOCATION_MODE_OFF;
                    }

                    return GetActualState(context) == STATE_ENABLED;
                }

                //@Override
                protected void OnPostExecute(Boolean result) {
                    SetCurrentState(
                        context,
                        result ? STATE_ENABLED : STATE_DISABLED);
                    UpdateWidget(context);
                }
            }.Execute();
        }
    }

    /**
     * Subclass of StateTracker for sync state.
     */
    private static const class SyncStateTracker extends StateTracker {
        public Int32 GetContainerId() { return R.id.btn_sync; }
        public Int32 GetButtonId() { return R.id.img_sync; }
        public Int32 GetIndicatorId() { return R.id.ind_sync; }
        public Int32 GetButtonDescription() { return R::string::gadget_sync; }
        public Int32 GetButtonImageId(Boolean on) {
            return on ? R.drawable.ic_appwidget_settings_sync_on_holo
                    : R.drawable.ic_appwidget_settings_sync_off_holo;
        }

        //@Override
        public Int32 GetActualState(Context context) {
            Boolean on = ContentResolver->GetMasterSyncAutomatically();
            return on ? STATE_ENABLED : STATE_DISABLED;
        }

        //@Override
        CARAPI OnActualStateChange(Context context, Intent unused) {
            SetCurrentState(context, GetActualState(context));
        }

        //@Override
        CARAPI RequestStateChange(final Context context, final Boolean desiredState) {
            final ConnectivityManager connManager =
                    (ConnectivityManager) context->GetSystemService(Context.CONNECTIVITY_SERVICE);
            final Boolean sync = ContentResolver->GetMasterSyncAutomatically();

            new AsyncTask<Void, Void, Boolean>() {
                //@Override
                protected Boolean DoInBackground(Void... args) {
                    // Turning sync on.
                    if (desiredState) {
                        if (!sync) {
                            ContentResolver->SetMasterSyncAutomatically(TRUE);
                        }
                        return TRUE;
                    }

                    // Turning sync off
                    if (sync) {
                        ContentResolver->SetMasterSyncAutomatically(FALSE);
                    }
                    return FALSE;
                }

                //@Override
                protected void OnPostExecute(Boolean result) {
                    SetCurrentState(
                        context,
                        result ? STATE_ENABLED : STATE_DISABLED);
                    UpdateWidget(context);
                }
            }.Execute();
        }
    }

    private static void CheckObserver(Context context) {
        if (sSettingsObserver == NULL) {
            sSettingsObserver = new SettingsObserver(new Handler(),
                    context->GetApplicationContext());
            sSettingsObserver->StartObserving();
        }
    }

    //@Override
    CARAPI OnUpdate(Context context, AppWidgetManager appWidgetManager,
            Int32[] appWidgetIds) {
        // Update each requested appWidgetId
        RemoteViews view = BuildUpdate(context);

        for (Int32 i = 0; i < appWidgetIds.length; i++) {
            appWidgetManager->UpdateAppWidget(appWidgetIds[i], view);
        }
    }

    //@Override
    CARAPI OnEnabled(Context context) {
        CheckObserver(context);
    }

    //@Override
    CARAPI OnDisabled(Context context) {
        if (sSettingsObserver != NULL) {
            sSettingsObserver->StopObserving();
            sSettingsObserver = NULL;
        }
    }

    /**
     * Load image for given widget and build {@link RemoteViews} for it.
     */
    static RemoteViews BuildUpdate(Context context) {
        RemoteViews views = new RemoteViews(context->GetPackageName(),
                R.layout.widget);
        views->SetOnClickPendingIntent(R.id.btn_wifi, GetLaunchPendingIntent(context,
                BUTTON_WIFI));
        views->SetOnClickPendingIntent(R.id.btn_brightness,
                GetLaunchPendingIntent(context,
                        BUTTON_BRIGHTNESS));
        views->SetOnClickPendingIntent(R.id.btn_sync,
                GetLaunchPendingIntent(context,
                        BUTTON_SYNC));
        views->SetOnClickPendingIntent(R.id.btn_location,
                GetLaunchPendingIntent(context, BUTTON_LOCATION));
        views->SetOnClickPendingIntent(R.id.btn_bluetooth,
                GetLaunchPendingIntent(context,
                        BUTTON_BLUETOOTH));

        UpdateButtons(views, context);
        return views;
    }

    /**
     * Updates the widget when something changes, or when a button is pushed.
     *
     * @param context
     */
    public static void UpdateWidget(Context context) {
        RemoteViews views = BuildUpdate(context);
        // Update specific list of appWidgetIds if given, otherwise default to all
        final AppWidgetManager gm = AppWidgetManager->GetInstance(context);
        gm->UpdateAppWidget(THIS_APPWIDGET, views);
        CheckObserver(context);
    }

    /**
     * Updates the buttons based on the underlying states of wifi, etc.
     *
     * @param views   The RemoteViews to update.
     * @param context
     */
    private static void UpdateButtons(RemoteViews views, Context context) {
        sWifiState->SetImageViewResources(context, views);
        sBluetoothState->SetImageViewResources(context, views);
        sLocationState->SetImageViewResources(context, views);
        sSyncState->SetImageViewResources(context, views);

        if (GetBrightnessMode(context)) {
            views->SetContentDescription(R.id.btn_brightness,
                    context->GetString(R::string::gadget_brightness_template,
                            context->GetString(R::string::gadget_brightness_state_auto)));
            views->SetImageViewResource(R.id.img_brightness,
                    R.drawable.ic_appwidget_settings_brightness_auto_holo);
            views->SetImageViewResource(R.id.ind_brightness,
                    R.drawable.appwidget_settings_ind_on_r_holo);
        } else {
            final Int32 brightness = GetBrightness(context);
            final PowerManager pm = (PowerManager)context->GetSystemService(Context.POWER_SERVICE);
            // Set the icon
            final Int32 full = (Int32)(pm->GetMaximumScreenBrightnessSetting()
                    * FULL_BRIGHTNESS_THRESHOLD);
            final Int32 half = (Int32)(pm->GetMaximumScreenBrightnessSetting()
                    * HALF_BRIGHTNESS_THRESHOLD);
            if (brightness > full) {
                views->SetContentDescription(R.id.btn_brightness,
                        context->GetString(R::string::gadget_brightness_template,
                                context->GetString(R::string::gadget_brightness_state_full)));
                views->SetImageViewResource(R.id.img_brightness,
                        R.drawable.ic_appwidget_settings_brightness_full_holo);
            } else if (brightness > half) {
                views->SetContentDescription(R.id.btn_brightness,
                        context->GetString(R::string::gadget_brightness_template,
                                context->GetString(R::string::gadget_brightness_state_half)));
                views->SetImageViewResource(R.id.img_brightness,
                        R.drawable.ic_appwidget_settings_brightness_half_holo);
            } else {
                views->SetContentDescription(R.id.btn_brightness,
                        context->GetString(R::string::gadget_brightness_template,
                                context->GetString(R::string::gadget_brightness_state_off)));
                views->SetImageViewResource(R.id.img_brightness,
                        R.drawable.ic_appwidget_settings_brightness_off_holo);
            }
            // Set the ON state
            if (brightness > half) {
                views->SetImageViewResource(R.id.ind_brightness,
                        R.drawable.appwidget_settings_ind_on_r_holo);
            } else {
                views->SetImageViewResource(R.id.ind_brightness,
                        R.drawable.appwidget_settings_ind_off_r_holo);
            }
        }
    }

    /**
     * Creates PendingIntent to notify the widget of a button click.
     *
     * @param context
     * @return
     */
    private static PendingIntent GetLaunchPendingIntent(Context context,
            Int32 buttonId) {
        Intent launchIntent = new Intent();
        launchIntent->SetClass(context, SettingsAppWidgetProvider.class);
        launchIntent->AddCategory(Intent.CATEGORY_ALTERNATIVE);
        launchIntent->SetData(Uri->Parse("custom:" + buttonId));
        PendingIntent pi = PendingIntent->GetBroadcast(context, 0 /* no requestCode */,
                launchIntent, 0 /* no flags */);
        return pi;
    }

    /**
     * Receives and processes a button pressed intent or state change.
     *
     * @param context
     * @param intent  Indicates the pressed button.
     */
    //@Override
    CARAPI OnReceive(Context context, Intent intent) {
        super->OnReceive(context, intent);
        String action = intent->GetAction();
        if (WifiManager.WIFI_STATE_CHANGED_ACTION->Equals(action)) {
            sWifiState->OnActualStateChange(context, intent);
        } else if (BluetoothAdapter.ACTION_STATE_CHANGED->Equals(action)) {
            sBluetoothState->OnActualStateChange(context, intent);
        } else if (LocationManager.MODE_CHANGED_ACTION->Equals(action)) {
            sLocationState->OnActualStateChange(context, intent);
        } else if (ContentResolver.ACTION_SYNC_CONN_STATUS_CHANGED->Equals(action)) {
            sSyncState->OnActualStateChange(context, intent);
        } else if (intent->HasCategory(Intent.CATEGORY_ALTERNATIVE)) {
            Uri data = intent->GetData();
            Int32 buttonId = Integer->ParseInt(data->GetSchemeSpecificPart());
            if (buttonId == BUTTON_WIFI) {
                sWifiState->ToggleState(context);
            } else if (buttonId == BUTTON_BRIGHTNESS) {
                ToggleBrightness(context);
            } else if (buttonId == BUTTON_SYNC) {
                sSyncState->ToggleState(context);
            } else if (buttonId == BUTTON_LOCATION) {
                sLocationState->ToggleState(context);
            } else if (buttonId == BUTTON_BLUETOOTH) {
                sBluetoothState->ToggleState(context);
            }
        } else {
            // Don't fall-through to updating the widget.  The Intent
            // was something unrelated or that our super class took
            // care of.
            return;
        }

        // State changes fall through
        UpdateWidget(context);
    }

    /**
     * Gets brightness level.
     *
     * @param context
     * @return brightness level between 0 and 255.
     */
    private static Int32 GetBrightness(Context context) {
        try {
            Int32 brightness = Settings.System->GetInt(context->GetContentResolver(),
                    Settings.System.SCREEN_BRIGHTNESS);
            return brightness;
        } catch (Exception e) {
        }
        return 0;
    }

    /**
     * Gets state of brightness mode.
     *
     * @param context
     * @return TRUE if auto brightness is on.
     */
    private static Boolean GetBrightnessMode(Context context) {
        try {
            Int32 brightnessMode = Settings.System->GetInt(context->GetContentResolver(),
                    Settings.System.SCREEN_BRIGHTNESS_MODE);
            return brightnessMode == Settings.System.SCREEN_BRIGHTNESS_MODE_AUTOMATIC;
        } catch (Exception e) {
            Logger::D(TAG, "getBrightnessMode: " + e);
        }
        return FALSE;
    }

    /**
     * Increases or decreases the brightness.
     *
     * @param context
     */
    private void ToggleBrightness(Context context) {
        try {
            IPowerManager power = IPowerManager.Stub->AsInterface(
                    ServiceManager->GetService("power"));
            if (power != NULL) {
                PowerManager pm = (PowerManager)context->GetSystemService(Context.POWER_SERVICE);

                ContentResolver cr = context->GetContentResolver();
                Int32 brightness = Settings.System->GetInt(cr,
                        Settings.System.SCREEN_BRIGHTNESS);
                Int32 brightnessMode = Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL;
                //Only get brightness setting if available
                if (context->GetResources()->GetBoolean(
                        R.bool.config_automatic_brightness_available)) {
                    brightnessMode = Settings.System->GetInt(cr,
                            Settings.System.SCREEN_BRIGHTNESS_MODE);
                }

                // Rotate AUTO -> MINIMUM -> DEFAULT -> MAXIMUM
                // Technically, not a toggle...
                if (brightnessMode == Settings.System.SCREEN_BRIGHTNESS_MODE_AUTOMATIC) {
                    brightness = pm->GetMinimumScreenBrightnessSetting();
                    brightnessMode = Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL;
                } else if (brightness < pm->GetDefaultScreenBrightnessSetting()) {
                    brightness = pm->GetDefaultScreenBrightnessSetting();
                } else if (brightness < pm->GetMaximumScreenBrightnessSetting()) {
                    brightness = pm->GetMaximumScreenBrightnessSetting();
                } else {
                    brightnessMode = Settings.System.SCREEN_BRIGHTNESS_MODE_AUTOMATIC;
                    brightness = pm->GetMinimumScreenBrightnessSetting();
                }

                if (context->GetResources()->GetBoolean(
                        R.bool.config_automatic_brightness_available)) {
                    // Set screen brightness mode (automatic or manual)
                    Settings.System->PutInt(context->GetContentResolver(),
                            Settings.System.SCREEN_BRIGHTNESS_MODE,
                            brightnessMode);
                } else {
                    // Make sure we set the brightness if automatic mode isn't available
                    brightnessMode = Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL;
                }
                if (brightnessMode == Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL) {
                    power->SetTemporaryScreenBrightnessSettingOverride(brightness);
                    Settings.System->PutInt(cr, Settings.System.SCREEN_BRIGHTNESS, brightness);
                }
            }
        } catch (RemoteException e) {
            Logger::D(TAG, "toggleBrightness: " + e);
        } catch (Settings.SettingNotFoundException e) {
            Logger::D(TAG, "toggleBrightness: " + e);
        }
    }

    /** Observer to watch for changes to the BRIGHTNESS setting */
    private static class SettingsObserver extends ContentObserver {

        private Context mContext;

        SettingsObserver(Handler handler, Context context) {
            Super(handler);
            mContext = context;
        }

        void StartObserving() {
            ContentResolver resolver = mContext->GetContentResolver();
            // Listen to brightness and brightness mode
            resolver->RegisterContentObserver(Settings.System
                    .GetUriFor(Settings.System.SCREEN_BRIGHTNESS), FALSE, this);
            resolver->RegisterContentObserver(Settings.System
                    .GetUriFor(Settings.System.SCREEN_BRIGHTNESS_MODE), FALSE, this);
        }

        void StopObserving() {
            mContext->GetContentResolver()->UnregisterContentObserver(this);
        }

        //@Override
        CARAPI OnChange(Boolean selfChange) {
            UpdateWidget(mContext);
        }
    }

}

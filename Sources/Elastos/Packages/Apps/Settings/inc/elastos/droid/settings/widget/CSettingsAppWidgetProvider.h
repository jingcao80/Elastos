
#ifndef __ELASTOS_DROID_SETTINGS_WIDGET_CSETTINGSAPPWIDGETPROVIDER_H__
#define __ELASTOS_DROID_SETTINGS_WIDGET_CSETTINGSAPPWIDGETPROVIDER_H__

#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Wifi.h"
#include "_Elastos_Droid_Settings_Widget_CSettingsAppWidgetProvider.h"
#include "elastos/droid/appwidget/AppWidgetProvider.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/AsyncTask.h"

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::AppWidget::AppWidgetProvider;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
// using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IHandler;
// using Elastos::Droid::Os::IIPowerManager;
// using Elastos::Droid::Os::IServiceManager;
// using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Droid::Wifi::IWifiManager;
// using Elastos::Droid::Settings::Bluetooth::ILocalBluetoothAdapter;
// using Elastos::Droid::Settings::Bluetooth::ILocalBluetoothManager;
using Elastos::Core::IBoolean;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Widget {

/**
 * Provides control of power-related settings from a widget.
 */
CarClass(CSettingsAppWidgetProvider)
    , public AppWidgetProvider
    , public ISettingsAppWidgetProvider
{
private:
    /**
     * The state machine for a setting's toggling, tracking reality
     * versus the user's intent.
     *
     * This is necessary because reality moves relatively slowly
     * (turning on &amp; off radio drivers), compared to user's
     * expectations.
     */
    class StateTracker
        : public Object
    {
    public:
        StateTracker();

        ~StateTracker();

        /**
         * User pressed a button to change the state.  Something
         * should immediately appear to the user afterwards, even if
         * we effectively do nothing.  Their press must be heard.
         */
        CARAPI ToggleState(
            /* [in] */ IContext* context);

        /**
         * Updates the remote views depending on the state (off, on,
         * turning off, turning on) of the setting.
         */
        CARAPI SetImageViewResources(
            /* [in] */ IContext* context,
            /* [in] */ IRemoteViews* views);

        /**
         * If we're in a transition mode, this returns TRUE if we're
         * transitioning towards being enabled.
         */
        CARAPI_(Boolean) IsTurningOn();

        /**
         * Returns simplified 3-state value from underlying 5-state.
         *
         * @param context
         * @return STATE_ENABLED, STATE_DISABLED, or STATE_INTERMEDIATE
         */
        CARAPI_(Int32) GetTriState(
            /* [in] */ IContext* context);

        /**
         * Returns the position in the button bar - either POS_LEFT, POS_RIGHT or POS_CENTER.
         */
        CARAPI_(Int32) GetPosition();

        /**
         * Return the ID of the clickable container for the setting.
         */
        virtual CARAPI_(Int32) GetContainerId() = 0;

        /**
         * Return the ID of the main large image button for the setting.
         */
        virtual CARAPI_(Int32) GetButtonId() = 0;

        /**
         * Returns the small indicator image ID underneath the setting.
         */
        virtual CARAPI_(Int32) GetIndicatorId() = 0;

        /**
         * Returns the resource ID of the setting's content description.
         */
        virtual CARAPI_(Int32) GetButtonDescription() = 0;

        /**
         * Returns the resource ID of the image to show as a function of
         * the on-vs-off state.
         */
        virtual CARAPI_(Int32) GetButtonImageId(
            /* [in] */ Boolean on) = 0;

        /**
         * Update internal state from a broadcast state change.
         */
        virtual CARAPI OnActualStateChange(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent) = 0;

        /**
         * Gets underlying actual state.
         *
         * @param context
         * @return STATE_ENABLED, STATE_DISABLED, STATE_ENABLING, STATE_DISABLING,
         *         or or STATE_UNKNOWN.
         */
        virtual CARAPI_(Int32) GetActualState(
            /* [in] */ IContext* context) = 0;

    protected:

        /**
         * Sets the value that we're now in.  To be called from onActualStateChange.
         *
         * @param newState one of STATE_DISABLED, STATE_ENABLED, STATE_TURNING_ON,
         *                 STATE_TURNING_OFF, STATE_UNKNOWN
         */
        CARAPI_(void) SetCurrentState(
            /* [in] */ IContext* context,
            /* [in] */ Int32 newState);
        /**
         * Actually make the desired change to the underlying radio
         * API.
         */
        virtual CARAPI RequestStateChange(
            /* [in] */ IContext* context,
            /* [in] */ Boolean desiredState) = 0;

    private:
        /**
         * Returns the gadget state template populated with the gadget
         * description and state.
         */
        CARAPI_(String) GetContentDescription(
            /* [in] */ IContext* context,
            /* [in] */ Int32 stateResId);

    private:
        // Is the state in the process of changing?
        Boolean mInTransition;
        AutoPtr<IBoolean> mActualState;  // initially not set
        AutoPtr<IBoolean> mIntendedState;  // initially not set

        // Did a toggle request arrive while a state update was
        // already in-flight?  If so, the mIntendedState needs to be
        // requested when the other one is done, unless we happened to
        // arrive at that state already.
        Boolean mDeferredStateChangeRequestNeeded;
    };

    /**
     * Subclass of StateTracker to get/set Wifi state.
     */
    class WifiStateTracker
        : public StateTracker
    {
    public:
        virtual CARAPI_(Int32) GetContainerId();
        virtual CARAPI_(Int32) GetButtonId();
        virtual CARAPI_(Int32) GetIndicatorId();
        virtual CARAPI_(Int32) GetButtonDescription();
        virtual CARAPI_(Int32) GetButtonImageId(
            /* [in] */ Boolean on);

        //@Override
        virtual CARAPI_(Int32) GetPosition();

        //@Override
        virtual CARAPI_(Int32) GetActualState(
            /* [in] */ IContext* context);

        //@Override
        virtual CARAPI OnActualStateChange(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    protected:
        //@Override
        virtual CARAPI RequestStateChange(
            /* [in] */ IContext* context,
            /* [in] */ Boolean desiredState);

    private:
        /**
         * Converts WifiManager's state values into our
         * Wifi/Bluetooth-common state values.
         */
        static CARAPI_(Int32) WifiStateToFiveState(
            /* [in] */ Int32 wifiState);
    };

    // Actually request the wifi change and persistent
    // settings write off the UI thread, as it can take a
    // user-noticeable amount of time, especially if there's
    // disk contention.
    class WifiStateTrackerTask
        //<Void, Void, Void>
        : public AsyncTask
    {
    public:
        WifiStateTrackerTask(
            /* [in] */ WifiStateTracker* host,
            /* [in] */ IWifiManager* wifiManager,
            /* [in] */ Boolean desiredState);

        ~WifiStateTrackerTask();

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

    private:
        WifiStateTracker* mHost;
        AutoPtr<IWifiManager> mWifiManager;
        Boolean mDesiredState;
    };

    /** Observer to watch for changes to the BRIGHTNESS setting */
    class SettingsObserver
        : public ContentObserver
    {
        friend class CSettingsAppWidgetProvider;

        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ IContext* context);

        CARAPI_(void) StartObserving();

        CARAPI_(void) StopObserving();

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        AutoPtr<IContext> mContext;
    };

    // /**
    //  * Subclass of StateTracker to get/set Bluetooth state.
    //  */
    // private static const class BluetoothStateTracker extends StateTracker {
    //     public Int32 GetContainerId() { return R::id::btn_bluetooth; }
    //     public Int32 GetButtonId() { return R::id::img_bluetooth; }
    //     public Int32 GetIndicatorId() { return R::id::ind_bluetooth; }
    //     public Int32 GetButtonDescription() { return R::string::gadget_bluetooth; }
    //     public Int32 GetButtonImageId(Boolean on) {
    //         return on ? R::drawable::ic_appwidget_settings_bluetooth_on_holo
    //                 : R::drawable::ic_appwidget_settings_bluetooth_off_holo;
    //     }

    //     //@Override
    //     public Int32 GetActualState(Context context) {
    //         if (sLocalBluetoothAdapter == NULL) {
    //             LocalBluetoothManager manager = LocalBluetoothManager->GetInstance(context);
    //             if (manager == NULL) {
    //                 return STATE_UNKNOWN;  // On emulator?
    //             }
    //             sLocalBluetoothAdapter = manager->GetBluetoothAdapter();
    //         }
    //         return BluetoothStateToFiveState(sLocalBluetoothAdapter->GetBluetoothState());
    //     }

    //     //@Override
    //     protected void RequestStateChange(Context context, final Boolean desiredState) {
    //         if (sLocalBluetoothAdapter == NULL) {
    //             Logger::D(TAG, "No LocalBluetoothManager");
    //             return;
    //         }
    //         // Actually request the Bluetooth change and persistent
    //         // settings write off the UI thread, as it can take a
    //         // user-noticeable amount of time, especially if there's
    //         // disk contention.
    //         new AsyncTask<Void, Void, Void>() {
    //             //@Override
    //             protected Void DoInBackground(Void... args) {
    //                 sLocalBluetoothAdapter->SetBluetoothEnabled(desiredState);
    //                 return NULL;
    //             }
    //         }.Execute();
    //     }

    //     //@Override
    //     CARAPI OnActualStateChange(Context context, Intent intent) {
    //         if (!BluetoothAdapter.ACTION_STATE_CHANGED->Equals(intent->GetAction())) {
    //             return;
    //         }
    //         Int32 bluetoothState = intent->GetIntExtra(BluetoothAdapter.EXTRA_STATE, -1);
    //         SetCurrentState(context, BluetoothStateToFiveState(bluetoothState));
    //     }

    //     /**
    //      * Converts BluetoothAdapter's state values into our
    //      * Wifi/Bluetooth-common state values.
    //      */
    //     private static Int32 BluetoothStateToFiveState(Int32 bluetoothState) {
    //         switch (bluetoothState) {
    //             case BluetoothAdapter.STATE_OFF:
    //                 return STATE_DISABLED;
    //             case BluetoothAdapter.STATE_ON:
    //                 return STATE_ENABLED;
    //             case BluetoothAdapter.STATE_TURNING_ON:
    //                 return STATE_TURNING_ON;
    //             case BluetoothAdapter.STATE_TURNING_OFF:
    //                 return STATE_TURNING_OFF;
    //             default:
    //                 return STATE_UNKNOWN;
    //         }
    //     }
    // };

    // /**
    //  * Subclass of StateTracker for location state.
    //  */
    // private static const class LocationStateTracker extends StateTracker {
    //     private Int32 mCurrentLocationMode = Settings.Secure.LOCATION_MODE_OFF;

    //     public Int32 GetContainerId() { return R::id::btn_location; }
    //     public Int32 GetButtonId() { return R::id::img_location; }
    //     public Int32 GetIndicatorId() { return R::id::ind_location; }
    //     public Int32 GetButtonDescription() { return R::string::gadget_location; }
    //     public Int32 GetButtonImageId(Boolean on) {
    //         if (on) {
    //             switch (mCurrentLocationMode) {
    //                 case Settings.Secure.LOCATION_MODE_HIGH_ACCURACY:
    //                 case Settings.Secure.LOCATION_MODE_SENSORS_ONLY:
    //                     return R::drawable::ic_appwidget_settings_location_on_holo;
    //                 default:
    //                     return R::drawable::ic_appwidget_settings_location_saving_holo;
    //             }
    //         }

    //         return R::drawable::ic_appwidget_settings_location_off_holo;
    //     }

    //     //@Override
    //     public Int32 GetActualState(Context context) {
    //         ContentResolver resolver = context->GetContentResolver();
    //         mCurrentLocationMode = Settings.Secure->GetInt(resolver,
    //                 Settings.Secure.LOCATION_MODE, Settings.Secure.LOCATION_MODE_OFF);
    //         return (mCurrentLocationMode == Settings.Secure.LOCATION_MODE_OFF)
    //                 ? STATE_DISABLED : STATE_ENABLED;
    //     }

    //     //@Override
    //     CARAPI OnActualStateChange(Context context, Intent unused) {
    //         // Note: the broadcast location providers changed intent
    //         // doesn't include an extras bundles saying what the new value is.
    //         SetCurrentState(context, GetActualState(context));
    //     }

    //     //@Override
    //     CARAPI RequestStateChange(final Context context, final Boolean desiredState) {
    //         final ContentResolver resolver = context->GetContentResolver();
    //         new AsyncTask<Void, Void, Boolean>() {
    //             //@Override
    //             protected Boolean DoInBackground(Void... args) {
    //                 final UserManager um =
    //                         (UserManager) context->GetSystemService(Context.USER_SERVICE);
    //                 if (!um->HasUserRestriction(UserManager.DISALLOW_SHARE_LOCATION)) {
    //                     Int32 currentMode = Settings.Secure->GetInt(resolver,
    //                             Settings.Secure.LOCATION_MODE, Settings.Secure.LOCATION_MODE_OFF);
    //                     Int32 mode = Settings.Secure.LOCATION_MODE_HIGH_ACCURACY;
    //                     switch (currentMode) {
    //                         case Settings.Secure.LOCATION_MODE_HIGH_ACCURACY:
    //                             mode = Settings.Secure.LOCATION_MODE_BATTERY_SAVING;
    //                             break;
    //                         case Settings.Secure.LOCATION_MODE_BATTERY_SAVING:
    //                             mode = Settings.Secure.LOCATION_MODE_HIGH_ACCURACY;
    //                             break;
    //                         case Settings.Secure.LOCATION_MODE_SENSORS_ONLY:
    //                             mode = Settings.Secure.LOCATION_MODE_OFF;
    //                             break;
    //                         case Settings.Secure.LOCATION_MODE_OFF:
    //                             mode = Settings.Secure.LOCATION_MODE_HIGH_ACCURACY;
    //                             break;
    //                     }
    //                     Settings.Secure->PutInt(resolver, Settings.Secure.LOCATION_MODE, mode);
    //                     return mode != Settings.Secure.LOCATION_MODE_OFF;
    //                 }

    //                 return GetActualState(context) == STATE_ENABLED;
    //             }

    //             //@Override
    //             protected void OnPostExecute(Boolean result) {
    //                 SetCurrentState(
    //                     context,
    //                     result ? STATE_ENABLED : STATE_DISABLED);
    //                 UpdateWidget(context);
    //             }
    //         }.Execute();
    //     }
    // };

    // /**
    //  * Subclass of StateTracker for sync state.
    //  */
    // private static const class SyncStateTracker extends StateTracker {
    //     public Int32 GetContainerId() { return R::id::btn_sync; }
    //     public Int32 GetButtonId() { return R::id::img_sync; }
    //     public Int32 GetIndicatorId() { return R::id::ind_sync; }
    //     public Int32 GetButtonDescription() { return R::string::gadget_sync; }
    //     public Int32 GetButtonImageId(Boolean on) {
    //         return on ? R::drawable::ic_appwidget_settings_sync_on_holo
    //                 : R::drawable::ic_appwidget_settings_sync_off_holo;
    //     }

    //     //@Override
    //     public Int32 GetActualState(Context context) {
    //         Boolean on = ContentResolver->GetMasterSyncAutomatically();
    //         return on ? STATE_ENABLED : STATE_DISABLED;
    //     }

    //     //@Override
    //     CARAPI OnActualStateChange(Context context, Intent unused) {
    //         SetCurrentState(context, GetActualState(context));
    //     }

    //     //@Override
    //     CARAPI RequestStateChange(final Context context, final Boolean desiredState) {
    //         final ConnectivityManager connManager =
    //                 (ConnectivityManager) context->GetSystemService(Context.CONNECTIVITY_SERVICE);
    //         final Boolean sync = ContentResolver->GetMasterSyncAutomatically();

    //         new AsyncTask<Void, Void, Boolean>() {
    //             //@Override
    //             protected Boolean DoInBackground(Void... args) {
    //                 // Turning sync on.
    //                 if (desiredState) {
    //                     if (!sync) {
    //                         ContentResolver->SetMasterSyncAutomatically(TRUE);
    //                     }
    //                     return TRUE;
    //                 }

    //                 // Turning sync off
    //                 if (sync) {
    //                     ContentResolver->SetMasterSyncAutomatically(FALSE);
    //                 }
    //                 return FALSE;
    //             }

    //             //@Override
    //             protected void OnPostExecute(Boolean result) {
    //                 SetCurrentState(
    //                     context,
    //                     result ? STATE_ENABLED : STATE_DISABLED);
    //                 UpdateWidget(context);
    //             }
    //         }.Execute();
    //     }
    // };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CSettingsAppWidgetProvider();

    ~CSettingsAppWidgetProvider();

    CARAPI constructor();

    //@Override
    CARAPI OnUpdate(
        /* [in] */ IContext* context,
        /* [in] */ IAppWidgetManager* appWidgetManager,
        /* [in] */ ArrayOf<Int32>* appWidgetIds);

    //@Override
    CARAPI OnEnabled(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnDisabled(
        /* [in] */ IContext* context);

    /**
     * Load image for given widget and build {@link RemoteViews} for it.
     */
    static CARAPI_(AutoPtr<IRemoteViews>) BuildUpdate(
        /* [in] */ IContext* context);

    /**
     * Updates the widget when something changes, or when a button is pushed.
     *
     * @param context
     */
    static CARAPI_(void) UpdateWidget(
        /* [in] */ IContext* context);

    /**
     * Receives and processes a button pressed intent or state change.
     *
     * @param context
     * @param intent  Indicates the pressed button.
     */
    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    static CARAPI_(void) CheckObserver(
        /* [in] */ IContext* context);

    /**
     * Updates the buttons based on the underlying states of wifi, etc.
     *
     * @param views   The RemoteViews to update.
     * @param context
     */
    static CARAPI_(void) UpdateButtons(
        /* [in] */ IRemoteViews* views,
        /* [in] */ IContext* context);

    /**
     * Creates PendingIntent to notify the widget of a button click.
     *
     * @param context
     * @return
     */
    static CARAPI_(AutoPtr<IPendingIntent>) GetLaunchPendingIntent(
        /* [in] */ IContext* context,
        /* [in] */ Int32 buttonId);

    /**
     * Gets brightness level.
     *
     * @param context
     * @return brightness level between 0 and 255.
     */
    static CARAPI_(Int32) GetBrightness(
        /* [in] */ IContext* context);

    /**
     * Gets state of brightness mode.
     *
     * @param context
     * @return TRUE if auto brightness is on.
     */
    static CARAPI_(Boolean) GetBrightnessMode(
        /* [in] */ IContext* context);

    /**
     * Increases or decreases the brightness.
     *
     * @param context
     */
    // void ToggleBrightness(Context context) {
    //     try {
    //         IPowerManager power = IPowerManager.Stub->AsInterface(
    //                 ServiceManager->GetService("power"));
    //         if (power != NULL) {
    //             PowerManager pm = (PowerManager)context->GetSystemService(Context.POWER_SERVICE);

    //             ContentResolver cr = context->GetContentResolver();
    //             Int32 brightness = Settings.System->GetInt(cr,
    //                     Settings.System.SCREEN_BRIGHTNESS);
    //             Int32 brightnessMode = Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL;
    //             //Only get brightness setting if available
    //             if (context->GetResources()->GetBoolean(
    //                     R.bool.config_automatic_brightness_available)) {
    //                 brightnessMode = Settings.System->GetInt(cr,
    //                         Settings.System.SCREEN_BRIGHTNESS_MODE);
    //             }

    //             // Rotate AUTO -> MINIMUM -> DEFAULT -> MAXIMUM
    //             // Technically, not a toggle...
    //             if (brightnessMode == Settings.System.SCREEN_BRIGHTNESS_MODE_AUTOMATIC) {
    //                 brightness = pm->GetMinimumScreenBrightnessSetting();
    //                 brightnessMode = Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL;
    //             } else if (brightness < pm->GetDefaultScreenBrightnessSetting()) {
    //                 brightness = pm->GetDefaultScreenBrightnessSetting();
    //             } else if (brightness < pm->GetMaximumScreenBrightnessSetting()) {
    //                 brightness = pm->GetMaximumScreenBrightnessSetting();
    //             } else {
    //                 brightnessMode = Settings.System.SCREEN_BRIGHTNESS_MODE_AUTOMATIC;
    //                 brightness = pm->GetMinimumScreenBrightnessSetting();
    //             }

    //             if (context->GetResources()->GetBoolean(
    //                     R.bool.config_automatic_brightness_available)) {
    //                 // Set screen brightness mode (automatic or manual)
    //                 Settings.System->PutInt(context->GetContentResolver(),
    //                         Settings.System.SCREEN_BRIGHTNESS_MODE,
    //                         brightnessMode);
    //             } else {
    //                 // Make sure we set the brightness if automatic mode isn't available
    //                 brightnessMode = Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL;
    //             }
    //             if (brightnessMode == Settings.System.SCREEN_BRIGHTNESS_MODE_MANUAL) {
    //                 power->SetTemporaryScreenBrightnessSettingOverride(brightness);
    //                 Settings.System->PutInt(cr, Settings.System.SCREEN_BRIGHTNESS, brightness);
    //             }
    //         }
    //     } catch (RemoteException e) {
    //         Logger::D(TAG, "toggleBrightness: " + e);
    //     } catch (Settings.SettingNotFoundException e) {
    //         Logger::D(TAG, "toggleBrightness: " + e);
    //     }
    // }

protected:
    static const String TAG;

    static const AutoPtr<IComponentName> THIS_APPWIDGET;

private:
    // static LocalBluetoothAdapter sLocalBluetoothAdapter = NULL;

    static const Int32 BUTTON_WIFI;
    static const Int32 BUTTON_BRIGHTNESS;
    static const Int32 BUTTON_SYNC;
    static const Int32 BUTTON_LOCATION;
    static const Int32 BUTTON_BLUETOOTH;

    // This widget keeps track of two sets of states:
    // "3-state": STATE_DISABLED, STATE_ENABLED, STATE_INTERMEDIATE
    // "5-state": STATE_DISABLED, STATE_ENABLED, STATE_TURNING_ON, STATE_TURNING_OFF, STATE_UNKNOWN
    static const Int32 STATE_DISABLED = 0;
    static const Int32 STATE_ENABLED = 1;
    static const Int32 STATE_TURNING_ON = 2;
    static const Int32 STATE_TURNING_OFF = 3;
    static const Int32 STATE_UNKNOWN;
    static const Int32 STATE_INTERMEDIATE = 5;

    // Position in the widget bar, to enable different graphics for left, center and right buttons
    static const Int32 POS_LEFT;
    static const Int32 POS_CENTER;
    static const Int32 POS_RIGHT;

    static const AutoPtr< ArrayOf<Int32> > IND_DRAWABLE_OFF;

    static const AutoPtr< ArrayOf<Int32> > IND_DRAWABLE_MID;

    static const AutoPtr< ArrayOf<Int32> > IND_DRAWABLE_ON;

    /** Minimum brightness at which the indicator is shown at half-full and ON */
    static const Float HALF_BRIGHTNESS_THRESHOLD;
    /** Minimum brightness at which the indicator is shown at full */
    static const Float FULL_BRIGHTNESS_THRESHOLD;

    static const AutoPtr<StateTracker> sWifiState;
    // static const StateTracker sBluetoothState = new BluetoothStateTracker();
    // static const StateTracker sLocationState = new LocationStateTracker();
    // static const StateTracker sSyncState = new SyncStateTracker();
    static AutoPtr<SettingsObserver> sSettingsObserver;

};

}// namespace Widget
}// namespace Settings
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIDGET_CSETTINGSAPPWIDGETPROVIDER_H__
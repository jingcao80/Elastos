

package com.android.settings.location;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Location::ISettingInjectorService;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Widget::ISwitch;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::Widget::ISwitchBar;

using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IList;

/**
 * Location access settings.
 */
public class LocationSettings extends LocationSettingsBase
        implements SwitchBar.OnSwitchChangeListener {

    private static const String TAG = "LocationSettings";

    /** Key for preference screen "Mode" */
    private static const String KEY_LOCATION_MODE = "location_mode";
    /** Key for preference category "Recent location requests" */
    private static const String KEY_RECENT_LOCATION_REQUESTS = "recent_location_requests";
    /** Key for preference category "Location services" */
    private static const String KEY_LOCATION_SERVICES = "location_services";

    private SwitchBar mSwitchBar;
    private Switch mSwitch;
    private Boolean mValidListener = FALSE;
    private Preference mLocationMode;
    private PreferenceCategory mCategoryRecentLocationRequests;
    /** Receives UPDATE_INTENT  */
    private BroadcastReceiver mReceiver;
    private SettingsInjector injector;

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        final SettingsActivity activity = (SettingsActivity) GetActivity();

        mSwitchBar = activity->GetSwitchBar();
        mSwitch = mSwitchBar->GetSwitch();
        mSwitchBar->Show();
    }

    //@Override
    CARAPI OnDestroyView() {
        super->OnDestroyView();
        mSwitchBar->Hide();
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        CreatePreferenceHierarchy();
        if (!mValidListener) {
            mSwitchBar->AddOnSwitchChangeListener(this);
            mValidListener = TRUE;
        }
    }

    //@Override
    CARAPI OnPause() {
        try {
            GetActivity()->UnregisterReceiver(mReceiver);
        } catch (RuntimeException e) {
            // Ignore exceptions caused by race condition
            if (Log->IsLoggable(TAG, Log.VERBOSE)) {
                Logger::V(TAG, "Swallowing " + e);
            }
        }
        if (mValidListener) {
            mSwitchBar->RemoveOnSwitchChangeListener(this);
            mValidListener = FALSE;
        }
        super->OnPause();
    }

    private void AddPreferencesSorted(List<Preference> prefs, PreferenceGroup container) {
        // If there's some items to display, sort the items and add them to the container.
        Collections->Sort(prefs, new Comparator<Preference>() {
            //@Override
            public Int32 Compare(Preference lhs, Preference rhs) {
                return lhs->GetTitle()->ToString().CompareTo(rhs->GetTitle()->ToString());
            }
        });
        for (Preference entry : prefs) {
            container->AddPreference(entry);
        }
    }

    private PreferenceScreen CreatePreferenceHierarchy() {
        final SettingsActivity activity = (SettingsActivity) GetActivity();
        PreferenceScreen root = GetPreferenceScreen();
        if (root != NULL) {
            root->RemoveAll();
        }
        AddPreferencesFromResource(R.xml.location_settings);
        root = GetPreferenceScreen();

        mLocationMode = root->FindPreference(KEY_LOCATION_MODE);
        mLocationMode->SetOnPreferenceClickListener(
                new Preference->OnPreferenceClickListener() {
                    //@Override
                    public Boolean OnPreferenceClick(Preference preference) {
                        activity->StartPreferencePanel(
                                LocationMode.class->GetName(), NULL,
                                R::string::location_mode_screen_title, NULL, LocationSettings.this,
                                0);
                        return TRUE;
                    }
                });

        mCategoryRecentLocationRequests =
                (PreferenceCategory) root->FindPreference(KEY_RECENT_LOCATION_REQUESTS);
        RecentLocationApps recentApps = new RecentLocationApps(activity);
        List<Preference> recentLocationRequests = recentApps->GetAppList();
        if (recentLocationRequests->Size() > 0) {
            AddPreferencesSorted(recentLocationRequests, mCategoryRecentLocationRequests);
        } else {
            // If there's no item to display, add a "No recent apps" item.
            Preference banner = new Preference(activity);
            banner->SetLayoutResource(R.layout.location_list_no_item);
            banner->SetTitle(R::string::location_no_recent_apps);
            banner->SetSelectable(FALSE);
            mCategoryRecentLocationRequests->AddPreference(banner);
        }

        AddLocationServices(activity, root);

        RefreshLocationMode();
        return root;
    }

    /**
     * Add the settings injected by external apps into the "App Settings" category. Hides the
     * category if there are no injected settings.
     *
     * Reloads the settings whenever receives
     * {@link SettingInjectorService#ACTION_INJECTED_SETTING_CHANGED}.
     */
    private void AddLocationServices(Context context, PreferenceScreen root) {
        PreferenceCategory categoryLocationServices =
                (PreferenceCategory) root->FindPreference(KEY_LOCATION_SERVICES);
        injector = new SettingsInjector(context);
        List<Preference> locationServices = injector->GetInjectedSettings();

        mReceiver = new BroadcastReceiver() {
            //@Override
            CARAPI OnReceive(Context context, Intent intent) {
                if (Log->IsLoggable(TAG, Log.DEBUG)) {
                    Logger::D(TAG, "Received settings change intent: " + intent);
                }
                injector->ReloadStatusMessages();
            }
        };

        IntentFilter filter = new IntentFilter();
        filter->AddAction(SettingInjectorService.ACTION_INJECTED_SETTING_CHANGED);
        context->RegisterReceiver(mReceiver, filter);

        if (locationServices->Size() > 0) {
            AddPreferencesSorted(locationServices, categoryLocationServices);
        } else {
            // If there's no item to display, remove the whole category.
            root->RemovePreference(categoryLocationServices);
        }
    }

    //@Override
    public Int32 GetHelpResource() {
        return R::string::help_url_location_access;
    }

    //@Override
    CARAPI OnModeChanged(Int32 mode, Boolean restricted) {
        switch (mode) {
            case android.provider.Settings.Secure.LOCATION_MODE_OFF:
                mLocationMode->SetSummary(R::string::location_mode_location_off_title);
                break;
            case android.provider.Settings.Secure.LOCATION_MODE_SENSORS_ONLY:
                mLocationMode->SetSummary(R::string::location_mode_sensors_only_title);
                break;
            case android.provider.Settings.Secure.LOCATION_MODE_BATTERY_SAVING:
                mLocationMode->SetSummary(R::string::location_mode_battery_saving_title);
                break;
            case android.provider.Settings.Secure.LOCATION_MODE_HIGH_ACCURACY:
                mLocationMode->SetSummary(R::string::location_mode_high_accuracy_title);
                break;
            default:
                break;
        }

        // Restricted user can't change the location mode, so disable the master switch. But in some
        // corner cases, the location might still be enabled. In such case the master switch should
        // be disabled but checked.
        Boolean enabled = (mode != android.provider.Settings.Secure.LOCATION_MODE_OFF);
        // Disable the whole switch bar instead of the switch itself. If we disabled the switch
        // only, it would be re-enabled again if the switch bar is not disabled.
        mSwitchBar->SetEnabled(!restricted);
        mLocationMode->SetEnabled(enabled && !restricted);
        mCategoryRecentLocationRequests->SetEnabled(enabled);

        if (enabled != mSwitch->IsChecked()) {
            // set listener to NULL so that that code below doesn't trigger OnCheckedChanged()
            if (mValidListener) {
                mSwitchBar->RemoveOnSwitchChangeListener(this);
            }
            mSwitch->SetChecked(enabled);
            if (mValidListener) {
                mSwitchBar->AddOnSwitchChangeListener(this);
            }
        }
        // As a safety measure, also reloads on location mode change to ensure the settings are
        // up-to-date even if an affected app doesn't send the setting changed broadcast.
        injector->ReloadStatusMessages();
    }

    /**
     * Listens to the state change of the location master switch.
     */
    //@Override
    CARAPI OnSwitchChanged(Switch switchView, Boolean isChecked) {
        if (isChecked) {
            SetLocationMode(android.provider.Settings.Secure.LOCATION_MODE_HIGH_ACCURACY);
        } else {
            SetLocationMode(android.provider.Settings.Secure.LOCATION_MODE_OFF);
        }
    }
}

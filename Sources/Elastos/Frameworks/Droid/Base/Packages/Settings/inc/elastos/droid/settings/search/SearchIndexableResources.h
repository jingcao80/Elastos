

package com.android.settings.search;

using Elastos::Droid::Provider::ISearchIndexableResource;

using Elastos::Droid::Settings::IDataUsageSummary;
using Elastos::Droid::Settings::IDateTimeSettings;
using Elastos::Droid::Settings::IDevelopmentSettings;
using Elastos::Droid::Settings::IDeviceInfoSettings;
using Elastos::Droid::Settings::IDisplaySettings;
using Elastos::Droid::Settings::IHomeSettings;
using Elastos::Droid::Settings::IScreenPinningSettings;
using Elastos::Droid::Settings::IPrivacySettings;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISecuritySettings;
using Elastos::Droid::Settings::IWallpaperTypeSettings;
using Elastos::Droid::Settings::IWirelessSettings;
using Elastos::Droid::Settings::Accessibility::IAccessibilitySettings;
using Elastos::Droid::Settings::Bluetooth::IBluetoothSettings;
using Elastos::Droid::Settings::Deviceinfo::IMemory;
using Elastos::Droid::Settings::Fuelgauge::IBatterySaverSettings;
using Elastos::Droid::Settings::Fuelgauge::IPowerUsageSummary;
using Elastos::Droid::Settings::Inputmethod::IInputMethodAndLanguageSettings;
using Elastos::Droid::Settings::Location::ILocationSettings;
using Elastos::Droid::Settings::Net::IDataUsageMeteredSettings;
using Elastos::Droid::Settings::Notification::INotificationSettings;
using Elastos::Droid::Settings::Notification::IOtherSoundSettings;
using Elastos::Droid::Settings::Notification::IZenModeSettings;
using Elastos::Droid::Settings::Print::IPrintSettingsFragment;
using Elastos::Droid::Settings::Sim::ISimSettings;
using Elastos::Droid::Settings::Users::IUserSettings;
using Elastos::Droid::Settings::Voice::IVoiceInputSettings;
using Elastos::Droid::Settings::Wifi::IAdvancedWifiSettings;
using Elastos::Droid::Settings::Wifi::ISavedAccessPointsWifiSettings;
using Elastos::Droid::Settings::Wifi::IWifiSettings;

using Elastos::Utility::ICollection;
using Elastos::Utility::IHashMap;

public class SearchIndexableResources {

    public static Int32 NO_DATA_RES_ID = 0;

    private static HashMap<String, SearchIndexableResource> sResMap =
            new HashMap<String, SearchIndexableResource>();

    static {
        sResMap->Put(WifiSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(WifiSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        WifiSettings.class->GetName(),
                        R.drawable.ic_settings_wireless));

        sResMap->Put(AdvancedWifiSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(AdvancedWifiSettings.class->GetName()),
                        R.xml.wifi_advanced_settings,
                        AdvancedWifiSettings.class->GetName(),
                        R.drawable.ic_settings_wireless));

        sResMap->Put(SavedAccessPointsWifiSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(SavedAccessPointsWifiSettings.class->GetName()),
                        R.xml.wifi_display_saved_access_points,
                        SavedAccessPointsWifiSettings.class->GetName(),
                        R.drawable.ic_settings_wireless));

        sResMap->Put(BluetoothSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(BluetoothSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        BluetoothSettings.class->GetName(),
                        R.drawable.ic_settings_bluetooth2));

        sResMap->Put(SimSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(SimSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        SimSettings.class->GetName(),
                        R.drawable.ic_sim_sd));

        sResMap->Put(DataUsageSummary.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(DataUsageSummary.class->GetName()),
                        NO_DATA_RES_ID,
                        DataUsageSummary.class->GetName(),
                        R.drawable.ic_settings_data_usage));

        sResMap->Put(DataUsageMeteredSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(DataUsageMeteredSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        DataUsageMeteredSettings.class->GetName(),
                        R.drawable.ic_settings_data_usage));

        sResMap->Put(WirelessSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(WirelessSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        WirelessSettings.class->GetName(),
                        R.drawable.ic_settings_more));

        sResMap->Put(HomeSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(HomeSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        HomeSettings.class->GetName(),
                        R.drawable.ic_settings_home));

        sResMap->Put(DisplaySettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(DisplaySettings.class->GetName()),
                        NO_DATA_RES_ID,
                        DisplaySettings.class->GetName(),
                        R.drawable.ic_settings_display));

        sResMap->Put(WallpaperTypeSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(WallpaperTypeSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        WallpaperTypeSettings.class->GetName(),
                        R.drawable.ic_settings_display));

        sResMap->Put(NotificationSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(NotificationSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        NotificationSettings.class->GetName(),
                        R.drawable.ic_settings_notifications));

        sResMap->Put(OtherSoundSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(OtherSoundSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        OtherSoundSettings.class->GetName(),
                        R.drawable.ic_settings_notifications));

        sResMap->Put(ZenModeSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(ZenModeSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        ZenModeSettings.class->GetName(),
                        R.drawable.ic_settings_notifications));

        sResMap->Put(Memory.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(Memory.class->GetName()),
                        NO_DATA_RES_ID,
                        Memory.class->GetName(),
                        R.drawable.ic_settings_storage));

        sResMap->Put(PowerUsageSummary.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(PowerUsageSummary.class->GetName()),
                        R.xml.power_usage_summary,
                        PowerUsageSummary.class->GetName(),
                        R.drawable.ic_settings_battery));

        sResMap->Put(BatterySaverSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(BatterySaverSettings.class->GetName()),
                        R.xml.battery_saver_settings,
                        BatterySaverSettings.class->GetName(),
                        R.drawable.ic_settings_battery));

        sResMap->Put(UserSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(UserSettings.class->GetName()),
                        R.xml.user_settings,
                        UserSettings.class->GetName(),
                        R.drawable.ic_settings_multiuser));

        sResMap->Put(LocationSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(LocationSettings.class->GetName()),
                        R.xml.location_settings,
                        LocationSettings.class->GetName(),
                        R.drawable.ic_settings_location));

        sResMap->Put(SecuritySettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(SecuritySettings.class->GetName()),
                        NO_DATA_RES_ID,
                        SecuritySettings.class->GetName(),
                        R.drawable.ic_settings_security));

        sResMap->Put(ScreenPinningSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(ScreenPinningSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        ScreenPinningSettings.class->GetName(),
                        R.drawable.ic_settings_security));

        sResMap->Put(InputMethodAndLanguageSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(InputMethodAndLanguageSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        InputMethodAndLanguageSettings.class->GetName(),
                        R.drawable.ic_settings_language));

        sResMap->Put(VoiceInputSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(VoiceInputSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        VoiceInputSettings.class->GetName(),
                        R.drawable.ic_settings_language));

        sResMap->Put(PrivacySettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(PrivacySettings.class->GetName()),
                        NO_DATA_RES_ID,
                        PrivacySettings.class->GetName(),
                        R.drawable.ic_settings_backup));

        sResMap->Put(DateTimeSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(DateTimeSettings.class->GetName()),
                        R.xml.date_time_prefs,
                        DateTimeSettings.class->GetName(),
                        R.drawable.ic_settings_date_time));

        sResMap->Put(AccessibilitySettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(AccessibilitySettings.class->GetName()),
                        NO_DATA_RES_ID,
                        AccessibilitySettings.class->GetName(),
                        R.drawable.ic_settings_accessibility));

        sResMap->Put(PrintSettingsFragment.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(PrintSettingsFragment.class->GetName()),
                        NO_DATA_RES_ID,
                        PrintSettingsFragment.class->GetName(),
                        R.drawable.ic_settings_print));

        sResMap->Put(DevelopmentSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(DevelopmentSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        DevelopmentSettings.class->GetName(),
                        R.drawable.ic_settings_development));

        sResMap->Put(DeviceInfoSettings.class->GetName(),
                new SearchIndexableResource(
                        Ranking->GetRankForClassName(DeviceInfoSettings.class->GetName()),
                        NO_DATA_RES_ID,
                        DeviceInfoSettings.class->GetName(),
                        R.drawable.ic_settings_about));
    }

    private SearchIndexableResources() {
    }

    public static Int32 Size() {
        return sResMap->Size();
    }

    public static SearchIndexableResource GetResourceByName(String className) {
        return sResMap->Get(className);
    }

    public static Collection<SearchIndexableResource> Values() {
        return sResMap->Values();
    }
}

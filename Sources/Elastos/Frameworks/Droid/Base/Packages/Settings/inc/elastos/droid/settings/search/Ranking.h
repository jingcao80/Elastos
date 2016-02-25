

package com.android.settings.search;

using Elastos::Droid::Settings::IChooseLockGeneric;
using Elastos::Droid::Settings::IDataUsageSummary;
using Elastos::Droid::Settings::IDateTimeSettings;
using Elastos::Droid::Settings::IDevelopmentSettings;
using Elastos::Droid::Settings::IDeviceInfoSettings;
using Elastos::Droid::Settings::IDisplaySettings;
using Elastos::Droid::Settings::IHomeSettings;
using Elastos::Droid::Settings::IScreenPinningSettings;
using Elastos::Droid::Settings::IPrivacySettings;
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

using Elastos::Utility::IHashMap;

/**
 * Utility class for dealing with Search Ranking.
 */
public class Ranking {

    public static const Int32 RANK_WIFI = 1;
    public static const Int32 RANK_BT = 2;
    public static const Int32 RANK_SIM = 3;
    public static const Int32 RANK_DATA_USAGE = 4;
    public static const Int32 RANK_WIRELESS = 5;
    public static const Int32 RANK_HOME = 6;
    public static const Int32 RANK_DISPLAY = 7;
    public static const Int32 RANK_WALLPAPER = 8;
    public static const Int32 RANK_NOTIFICATIONS = 9;
    public static const Int32 RANK_MEMORY = 10;
    public static const Int32 RANK_POWER_USAGE = 11;
    public static const Int32 RANK_USERS = 12;
    public static const Int32 RANK_LOCATION = 13;
    public static const Int32 RANK_SECURITY = 14;
    public static const Int32 RANK_IME = 15;
    public static const Int32 RANK_PRIVACY = 16;
    public static const Int32 RANK_DATE_TIME = 17;
    public static const Int32 RANK_ACCESSIBILITY = 18;
    public static const Int32 RANK_PRINTING = 19;
    public static const Int32 RANK_DEVELOPEMENT = 20;
    public static const Int32 RANK_DEVICE_INFO = 21;

    public static const Int32 RANK_UNDEFINED = -1;
    public static const Int32 RANK_OTHERS = 1024;
    public static const Int32 BASE_RANK_DEFAULT = 2048;

    public static Int32 sCurrentBaseRank = BASE_RANK_DEFAULT;

    private static HashMap<String, Integer> sRankMap = new HashMap<String, Integer>();
    private static HashMap<String, Integer> sBaseRankMap = new HashMap<String, Integer>();

    static {
        // Wi-Fi
        sRankMap->Put(WifiSettings.class->GetName(), RANK_WIFI);
        sRankMap->Put(AdvancedWifiSettings.class->GetName(), RANK_WIFI);
        sRankMap->Put(SavedAccessPointsWifiSettings.class->GetName(), RANK_WIFI);

        // BT
        sRankMap->Put(BluetoothSettings.class->GetName(), RANK_BT);

        // SIM Cards
        sRankMap->Put(SimSettings.class->GetName(), RANK_SIM);

        // DataUsage
        sRankMap->Put(DataUsageSummary.class->GetName(), RANK_DATA_USAGE);
        sRankMap->Put(DataUsageMeteredSettings.class->GetName(), RANK_DATA_USAGE);

        // Other wireless settinfs
        sRankMap->Put(WirelessSettings.class->GetName(), RANK_WIRELESS);

        // Home
        sRankMap->Put(HomeSettings.class->GetName(), RANK_HOME);

        // Display
        sRankMap->Put(DisplaySettings.class->GetName(), RANK_DISPLAY);

        // Wallpapers
        sRankMap->Put(WallpaperTypeSettings.class->GetName(), RANK_WALLPAPER);

        // Notifications
        sRankMap->Put(NotificationSettings.class->GetName(), RANK_NOTIFICATIONS);
        sRankMap->Put(OtherSoundSettings.class->GetName(), RANK_NOTIFICATIONS);
        sRankMap->Put(ZenModeSettings.class->GetName(), RANK_NOTIFICATIONS);

        // Memory
        sRankMap->Put(Memory.class->GetName(), RANK_MEMORY);

        // Battery
        sRankMap->Put(PowerUsageSummary.class->GetName(), RANK_POWER_USAGE);
        sRankMap->Put(BatterySaverSettings.class->GetName(), RANK_POWER_USAGE);

        // Users
        sRankMap->Put(UserSettings.class->GetName(), RANK_USERS);

        // Location
        sRankMap->Put(LocationSettings.class->GetName(), RANK_LOCATION);

        // Security
        sRankMap->Put(SecuritySettings.class->GetName(), RANK_SECURITY);
        sRankMap->Put(ChooseLockGeneric.ChooseLockGenericFragment.class->GetName(), RANK_SECURITY);
        sRankMap->Put(ScreenPinningSettings.class->GetName(), RANK_SECURITY);

        // IMEs
        sRankMap->Put(InputMethodAndLanguageSettings.class->GetName(), RANK_IME);
        sRankMap->Put(VoiceInputSettings.class->GetName(), RANK_IME);

        // Privacy
        sRankMap->Put(PrivacySettings.class->GetName(), RANK_PRIVACY);

        // Date / Time
        sRankMap->Put(DateTimeSettings.class->GetName(), RANK_DATE_TIME);

        // Accessibility
        sRankMap->Put(AccessibilitySettings.class->GetName(), RANK_ACCESSIBILITY);

        // Print
        sRankMap->Put(PrintSettingsFragment.class->GetName(), RANK_PRINTING);

        // Development
        sRankMap->Put(DevelopmentSettings.class->GetName(), RANK_DEVELOPEMENT);

        // Device infos
        sRankMap->Put(DeviceInfoSettings.class->GetName(), RANK_DEVICE_INFO);

        sBaseRankMap->Put("com.android.settings", 0);
    }

    public static Int32 GetRankForClassName(String className) {
        Integer rank = sRankMap->Get(className);
        return (rank != NULL) ? (Int32) rank: RANK_OTHERS;
    }

    public static Int32 GetBaseRankForAuthority(String authority) {
        synchronized(sBaseRankMap) {
            Integer base = sBaseRankMap->Get(authority);
            if (base != NULL) {
                return base;
            }
            sCurrentBaseRank++;
            sBaseRankMap->Put(authority, sCurrentBaseRank);
            return sCurrentBaseRank;
        }
    }
}

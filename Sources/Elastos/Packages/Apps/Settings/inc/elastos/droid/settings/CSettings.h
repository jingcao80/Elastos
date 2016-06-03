
#ifndef __ELASTOS_DROID_SETTINGS_CSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CSETTINGS_H__

#include "elastos/droid/settings/SettingsActivity.h"
#include "_Elastos_Droid_Settings_CSettings.h"

using Elastos::Droid::Settings::SettingsActivity;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Top-level Settings activity
 */
CarClass(CSettings) , public SettingsActivity
{
public:
    CAR_OBJECT_DECL()

    /*
    * Settings subclasses for launching independently.
    */
    class BluetoothSettingsActivity : public SettingsActivity { /* empty */ };
    class WirelessSettingsActivity : public SettingsActivity { /* empty */ };
    class SimSettingsActivity : public SettingsActivity { /* empty */ };
    class TetherSettingsActivity : public SettingsActivity { /* empty */ };
    class VpnSettingsActivity : public SettingsActivity { /* empty */ };
    class DateTimeSettingsActivity : public SettingsActivity { /* empty */ };
    class StorageSettingsActivity : public SettingsActivity { /* empty */ };
    class WifiSettingsActivity : public SettingsActivity { /* empty */ };
    class WifiP2pSettingsActivity : public SettingsActivity { /* empty */ };
    class InputMethodAndLanguageSettingsActivity : public SettingsActivity { /* empty */ };
    class KeyboardLayoutPickerActivity : public SettingsActivity { /* empty */ };
    class InputMethodAndSubtypeEnablerActivity : public SettingsActivity { /* empty */ };
    class VoiceInputSettingsActivity : public SettingsActivity { /* empty */ };
    class SpellCheckersSettingsActivity : public SettingsActivity { /* empty */ };
    class LocalePickerActivity : public SettingsActivity { /* empty */ };
    class UserDictionarySettingsActivity : public SettingsActivity { /* empty */ };
    class HomeSettingsActivity : public SettingsActivity { /* empty */ };
    class DisplaySettingsActivity : public SettingsActivity { /* empty */ };
    class DeviceInfoSettingsActivity : public SettingsActivity { /* empty */ };
    class ApplicationSettingsActivity : public SettingsActivity { /* empty */ };
    class ManageApplicationsActivity : public SettingsActivity { /* empty */ };

    class AppOpsSummaryActivity
        : public SettingsActivity
    {
    public:
        //@Override
        CARAPI_(Boolean) IsValidFragment(
            /* [in] */ const String& className);
    };

    class StorageUseActivity : public SettingsActivity { /* empty */ };
    class DevelopmentSettingsActivity : public SettingsActivity { /* empty */ };
    class AccessibilitySettingsActivity : public SettingsActivity { /* empty */ };
    class CaptioningSettingsActivity : public SettingsActivity { /* empty */ };
    class AccessibilityInversionSettingsActivity : public SettingsActivity { /* empty */ };
    class AccessibilityContrastSettingsActivity : public SettingsActivity { /* empty */ };
    class AccessibilityDaltonizerSettingsActivity : public SettingsActivity { /* empty */ };
    class SecuritySettingsActivity : public SettingsActivity { /* empty */ };
    class UsageAccessSettingsActivity : public SettingsActivity { /* empty */ };
    class LocationSettingsActivity : public SettingsActivity { /* empty */ };
    class PrivacySettingsActivity : public SettingsActivity { /* empty */ };
    class RunningServicesActivity : public SettingsActivity { /* empty */ };
    class ManageAccountsSettingsActivity : public SettingsActivity { /* empty */ };
    class PowerUsageSummaryActivity : public SettingsActivity { /* empty */ };
    class BatterySaverSettingsActivity : public SettingsActivity { /* empty */ };
    class AccountSyncSettingsActivity : public SettingsActivity { /* empty */ };
    class AccountSettingsActivity : public SettingsActivity { /* empty */ };
    class AccountSyncSettingsInAddAccountActivity : public SettingsActivity { /* empty */ };
    class CryptKeeperSettingsActivity : public SettingsActivity { /* empty */ };
    class DeviceAdminSettingsActivity : public SettingsActivity { /* empty */ };
    class DataUsageSummaryActivity : public SettingsActivity { /* empty */ };
    class AdvancedWifiSettingsActivity : public SettingsActivity { /* empty */ };
    class SavedAccessPointsSettingsActivity : public SettingsActivity { /* empty */ };
    class TextToSpeechSettingsActivity : public SettingsActivity { /* empty */ };
    class AndroidBeamSettingsActivity : public SettingsActivity { /* empty */ };
    class WifiDisplaySettingsActivity : public SettingsActivity { /* empty */ };
    class DreamSettingsActivity : public SettingsActivity { /* empty */ };
    class NotificationStationActivity : public SettingsActivity { /* empty */ };
    class UserSettingsActivity : public SettingsActivity { /* empty */ };
    class NotificationAccessSettingsActivity : public SettingsActivity { /* empty */ };
    class ConditionProviderSettingsActivity : public SettingsActivity { /* empty */ };
    class UsbSettingsActivity : public SettingsActivity { /* empty */ };
    class TrustedCredentialsSettingsActivity : public SettingsActivity { /* empty */ };
    class PaymentSettingsActivity : public SettingsActivity { /* empty */ };
    class PrintSettingsActivity : public SettingsActivity { /* empty */ };
    class PrintJobSettingsActivity : public SettingsActivity { /* empty */ };
    class ZenModeSettingsActivity : public SettingsActivity { /* empty */ };
    class NotificationSettingsActivity : public SettingsActivity { /* empty */ };
    class NotificationAppListActivity : public SettingsActivity { /* empty */ };
    class AppNotificationSettingsActivity : public SettingsActivity { /* empty */ };
    class OtherSoundSettingsActivity : public SettingsActivity { /* empty */ };
    class QuickLaunchSettingsActivity : public SettingsActivity { /* empty */ };

    class TopLevelSettings : public SettingsActivity { /* empty */ };
    class ApnSettingsActivity : public SettingsActivity { /* empty */ };
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CSETTINGS_H__


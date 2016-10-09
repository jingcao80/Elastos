
#include "elastos/droid/settings/CDateTimeSettings.h"
#include "elastos/droid/settings/CDateTimeSettingsSetupWizard.h"
#include "elastos/droid/settings/CChooseLockGeneric.h"
#include "elastos/droid/settings/CChooseLockGenericFragment.h"
#include "elastos/droid/settings/CChooseLockGenericInternalActivity.h"
#include "elastos/droid/settings/CEncryptionInterstitial.h"
#include "elastos/droid/settings/CEncryptionInterstitialFragment.h"
#include "elastos/droid/settings/CSecuritySettings.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/CSettingsDialogFragment.h"

#include "elastos/droid/settings/CSettingsBluetoothSettingsActivity.h"
#include "elastos/droid/settings/CSettingsWirelessSettingsActivity.h"
#include "elastos/droid/settings/CSettingsSimSettingsActivity.h"
#include "elastos/droid/settings/CSettingsTetherSettingsActivity.h"
#include "elastos/droid/settings/CSettingsVpnSettingsActivity.h"
#include "elastos/droid/settings/CSettingsDateTimeSettingsActivity.h"
#include "elastos/droid/settings/CSettingsStorageSettingsActivity.h"
#include "elastos/droid/settings/CSettingsWifiSettingsActivity.h"
#include "elastos/droid/settings/CSettingsWifiP2pSettingsActivity.h"
#include "elastos/droid/settings/CSettingsInputMethodAndLanguageSettingsActivity.h"
#include "elastos/droid/settings/CSettingsKeyboardLayoutPickerActivity.h"
#include "elastos/droid/settings/CSettingsInputMethodAndSubtypeEnablerActivity.h"
#include "elastos/droid/settings/CSettingsVoiceInputSettingsActivity.h"
#include "elastos/droid/settings/CSettingsSpellCheckersSettingsActivity.h"
#include "elastos/droid/settings/CSettingsLocalePickerActivity.h"
#include "elastos/droid/settings/CSettingsUserDictionarySettingsActivity.h"
#include "elastos/droid/settings/CSettingsHomeSettingsActivity.h"
#include "elastos/droid/settings/CSettingsDisplaySettingsActivity.h"
#include "elastos/droid/settings/CSettingsDeviceInfoSettingsActivity.h"
#include "elastos/droid/settings/CSettingsApplicationSettingsActivity.h"
#include "elastos/droid/settings/CSettingsManageApplicationsActivity.h"
#include "elastos/droid/settings/CSettingsAppOpsSummaryActivity.h"
#include "elastos/droid/settings/CSettingsStorageUseActivity.h"
#include "elastos/droid/settings/CSettingsDevelopmentSettingsActivity.h"
#include "elastos/droid/settings/CSettingsAccessibilitySettingsActivity.h"
#include "elastos/droid/settings/CSettingsCaptioningSettingsActivity.h"
#include "elastos/droid/settings/CSettingsAccessibilityInversionSettingsActivity.h"
#include "elastos/droid/settings/CSettingsAccessibilityContrastSettingsActivity.h"
#include "elastos/droid/settings/CSettingsAccessibilityDaltonizerSettingsActivity.h"
#include "elastos/droid/settings/CSettingsSecuritySettingsActivity.h"
#include "elastos/droid/settings/CSettingsUsageAccessSettingsActivity.h"
#include "elastos/droid/settings/CSettingsLocationSettingsActivity.h"
#include "elastos/droid/settings/CSettingsPrivacySettingsActivity.h"
#include "elastos/droid/settings/CSettingsRunningServicesActivity.h"
#include "elastos/droid/settings/CSettingsManageAccountsSettingsActivity.h"
#include "elastos/droid/settings/CSettingsPowerUsageSummaryActivity.h"
#include "elastos/droid/settings/CSettingsAccountSyncSettingsActivity.h"
#include "elastos/droid/settings/CSettingsAccountSettingsActivity.h"
#include "elastos/droid/settings/CSettingsAccountSyncSettingsInAddAccountActivity.h"
#include "elastos/droid/settings/CSettingsCryptKeeperSettingsActivity.h"
#include "elastos/droid/settings/CSettingsDeviceAdminSettingsActivity.h"
#include "elastos/droid/settings/CSettingsDataUsageSummaryActivity.h"
#include "elastos/droid/settings/CSettingsAdvancedWifiSettingsActivity.h"
#include "elastos/droid/settings/CSettingsSavedAccessPointsSettingsActivity.h"
#include "elastos/droid/settings/CSettingsTextToSpeechSettingsActivity.h"
#include "elastos/droid/settings/CSettingsAndroidBeamSettingsActivity.h"
#include "elastos/droid/settings/CSettingsWifiDisplaySettingsActivity.h"
#include "elastos/droid/settings/CSettingsDreamSettingsActivity.h"
#include "elastos/droid/settings/CSettingsNotificationStationActivity.h"
#include "elastos/droid/settings/CSettingsUserSettingsActivity.h"
#include "elastos/droid/settings/CSettingsNotificationAccessSettingsActivity.h"
#include "elastos/droid/settings/CSettingsConditionProviderSettingsActivity.h"
#include "elastos/droid/settings/CSettingsUsbSettingsActivity.h"
#include "elastos/droid/settings/CSettingsTrustedCredentialsSettingsActivity.h"
#include "elastos/droid/settings/CSettingsPaymentSettingsActivity.h"
#include "elastos/droid/settings/CSettingsPrintSettingsActivity.h"
#include "elastos/droid/settings/CSettingsPrintJobSettingsActivity.h"
#include "elastos/droid/settings/CSettingsZenModeSettingsActivity.h"
#include "elastos/droid/settings/CSettingsNotificationSettingsActivity.h"
#include "elastos/droid/settings/CSettingsNotificationAppListActivity.h"
#include "elastos/droid/settings/CSettingsAppNotificationSettingsActivity.h"
#include "elastos/droid/settings/CSettingsOtherSoundSettingsActivity.h"
#include "elastos/droid/settings/CSettingsQuickLaunchSettingsActivity.h"

#include "elastos/droid/settings/CSettingsTopLevelSettings.h"
#include "elastos/droid/settings/CSettingsApnSettingsActivity.h"
// #include "elastos/droid/settings/CSettingsBlacklistSettingsActivity.h"
// #include "elastos/droid/settings/CSettingsProfilesSettingsActivity.h"
#include "elastos/droid/settings/CSettingsSoundSettingsActivity.h"
#include "elastos/droid/settings/CSettingsLockScreenSettingsActivity.h"
// #include "elastos/droid/settings/CSettingsLiveDisplayActivity.h"
// #include "elastos/droid/settings/CSettingsDisplayRotationActivity.h"
#include "elastos/droid/settings/CSettingsAnonymousStatsActivity.h"

#include "elastos/droid/settings/CWirelessSettings.h"
#include "elastos/droid/settings/CZonePicker.h"

namespace Elastos {
namespace Droid {
namespace Settings {

CAR_OBJECT_IMPL(CDateTimeSettings);
CAR_OBJECT_IMPL(CDateTimeSettingsSetupWizard);
CAR_OBJECT_IMPL(CChooseLockGeneric);
CAR_OBJECT_IMPL(CChooseLockGenericFragment);
CAR_OBJECT_IMPL(CChooseLockGenericInternalActivity);
CAR_OBJECT_IMPL(CEncryptionInterstitial);
CAR_OBJECT_IMPL(CEncryptionInterstitialFragment);
CAR_OBJECT_IMPL(CSecuritySettings);
CAR_OBJECT_IMPL(CSettingsActivity);
CAR_OBJECT_IMPL(CSettingsDialogFragment);

CAR_OBJECT_IMPL(CSettingsBluetoothSettingsActivity)
CAR_OBJECT_IMPL(CSettingsWirelessSettingsActivity)
CAR_OBJECT_IMPL(CSettingsSimSettingsActivity)
CAR_OBJECT_IMPL(CSettingsTetherSettingsActivity)
CAR_OBJECT_IMPL(CSettingsVpnSettingsActivity)
CAR_OBJECT_IMPL(CSettingsDateTimeSettingsActivity)
CAR_OBJECT_IMPL(CSettingsStorageSettingsActivity)
CAR_OBJECT_IMPL(CSettingsWifiSettingsActivity)
CAR_OBJECT_IMPL(CSettingsWifiP2pSettingsActivity)
CAR_OBJECT_IMPL(CSettingsInputMethodAndLanguageSettingsActivity)
CAR_OBJECT_IMPL(CSettingsKeyboardLayoutPickerActivity)
CAR_OBJECT_IMPL(CSettingsInputMethodAndSubtypeEnablerActivity)
CAR_OBJECT_IMPL(CSettingsVoiceInputSettingsActivity)
CAR_OBJECT_IMPL(CSettingsSpellCheckersSettingsActivity)
CAR_OBJECT_IMPL(CSettingsLocalePickerActivity)
CAR_OBJECT_IMPL(CSettingsUserDictionarySettingsActivity)
CAR_OBJECT_IMPL(CSettingsHomeSettingsActivity)
CAR_OBJECT_IMPL(CSettingsDisplaySettingsActivity)
CAR_OBJECT_IMPL(CSettingsDeviceInfoSettingsActivity)
CAR_OBJECT_IMPL(CSettingsApplicationSettingsActivity)
CAR_OBJECT_IMPL(CSettingsManageApplicationsActivity)
CAR_OBJECT_IMPL(CSettingsAppOpsSummaryActivity)
CAR_OBJECT_IMPL(CSettingsStorageUseActivity)
CAR_OBJECT_IMPL(CSettingsDevelopmentSettingsActivity)
CAR_OBJECT_IMPL(CSettingsAccessibilitySettingsActivity)
CAR_OBJECT_IMPL(CSettingsCaptioningSettingsActivity)
CAR_OBJECT_IMPL(CSettingsAccessibilityInversionSettingsActivity)
CAR_OBJECT_IMPL(CSettingsAccessibilityContrastSettingsActivity)
CAR_OBJECT_IMPL(CSettingsAccessibilityDaltonizerSettingsActivity)
CAR_OBJECT_IMPL(CSettingsSecuritySettingsActivity)
CAR_OBJECT_IMPL(CSettingsUsageAccessSettingsActivity)
CAR_OBJECT_IMPL(CSettingsLocationSettingsActivity)
CAR_OBJECT_IMPL(CSettingsPrivacySettingsActivity)
CAR_OBJECT_IMPL(CSettingsRunningServicesActivity)
CAR_OBJECT_IMPL(CSettingsManageAccountsSettingsActivity)
CAR_OBJECT_IMPL(CSettingsPowerUsageSummaryActivity)
CAR_OBJECT_IMPL(CSettingsAccountSyncSettingsActivity)
CAR_OBJECT_IMPL(CSettingsAccountSettingsActivity)
CAR_OBJECT_IMPL(CSettingsAccountSyncSettingsInAddAccountActivity)
CAR_OBJECT_IMPL(CSettingsCryptKeeperSettingsActivity)
CAR_OBJECT_IMPL(CSettingsDeviceAdminSettingsActivity)
CAR_OBJECT_IMPL(CSettingsDataUsageSummaryActivity)
CAR_OBJECT_IMPL(CSettingsAdvancedWifiSettingsActivity)
CAR_OBJECT_IMPL(CSettingsSavedAccessPointsSettingsActivity)
CAR_OBJECT_IMPL(CSettingsTextToSpeechSettingsActivity)
CAR_OBJECT_IMPL(CSettingsAndroidBeamSettingsActivity)
CAR_OBJECT_IMPL(CSettingsWifiDisplaySettingsActivity)
CAR_OBJECT_IMPL(CSettingsDreamSettingsActivity)
CAR_OBJECT_IMPL(CSettingsNotificationStationActivity)
CAR_OBJECT_IMPL(CSettingsUserSettingsActivity)
CAR_OBJECT_IMPL(CSettingsNotificationAccessSettingsActivity)
CAR_OBJECT_IMPL(CSettingsConditionProviderSettingsActivity)
CAR_OBJECT_IMPL(CSettingsUsbSettingsActivity)
CAR_OBJECT_IMPL(CSettingsTrustedCredentialsSettingsActivity)
CAR_OBJECT_IMPL(CSettingsPaymentSettingsActivity)
CAR_OBJECT_IMPL(CSettingsPrintSettingsActivity)
CAR_OBJECT_IMPL(CSettingsPrintJobSettingsActivity)
CAR_OBJECT_IMPL(CSettingsZenModeSettingsActivity)
CAR_OBJECT_IMPL(CSettingsNotificationSettingsActivity)
CAR_OBJECT_IMPL(CSettingsNotificationAppListActivity)
CAR_OBJECT_IMPL(CSettingsAppNotificationSettingsActivity)
CAR_OBJECT_IMPL(CSettingsOtherSoundSettingsActivity)
CAR_OBJECT_IMPL(CSettingsQuickLaunchSettingsActivity)

CAR_OBJECT_IMPL(CSettingsTopLevelSettings)
CAR_OBJECT_IMPL(CSettingsApnSettingsActivity)
// CAR_OBJECT_IMPL(CSettingsBlacklistSettingsActivity)
// CAR_OBJECT_IMPL(CSettingsProfilesSettingsActivity)
CAR_OBJECT_IMPL(CSettingsSoundSettingsActivity)
CAR_OBJECT_IMPL(CSettingsLockScreenSettingsActivity)
// CAR_OBJECT_IMPL(CSettingsLiveDisplayActivity)
// CAR_OBJECT_IMPL(CSettingsDisplayRotationActivity)
CAR_OBJECT_IMPL(CSettingsAnonymousStatsActivity)

CAR_OBJECT_IMPL(CWirelessSettings);
CAR_OBJECT_IMPL(CZonePicker);

} // namespace Settings
} // namespace Droid
} // namespace Elastos
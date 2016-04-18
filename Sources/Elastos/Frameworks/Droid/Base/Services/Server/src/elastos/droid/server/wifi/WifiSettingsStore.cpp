
#include "Elastos.Droid.Content.h"
#include "elastos/droid/server/wifi/WifiSettingsStore.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//                          WifiSettingsStore
//=====================================================================
const Int32 WifiSettingsStore::WIFI_DISABLED;
const Int32 WifiSettingsStore::WIFI_ENABLED;
const Int32 WifiSettingsStore::WIFI_ENABLED_AIRPLANE_OVERRIDE;
const Int32 WifiSettingsStore::WIFI_DISABLED_AIRPLANE_ON;

WifiSettingsStore::WifiSettingsStore(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // mContext = context;
    // mAirplaneModeOn = getPersistedAirplaneModeOn();
    // mPersistWifiState = getPersistedWifiState();
    // mScanAlwaysAvailable = getPersistedScanAlwaysAvailable();
}

// synchronized
ECode WifiSettingsStore::IsWifiToggleEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (!mCheckSavedStateAtBoot) {
    //     mCheckSavedStateAtBoot = true;
    //     if (testAndClearWifiSavedState()) return true;
    // }
    //
    // if (mAirplaneModeOn) {
    //     return mPersistWifiState == WIFI_ENABLED_AIRPLANE_OVERRIDE;
    // } else {
    //     return mPersistWifiState != WIFI_DISABLED;
    // }
    assert(0);
    return NOERROR;
}

// synchronized
ECode WifiSettingsStore::IsAirplaneModeOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAirplaneModeOn;
    assert(0);
    return NOERROR;
}

// synchronized
ECode WifiSettingsStore::IsScanAlwaysAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return !mAirplaneModeOn && mScanAlwaysAvailable;
    assert(0);
    return NOERROR;
}

// synchronized
ECode WifiSettingsStore::HandleWifiToggled(
    /* [in] */ Boolean wifiEnabled,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Can Wi-Fi be toggled in airplane mode ?
    // if (mAirplaneModeOn && !isAirplaneToggleable()) {
    //     return false;
    // }
    //
    // if (wifiEnabled) {
    //     if (mAirplaneModeOn) {
    //         persistWifiState(WIFI_ENABLED_AIRPLANE_OVERRIDE);
    //     } else {
    //         persistWifiState(WIFI_ENABLED);
    //     }
    // } else {
    //     // When wifi state is disabled, we do not care
    //     // if airplane mode is on or not. The scenario of
    //     // wifi being disabled due to airplane mode being turned on
    //     // is handled handleAirplaneModeToggled()
    //     persistWifiState(WIFI_DISABLED);
    // }
    // return true;
    assert(0);
    return NOERROR;
}

// synchronized
ECode WifiSettingsStore::HandleAirplaneModeToggled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // Is Wi-Fi sensitive to airplane mode changes ?
    // if (!isAirplaneSensitive()) {
    //     return false;
    // }
    //
    // mAirplaneModeOn = getPersistedAirplaneModeOn();
    // if (mAirplaneModeOn) {
    //     // Wifi disabled due to airplane on
    //     if (mPersistWifiState == WIFI_ENABLED) {
    //         persistWifiState(WIFI_DISABLED_AIRPLANE_ON);
    //     }
    // } else {
    //     /* On airplane mode disable, restore wifi state if necessary */
    //     if (testAndClearWifiSavedState() ||
    //             mPersistWifiState == WIFI_ENABLED_AIRPLANE_OVERRIDE) {
    //         persistWifiState(WIFI_ENABLED);
    //     }
    // }
    // return true;
    assert(0);
    return NOERROR;
}

// synchronized
ECode WifiSettingsStore::HandleWifiScanAlwaysAvailableToggled()
{
    // ==================before translated======================
    // mScanAlwaysAvailable = getPersistedScanAlwaysAvailable();
    assert(0);
    return NOERROR;
}

ECode WifiSettingsStore::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    VALIDATE_NOT_NULL(fd);
    VALIDATE_NOT_NULL(pw);
    VALIDATE_NOT_NULL(args);
    // ==================before translated======================
    // pw.println("mPersistWifiState " + mPersistWifiState);
    // pw.println("mAirplaneModeOn " + mAirplaneModeOn);
    assert(0);
    return NOERROR;
}

void WifiSettingsStore::PersistWifiState(
    /* [in] */ Int32 state)
{
    // ==================before translated======================
    // final ContentResolver cr = mContext.getContentResolver();
    // mPersistWifiState = state;
    // Settings.Global.putInt(cr, Settings.Global.WIFI_ON, state);
    assert(0);
}

Boolean WifiSettingsStore::IsAirplaneSensitive()
{
    // ==================before translated======================
    // String airplaneModeRadios = Settings.Global.getString(mContext.getContentResolver(),
    //         Settings.Global.AIRPLANE_MODE_RADIOS);
    // return airplaneModeRadios == null
    //         || airplaneModeRadios.contains(Settings.Global.RADIO_WIFI);
    assert(0);
    return FALSE;
}

Boolean WifiSettingsStore::IsAirplaneToggleable()
{
    // ==================before translated======================
    // String toggleableRadios = Settings.Global.getString(mContext.getContentResolver(),
    //         Settings.Global.AIRPLANE_MODE_TOGGLEABLE_RADIOS);
    // return toggleableRadios != null
    //         && toggleableRadios.contains(Settings.Global.RADIO_WIFI);
    assert(0);
    return FALSE;
}

Boolean WifiSettingsStore::TestAndClearWifiSavedState()
{
    // ==================before translated======================
    // final ContentResolver cr = mContext.getContentResolver();
    // int wifiSavedState = 0;
    // try {
    //     wifiSavedState = Settings.Global.getInt(cr, Settings.Global.WIFI_SAVED_STATE);
    //     if(wifiSavedState == 1)
    //         Settings.Global.putInt(cr, Settings.Global.WIFI_SAVED_STATE, 0);
    // } catch (Settings.SettingNotFoundException e) {
    //     ;
    // }
    // return (wifiSavedState == 1);
    assert(0);
    return FALSE;
}

Int32 WifiSettingsStore::GetPersistedWifiState()
{
    // ==================before translated======================
    // final ContentResolver cr = mContext.getContentResolver();
    // try {
    //     return Settings.Global.getInt(cr, Settings.Global.WIFI_ON);
    // } catch (Settings.SettingNotFoundException e) {
    //     Settings.Global.putInt(cr, Settings.Global.WIFI_ON, WIFI_DISABLED);
    //     return WIFI_DISABLED;
    // }
    assert(0);
    return 0;
}

Boolean WifiSettingsStore::GetPersistedAirplaneModeOn()
{
    // ==================before translated======================
    // return Settings.Global.getInt(mContext.getContentResolver(),
    //         Settings.Global.AIRPLANE_MODE_ON, 0) == 1;
    assert(0);
    return FALSE;
}

Boolean WifiSettingsStore::GetPersistedScanAlwaysAvailable()
{
    // ==================before translated======================
    // return Settings.Global.getInt(mContext.getContentResolver(),
    //         Settings.Global.WIFI_SCAN_ALWAYS_AVAILABLE,
    //         0) == 1;
    assert(0);
    return FALSE;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos



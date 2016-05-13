#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/server/wifi/WifiSettingsStore.h"
#include "elastos/core/StringUtils.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

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
    : mAirplaneModeOn(FALSE)
    , mScanAlwaysAvailable(FALSE)
    , mCheckSavedStateAtBoot(FALSE)
{
    mContext = context;
    mAirplaneModeOn = GetPersistedAirplaneModeOn();
    mPersistWifiState = GetPersistedWifiState();
    mScanAlwaysAvailable = GetPersistedScanAlwaysAvailable();
}

// synchronized
ECode WifiSettingsStore::IsWifiToggleEnabled(
    /* [out] */ Boolean* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result);
    if (!mCheckSavedStateAtBoot) {
        mCheckSavedStateAtBoot = TRUE;
        if (TestAndClearWifiSavedState()) {
            *result = TRUE;
            return NOERROR;
        }
    }

    if (mAirplaneModeOn) {
        *result = mPersistWifiState == WIFI_ENABLED_AIRPLANE_OVERRIDE;
        return NOERROR;
    } else {
        *result = mPersistWifiState != WIFI_DISABLED;
        return NOERROR;
    }
    return NOERROR;
}

// synchronized
ECode WifiSettingsStore::IsAirplaneModeOn(
    /* [out] */ Boolean* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result);
    *result = mAirplaneModeOn;
    return NOERROR;
}

// synchronized
ECode WifiSettingsStore::IsScanAlwaysAvailable(
    /* [out] */ Boolean* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result);
    *result = !mAirplaneModeOn && mScanAlwaysAvailable;
    return NOERROR;
}

// synchronized
ECode WifiSettingsStore::HandleWifiToggled(
    /* [in] */ Boolean wifiEnabled,
    /* [out] */ Boolean* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result);
    // Can Wi-Fi be toggled in airplane mode ?
    if (mAirplaneModeOn && !IsAirplaneToggleable()) {
        *result = FALSE;
        return NOERROR;
    }

    if (wifiEnabled) {
        if (mAirplaneModeOn) {
            PersistWifiState(WIFI_ENABLED_AIRPLANE_OVERRIDE);
        } else {
            PersistWifiState(WIFI_ENABLED);
        }
    } else {
        // When wifi state is disabled, we do not care
        // if airplane mode is on or not. The scenario of
        // wifi being disabled due to airplane mode being turned on
        // is handled handleAirplaneModeToggled()
        PersistWifiState(WIFI_DISABLED);
    }
    *result = TRUE;
    return NOERROR;
}

// synchronized
ECode WifiSettingsStore::HandleAirplaneModeToggled(
    /* [out] */ Boolean* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result);
    // // Is Wi-Fi sensitive to airplane mode changes ?
    if (!IsAirplaneSensitive()) {
        *result = FALSE;
        return NOERROR;
    }

    mAirplaneModeOn = GetPersistedAirplaneModeOn();
    if (mAirplaneModeOn) {
        // Wifi disabled due to airplane on
        if (mPersistWifiState == WIFI_ENABLED) {
            PersistWifiState(WIFI_DISABLED_AIRPLANE_ON);
        }
    } else {
        /* On airplane mode disable, restore wifi state if necessary */
        if (TestAndClearWifiSavedState() ||
                mPersistWifiState == WIFI_ENABLED_AIRPLANE_OVERRIDE) {
            PersistWifiState(WIFI_ENABLED);
        }
    }
    *result = TRUE;
    return NOERROR;
}

// synchronized
ECode WifiSettingsStore::HandleWifiScanAlwaysAvailableToggled()
{
    AutoLock lock(this);
    mScanAlwaysAvailable = GetPersistedScanAlwaysAvailable();
    return NOERROR;
}

ECode WifiSettingsStore::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("mPersistWifiState ") + StringUtils::ToString(mPersistWifiState));
    pw->Println(String("mAirplaneModeOn ") + StringUtils::BooleanToString(mAirplaneModeOn));
    return NOERROR;
}

void WifiSettingsStore::PersistWifiState(
    /* [in] */ Int32 state)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    mPersistWifiState = state;
    Boolean r;
    Settings::Global::PutInt32(cr, ISettingsGlobal::WIFI_ON, state, &r);
}

Boolean WifiSettingsStore::IsAirplaneSensitive()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String airplaneModeRadios;
    Settings::Global::GetString(cr, ISettingsGlobal::AIRPLANE_MODE_RADIOS, &airplaneModeRadios);
    return airplaneModeRadios.IsNull()
            || airplaneModeRadios.Contains(ISettingsGlobal::RADIO_WIFI);
}

Boolean WifiSettingsStore::IsAirplaneToggleable()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String toggleableRadios;
    Settings::Global::GetString(cr, ISettingsGlobal::AIRPLANE_MODE_TOGGLEABLE_RADIOS, &toggleableRadios);
    return toggleableRadios.IsNull()
            && toggleableRadios.Contains(ISettingsGlobal::RADIO_WIFI);
}

Boolean WifiSettingsStore::TestAndClearWifiSavedState()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 wifiSavedState = 0;
    //try {
    Settings::Global::GetInt32(cr, ISettingsGlobal::WIFI_SAVED_STATE, &wifiSavedState);
    if(wifiSavedState == 1) {
        Boolean r;
        Settings::Global::PutInt32(cr, ISettingsGlobal::WIFI_SAVED_STATE, 0, &r);
    }
    //} catch (Settings.SettingNotFoundException e) {
        ;
    //}
    return (wifiSavedState == 1);
}

Int32 WifiSettingsStore::GetPersistedWifiState()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    //try {
    Int32 r;
    ECode ec = Settings::Global::GetInt32(cr, ISettingsGlobal::WIFI_ON, &r);
    Logger::D("WifiSettingsStore", "WifiSettingsStore::GetPersistedWifiState, r=%d", r);
    //} catch (Settings.SettingNotFoundException e) {
    if (FAILED(ec)) {
        Boolean b;
        Settings::Global::PutInt32(cr, ISettingsGlobal::WIFI_ON, WIFI_DISABLED, &b);
        return WIFI_DISABLED;
    }
    //}
    return r;
}

Boolean WifiSettingsStore::GetPersistedAirplaneModeOn()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 res;
    Settings::Global::GetInt32(cr, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &res);
    return res == 1;
}

Boolean WifiSettingsStore::GetPersistedScanAlwaysAvailable()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 res;
    Settings::Global::GetInt32(cr, ISettingsGlobal::WIFI_SCAN_ALWAYS_AVAILABLE, 0, &res);
    return res == 1;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

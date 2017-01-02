//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFISETTINGSSTORE_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFISETTINGSSTORE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Content::IContext;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

/* Tracks persisted settings for Wi-Fi and airplane mode interaction */
class WifiSettingsStore
    : public Object
{
public:
    WifiSettingsStore(
        /* [in] */ IContext* context);

    // synchronized
    virtual CARAPI IsWifiToggleEnabled(
        /* [out] */ Boolean* result);

    /**
      * Returns true if airplane mode is currently on.
      * @return {@code true} if airplane mode is on.
      */
    // synchronized
    virtual CARAPI IsAirplaneModeOn(
        /* [out] */ Boolean* result);

    // synchronized
    virtual CARAPI IsScanAlwaysAvailable(
        /* [out] */ Boolean* result);

    // synchronized
    virtual CARAPI HandleWifiToggled(
        /* [in] */ Boolean wifiEnabled,
        /* [out] */ Boolean* result);

    // synchronized
    virtual CARAPI HandleAirplaneModeToggled(
        /* [out] */ Boolean* result);

    // synchronized
    virtual CARAPI HandleWifiScanAlwaysAvailableToggled();

    virtual CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(void) PersistWifiState(
        /* [in] */ Int32 state);

    /* Does Wi-Fi need to be disabled when airplane mode is on ? */
    CARAPI_(Boolean) IsAirplaneSensitive();

    /* Is Wi-Fi allowed to be re-enabled while airplane mode is on ? */
    CARAPI_(Boolean) IsAirplaneToggleable();

    /* After a reboot, we restore wi-fi to be on if it was turned off temporarily for tethering.
       * The settings app tracks the saved state, but the framework has to check it at boot to
       * make sure the wi-fi is turned on in case it was turned off for the purpose of tethering.
       *
       * Note that this is not part of the regular WIFI_ON setting because this only needs to
       * be controlled through the settings app and not the Wi-Fi public API.
       */
    CARAPI_(Boolean) TestAndClearWifiSavedState();

    CARAPI_(Int32) GetPersistedWifiState();

    CARAPI_(Boolean) GetPersistedAirplaneModeOn();

    CARAPI_(Boolean) GetPersistedScanAlwaysAvailable();

private:
    /* Values tracked in Settings.Global.WIFI_ON */
    static const Int32 WIFI_DISABLED = 0;
    static const Int32 WIFI_ENABLED = 1;
    /* Wifi enabled while in airplane mode */
    static const Int32 WIFI_ENABLED_AIRPLANE_OVERRIDE = 2;
    /* Wifi disabled due to airplane mode on */
    static const Int32 WIFI_DISABLED_AIRPLANE_ON = 3;
    /* Persisted state that tracks the wifi & airplane interaction from settings */
    Int32 mPersistWifiState;
    /* Tracks current airplane mode state */
    Boolean mAirplaneModeOn;
    /* Tracks the setting of scan being available even when wi-fi is turned off
      */
    Boolean mScanAlwaysAvailable;
    AutoPtr<IContext> mContext;
    /* Tracks if we have checked the saved wi-fi state after boot */
    Boolean mCheckSavedStateAtBoot;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFISETTINGSSTORE_H__


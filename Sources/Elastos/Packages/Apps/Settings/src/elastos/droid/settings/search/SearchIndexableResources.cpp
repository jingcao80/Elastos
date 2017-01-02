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

#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/search/SearchIndexableResources.h"
#include "elastos/droid/settings/search/Ranking.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Provider::CSearchIndexableResource;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

Int32 SearchIndexableResources::NO_DATA_RES_ID = 0;

static Boolean InitPut(
    /* [in] */ IHashMap* map,
    /* [in] */ const String& className,
    /* [in] */ Int32 xmlResId,
    /* [in] */ Int32 iconResId)
{
    Int32 rank = Ranking::GetRankForClassName(className);

    AutoPtr<ISearchIndexableResource> sir;
    CSearchIndexableResource::New(rank, xmlResId, className, iconResId,
            (ISearchIndexableResource**)&sir);

    map->Put(CoreUtils::Convert(className), sir);
    return TRUE;
}

static AutoPtr<IHashMap> InitsResMap()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);

    InitPut(map, String("Elastos.Droid.Settings.Wifi.CWifiSettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_wireless);

    InitPut(map, String("Elastos.Droid.Settings.Wifi.CAdvancedWifiSettings"),
            R::xml::wifi_advanced_settings, R::drawable::ic_settings_wireless);

    InitPut(map, String("Elastos.Droid.Settings.Wifi.CSavedAccessPointsWifiSettings"),
            R::xml::wifi_display_saved_access_points,
            R::drawable::ic_settings_wireless);

    InitPut(map, String("Elastos.Droid.Settings.Bluetooth.CBluetoothSettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_bluetooth2);

    InitPut(map, String("Elastos.Droid.Settings.Sim.CSimSettings"),
            SearchIndexableResources::NO_DATA_RES_ID, R::drawable::ic_sim_sd);

    InitPut(map, String("Elastos.Droid.Settings.CDataUsageSummary"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_data_usage);

    InitPut(map, String("Elastos.Droid.Settings.Net.CDataUsageMeteredSettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_data_usage);

    InitPut(map, String("Elastos.Droid.Settings.CWirelessSettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_more);

    InitPut(map, String("Elastos.Droid.Settings.CHomeSettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_home);

    InitPut(map, String("Elastos.Droid.Settings.CDisplaySettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_display);

    InitPut(map, String("Elastos.Droid.Settings.CWallpaperTypeSettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_display);

    InitPut(map, String("Elastos.Droid.Settings.Notification.CNotificationSettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_notifications);

    InitPut(map, String("Elastos.Droid.Settings.Notification.COtherSoundSettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_notifications);

    InitPut(map, String("Elastos.Droid.Settings.Notification.CZenModeSettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_notifications);

    InitPut(map, String("Elastos.Droid.Settings.Deviceinfo.CMemory"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_storage);

    InitPut(map, String("Elastos.Droid.Settings.Fuelgauge.CPowerUsageSummary"),
            R::xml::power_usage_summary, R::drawable::ic_settings_battery);

    InitPut(map, String("Elastos.Droid.Settings.Fuelgauge.CBatterySaverSettings"),
            R::xml::battery_saver_settings, R::drawable::ic_settings_battery);

    InitPut(map, String("Elastos.Droid.Settings.Users.CUserSettings"),
            R::xml::user_settings, R::drawable::ic_settings_multiuser);

    InitPut(map, String("Elastos.Droid.Settings.Location.CLocationSettings"),
            R::xml::location_settings, R::drawable::ic_settings_location);

    InitPut(map, String("Elastos.Droid.Settings.CSecuritySettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_security);

    InitPut(map, String("Elastos.Droid.Settings.CScreenPinningSettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_security);

    InitPut(map, String("Elastos.Droid.Settings.Inputmethod.CInputMethodAndLanguageSettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_language);

    InitPut(map, String("Elastos.Droid.Settings.Voice.CVoiceInputSettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_language);

    InitPut(map, String("Elastos.Droid.Settings.CPrivacySettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_backup);

    InitPut(map, String("Elastos.Droid.Settings.CDateTimeSettings"),
            R::xml::date_time_prefs, R::drawable::ic_settings_date_time);

    InitPut(map, String("Elastos.Droid.Settings.Accessibility.CAccessibilitySettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_accessibility);

    InitPut(map, String("Elastos.Droid.Settings.Print.CPrintSettingsFragment"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_print);

    InitPut(map, String("Elastos.Droid.Settings.CDevelopmentSettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_development);

    InitPut(map, String("Elastos.Droid.Settings.CDeviceInfoSettings"),
            SearchIndexableResources::NO_DATA_RES_ID,
            R::drawable::ic_settings_about);

    return map;
}

AutoPtr<IHashMap> SearchIndexableResources::sResMap = InitsResMap();

Int32 SearchIndexableResources::GetSize()
{
    Int32 size;
    sResMap->GetSize(&size);
    return size;
}

AutoPtr<ISearchIndexableResource> SearchIndexableResources::GetResourceByName(
    /* [in] */ const String& className)
{
    AutoPtr<IInterface> obj;
    sResMap->Get(CoreUtils::Convert(className), (IInterface**)&obj);
    return ISearchIndexableResource::Probe(obj);
}

AutoPtr<ICollection> SearchIndexableResources::GetValues()
{
    AutoPtr<ICollection> coll;
    sResMap->GetValues((ICollection**)&coll);
    return coll;
}

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos

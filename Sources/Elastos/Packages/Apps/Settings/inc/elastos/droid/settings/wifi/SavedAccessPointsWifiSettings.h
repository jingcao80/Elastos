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

#ifndef __ELASTOS_DROID_SETTINGS_WIFI_SAVEDACCESSPOINTSWIFISETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_SAVEDACCESSPOINTSWIFISETTINGS_H__

#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"
#include "elastos/droid/settings/wifi/CWifiDialog.h"

using Elastos::Droid::Settings::Search::BaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;
using Elastos::Droid::Settings::SettingsPreferenceFragment;

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::View::IView;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

/**
 * UI to manage saved networks/access points.
 */
class SavedAccessPointsWifiSettings
    : public SettingsPreferenceFragment
    , public IDialogInterfaceOnClickListener
    , public IIndexable
{
public:
    /**
     * For search.
     */
    class MyBaseSearchIndexProvider
        : public BaseSearchIndexProvider
    {
    public:
        MyBaseSearchIndexProvider();

        ~MyBaseSearchIndexProvider();

        //@Override
        CARAPI GetRawDataToIndex(
            /* [in] */ IContext* context,
            /* [in] */ Boolean enabled,
            /* [out] */ IList** list);//List<SearchIndexableRaw>
    };

public:
    CAR_INTERFACE_DECL()

    SavedAccessPointsWifiSettings();

    ~SavedAccessPointsWifiSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnCreateDialog(
        /* [in] */ Int32 dialogId,
        /* [out] */ IDialog** dialog);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialogInterface,
        /* [in] */ Int32 button);

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* screen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    static CARAPI_(AutoPtr<IIndexableSearchIndexProvider>) GetSEARCH_INDEX_DATA_PROVIDER();

private:
    CARAPI_(void) InitPreferences();

    static CARAPI_(AutoPtr<IList>) ConstructSavedAccessPoints(
        /* [in] */ IContext* context,
        /* [in] */ IWifiManager* wifiManager); // List<AccessPoint>

    using SettingsPreferenceFragment::ShowDialog;

    CARAPI_(void) ShowDialog(
        /* [in] */ IAccessPoint* accessPoint,
        /* [in] */ Boolean edit);

private:
    static const String TAG;

    AutoPtr<CWifiDialog> mDialog;
    AutoPtr<IWifiManager> mWifiManager;
    AutoPtr<IAccessPoint> mDlgAccessPoint;
    AutoPtr<IBundle> mAccessPointSavedState;
    AutoPtr<IAccessPoint> mSelectedAccessPoint;

    // Instance state key
    static const String SAVE_DIALOG_ACCESS_POINT_STATE;

    static AutoPtr<IIndexableSearchIndexProvider> SEARCH_INDEX_DATA_PROVIDER;
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_SAVEDACCESSPOINTSWIFISETTINGS_H__
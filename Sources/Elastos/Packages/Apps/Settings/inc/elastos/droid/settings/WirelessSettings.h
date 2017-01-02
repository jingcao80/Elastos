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

#ifndef __ELASTOS_DROID_SETTINGS_WIRELESSSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_WIRELESSSETTINGS_H__

#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"
#include "elastos/droid/settings/CAppListPreference.h"
#include "elastos/droid/settings/AirplaneModeEnabler.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Settings::Search::BaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;
using Elastos::Droid::Settings::SettingsPreferenceFragment;

// using Elastos::Droid::App::IActivity;
// using Elastos::Droid::App::IAlertDialog;
// using Elastos::Droid::App::IDialog;
// using Elastos::Droid::App::Admin::IDevicePolicyManager;
// using Elastos::Droid::Content::IDialogInterface;
// using Elastos::Droid::Os::IBundle;
// using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Settings::SettingsPreferenceFragment;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;
using Elastos::Droid::Telephony::ITelephonyManager;

// using Elastos::Droid::Settings::Nfc::INfcEnabler;

namespace Elastos {
namespace Droid {
namespace Settings {

class WirelessSettings
    : public SettingsPreferenceFragment
    , public IPreferenceOnPreferenceChangeListener
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
        CARAPI GetXmlResourcesToIndex(
            /* [in] */ IContext* context,
            /* [in] */ Boolean enabled,
            /* [out] */ IList** list); /* List<SearchIndexableResource> */

        //@Override
        CARAPI GetNonIndexableKeys(
            /* [in] */ IContext* context,
            /* [out] */ IList** list); /* List<String>  */
    };

public:
    CAR_INTERFACE_DECL();

    WirelessSettings();

    ~WirelessSettings();

    CARAPI constructor();

    /**
     * Invoked on each preference click in this hierarchy, overrides
     * PreferenceFragment's implementation.  Used to make sure we track the
     * preference click events.
     */
    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    CARAPI OnManageMobilePlanClick();

    //@Override
    CARAPI OnCreateDialog(
        /* [in] */ Int32 dialogId,
        /* [out] */ IDialog** dialog);

    static CARAPI_(Boolean) IsRadioAllowed(
        /* [in] */ IContext* context,
        /* [in] */ const String& type);

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    //@Override
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* newValue,
        /* [out] */ Boolean* result);

    static CARAPI_(AutoPtr<IIndexableSearchIndexProvider>) GetSEARCH_INDEX_DATA_PROVIDER();

protected:
    //@Override
    CARAPI_(Int32) GetHelpResource();

private:
    CARAPI_(void) InitSmsApplicationSetting();

    CARAPI_(void) Log(
        /* [in] */ const String& s);

    CARAPI_(Boolean) IsSmsSupported();

public:
    static const String EXIT_ECM_RESULT;
    static const Int32 REQUEST_CODE_EXIT_ECM;

private:
    static const String TAG;

    static const String KEY_TOGGLE_AIRPLANE;
    static const String KEY_TOGGLE_NFC;
    static const String KEY_WIMAX_SETTINGS;
    static const String KEY_ANDROID_BEAM_SETTINGS;
    static const String KEY_VPN_SETTINGS;
    static const String KEY_TETHER_SETTINGS;
    static const String KEY_PROXY_SETTINGS;
    static const String KEY_MOBILE_NETWORK_SETTINGS;
    static const String KEY_MANAGE_MOBILE_PLAN;
    static const String KEY_SMS_APPLICATION;
    static const String KEY_TOGGLE_NSD; //network service discovery
    static const String KEY_CELL_BROADCAST_SETTINGS;

    AutoPtr<AirplaneModeEnabler> mAirplaneModeEnabler;
    AutoPtr<ISwitchPreference> mAirplaneModePreference;
    // private NfcEnabler mNfcEnabler;
    // private NfcAdapter mNfcAdapter;
    // private NsdEnabler mNsdEnabler;

    AutoPtr<IConnectivityManager> mCm;
    AutoPtr<ITelephonyManager> mTm;
    AutoPtr<IPackageManager> mPm;
    AutoPtr<IUserManager> mUm;

    static const Int32 MANAGE_MOBILE_PLAN_DIALOG_ID;
    static const String SAVED_MANAGE_MOBILE_PLAN_MSG;

    AutoPtr<CAppListPreference> mSmsApplicationPreference;

    String mManageMobilePlanMessage;

    static AutoPtr<IIndexableSearchIndexProvider> SEARCH_INDEX_DATA_PROVIDER;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIRELESSSETTINGS_H__
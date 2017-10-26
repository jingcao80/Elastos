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

#ifndef __ELASTOS_DROID_SETTINGS_WIFI_WIFISETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_WIFISETTINGS_H__

#include "Elastos.Droid.Nfc.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "elastos/droid/settings/RestrictedSettingsFragment.h"
#include "elastos/droid/settings/wifi/WifiEnabler.h"
#include "elastos/droid/settings/wifi/CWifiDialog.h"
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::INetworkScorerAppData;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Settings::Search::BaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;
using Elastos::Droid::View::IContextMenu;
using Elastos::Droid::View::IContextMenuInfo;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::IWifiManagerActionListener;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

/**
 * Two types of UI are provided here.
 *
 * The first is for "usual Settings", appearing as any other Setup fragment.
 *
 * The second is for Setup Wizard, with a simplified interface that hides the action bar
 * and menus.
 */
class WifiSettings
    : public RestrictedSettingsFragment
    , public IDialogInterfaceOnClickListener
    , public IIndexable
{
public:
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

private:
    /** A restricted multimap for use in constructAccessPoints */
    // class Multimap<K,V>
    // {
    //     Multimap();

    //     ~Multimap();

    //     /** retrieve a non-NULL list of values with key K */
    //     List<V> GetAll(K key) {
    //         List<V> values = store->Get(key);
    //         return values != NULL ? values : Collections.<V>EmptyList();
    //     }

    //     void Put(K key, V val) {
    //         List<V> curVals = store->Get(key);
    //         if (curVals == NULL) {
    //             curVals = new ArrayList<V>(3);
    //             store->Put(key, curVals);
    //         }
    //         curVals->Add(val);
    //     }

    // private:
    //     private final HashMap<K,List<V>> store = new HashMap<K,List<V>>();
    // };
    class Multimap
        : public Object
    {
    public:
        Multimap();

        ~Multimap();

        CARAPI_(AutoPtr<IList>) GetAll(
            /* [in] */ const String& key);

        CARAPI_(void) Put(
            /* [in] */ const String& key,
            /* [in] */ IAccessPoint* val);

    public:
        AutoPtr<IHashMap> mStore;
    };

    class Scanner
        : public Handler
    {
    public:
        TO_STRING_IMPL("WifiSettings::Scanner")

        Scanner(
            /* [in] */ WifiSettings* wifiSettings);

        ~Scanner();

        CARAPI_(void) Resume();

        CARAPI_(void) ForceScan();

        CARAPI_(void) Pause();

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* message);

    private:
        Int32 mRetry;
        WifiSettings* mWifiSettings;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("WifiSettings::MyBroadcastReceiver")

        MyBroadcastReceiver(
            /* [in] */ WifiSettings* host);

        ~MyBroadcastReceiver();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiSettings* mHost;
    };

    class WifiManagerActionListener
        : public Object
        , public IWifiManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL();

        WifiManagerActionListener(
            /* [in] */ WifiSettings* host,
            /* [in] */ Int32 id);

        ~WifiManagerActionListener();

        //@Override
        CARAPI OnSuccess();

        //@Override
        CARAPI OnFailure(
            /* [in] */ Int32 reason);

    private:
        WifiSettings* mHost;
        Int32 mId;
    };

    class ViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        ViewOnClickListener(
            /* [in] */ WifiSettings* host,
            /* [in] */ Int32 id);

        ~ViewOnClickListener();

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        WifiSettings* mHost;
        Int32 mId;
    };

public:
    CAR_INTERFACE_DECL();

    WifiSettings();

    ~WifiSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* resultData);

    //@Override
    CARAPI OnDestroyView();

    //@Override
    CARAPI OnStart();

    /**
     * @return new WifiEnabler or NULL (as overridden by WifiSettingsForSetupWizard)
     */
    /* package */
    CARAPI_(AutoPtr<WifiEnabler>) CreateWifiEnabler();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuInflater* inflater);

    /**
     * @param menu
     */
    CARAPI_(void) AddOptionsMenuItems(
        /* [in] */ IMenu* menu);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu,
        /* [in] */ IView* view,
        /* [in] */ IContextMenuInfo* info);

    //@Override
    CARAPI OnContextItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* screen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnCreateDialog(
        /* [in] */ Int32 dialogId,
        /* [out] */ IDialog** dialog);

    /**
     * Returns the Network Scorer for the Wifi Assistant App.
     */
    static CARAPI_(AutoPtr<INetworkScorerAppData>) GetWifiAssistantApp(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialogInterface,
        /* [in] */ Int32 button);

    /* package */
    CARAPI_(void) Submit(
        /* [in] */ IWifiConfigController* configController);

    /* package */
    CARAPI_(void) Forget();

    /**
     * Refreshes acccess points and ask Wifi module to scan networks again.
     */
    /* package */
    CARAPI_(void) RefreshAccessPoints();
    /**
     * Called when "add network" button is pressed.
     */
    /* package */
    CARAPI_(void) OnAddNetworkPressed();

    /* package */
    CARAPI_(Int32) GetAccessPointsCount();

    /**
     * Requests wifi module to pause wifi scan. May be ignored when the module is disabled.
     */
    /* package */
    CARAPI_(void) PauseWifiScan();

    /**
     * Requests wifi module to resume wifi scan. May be ignored when the module is disabled.
     */
    /* package */
    CARAPI_(void) ResumeWifiScan();

protected:
    CARAPI_(AutoPtr<ITextView>) InitEmptyView();

    CARAPI_(void) Connect(
        /* [in] */ IWifiConfiguration* config);

    CARAPI_(void) Connect(
        /* [in] */ Int32 networkId);

    //@Override
    CARAPI_(Int32) GetHelpResource();

private:
    using RestrictedSettingsFragment::ShowDialog;

    CARAPI_(void) ShowDialog(
        /* [in] */ IAccessPoint* accessPoint,
        /* [in] */ Boolean edit);

    /**
     * Shows the latest access points available with supplemental information like
     * the strength of network and the security for it.
     */
    CARAPI_(void) UpdateAccessPoints();

    CARAPI_(void) PrepareWifiAssistantCard();

    CARAPI_(void) DisableWifiAssistantCardUntilPlatformUpgrade();

    CARAPI_(void) SetOffMessage();

    CARAPI_(void) AddMessagePreference(
        /* [in] */ Int32 messageId);

    /** Returns sorted list of access points */
    static CARAPI_(AutoPtr<IList>) ConstructAccessPoints(
        /* [in] */ IContext* context,
        /* [in] */ IWifiManager* wifiManager,
        /* [in] */ IWifiInfo* lastInfo,
        /* [in] */ NetworkInfoDetailedState lastState);

    CARAPI_(void) HandleEvent(
        /* [in] */ IIntent* intent);

    CARAPI_(void) UpdateConnectionState(
        /* [in] */ NetworkInfoDetailedState state);

    CARAPI_(void) UpdateWifiState(
        /* [in] */ Int32 state);

    /**
     * Renames/replaces "Next" button when appropriate. "Next" button usually exists in
     * Wifi setup screens, not in usual wifi settings screen.
     *
     * @param enabled TRUE when the device is connected to a wifi network.
     */
    CARAPI_(void) ChangeNextButtonState(
        /* [in] */ Boolean enabled);

public:
    static const Int32 WIFI_DIALOG_ID = 1;
    /* package */ static const Int32 MENU_ID_WPS_PBC = IMenu::FIRST;
    /* package */ static const Int32 MENU_ID_ADD_NETWORK = IMenu::FIRST + 3;
    /* package */ static const Int32 WPS_PBC_DIALOG_ID = 2;

    /* package */ AutoPtr<IWifiManager> mWifiManager;

    /** verbose logging flag. this flag is set thru developer debugging options
     * and used so as to assist with in-the-field WiFi connectivity debugging  */
    static Int32 mVerboseLogging;

    /* End of "used in Wifi Setup context" */

    static CARAPI_(AutoPtr<IIndexableSearchIndexProvider>) GetSEARCH_INDEX_DATA_PROVIDER();

private:
    static AutoPtr<IIndexableSearchIndexProvider> SEARCH_INDEX_DATA_PROVIDER;

    static const String TAG;

    static const Int32 REQUEST_ENABLE_WIFI_ASSISTANT;

    static const Int32 MENU_ID_WPS_PIN = IMenu::FIRST + 1;
    static const Int32 MENU_ID_SAVED_NETWORK = IMenu::FIRST + 2;
    static const Int32 MENU_ID_ADVANCED = IMenu::FIRST + 4;
    static const Int32 MENU_ID_SCAN = IMenu::FIRST + 5;
    static const Int32 MENU_ID_CONNECT = IMenu::FIRST + 6;
    static const Int32 MENU_ID_FORGET = IMenu::FIRST + 7;
    static const Int32 MENU_ID_MODIFY = IMenu::FIRST + 8;
    static const Int32 MENU_ID_WRITE_NFC = IMenu::FIRST + 9;

    static const String KEY_ASSISTANT_DISMISS_PLATFORM;

    static const Int32 WPS_PIN_DIALOG_ID = 3;
    static const Int32 WRITE_NFC_DIALOG_ID = 6;

    // Combo scans can take 5-6s to complete - set to 10s.
    static const Int32 WIFI_RESCAN_INTERVAL_MS;

    // Instance state keys
    static const String SAVE_DIALOG_EDIT_MODE;
    static const String SAVE_DIALOG_ACCESS_POINT_STATE;

    static Boolean mSavedNetworksExist;

    // this Boolean extra specifies whether to disable the Next button when not connected. Used by
    // account creation outside of setup wizard.
    static const String EXTRA_ENABLE_NEXT_ON_CONNECT;

    AutoPtr<IIntentFilter> mFilter;
    AutoPtr<BroadcastReceiver> mReceiver;
    AutoPtr<Scanner> mScanner;

    AutoPtr<IWifiManagerActionListener> mConnectListener;
    AutoPtr<IWifiManagerActionListener> mSaveListener;
    AutoPtr<IWifiManagerActionListener> mForgetListener;

    AutoPtr<WifiEnabler> mWifiEnabler;
    // An access point being editted is stored here.
    AutoPtr<IAccessPoint> mSelectedAccessPoint;

    NetworkInfoDetailedState mLastState;
    AutoPtr<IWifiInfo> mLastInfo;

    AutoPtr<IAtomicBoolean> mConnected;

    AutoPtr<CWifiDialog> mDialog;
    AutoPtr<IDialog> /*CWriteWifiConfigToNfcDialog*/ mWifiToNfcDialog;

    AutoPtr<ITextView> mEmptyView;

    // should Next button only be enabled when we have a connection?
    Boolean mEnableNextOnConnection;

    // Save the dialog details
    Boolean mDlgEdit;
    AutoPtr<IAccessPoint> mDlgAccessPoint;
    AutoPtr<IBundle> mAccessPointSavedState;
    AutoPtr<IView> mWifiAssistantCard;
    AutoPtr<INetworkScorerAppData> mWifiAssistantApp;
};

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_WIFISETTINGS_H__

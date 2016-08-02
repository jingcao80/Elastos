#ifndef __ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PSETTINGS_H__

#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_Droid_Settings_Wifi_P2p_CWifiP2pSettings.h"
#include "elastos/droid/settings/wifi/p2p/CWifiP2pPersistentGroup.h"
#include "elastos/droid/settings/wifi/p2p/CWifiP2pPeer.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::CPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Wifi::P2p::IWifiP2pDevice;
using Elastos::Droid::Wifi::P2p::IWifiP2pDeviceList;
using Elastos::Droid::Wifi::P2p::IWifiP2pGroupList;
using Elastos::Droid::Wifi::P2p::IWifiP2pManager;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerChannel;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerPeerListListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerActionListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pManagerPersistentGroupInfoListener;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {
namespace P2p {

/*
 * Displays Wi-fi p2p settings UI
 */
CarClass(CWifiP2pSettings)
    , public SettingsPreferenceFragment
    , public IWifiP2pManagerPersistentGroupInfoListener
    , public IWifiP2pManagerPeerListListener
{
private:
    class InitBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CWifiP2pSettings::InitBroadcastReceiver")

        InitBroadcastReceiver(
            /* [in] */ CWifiP2pSettings* host);

        ~InitBroadcastReceiver();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CWifiP2pSettings* mHost;
    };

    class WifiP2pManagerActionListener
        : public Object
        , public IWifiP2pManagerActionListener
    {
        friend class DialogInterfaceOnClickListener;
    public:
        CAR_INTERFACE_DECL()

        WifiP2pManagerActionListener(
            /* [in] */ CWifiP2pSettings* host,
            /* [in] */ Int32 arg);

        ~WifiP2pManagerActionListener();

        CARAPI OnSuccess();

        CARAPI OnFailure(
            /* [in] */ Int32 reason);

    private:
        CWifiP2pSettings* mHost;
        Int32 mArg;
    };

    class DialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        DialogInterfaceOnClickListener(
            /* [in] */ CWifiP2pSettings* host,
            /* [in] */ Int32 arg);

        ~DialogInterfaceOnClickListener();

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CWifiP2pSettings* mHost;
        Int32 mArg;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CWifiP2pSettings();

    ~CWifiP2pSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuInflater* inflater);

    //@Override
    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* screen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnCreateDialog(
        /* [in] */ Int32 id,
        /* [out] */ IDialog** resultDialog);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    //@Override
    CARAPI OnPersistentGroupInfoAvailable(
        /* [in] */ IWifiP2pGroupList* groups);

    //@Override
    CARAPI OnPeersAvailable(
        /* [in] */ IWifiP2pDeviceList* peers);

private:
    CARAPI_(void) HandlePeersChanged();

    CARAPI_(void) HandleP2pStateChanged();

    CARAPI_(void) UpdateSearchMenu(
        /* [in] */ Boolean searching);

    CARAPI_(void) StartSearch();

    CARAPI_(void) UpdateDevicePref();

private:
    static const String TAG;
    static const Boolean DBG;
    static const Int32 MENU_ID_SEARCH = IMenu::FIRST;
    static const Int32 MENU_ID_RENAME = IMenu::FIRST + 1;

    AutoPtr<IIntentFilter> mIntentFilter;
    AutoPtr<IWifiP2pManager> mWifiP2pManager;
    AutoPtr<IWifiP2pManagerChannel> mChannel;
    AutoPtr<IDialogInterfaceOnClickListener> mRenameListener;
    AutoPtr<IDialogInterfaceOnClickListener> mDisconnectListener;
    AutoPtr<IDialogInterfaceOnClickListener> mCancelConnectListener;
    AutoPtr<IDialogInterfaceOnClickListener> mDeleteGroupListener;
    AutoPtr<CWifiP2pPeer> mSelectedWifiPeer;
    AutoPtr<CWifiP2pPersistentGroup> mSelectedGroup;
    String mSelectedGroupName;
    AutoPtr<IEditText> mDeviceNameText;

    Boolean mWifiP2pEnabled;
    Boolean mWifiP2pSearching;
    Int32 mConnectedDevices;
    Boolean mLastGroupFormed;

    AutoPtr<IPreferenceGroup> mPeersGroup;
    AutoPtr<IPreferenceGroup> mPersistentGroup;
    AutoPtr<IPreference> mThisDevicePref;

    static const Int32 DIALOG_DISCONNECT;
    static const Int32 DIALOG_CANCEL_CONNECT;
    static const Int32 DIALOG_RENAME;
    static const Int32 DIALOG_DELETE_GROUP;

    static const String SAVE_DIALOG_PEER;
    static const String SAVE_DEVICE_NAME;
    static const String SAVE_SELECTED_GROUP;

    AutoPtr<IWifiP2pDevice> mThisDevice;
    AutoPtr<IWifiP2pDeviceList> mPeers;

    String mSavedDeviceName;
    AutoPtr<BroadcastReceiver> mReceiver;
};

} // namespace P2p
} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PSETTINGS_H__

#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/wifi/p2p/CWifiP2pSettings.h"
#include "../../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::CPreferenceCategory;
using Elastos::Droid::Text::CLengthFilter;
using Elastos::Droid::Text::IInputFilter;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Widget::CEditText;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Wifi::IWpsInfo;
using Elastos::Droid::Wifi::P2p::CWifiP2pConfig;
using Elastos::Droid::Wifi::P2p::IWifiP2pConfig;
using Elastos::Droid::Wifi::P2p::CWifiP2pDeviceList;
using Elastos::Droid::Wifi::P2p::IWifiP2pInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pGroup;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerPersistentGroupInfoListener;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerPeerListListener;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pManagerActionListener;
using Elastos::Core::CoreUtils;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {
namespace P2p {

const String CWifiP2pSettings::TAG("CWifiP2pSettings");
const Boolean CWifiP2pSettings::DBG = FALSE;
const Int32 CWifiP2pSettings::MENU_ID_SEARCH;
const Int32 CWifiP2pSettings::MENU_ID_RENAME;

const Int32 CWifiP2pSettings::DIALOG_DISCONNECT  = 1;
const Int32 CWifiP2pSettings::DIALOG_CANCEL_CONNECT = 2;
const Int32 CWifiP2pSettings::DIALOG_RENAME = 3;
const Int32 CWifiP2pSettings::DIALOG_DELETE_GROUP = 4;

const String CWifiP2pSettings::SAVE_DIALOG_PEER("PEER_STATE");
const String CWifiP2pSettings::SAVE_DEVICE_NAME("DEV_NAME");
const String CWifiP2pSettings::SAVE_SELECTED_GROUP("GROUP_NAME");

//===============================================================================
//                  CWifiP2pSettings::InitBroadcastReceiver
//===============================================================================

CWifiP2pSettings::InitBroadcastReceiver::InitBroadcastReceiver(
    /* [in] */ CWifiP2pSettings* host)
    : mHost(host)
{}

CWifiP2pSettings::InitBroadcastReceiver::~InitBroadcastReceiver()
{}

ECode CWifiP2pSettings::InitBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (IWifiP2pManager::WIFI_P2P_STATE_CHANGED_ACTION.Equals(action)) {
        Int32 value;
        intent->GetInt32Extra(IWifiP2pManager::EXTRA_WIFI_STATE,
                IWifiP2pManager::WIFI_P2P_STATE_DISABLED, &value);
        mHost->mWifiP2pEnabled = value == IWifiP2pManager::WIFI_P2P_STATE_ENABLED;
        mHost->HandleP2pStateChanged();
    }
    else if (IWifiP2pManager::WIFI_P2P_PEERS_CHANGED_ACTION.Equals(action)) {
        AutoPtr<IParcelable> value;
        intent->GetParcelableExtra(
                IWifiP2pManager::EXTRA_P2P_DEVICE_LIST, (IParcelable**)&value);
        mHost->mPeers = IWifiP2pDeviceList::Probe(value);
        mHost->HandlePeersChanged();
    }
    else if (IWifiP2pManager::WIFI_P2P_CONNECTION_CHANGED_ACTION.Equals(action)) {
        if (mHost->mWifiP2pManager == NULL) return NOERROR;
        AutoPtr<IParcelable> value;
        intent->GetParcelableExtra(
                IWifiP2pManager::EXTRA_NETWORK_INFO, (IParcelable**)&value);
        INetworkInfo* networkInfo = INetworkInfo::Probe(value);
        value = NULL;
        intent->GetParcelableExtra(
                IWifiP2pManager::EXTRA_WIFI_P2P_INFO, (IParcelable**)&value);
        IWifiP2pInfo* wifip2pinfo = IWifiP2pInfo::Probe(value);
        Boolean res;
        networkInfo->IsConnected(&res);
        if (res) {
            if (DBG) Logger::D(TAG, "Connected");
        }
        else if (mHost->mLastGroupFormed != TRUE) {
            //start a search when we are disconnected
            //but not on group removed broadcast event
            mHost->StartSearch();
        }
        wifip2pinfo->GetGroupFormed(&(mHost->mLastGroupFormed));
    }
    else if (IWifiP2pManager::WIFI_P2P_THIS_DEVICE_CHANGED_ACTION.Equals(action)) {
        AutoPtr<IParcelable> value;
        intent->GetParcelableExtra(
                IWifiP2pManager::EXTRA_WIFI_P2P_DEVICE, (IParcelable**)&value);
        mHost->mThisDevice = IWifiP2pDevice::Probe(value);
        if (DBG) Logger::D(TAG, "Update device info: %s", TO_CSTR(mHost->mThisDevice));
        mHost->UpdateDevicePref();
    }
    else if (IWifiP2pManager::WIFI_P2P_DISCOVERY_CHANGED_ACTION.Equals(action)) {
        Int32 discoveryState;
        intent->GetInt32Extra(IWifiP2pManager::EXTRA_DISCOVERY_STATE,
                IWifiP2pManager::WIFI_P2P_DISCOVERY_STOPPED, &discoveryState);
        if (DBG) Logger::D(TAG, "Discovery state changed: %d", discoveryState);
        if (discoveryState == IWifiP2pManager::WIFI_P2P_DISCOVERY_STARTED) {
            mHost->UpdateSearchMenu(TRUE);
        }
        else {
            mHost->UpdateSearchMenu(FALSE);
        }
    }
    else if (IWifiP2pManager::WIFI_P2P_PERSISTENT_GROUPS_CHANGED_ACTION.Equals(action)) {
        if (mHost->mWifiP2pManager != NULL) {
            mHost->mWifiP2pManager->RequestPersistentGroupInfo(mHost->mChannel, mHost);
        }
    }
    return NOERROR;
}

//===============================================================================
//                  CWifiP2pSettings::WifiP2pManagerActionListener
//===============================================================================

CAR_INTERFACE_IMPL(CWifiP2pSettings::WifiP2pManagerActionListener, Object, IWifiP2pManagerActionListener)

CWifiP2pSettings::WifiP2pManagerActionListener::WifiP2pManagerActionListener(
    /* [in] */ CWifiP2pSettings* host,
    /* [in] */ Int32 arg)
    : mHost(host)
    , mArg(arg)
{}

CWifiP2pSettings::WifiP2pManagerActionListener::~WifiP2pManagerActionListener()
{}

ECode CWifiP2pSettings::WifiP2pManagerActionListener::OnSuccess()
{
    if (DBG) {
        switch (mArg) {
            case 1:
                Logger::D(TAG, " device rename success");
                break;
            case 2:
                Logger::D(TAG, " remove group success");
                break;
            case 3:
                Logger::D(TAG, " cancel connect success");
                break;
            case 4:
                Logger::D(TAG, " delete group success");
                break;
            case 5:
                Logger::D(TAG, " connect success");
                break;
            case 6:
                break;
        }
    }
    return NOERROR;
}

ECode CWifiP2pSettings::WifiP2pManagerActionListener::OnFailure(
    /* [in] */ Int32 reason)
{
    switch (mArg) {
        case 1: {
            AutoPtr<IActivity> activity;
            mHost->GetActivity((IActivity**)&activity);
            AutoPtr<IToastHelper> helper;
            CToastHelper::AcquireSingleton((IToastHelper**)&helper);
            AutoPtr<IToast> toast;
            helper->MakeText(IContext::Probe(activity),
                    R::string::wifi_p2p_failed_rename_message,
                    IToast::LENGTH_LONG, (IToast**)&toast);
            toast->Show();
            break;
        }
        case 2:
            if (DBG) {
                Logger::D(TAG, " remove group fail %d", reason);
            }
            break;
        case 3:
            if (DBG) {
                Logger::D(TAG, " cancel connect fail %d", reason);
            }
            break;
        case 4:
            if (DBG) {
                Logger::D(TAG, " delete group fail %d", reason);
            }
            break;
        case 5: {
            Logger::E(TAG, " connect fail %d", reason);
            AutoPtr<IActivity> activity;
            mHost->GetActivity((IActivity**)&activity);
            AutoPtr<IToastHelper> helper;
            CToastHelper::AcquireSingleton((IToastHelper**)&helper);
            AutoPtr<IToast> toast;
            helper->MakeText(IContext::Probe(activity),
                    R::string::wifi_p2p_failed_connect_message,
                    IToast::LENGTH_SHORT, (IToast**)&toast);
            toast->Show();
            break;
        }
        case 6:
            if (DBG) {
                Logger::D(TAG, " discover fail %d", reason);
            }
            break;
    }
    return NOERROR;
}
//===============================================================================
//                  CWifiP2pSettings::DialogInterfaceOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CWifiP2pSettings::DialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener)

CWifiP2pSettings::DialogInterfaceOnClickListener::DialogInterfaceOnClickListener(
    /* [in] */ CWifiP2pSettings* host,
    /* [in] */ Int32 arg)
    : mHost(host)
    , mArg(arg)
{}

CWifiP2pSettings::DialogInterfaceOnClickListener::~DialogInterfaceOnClickListener()
{}

ECode CWifiP2pSettings::DialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    switch (mArg) {
        case 1: {
            if (which == IDialogInterface::BUTTON_POSITIVE) {
                if (mHost->mWifiP2pManager != NULL) {
                    AutoPtr<WifiP2pManagerActionListener> listener = new WifiP2pManagerActionListener(mHost, 1);
                    AutoPtr<ICharSequence> seq;
                    ITextView::Probe(mHost->mDeviceNameText)->GetText((ICharSequence**)&seq);
                    mHost->mWifiP2pManager->SetDeviceName(mHost->mChannel, CoreUtils::Unbox(seq), listener);
                }
            }
            break;
        }
        case 2: {
            if (which == IDialogInterface::BUTTON_POSITIVE) {
                if (mHost->mWifiP2pManager != NULL) {
                    AutoPtr<WifiP2pManagerActionListener> listener = new WifiP2pManagerActionListener(mHost, 2);
                    mHost->mWifiP2pManager->RemoveGroup(mHost->mChannel, listener);
                }
            }
            break;
        }
        case 3: {
            if (which == IDialogInterface::BUTTON_POSITIVE) {
                if (mHost->mWifiP2pManager != NULL) {
                    AutoPtr<WifiP2pManagerActionListener> listener = new WifiP2pManagerActionListener(mHost, 3);
                    mHost->mWifiP2pManager->CancelConnect(mHost->mChannel, listener);
                }
            }
            break;
        }
        case 4: {
            if (which == IDialogInterface::BUTTON_POSITIVE) {
                if (mHost->mWifiP2pManager != NULL) {
                    if (mHost->mSelectedGroup != NULL) {
                        if (DBG) Logger::D(TAG, " deleting group %s", (mHost->mSelectedGroup->GetGroupName()).string());
                        AutoPtr<WifiP2pManagerActionListener> listener = new WifiP2pManagerActionListener(mHost, 4);
                        mHost->mWifiP2pManager->DeletePersistentGroup(mHost->mChannel,
                                mHost->mSelectedGroup->GetNetworkId(), listener);
                        mHost->mSelectedGroup = NULL;
                    }
                    else {
                        if (DBG) Logger::W(TAG, " No selected group to delete!" );
                    }
                }
            }
            else if (which == IDialogInterface::BUTTON_NEGATIVE) {
                if (DBG) {
                    Logger::D(TAG, " forgetting selected group %s", (mHost->mSelectedGroup->GetGroupName()).string());
                }
                mHost->mSelectedGroup = NULL;
            }
            break;
        }
    }

    return NOERROR;
}

//===============================================================================
//                  CWifiP2pSettings
//===============================================================================

CAR_OBJECT_IMPL(CWifiP2pSettings)

CAR_INTERFACE_IMPL_2(CWifiP2pSettings, SettingsPreferenceFragment, IWifiP2pManagerPersistentGroupInfoListener, IWifiP2pManagerPeerListListener)

CWifiP2pSettings::CWifiP2pSettings()
    : mWifiP2pEnabled(FALSE)
    , mWifiP2pSearching(FALSE)
    , mConnectedDevices(0)
    , mLastGroupFormed(FALSE)
{
    CIntentFilter::New((IIntentFilter**)&mIntentFilter);
    CWifiP2pDeviceList::New((IWifiP2pDeviceList**)&mPeers);
    mReceiver = new InitBroadcastReceiver(this);
}

CWifiP2pSettings::~CWifiP2pSettings()
{}

ECode CWifiP2pSettings::constructor()
{
    if (DBG) Logger::D(TAG, "Creating CWifiP2pSettings ...");
    return NOERROR;
}

ECode CWifiP2pSettings::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    AddPreferencesFromResource(R::xml::wifi_p2p_settings);

    mIntentFilter->AddAction(IWifiP2pManager::WIFI_P2P_STATE_CHANGED_ACTION);
    mIntentFilter->AddAction(IWifiP2pManager::WIFI_P2P_PEERS_CHANGED_ACTION);
    mIntentFilter->AddAction(IWifiP2pManager::WIFI_P2P_CONNECTION_CHANGED_ACTION);
    mIntentFilter->AddAction(IWifiP2pManager::WIFI_P2P_THIS_DEVICE_CHANGED_ACTION);
    mIntentFilter->AddAction(IWifiP2pManager::WIFI_P2P_DISCOVERY_CHANGED_ACTION);
    mIntentFilter->AddAction(IWifiP2pManager::WIFI_P2P_PERSISTENT_GROUPS_CHANGED_ACTION);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj = GetSystemService(IContext::WIFI_P2P_SERVICE);
    mWifiP2pManager = IWifiP2pManager::Probe(obj);
    if (mWifiP2pManager != NULL) {
        AutoPtr<ILooper> looper;
        IContext::Probe(activity)->GetMainLooper((ILooper**)&looper);
        mWifiP2pManager->Initialize(IContext::Probe(activity), looper, NULL, (IWifiP2pManagerChannel**)&mChannel);
        if (mChannel == NULL) {
            //Failure to set up connection
            Logger::E(TAG, "Failed to set up connection with wifi p2p service");
            mWifiP2pManager = NULL;
        }
    }
    else {
        Logger::E(TAG, "mWifiP2pManager is NULL !");
    }

    Boolean res;
    if (savedInstanceState != NULL && (savedInstanceState->ContainsKey(SAVE_DIALOG_PEER, &res), res)) {
        AutoPtr<IParcelable> value;
        savedInstanceState->GetParcelable(SAVE_DIALOG_PEER, (IParcelable**)&value);
        IWifiP2pDevice* device = IWifiP2pDevice::Probe(value);
        CWifiP2pPeer::NewByFriend(IContext::Probe(activity), device, (CWifiP2pPeer**)&mSelectedWifiPeer);
    }
    if (savedInstanceState != NULL && (savedInstanceState->ContainsKey(SAVE_DEVICE_NAME, &res), res)) {
        savedInstanceState->GetString(SAVE_DEVICE_NAME, &mSavedDeviceName);
    }
    if (savedInstanceState != NULL && (savedInstanceState->ContainsKey(SAVE_SELECTED_GROUP, &res), res)) {
        savedInstanceState->GetString(SAVE_SELECTED_GROUP, &mSelectedGroupName);
    }

    mRenameListener = new DialogInterfaceOnClickListener(this, 1);

    //disconnect dialog listener
    mDisconnectListener = new DialogInterfaceOnClickListener(this, 2);

    //cancel connect dialog listener
    mCancelConnectListener = new DialogInterfaceOnClickListener(this, 3);

    //delete persistent group dialog listener
    mDeleteGroupListener = new DialogInterfaceOnClickListener(this, 4);

    SetHasOptionsMenu(TRUE);

    AutoPtr<IPreferenceScreen> preferenceScreen;
    GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
    IPreferenceGroup* _preferenceScreen = IPreferenceGroup::Probe(preferenceScreen);
    _preferenceScreen->RemoveAll();
    _preferenceScreen->SetOrderingAsAdded(TRUE);

    CPreference::New(IContext::Probe(activity), (IPreference**)&mThisDevicePref);
    mThisDevicePref->SetPersistent(FALSE);
    mThisDevicePref->SetSelectable(FALSE);
    _preferenceScreen->AddPreference(mThisDevicePref, &res);

    CPreferenceCategory::New(IContext::Probe(activity), (IPreferenceGroup**)&mPeersGroup);
    IPreference* _peersGroup = IPreference::Probe(mPeersGroup);
    _peersGroup->SetTitle(R::string::wifi_p2p_peer_devices);
    _preferenceScreen->AddPreference(_peersGroup, &res);

    CPreferenceCategory::New(IContext::Probe(activity), (IPreferenceGroup**)&mPersistentGroup);
    IPreference* _persistentGroup = IPreference::Probe(mPersistentGroup);
    _persistentGroup->SetTitle(R::string::wifi_p2p_remembered_groups);
    _preferenceScreen->AddPreference(_persistentGroup, &res);

    return SettingsPreferenceFragment::OnActivityCreated(savedInstanceState);
}

ECode CWifiP2pSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    IContext::Probe(activity)->RegisterReceiver(mReceiver, mIntentFilter, (IIntent**)&intent);
    if (mWifiP2pManager != NULL) {
        mWifiP2pManager->RequestPeers(mChannel, this);
    }
    return NOERROR;
}

ECode CWifiP2pSettings::OnPause()
{
    SettingsPreferenceFragment::OnPause();
    if (mWifiP2pManager != NULL) {
        mWifiP2pManager->StopPeerDiscovery(mChannel, NULL);
    }
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext::Probe(activity)->UnregisterReceiver(mReceiver);
    return NOERROR;
}

ECode CWifiP2pSettings::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuInflater* inflater)
{
    Int32 textId = mWifiP2pSearching ? R::string::wifi_p2p_menu_searching :
            R::string::wifi_p2p_menu_search;
    AutoPtr<IMenuItem> item;
    menu->Add(IMenu::NONE, MENU_ID_SEARCH, 0, textId, (IMenuItem**)&item);
    item->SetEnabled(mWifiP2pEnabled);
    item->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_IF_ROOM);
    item = NULL;
    menu->Add(IMenu::NONE, MENU_ID_RENAME, 0, R::string::wifi_p2p_menu_rename, (IMenuItem**)&item);
    item->SetEnabled(mWifiP2pEnabled);
    item->SetShowAsAction(IMenuItem::SHOW_AS_ACTION_IF_ROOM);
    return SettingsPreferenceFragment::OnCreateOptionsMenu(menu, inflater);
}

ECode CWifiP2pSettings::OnPrepareOptionsMenu(
    /* [in] */ IMenu* menu)
{
    AutoPtr<IMenuItem> searchMenu;
    menu->FindItem(MENU_ID_SEARCH, (IMenuItem**)&searchMenu);
    AutoPtr<IMenuItem> renameMenu;
    menu->FindItem(MENU_ID_RENAME, (IMenuItem**)&renameMenu);
    if (mWifiP2pEnabled) {
        searchMenu->SetEnabled(TRUE);
        renameMenu->SetEnabled(TRUE);
    }
    else {
        searchMenu->SetEnabled(FALSE);
        renameMenu->SetEnabled(FALSE);
    }

    if (mWifiP2pSearching) {
        searchMenu->SetTitle(R::string::wifi_p2p_menu_searching);
    }
    else {
        searchMenu->SetTitle(R::string::wifi_p2p_menu_search);
    }
    return NOERROR;
}

ECode CWifiP2pSettings::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 id;
    item->GetItemId(&id);
    switch (id) {
        case MENU_ID_SEARCH:
            StartSearch();
            *result = TRUE;
            return NOERROR;
        case MENU_ID_RENAME:
            ShowDialog(DIALOG_RENAME);
            *result = TRUE;
            return NOERROR;
    }
    return SettingsPreferenceFragment::OnOptionsItemSelected(item, result);
}

ECode CWifiP2pSettings::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* screen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (IWifiP2pPeer::Probe(preference) != NULL) {
        mSelectedWifiPeer = (CWifiP2pPeer*)IWifiP2pPeer::Probe(preference);

        Int32 status;
        mSelectedWifiPeer->mDevice->GetStatus(&status);
        if (status == IWifiP2pDevice::CONNECTED) {
            ShowDialog(DIALOG_DISCONNECT);
        }
        else if (status == IWifiP2pDevice::INVITED) {
            ShowDialog(DIALOG_CANCEL_CONNECT);
        }
        else {
            AutoPtr<IWifiP2pConfig> config;
            CWifiP2pConfig::New((IWifiP2pConfig**)&config);
            String deviceAddress;
            mSelectedWifiPeer->mDevice->GetDeviceAddress(&deviceAddress);
            config->SetDeviceAddress(deviceAddress);

            AutoPtr<ISystemProperties> sp;
            CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
            Int32 forceWps;
            sp->GetInt32(String("wifidirect.wps"), -1, &forceWps);

            AutoPtr<IWpsInfo> wps;
            config->GetWps((IWpsInfo**)&wps);
            if (forceWps != -1) {
                wps->SetSetup(forceWps);
            }
            else {
                Boolean res;
                if (mSelectedWifiPeer->mDevice->WpsPbcSupported(&res), res) {
                    wps->SetSetup(IWpsInfo::PBC);
                }
                else if (mSelectedWifiPeer->mDevice->WpsKeypadSupported(&res), res) {
                    wps->SetSetup(IWpsInfo::KEYPAD);
                }
                else {
                    wps->SetSetup(IWpsInfo::DISPLAY);
                }
            }

            AutoPtr<WifiP2pManagerActionListener> listener = new WifiP2pManagerActionListener(this, 5);
            mWifiP2pManager->Connect(mChannel, config, listener);
        }
    }
    else if (IWifiP2pPersistentGroup::Probe(preference) != NULL) {
        mSelectedGroup = (CWifiP2pPersistentGroup*)IWifiP2pPersistentGroup::Probe(preference);
        ShowDialog(DIALOG_DELETE_GROUP);
    }
    return SettingsPreferenceFragment::OnPreferenceTreeClick(screen, preference, result);
}

ECode CWifiP2pSettings::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [out] */ IDialog** resultDialog)
{
    VALIDATE_NOT_NULL(resultDialog)
    *resultDialog = NULL;

    String deviceName0;
    mSelectedWifiPeer->mDevice->GetDeviceName(&deviceName0);
    String deviceAddress;
    mSelectedWifiPeer->mDevice->GetDeviceAddress(&deviceAddress);

    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);

    IContext* activity = IContext::Probe(_activity);

    if (id == DIALOG_DISCONNECT) {
        String deviceName = TextUtils::IsEmpty(deviceName0) ? deviceAddress : deviceName0;
        String msg;
        if (mConnectedDevices > 1) {
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
            args->Set(0, CoreUtils::Convert(deviceName));
            args->Set(1, CoreUtils::Convert(mConnectedDevices - 1));
            activity->GetString(R::string::wifi_p2p_disconnect_multiple_message, args, &msg);
        }
        else {
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, CoreUtils::Convert(deviceName));
            activity->GetString(R::string::wifi_p2p_disconnect_message, args, &msg);
        }
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(activity, (IAlertDialogBuilder**)&builder);
        builder->SetTitle(R::string::wifi_p2p_disconnect_title);
        builder->SetMessage(CoreUtils::Convert(msg));
        String str;
        activity->GetString(R::string::dlg_ok, &str);
        builder->SetPositiveButton(CoreUtils::Convert(str), mDisconnectListener);
        activity->GetString(R::string::dlg_cancel, &str);
        builder->SetNegativeButton(CoreUtils::Convert(str), NULL);
        AutoPtr<IAlertDialog> dialog;
        builder->Create((IAlertDialog**)&dialog);
        *resultDialog = IDialog::Probe(dialog);
        REFCOUNT_ADD(*resultDialog)
        return NOERROR;
    }
    else if (id == DIALOG_CANCEL_CONNECT) {
        Int32 stringId = R::string::wifi_p2p_cancel_connect_message;
        String deviceName = TextUtils::IsEmpty(deviceName0) ? deviceAddress : deviceName0;

        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(activity, (IAlertDialogBuilder**)&builder);
        builder->SetTitle(R::string::wifi_p2p_cancel_connect_title);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(deviceName));
        String str;
        activity->GetString(stringId, args, &str);
        builder->SetMessage(CoreUtils::Convert(str));
        activity->GetString(R::string::dlg_ok, &str);
        builder->SetPositiveButton(CoreUtils::Convert(str), mCancelConnectListener);
        activity->GetString(R::string::dlg_cancel, &str);
        builder->SetNegativeButton(CoreUtils::Convert(str), NULL);
        AutoPtr<IAlertDialog> dialog;
        builder->Create((IAlertDialog**)&dialog);
        *resultDialog = IDialog::Probe(dialog);
        REFCOUNT_ADD(*resultDialog)
        return NOERROR;
    }
    else if (id == DIALOG_RENAME) {
        mDeviceNameText = NULL;
        CEditText::New(activity, (IEditText**)&mDeviceNameText);

        AutoPtr<IInputFilter> filter;
        CLengthFilter::New(30, (IInputFilter**)&filter);
        AutoPtr< ArrayOf<IInputFilter*> > filters = ArrayOf<IInputFilter*>::Alloc(1);
        filters->Set(0, filter);
        ITextView* deviceNameText = ITextView::Probe(mDeviceNameText);
        deviceNameText->SetFilters(filters);
        String thisDeviceName;
        if (!mSavedDeviceName.IsNull()) {
            deviceNameText->SetText(CoreUtils::Convert(mSavedDeviceName));
            mDeviceNameText->SetSelection(mSavedDeviceName.GetLength());
        }
        else if (mThisDevice != NULL && !TextUtils::IsEmpty((mThisDevice->GetDeviceName(&thisDeviceName), thisDeviceName))) {
            deviceNameText->SetText(CoreUtils::Convert(thisDeviceName));
            mDeviceNameText->SetSelection(0, thisDeviceName.GetLength());
        }
        mSavedDeviceName = String(NULL);

        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(activity, (IAlertDialogBuilder**)&builder);
        builder->SetTitle(R::string::wifi_p2p_menu_rename);
        builder->SetView(IView::Probe(mDeviceNameText));
        String str;
        activity->GetString(R::string::dlg_ok, &str);
        builder->SetPositiveButton(CoreUtils::Convert(str), mRenameListener);
        activity->GetString(R::string::dlg_cancel, &str);
        builder->SetNegativeButton(CoreUtils::Convert(str), NULL);
        AutoPtr<IAlertDialog> dialog;
        builder->Create((IAlertDialog**)&dialog);
        *resultDialog = IDialog::Probe(dialog);
        REFCOUNT_ADD(*resultDialog)
        return NOERROR;
    }
    else if (id == DIALOG_DELETE_GROUP) {
        Int32 stringId = R::string::wifi_p2p_delete_group_message;

        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(activity, (IAlertDialogBuilder**)&builder);
        String str;
        activity->GetString(stringId, &str);
        builder->SetMessage(CoreUtils::Convert(str));
        activity->GetString(R::string::dlg_ok, &str);
        builder->SetPositiveButton(CoreUtils::Convert(str), mDeleteGroupListener);
        activity->GetString(R::string::dlg_cancel, &str);
        builder->SetNegativeButton(CoreUtils::Convert(str), mDeleteGroupListener);
        AutoPtr<IAlertDialog> dialog;
        builder->Create((IAlertDialog**)&dialog);
        *resultDialog = IDialog::Probe(dialog);
        REFCOUNT_ADD(*resultDialog)
        return NOERROR;
    }
    return NOERROR;
}

ECode CWifiP2pSettings::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    if (mSelectedWifiPeer != NULL) {
        outState->PutParcelable(SAVE_DIALOG_PEER, IParcelable::Probe(mSelectedWifiPeer->mDevice));
    }
    if (mDeviceNameText != NULL) {
        AutoPtr<ICharSequence> cs;
        ITextView::Probe(mDeviceNameText)->GetText((ICharSequence**)&cs);
        outState->PutString(SAVE_DEVICE_NAME, CoreUtils::Unbox(cs));
    }
    if (mSelectedGroup != NULL) {
        outState->PutString(SAVE_SELECTED_GROUP, mSelectedGroup->GetGroupName());
    }
    return NOERROR;
}

void CWifiP2pSettings::HandlePeersChanged()
{
    mPeersGroup->RemoveAll();

    mConnectedDevices = 0;
    if (DBG) Logger::D(TAG, "List of available peers");

    AutoPtr<ICollection> coll;
    mPeers->GetDeviceList((ICollection**)&coll);
    AutoPtr<IIterator> iterator;
    Boolean res;
    while (iterator->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        IWifiP2pDevice* peer = IWifiP2pDevice::Probe(obj);

        if (DBG) Logger::D(TAG, "-> %s", TO_CSTR(peer));
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IPreference> preference;
        CWifiP2pPeer::New(IContext::Probe(activity), peer, (IPreference**)&preference);
        mPeersGroup->AddPreference(preference, &res);
        Int32 status;
        peer->GetStatus(&status);
        if (status == IWifiP2pDevice::CONNECTED) mConnectedDevices++;
    }
    if (DBG) Logger::D(TAG, " mConnectedDevices %d", mConnectedDevices);
}

ECode CWifiP2pSettings::OnPersistentGroupInfoAvailable(
    /* [in] */ IWifiP2pGroupList* groups)
{
    mPersistentGroup->RemoveAll();

    AutoPtr<ICollection> coll;
    groups->GetGroupList((ICollection**)&coll);
    AutoPtr<IIterator> iterator;
    Boolean res;
    while (iterator->HasNext(&res), res) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        IWifiP2pGroup* group = IWifiP2pGroup::Probe(obj);

        if (DBG) Logger::D(TAG, " group %s", TO_CSTR(group));
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<CWifiP2pPersistentGroup> wppg;
        CWifiP2pPersistentGroup::NewByFriend(IContext::Probe(activity), group, (CWifiP2pPersistentGroup**)&wppg);
        mPersistentGroup->AddPreference(IPreference::Probe(wppg), &res);
        if (wppg->GetGroupName().Equals(mSelectedGroupName)) {
            if (DBG) Logger::D(TAG, "Selecting group %s", (wppg->GetGroupName()).string());
            mSelectedGroup = wppg;
            mSelectedGroupName = String(NULL);
        }
    }
    if (!mSelectedGroupName.IsNull()) {
        // Looks like there's a dialog pending getting user confirmation to delete the
        // selected group. When user hits OK on that dialog, we won't do anything; but we
        // shouldn't be in this situation in first place, because these groups are persistent
        // groups and they shouldn't just get deleted!
        Logger::W(TAG, " Selected group %s disappered on next query ", mSelectedGroupName.string());
    }
    return NOERROR;
}

ECode CWifiP2pSettings::OnPeersAvailable(
    /* [in] */ IWifiP2pDeviceList* peers)
{
    if (DBG) Logger::D(TAG, "Requested peers are available");
    mPeers = peers;
    HandlePeersChanged();
    return NOERROR;
}

void CWifiP2pSettings::HandleP2pStateChanged()
{
    UpdateSearchMenu(FALSE);
    mThisDevicePref->SetEnabled(mWifiP2pEnabled);
    IPreference::Probe(mPeersGroup)->SetEnabled(mWifiP2pEnabled);
    IPreference::Probe(mPersistentGroup)->SetEnabled(mWifiP2pEnabled);
}

void CWifiP2pSettings::UpdateSearchMenu(
    /* [in] */ Boolean searching)
{
   mWifiP2pSearching = searching;
   AutoPtr<IActivity> activity;
   GetActivity((IActivity**)&activity);
   if (activity != NULL) activity->InvalidateOptionsMenu();
}

void CWifiP2pSettings::StartSearch()
{
    if (mWifiP2pManager != NULL && !mWifiP2pSearching) {
        AutoPtr<WifiP2pManagerActionListener> listener = new WifiP2pManagerActionListener(this, 6);
        mWifiP2pManager->DiscoverPeers(mChannel, listener);
    }
}

void CWifiP2pSettings::UpdateDevicePref()
{
    if (mThisDevice != NULL) {
        String deviceName;
        mThisDevice->GetDeviceName(&deviceName);
        if (TextUtils::IsEmpty(deviceName)) {
            String deviceAddress;
            mThisDevice->GetDeviceAddress(&deviceAddress);
            mThisDevicePref->SetTitle(CoreUtils::Convert(deviceAddress));
        }
        else {
            mThisDevicePref->SetTitle(CoreUtils::Convert(deviceName));
        }
    }
}

} // namespace P2p
} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos
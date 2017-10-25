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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.Net.h"
#include "elastos/droid/server/wifi/p2p/WifiP2pServiceImpl.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringToIntegral.h>
#include "elastos/core/CoreUtils.h"

#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/server/wifi/WifiStateMachine.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IActivityManagerRunningTaskInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnKeyListener;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::R;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IStaticIpConfiguration;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Net::INetworkUtils;
using Elastos::Droid::Net::CLinkAddress;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED;
using Elastos::Droid::Net::NetworkInfoDetailedState_FAILED;
using Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED;
using Elastos::Droid::Net::IInterfaceConfiguration;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::IDhcpStateMachineHelper;
using Elastos::Droid::Net::CDhcpStateMachineHelper;
using Elastos::Droid::Server::Wifi::WifiStateMachine;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Wifi::P2p::IWifiP2pInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pManager;
using Elastos::Droid::Wifi::P2p::CWifiP2pConfig;
using Elastos::Droid::Wifi::P2p::CWifiP2pDevice;
using Elastos::Droid::Wifi::P2p::CWifiP2pDeviceList;
using Elastos::Droid::Wifi::P2p::CWifiP2pGroup;
using Elastos::Droid::Wifi::P2p::CWifiP2pGroupList;
using Elastos::Droid::Wifi::P2p::CWifiP2pInfo;
using Elastos::Droid::Wifi::P2p::CWifiP2pConfig;
using Elastos::Droid::Wifi::P2p::Nsd::EIID_IWifiP2pServiceInfo;
using Elastos::Droid::Wifi::P2p::Nsd::EIID_IWifiP2pServiceRequest;
using Elastos::Droid::Wifi::P2p::EIID_IIWifiP2pManager;
using Elastos::Droid::Wifi::P2p::EIID_IWifiP2pGroupListGroupDeleteListener;
using Elastos::Droid::Wifi::P2p::IWifiP2pProvDiscEvent;
using Elastos::Core::CoreUtils;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringToIntegral;
using Elastos::Core::IInteger32;
using Elastos::Core::IThread;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::Net::IInetAddress;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {
namespace P2p {

const String WifiP2pServiceImpl::TAG("WifiP2pService");
Boolean WifiP2pServiceImpl::DBG = FALSE;
const String WifiP2pServiceImpl::NETWORKTYPE("WIFI_P2P");

const Boolean WifiP2pServiceImpl::JOIN_GROUP = TRUE;
const Boolean WifiP2pServiceImpl::FORM_GROUP = FALSE;

const Boolean WifiP2pServiceImpl::RELOAD = TRUE;
const Boolean WifiP2pServiceImpl::NO_RELOAD = FALSE;

/* Two minutes comes from the wpa_supplicant setting */
const Int32 WifiP2pServiceImpl::GROUP_CREATING_WAIT_TIME_MS = 120 * 1000;

const Int32 WifiP2pServiceImpl::DISABLE_P2P_WAIT_TIME_MS = 5 * 1000;

/* Set a two minute discover timeout to avoid STA scans from being blocked */
const Int32 WifiP2pServiceImpl::DISCOVER_TIMEOUT_S = 120;

/* Idle time after a peer is gone when the group is torn down */
const Int32 WifiP2pServiceImpl::GROUP_IDLE_TIME_S = 10;

const Int32 WifiP2pServiceImpl::BASE = IProtocol::BASE_WIFI_P2P_SERVICE;

const Int32 WifiP2pServiceImpl::GROUP_CREATING_TIMED_OUT; // = IProtocol::BASE_WIFI_P2P_SERVICE + 1;
/* User accepted a peer request */
const Int32 WifiP2pServiceImpl::PEER_CONNECTION_USER_ACCEPT    ; // = IProtocol::BASE_WIFI_P2P_SERVICE + 2;
/* User rejected a peer request */
const Int32 WifiP2pServiceImpl::PEER_CONNECTION_USER_REJECT    ; //= IProtocol::BASE_WIFI_P2P_SERVICE + 3;
/* User wants to disconnect wifi in favour of p2p */
const Int32 WifiP2pServiceImpl::DROP_WIFI_USER_ACCEPT          ; //= IProtocol::BASE_WIFI_P2P_SERVICE + 4;
/* User wants to keep his wifi connection and drop p2p */
const Int32 WifiP2pServiceImpl::DROP_WIFI_USER_REJECT          ; //= IProtocol::BASE_WIFI_P2P_SERVICE + 5;
const Int32 WifiP2pServiceImpl::DISABLE_P2P_TIMED_OUT; // = IProtocol::BASE_WIFI_P2P_SERVICE + 6;
const Int32 WifiP2pServiceImpl::P2P_CONNECTION_CHANGED; // = IProtocol::BASE_WIFI_P2P_SERVICE + 11;
const Int32 WifiP2pServiceImpl::DISCONNECT_WIFI_REQUEST; // = IProtocol::BASE_WIFI_P2P_SERVICE + 12;
const Int32 WifiP2pServiceImpl::DISCONNECT_WIFI_RESPONSE; // = IProtocol::BASE_WIFI_P2P_SERVICE + 13;
const Int32 WifiP2pServiceImpl::SET_MIRACAST_MODE; // = IProtocol::BASE_WIFI_P2P_SERVICE + 14;
const Int32 WifiP2pServiceImpl::BLOCK_DISCOVERY; // = IProtocol::BASE_WIFI_P2P_SERVICE + 15;
const Int32 WifiP2pServiceImpl::SET_COUNTRY_CODE; // = IProtocol::BASE_WIFI_P2P_SERVICE + 16;
const Int32 WifiP2pServiceImpl::P2P_MIRACAST_MODE_CHANGED; // = IProtocol::BASE_WIFI_P2P_SERVICE + 17;
const Int32 WifiP2pServiceImpl::ENABLED = 1;
const Int32 WifiP2pServiceImpl::DISABLED = 0;

/* Is chosen as a unique range to avoid conflict with
   the range defined in Tethering.java */
const String WifiP2pServiceImpl::SERVER_ADDRESS("192.168.49.1");

Int32 WifiP2pServiceImpl::mGroupCreatingTimeoutIndex = 0;
Int32 WifiP2pServiceImpl::mDisableP2pTimeoutIndex = 0;

//==================================================================
// WifiP2pServiceImpl::ClientInfo
//==================================================================
WifiP2pServiceImpl::ClientInfo::ClientInfo(
    /* [in] */ IMessenger* m,
    /* [in] */ WifiP2pServiceImpl* host)
    : mMessenger(m)
    , mHost(host)
{
}

ECode WifiP2pServiceImpl::ClientInfo::GetMessenger(
    /*[out] */ IMessenger** messenger)
{
    VALIDATE_NOT_NULL(messenger);
    *messenger = mMessenger;
    REFCOUNT_ADD(*messenger);
    return NOERROR;
}

ECode WifiP2pServiceImpl::ClientInfo::SetMessenger(
    /*[in] */ IMessenger* messenger)
{
    mMessenger = messenger;
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl
//==================================================================
CAR_INTERFACE_IMPL_2(WifiP2pServiceImpl, Object, IIWifiP2pManager, IBinder);

WifiP2pServiceImpl::WifiP2pServiceImpl()
    : mP2pSupported(FALSE)
    , mAutonomousGroup(FALSE)
    , mJoinExistingGroup(FALSE)
    , mIsInvite(FALSE)
    , mDiscoveryStarted(FALSE)
    , mDiscoveryBlocked(FALSE)
    , mDiscoveryPostponed(FALSE)
    , mTemporarilyDisconnectedWifi(FALSE)
    , mServiceTransactionId(0)
{
    mReplyChannel = new AsyncChannel();
    CWifiP2pDevice::New((IWifiP2pDevice**)&mThisDevice);
}

WifiP2pServiceImpl::~WifiP2pServiceImpl()
{
}

ECode WifiP2pServiceImpl::constructor(
    /* [in] */ IContext *ctx)
{
    mContext = ctx;

    //STOPSHIP: get this from native side
    mInterface = String("p2p0");

    CNetworkInfo::New(IConnectivityManager::TYPE_WIFI_P2P, 0, NETWORKTYPE,
        String(""), (INetworkInfo**)&mNetworkInfo);

    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    pm->HasSystemFeature(IPackageManager::FEATURE_WIFI_DIRECT, &mP2pSupported);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    String str;
    res->GetString(R::string::config_wifi_p2p_device_type, &str);
    mThisDevice->SetPrimaryDeviceType(str);

    AutoPtr<IHandlerThread> wifiP2pThread;
    CHandlerThread::New(String("WifiP2pService"), (IHandlerThread**)&wifiP2pThread);
    IThread::Probe(wifiP2pThread)->Start();
    AutoPtr<ILooper> looper;
    wifiP2pThread->GetLooper((ILooper**)&looper);
    mClientHandler = new ClientHandler(this, looper);

    Slogger::I(TAG, " WifiP2pService start! P2pSupported %d, device type: %s", mP2pSupported, str.string());
    mP2pStateMachine = new P2pStateMachine(TAG, looper, mP2pSupported, this);
    mP2pStateMachine->Start();
    return NOERROR;
}

WifiP2pServiceImpl::P2pStatus WifiP2pServiceImpl::ValueOf(
    /* [in] */ Int32 error)
{
    switch(error) {
    case 0 :
        return P2pStatus_SUCCESS;
    case 1:
        return P2pStatus_INFORMATION_IS_CURRENTLY_UNAVAILABLE;
    case 2:
        return P2pStatus_INCOMPATIBLE_PARAMETERS;
    case 3:
        return P2pStatus_LIMIT_REACHED;
    case 4:
        return P2pStatus_INVALID_PARAMETER;
    case 5:
        return P2pStatus_UNABLE_TO_ACCOMMODATE_REQUEST;
    case 6:
        return P2pStatus_PREVIOUS_PROTOCOL_ERROR;
    case 7:
        return P2pStatus_NO_COMMON_CHANNEL;
    case 8:
        return P2pStatus_UNKNOWN_P2P_GROUP;
    case 9:
        return P2pStatus_BOTH_GO_INTENT_15;
    case 10:
        return P2pStatus_INCOMPATIBLE_PROVISIONING_METHOD;
    case 11:
        return P2pStatus_REJECTED_BY_USER;
    default:
        return P2pStatus_UNKNOWN;
    }
}

ECode WifiP2pServiceImpl::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    /*TODO
    if (mContext.checkCallingOrSelfPermission(android.Manifest.permission.DUMP)
            != PackageManager.PERMISSION_GRANTED) {
        pw.println("Permission Denial: can't dump WifiP2pService from from pid="
                + Binder.getCallingPid()
                + ", uid=" + Binder.getCallingUid());
        return;
    }
    mP2pStateMachine.dump(fd, pw, args);
    pw.println("mAutonomousGroup " + mAutonomousGroup);
    pw.println("mJoinExistingGroup " + mJoinExistingGroup);
    pw.println("mDiscoveryStarted " + mDiscoveryStarted);
    pw.println("mNetworkInfo " + mNetworkInfo);
    pw.println("mTemporarilyDisconnectedWifi " + mTemporarilyDisconnectedWifi);
    pw.println("mServiceDiscReqId " + mServiceDiscReqId);
    pw.println();
    */
    return NOERROR;
}


ECode WifiP2pServiceImpl::ConnectivityServiceReady()
{
    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::NETWORKMANAGEMENT_SERVICE);
    mNwService = IINetworkManagementService::Probe(obj);
    return NOERROR;
}

ECode WifiP2pServiceImpl::GetMessenger(
    /* [out] */ IMessenger** msg)
{
    VALIDATE_NOT_NULL(msg);

    FAIL_RETURN(EnforceAccessPermission());
    FAIL_RETURN(EnforceChangePermission());

    return CMessenger::New(mClientHandler, msg);
}

ECode WifiP2pServiceImpl::GetP2pStateMachineMessenger(
    /* [out] */ IMessenger** messenger)
{
    VALIDATE_NOT_NULL(messenger);
    EnforceConnectivityInternalOrLocationHardwarePermission();
    EnforceAccessPermission();
    EnforceChangePermission();
    AutoPtr<IHandler> handler;
    mP2pStateMachine->GetHandler((IHandler**)&handler);
    return CMessenger::New(handler, messenger);
}

void WifiP2pServiceImpl::EnableVerboseLogging(
    /* [in] */ Int32 verbose)
{
    if (verbose > 0 ) {
        DBG = TRUE;
    } else {
        DBG = FALSE;
    }
}

ECode WifiP2pServiceImpl::SetMiracastMode(
    /* [in] */ Int32 mode)
{
    EnforceConnectivityInternalPermission();
    mP2pStateMachine->SendMessage(SET_MIRACAST_MODE, mode);
    return NOERROR;
}

ECode WifiP2pServiceImpl::EnforceAccessPermission()
{
    return mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::ACCESS_WIFI_STATE,
        String("WifiP2pService"));
}

ECode WifiP2pServiceImpl::EnforceChangePermission()
{
    return mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::CHANGE_WIFI_STATE,
        String("WifiP2pService"));
}

ECode WifiP2pServiceImpl::EnforceConnectivityInternalPermission()
{
    return mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL,
            String("WifiP2pService"));
}

Int32 WifiP2pServiceImpl::CheckConnectivityInternalPermission()
{
    Int32 result;
    mContext->CheckCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL,
            &result);
    return result;
}

Int32 WifiP2pServiceImpl::CheckLocationHardwarePermission()
{
    Int32 result;
    mContext->CheckCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::LOCATION_HARDWARE,
            &result);
    return result;
}

ECode WifiP2pServiceImpl::EnforceConnectivityInternalOrLocationHardwarePermission()
{
    if (CheckConnectivityInternalPermission() != IPackageManager::PERMISSION_GRANTED
            && CheckLocationHardwarePermission() != IPackageManager::PERMISSION_GRANTED) {
        EnforceConnectivityInternalPermission();
    }
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::GroupDeleteListener
//==================================================================
CAR_INTERFACE_IMPL(WifiP2pServiceImpl::P2pStateMachine::GroupDeleteListener,
        Object, IWifiP2pGroupListGroupDeleteListener)

WifiP2pServiceImpl::P2pStateMachine::GroupDeleteListener::GroupDeleteListener(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::GroupDeleteListener::OnDeleteGroup(
    /* [in] */ Int32 netId)
{
    if (WifiP2pServiceImpl::DBG)
        Slogger::E(WifiP2pServiceImpl::TAG, "called onDeleteGroup() netId=%d", netId);

    mHost->mWifiNative->RemoveNetwork(netId);
    mHost->mWifiNative->SaveConfig();
    mHost->SendP2pPersistentGroupsChangedBroadcast();
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::P2pStateMachine(
    /* [in] */ const String& name,
    /* [in] */ ILooper* looper,
    /* [in] */ Boolean p2pSupported,
    /* [in] */ WifiP2pServiceImpl* host)
    : mHost(host)
{
    StateMachine::constructor(name);
    mDefaultState = new DefaultState(this);
    mP2pNotSupportedState = new P2pNotSupportedState(this);
    mP2pDisablingState = new P2pDisablingState(this);
    mP2pDisabledState = new P2pDisabledState(this);
    mP2pEnablingState = new P2pEnablingState(this);
    mP2pEnabledState = new P2pEnabledState(this);
    // Inactive is when p2p is enabled with no connectivity
    mInactiveState = new InactiveState(this);
    mGroupCreatingState = new GroupCreatingState(this);
    mUserAuthorizingInviteRequestState = new UserAuthorizingInviteRequestState(this);
    mUserAuthorizingNegotiationRequestState = new UserAuthorizingNegotiationRequestState(this);
    mProvisionDiscoveryState = new ProvisionDiscoveryState(this);
    mGroupNegotiationState = new GroupNegotiationState(this);
    mFrequencyConflictState = new FrequencyConflictState(this);

    mGroupCreatedState = new GroupCreatedState(this);
    mUserAuthorizingJoinState = new UserAuthorizingJoinState(this);
    mOngoingGroupRemovalState = new OngoingGroupRemovalState(this);

    mWifiNative = new WifiNative(mHost->mInterface);
    mWifiMonitor = new WifiMonitor(this, mWifiNative);

    CWifiP2pDeviceList::New((IWifiP2pDeviceList**)&mPeers);

    CWifiP2pDeviceList::New((IWifiP2pDeviceList**)&mPeersLostDuringConnection);
    AutoPtr<IWifiP2pGroupListGroupDeleteListener> listener = new GroupDeleteListener(this);
    CWifiP2pGroupList::New(NULL, listener, (IWifiP2pGroupList**)&mGroups);
    CWifiP2pInfo::New((IWifiP2pInfo**)&mWifiP2pInfo);
    mPendingReformGroupIndication = FALSE;
    CWifiP2pConfig::New((IWifiP2pConfig**)&mSavedPeerConfig);

    AddState(mDefaultState);
    AddState(mP2pNotSupportedState, mDefaultState);
    AddState(mP2pDisablingState, mDefaultState);
    AddState(mP2pDisabledState, mDefaultState);
    AddState(mP2pEnablingState, mDefaultState);
    AddState(mP2pEnabledState, mDefaultState);
    AddState(mInactiveState, mP2pEnabledState);
    AddState(mGroupCreatingState, mP2pEnabledState);
    AddState(mUserAuthorizingInviteRequestState, mGroupCreatingState);
    AddState(mUserAuthorizingNegotiationRequestState, mGroupCreatingState);
    AddState(mProvisionDiscoveryState, mGroupCreatingState);
    AddState(mGroupNegotiationState, mGroupCreatingState);
    AddState(mFrequencyConflictState, mGroupCreatingState);
    AddState(mGroupCreatedState, mP2pEnabledState);
    AddState(mUserAuthorizingJoinState, mGroupCreatedState);
    AddState(mOngoingGroupRemovalState, mGroupCreatedState);

    if (p2pSupported) {
        SetInitialState(mP2pDisabledState);
    }
    else {
        SetInitialState(mP2pNotSupportedState);
    }

    SetLogRecSize(50);
    SetLogOnlyTransitions(TRUE);
}

ECode WifiP2pServiceImpl::P2pStateMachine::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    assert(0);
    //TODO
    //super.dump(fd, pw, args);
    //pw.println("mWifiP2pInfo " + mWifiP2pInfo);
    //pw.println("mGroup " + mGroup);
    //pw.println("mSavedPeerConfig " + mSavedPeerConfig);
    //pw.println("mSavedP2pGroup " + mSavedP2pGroup);
    //pw.println();
}

ECode WifiP2pServiceImpl::P2pStateMachine::SendP2pStateChangedBroadcast(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiP2pManager::WIFI_P2P_STATE_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    if (enabled) {
        intent->PutExtra(IWifiP2pManager::EXTRA_WIFI_STATE,
            IWifiP2pManager::WIFI_P2P_STATE_ENABLED);
    } else {
        intent->PutExtra(IWifiP2pManager::EXTRA_WIFI_STATE,
            IWifiP2pManager::WIFI_P2P_STATE_DISABLED);
    }
    return mHost->mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

ECode WifiP2pServiceImpl::P2pStateMachine::SendP2pDiscoveryChangedBroadcast(
    /* [in] */ Boolean started)
{
    if (mHost->mDiscoveryStarted == started) return NOERROR;
    mHost->mDiscoveryStarted = started;

    if (WifiP2pServiceImpl::DBG)
        Slogger::D(WifiP2pServiceImpl::TAG, "discovery change broadcast %d", started);

    AutoPtr<IIntent> intent;
    CIntent::New(IWifiP2pManager::WIFI_P2P_DISCOVERY_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(IWifiP2pManager::EXTRA_DISCOVERY_STATE, started ?
        IWifiP2pManager::WIFI_P2P_DISCOVERY_STARTED : IWifiP2pManager::WIFI_P2P_DISCOVERY_STOPPED);
    return mHost->mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

ECode WifiP2pServiceImpl::P2pStateMachine::SendThisDeviceChangedBroadcast()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiP2pManager::WIFI_P2P_THIS_DEVICE_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    AutoPtr<IWifiP2pDevice> device;
    CWifiP2pDevice::New(mHost->mThisDevice, (IWifiP2pDevice**)&device);
    intent->PutExtra(
        IWifiP2pManager::EXTRA_WIFI_P2P_DEVICE,
        IParcelable::Probe(device.Get()));
    return mHost->mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

ECode WifiP2pServiceImpl::P2pStateMachine::SendPeersChangedBroadcast()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiP2pManager::WIFI_P2P_PEERS_CHANGED_ACTION, (IIntent**)&intent);
    AutoPtr<IWifiP2pDeviceList> wpdl;
    CWifiP2pDeviceList::New(mPeers, (IWifiP2pDeviceList**)&wpdl);
    intent->PutExtra(IWifiP2pManager::EXTRA_P2P_DEVICE_LIST, IParcelable::Probe(wpdl));
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    return mHost->mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
}

ECode WifiP2pServiceImpl::P2pStateMachine::SendP2pConnectionChangedBroadcast()
{
    if (WifiP2pServiceImpl::DBG) Logd("sending p2p connection changed broadcast");
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiP2pManager::WIFI_P2P_CONNECTION_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT
        | IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    AutoPtr<IWifiP2pInfo> wifiInfo;
    CWifiP2pInfo::New(mWifiP2pInfo, (IWifiP2pInfo**)&wifiInfo);
    intent->PutExtra(
        IWifiP2pManager::EXTRA_WIFI_P2P_INFO,
        IParcelable::Probe(wifiInfo.Get()));

    AutoPtr<INetworkInfo> netInfo;
    CNetworkInfo::New(mHost->mNetworkInfo, (INetworkInfo**)&netInfo);
    intent->PutExtra(
        IWifiP2pManager::EXTRA_NETWORK_INFO,
        IParcelable::Probe(netInfo.Get()));

    AutoPtr<IWifiP2pGroup> group;
    CWifiP2pGroup::New(mGroup, (IWifiP2pGroup**)&group);
    intent->PutExtra(
        IWifiP2pManager::EXTRA_WIFI_P2P_GROUP,
        IParcelable::Probe(group.Get()));

    mHost->mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);

    AutoPtr<INetworkInfo> networkInfo;
    CNetworkInfo::New(mHost->mNetworkInfo, (INetworkInfo**)&networkInfo);
    mHost->mWifiChannel->SendMessage(
        WifiP2pServiceImpl::P2P_CONNECTION_CHANGED,
        networkInfo->Probe(EIID_IInterface));
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::SendMiracastModeChanged(
    /* [in] */ Int32 mode)
{
    mHost->mWifiChannel->SendMessage(WifiP2pServiceImpl::P2P_MIRACAST_MODE_CHANGED, mode);
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::SendP2pPersistentGroupsChangedBroadcast()
{
    if (WifiP2pServiceImpl::DBG) Logd("sending p2p persistent groups changed broadcast");
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiP2pManager::WIFI_P2P_PERSISTENT_GROUPS_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    return mHost->mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

ECode WifiP2pServiceImpl::P2pStateMachine::StartDhcpServer(
    /* [in] */ const String& intf)
{
    // try {
    AutoPtr<IInterfaceConfiguration> ifcg;
    mHost->mNwService->GetInterfaceConfig(intf, (IInterfaceConfiguration**)&ifcg);

    AutoPtr<IInetAddress> addr;
    NetworkUtils::NumericToInetAddress(SERVER_ADDRESS, (IInetAddress**)&addr);
    AutoPtr<ILinkAddress> linkAddr;
    CLinkAddress::New(addr, 24, (ILinkAddress**)&linkAddr);
    ifcg->SetLinkAddress(linkAddr);
    ifcg->SetInterfaceUp();
    mHost->mNwService->SetInterfaceConfig(intf, ifcg);
    /* This starts the dnsmasq server */
    AutoPtr<IInterface> svrObj;
    mHost->mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&svrObj);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(svrObj);

    AutoPtr<ArrayOf<String> > tetheringDhcpRanges;
    cm->GetTetheredDhcpRanges((ArrayOf<String>**)&tetheringDhcpRanges);
    Boolean started = FALSE;
    if (mHost->mNwService->IsTetheringStarted(&started), started) {
        if (WifiP2pServiceImpl::DBG) Logd("Stop existing tethering and restart it");
        mHost->mNwService->StopTethering();
    }
    mHost->mNwService->TetherInterface(intf);
    return mHost->mNwService->StartTethering(tetheringDhcpRanges);
    // } catch (Exception e) {
    //     Loge("Error configuring interface " + intf + ", :" + e);
    //     return;
    // }

    Slogger::D(WifiP2pServiceImpl::TAG, "Started Dhcp server on %s", intf.string());
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::StopDhcpServer(
    /* [in] */ const String& intf)
{
    // try {
    ECode ec = NOERROR;
    mHost->mNwService->UntetherInterface(intf);
    AutoPtr<ArrayOf<String> > interfaces;
    mHost->mNwService->ListTetheredInterfaces((ArrayOf<String>**)&interfaces);
    for (Int32 i = 0; i < interfaces->GetLength(); ++i) {
        String tmp = (*interfaces)[i];
        Logd(String("List all interfaces ") + tmp);
        if (tmp.Compare(intf) != 0) {
            Logd("Found other tethering interfaces, so keep tethering alive");
            return NOERROR;
        }
    }
    ec = mHost->mNwService->StopTethering();

    if (FAILED(ec)) {
        Slogger::E(WifiP2pServiceImpl::TAG, "Error stopping Dhcp server, ec=%08x", ec);
        return NOERROR;
    }
    // } catch (Exception e) {
    //     Loge("Error stopping Dhcp server" + e);
    //     return;
    // }

    Logd("Stopped Dhcp server");
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::NotifyP2pEnableFailure()
{
    Logd("p2p: NotifyP2pEnableFailure.");
    AutoPtr<IResourcesHelper> resHelper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&resHelper);
    AutoPtr<IResources> r;
    resHelper->GetSystem((IResources**)&r);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mHost->mContext, (IAlertDialogBuilder**)&builder);

    String title, msg, ok;
    r->GetString(R::string::wifi_p2p_dialog_title, &title);
    r->GetString(R::string::wifi_p2p_failed_message, &msg);
    r->GetString(R::string::ok, &ok);

    AutoPtr<ICharSequence> csTitle;
    AutoPtr<ICharSequence> csMsg;
    AutoPtr<ICharSequence> csOk;
    CString::New(title, (ICharSequence**)&csTitle);
    CString::New(msg, (ICharSequence**)&csMsg);
    CString::New(ok, (ICharSequence**)&csOk);

    builder->SetTitle(csTitle);
    builder->SetMessage(csMsg);
    builder->SetPositiveButton(csOk, NULL);

    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);

    AutoPtr<IWindow> win;
    IDialog::Probe(dialog)->GetWindow((IWindow**)&win);
    win->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    return IDialog::Probe(dialog)->Show();
}

ECode WifiP2pServiceImpl::P2pStateMachine::AddRowToDialog(
    /* [in] */ IViewGroup* group,
    /* [in] */ Int32 stringId,
    /* [in] */ const String& value)
{
    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "p2p: AddRowToDialog: %s.", value.string());
    }

    AutoPtr<IResourcesHelper> resHelper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&resHelper);
    AutoPtr<IResources> r;
    resHelper->GetSystem((IResources**)&r);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(mHost->mContext, (ILayoutInflater**)&inflater);
    AutoPtr<IView> row;
    inflater->Inflate(R::layout::wifi_p2p_dialog_row, group, FALSE, (IView**)&row);
    AutoPtr<IView> tempView;
    row->FindViewById(Elastos::Droid::R::id::name, (IView**)&tempView);
    AutoPtr<ITextView> tx = ITextView::Probe(tempView);
    if (tx) {
        String id;
        r->GetString(stringId, &id);
        AutoPtr<ICharSequence> seq;
        CString::New(id, (ICharSequence**)&seq);
        tx->SetText(seq);
    }

    tempView = NULL;
    row->FindViewById(Elastos::Droid::R::id::value, (IView**)&tempView);
    tx = ITextView::Probe(tempView);
    if (tx) {
        AutoPtr<ICharSequence> seq;
        CString::New(value, (ICharSequence**)&seq);
        tx->SetText(seq);
    }

    return group->AddView(row);
}

ECode WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationSent(
    /* [in] */ const String& pin,
    /* [in] */ const String& peerAddress)
{
    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "p2p: AddRowToDialog: pin: %s, address: %s.",
            pin.string(), peerAddress.string());
    }

    AutoPtr<IResourcesHelper> resHelper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&resHelper);
    AutoPtr<IResources> r;
    resHelper->GetSystem((IResources**)&r);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(mHost->mContext, (ILayoutInflater**)&inflater);
    AutoPtr<IView> textEntryView;
    inflater->Inflate(R::layout::wifi_p2p_dialog, NULL, (IView**)&textEntryView);

    AutoPtr<IView> tempView;
    textEntryView->FindViewById(R::id::info, (IView**)&tempView);
    AutoPtr<IViewGroup> group = IViewGroup::Probe(tempView);
    assert(group != NULL);

    String name = GetDeviceName(peerAddress);
    AddRowToDialog(group, R::string::wifi_p2p_to_message, name);
    AddRowToDialog(group, R::string::wifi_p2p_show_pin_message, pin);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mHost->mContext, (IAlertDialogBuilder**)&builder);
    String title, ok;
    r->GetString(R::string::wifi_p2p_invitation_sent_title, &title);
    r->GetString(R::string::ok, &ok);

    AutoPtr<ICharSequence> csTitle;
    AutoPtr<ICharSequence> csOk;
    CString::New(title, (ICharSequence**)&csTitle);
    CString::New(ok, (ICharSequence**)&csOk);

    builder->SetTitle(csTitle);
    builder->SetView(textEntryView);
    builder->SetPositiveButton(csOk, NULL);

    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);

    AutoPtr<IWindow> win;
    IDialog::Probe(dialog)->GetWindow((IWindow**)&win);
    win->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    return IDialog::Probe(dialog)->Show();
}

ECode WifiP2pServiceImpl::P2pStateMachine::NotifyP2pProvDiscShowPinRequest(
    /* [in] */ const String& pin,
    /* [in] */ const String& peerAddress)
{
    AutoPtr<IResourcesHelper> resHelper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&resHelper);
    AutoPtr<IResources> r;
    resHelper->GetSystem((IResources**)&r);
    String tempDevAddress = peerAddress;
    String tempPin = pin;

    AutoPtr<IView> textEntryView;
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(mHost->mContext, (ILayoutInflater**)&inflater);
    inflater->Inflate(R::layout::wifi_p2p_dialog, NULL, (IView**)&textEntryView);

    AutoPtr<IView> tempView;
    textEntryView->FindViewById(R::id::info, (IView**)&tempView);
    AutoPtr<IViewGroup> group = IViewGroup::Probe(tempView);
    AddRowToDialog(group, R::string::wifi_p2p_to_message, GetDeviceName(peerAddress));
    AddRowToDialog(group, R::string::wifi_p2p_show_pin_message, pin);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mHost->mContext, (IAlertDialogBuilder**)&builder);
    String title;
    r->GetString(R::string::wifi_p2p_invitation_sent_title, &title);
    builder->SetTitle(CoreUtils::Convert(title));
    builder->SetView(textEntryView);
    String accept;
    r->GetString(R::string::accept, &accept);
    AutoPtr<IDialogInterfaceOnClickListener> listener =
        new NotifyP2pProvDiscShowPinRequestOnClickListener(this, tempDevAddress, tempPin);
    builder->SetPositiveButton(CoreUtils::Convert(accept), listener);
    builder->SetCancelable(FALSE);
    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);
    AutoPtr<IWindow> win;
    IDialog::Probe(dialog)->GetWindow((IWindow**)&win);
    win->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    return IDialog::Probe(dialog)->Show();
}

ECode WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceived()
{
    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "p2p: NotifyInvitationReceived");
    }

    AutoPtr<IResourcesHelper> resHelper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&resHelper);
    AutoPtr<IResources> r;
    resHelper->GetSystem((IResources**)&r);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(mHost->mContext, (ILayoutInflater**)&inflater);
    AutoPtr<IView> textEntryView;
    inflater->Inflate(R::layout::wifi_p2p_dialog, NULL, (IView**)&textEntryView);

    AutoPtr<IView> tempView;
    textEntryView->FindViewById(R::id::info, (IView**)&tempView);
    AutoPtr<IViewGroup> group = IViewGroup::Probe(tempView);

    String addr;
    mSavedPeerConfig->GetDeviceAddress(&addr);
    AddRowToDialog(group, R::string::wifi_p2p_from_message, GetDeviceName(addr));

    AutoPtr<IView> editTextView;
    textEntryView->FindViewById(Elastos::Droid::R::id::wifi_p2p_wps_pin, (IView**)&editTextView);
    AutoPtr<Elastos::Droid::Widget::IEditText> pin = IEditText::Probe(editTextView);

    AutoPtr<IAlertDialog> dialog;
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mHost->mContext, (IAlertDialogBuilder**)&builder);

    String title, accept;
    r->GetString(Elastos::Droid::R::string::wifi_p2p_invitation_to_connect_title, &title);
    r->GetString(Elastos::Droid::R::string::accept, &accept);
    builder->SetTitle(CoreUtils::Convert(title));
    builder->SetView(textEntryView);

    AutoPtr<IWpsInfo> wps;
    mSavedPeerConfig->GetWps((IWpsInfo**)&wps);

    AutoPtr<IDialogInterfaceOnClickListener> positiveListener =
        new NotifyInvitationReceivedPositiveButtonListener(this, wps, pin);
    builder->SetPositiveButton(CoreUtils::Convert(accept), positiveListener);

    AutoPtr<IDialogInterfaceOnClickListener> negativeListener =
        new NotifyInvitationReceivedNegativeButtonListener(this);
    String decline;
    r->GetString(R::string::decline, &decline);
    builder->SetNegativeButton(CoreUtils::Convert(decline), negativeListener);

    AutoPtr<IDialogInterfaceOnCancelListener> cancelListener =
        new NotifyInvitationReceivedDialogCancelListener(this);
    builder->SetOnCancelListener(cancelListener);
    builder->Create((IAlertDialog**)&dialog);

    //make the enter pin area or the display pin area visible
    Int32 setup;
    wps->GetSetup(&setup);
    AutoPtr<IView> v;
    String pinStr;
    switch (setup) {
        case IWpsInfo::KEYPAD:
            if (WifiP2pServiceImpl::DBG) Logd("Enter pin section visible");
            textEntryView->FindViewById(Elastos::Droid::R::id::enter_pin_section, (IView**)&v);
            v->SetVisibility(IView::VISIBLE);
            break;
        case IWpsInfo::DISPLAY:
            if (WifiP2pServiceImpl::DBG) Logd("Shown pin section visible");
            wps->GetPin(&pinStr);
            AddRowToDialog(group, Elastos::Droid::R::string::wifi_p2p_show_pin_message, pinStr);
            break;
        default:
            break;
    }

    AutoPtr<IConfiguration> config;
    r->GetConfiguration((IConfiguration**)&config);
    Int32 uiMode;
    config->GetUiMode(&uiMode);
    if ((uiMode & IConfiguration::UI_MODE_TYPE_APPLIANCE) ==
            IConfiguration::UI_MODE_TYPE_APPLIANCE) {
        // For appliance devices, add a key listener which accepts.
        AutoPtr<IDialogInterfaceOnKeyListener> keyListener =
            new NotifyInvitationReceivedDialogKeyListener(this, dialog);
        IDialog::Probe(dialog)->SetOnKeyListener(keyListener);
        // TODO: add timeout for this dialog.
        // TODO: update UI in appliance mode to tell user what to do.
    }

    AutoPtr<IWindow> win;
    IDialog::Probe(dialog)->GetWindow((IWindow**)&win);
    win->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    return IDialog::Probe(dialog)->Show();
}

/**
 * Synchronize the persistent group list between
 * wpa_supplicant and mGroups.
 */
ECode WifiP2pServiceImpl::P2pStateMachine::UpdatePersistentNetworks(
    /* [in] */ Boolean reload)
{
    String listStr = mWifiNative->ListNetworks();
    if (listStr.IsNull()) return NOERROR;

    Boolean isSaveRequired = FALSE;
    AutoPtr< ArrayOf<String> > lines;
    StringUtils::Split(listStr, String("\n"), (ArrayOf<String>**)&lines);
    if (lines == NULL) return NOERROR;

    Boolean cleared;
    if (reload) mGroups->Clear(&cleared);

    // Skip the first line, which is a header
    for (Int32 i = 1; i < lines->GetLength(); i++) {
        AutoPtr< ArrayOf<String> > result;
        StringUtils::Split((*lines)[i], String("\t"), (ArrayOf<String>**)&result);

        if (result == NULL || result->GetLength() < 4) {
            continue;
        }
        // network-id | ssid | bssid | flags
        Int32 netId = -1;
        String ssid = (*result)[1];
        String bssid = (*result)[2];
        String flags = (*result)[3];
        // try {
        ECode ec = StringToIntegral::Parse((*result)[0], 10, &netId);
        if (FAILED(ec)) {
            Slogger::E(WifiP2pServiceImpl::TAG, "NumberFormatException %s", (*result)[0].string());
            continue;
        }
        // } catch(NumberFormatException e) {
        //     e.printStackTrace();
        //     continue;
        // }

        if (flags.IndexOf("[CURRENT]") != -1) {
            continue;
        }
        if (flags.IndexOf("[P2P-PERSISTENT]") == -1) {
            /*
             * The unused profile is sometimes remained when the p2p group formation is failed.
             * So, we clean up the p2p group here.
             */
            if (WifiP2pServiceImpl::DBG)
                Slogger::D(WifiP2pServiceImpl::TAG, "clean up the unused persistent group. netId=%d", netId);
            mWifiNative->RemoveNetwork(netId);
            isSaveRequired = TRUE;
            continue;
        }

        Boolean b;
        mGroups->Contains(netId, &b);
        if (b) {
            continue;
        }

        AutoPtr<IWifiP2pGroup> group;
        CWifiP2pGroup::New((IWifiP2pGroup**)&group);
        group->SetNetworkId(netId);
        group->SetNetworkName(ssid);
        String mode = mWifiNative->GetNetworkVariable(netId, String("mode"));
        if (!mode.IsNull() && mode.Equals("3")) {
            group->SetIsGroupOwner(TRUE);
        }
        String addr;
        mHost->mThisDevice->GetDeviceAddress(&addr);
        if (bssid.EqualsIgnoreCase(addr)) {
            group->SetOwner(mHost->mThisDevice);
        }
        else {
            AutoPtr<IWifiP2pDevice> device;
            CWifiP2pDevice::New((IWifiP2pDevice**)&device);
            device->SetDeviceAddress(bssid);
            group->SetOwner(device);
        }
        mGroups->Add(group);
        isSaveRequired = TRUE;
    }

    if (reload || isSaveRequired) {
        mWifiNative->SaveConfig();
        SendP2pPersistentGroupsChangedBroadcast();
    }
    return NOERROR;
}

Boolean WifiP2pServiceImpl::P2pStateMachine::IsConfigInvalid(
    /* [in] */ IWifiP2pConfig* config)
{
    if (config == NULL) return TRUE;
    String deviceAddress;
    config->GetDeviceAddress(&deviceAddress);
    if (TextUtils::IsEmpty(deviceAddress)) return TRUE;
    AutoPtr<IWifiP2pDevice> dev;
    mPeers->Get(deviceAddress, (IWifiP2pDevice**)&dev);
    if (dev == NULL) return TRUE;
    return FALSE;
}

AutoPtr<IWifiP2pDevice> WifiP2pServiceImpl::P2pStateMachine::FetchCurrentDeviceDetails(
    /* [in] */ IWifiP2pConfig* config)
{
    /* Fetch & update group capability from supplicant on the device */
    String deviceAddress;
    config->GetDeviceAddress(&deviceAddress);
    Int32 gc = mWifiNative->GetGroupCapability(deviceAddress);
    mPeers->UpdateGroupCapability(deviceAddress, gc);
    AutoPtr<IWifiP2pDevice> dev;
    mPeers->Get(deviceAddress, (IWifiP2pDevice**)&dev);
    return dev;
}

ECode WifiP2pServiceImpl::P2pStateMachine::P2pConnectWithPinDisplay(
    /* [in] */ IWifiP2pConfig* config)
{
    Boolean join = FALSE;
    AutoPtr<IWifiP2pDevice> dev = FetchCurrentDeviceDetails(config);

    if (mHost->mIsInvite) {
        join = TRUE;
    } else {
        dev->IsGroupOwner(&join);
    }

    String pin = mWifiNative->P2pConnect(config, join);
    //try {
    Int32 ival;
    ECode ec = StringToIntegral::Parse(pin, 10, &ival);
    if (SUCCEEDED(ec)) {
        String deviceAddress;
        config->GetDeviceAddress(&deviceAddress);
        NotifyInvitationSent(pin, deviceAddress);
    }
    //} catch (NumberFormatException ignore) {
        // do nothing if p2pConnect did not return a pin
    //}
    mHost->mIsInvite = FALSE;
    return NOERROR;
}

Boolean WifiP2pServiceImpl::P2pStateMachine::ReinvokePersistentGroup(
    /* [in] */ IWifiP2pConfig* config)
{
    AutoPtr<IWifiP2pDevice> dev = FetchCurrentDeviceDetails(config);
    Boolean join;
    dev->IsGroupOwner(&join);
    String address,ssid;
    dev->GetDeviceAddress(&address);
    ssid = mWifiNative->P2pGetSsid(address);
    if (WifiP2pServiceImpl::DBG)
        Slogger::D(WifiP2pServiceImpl::TAG, "target ssid is %s join %d", ssid.string(), join);

    Boolean b;
    dev->IsGroupLimit(&b);
    if (join && b) {
        if (WifiP2pServiceImpl::DBG) Logd("target device reaches group limit.");

        // if the target group has reached the limit,
        // try group formation.
        join = FALSE;
    }
    else if (join) {
        Int32 netId;
        mGroups->GetNetworkId(address, ssid, &netId);
        if (netId >= 0) {
            // Skip WPS and start 4way handshake immediately.
            b = mWifiNative->P2pGroupAdd(netId);
            if (!b) {
                return FALSE;
            }
            return TRUE;
        }
    }

    dev->IsDeviceLimit(&b);
    if (!join && b) {
        Loge("target device reaches the device limit.");
        return FALSE;
    }

    dev->IsInvitationCapable(&b);
    if (!join && b) {
        Int32 netId = IWifiP2pGroup::PERSISTENT_NET_ID;
        Int32 id;
        config->GetNetId(&id);
        if (id >= 0) {
            String oAddr;
            String addr;
            config->GetDeviceAddress(&addr);
            mGroups->GetOwnerAddr(id, &oAddr);
            if (addr.Equals(oAddr)) {
                netId = id;
            }
        }
        else {
            mGroups->GetNetworkId(address, &netId);
        }
        if (netId < 0) {
            netId = GetNetworkIdFromClientList(address);
        }

        if (WifiP2pServiceImpl::DBG)
            Slogger::D("netId related with %s = %d", address.string(), netId);

        if (netId >= 0) {
            // Invoke the persistent group.
            if (mWifiNative->P2pReinvoke(netId, address)) {
                // Save network id. It'll be used when an invitation result event is received.
                config->SetNetId(netId);
                return TRUE;
            }
            else {
                Loge("p2pReinvoke() failed, update networks");
                UpdatePersistentNetworks(RELOAD);
                return FALSE;
            }
        }
    }

    return FALSE;
}

Int32 WifiP2pServiceImpl::P2pStateMachine::GetNetworkIdFromClientList(
    /* [in] */ const String& deviceAddress)
{
    if (deviceAddress == NULL) return -1;

    AutoPtr<ArrayOf<IInterface*> > groups;
    AutoPtr<ICollection> list;
    mGroups->GetGroupList((ICollection**)&list);
    list->ToArray((ArrayOf<IInterface*>**)&groups);
    if (groups != NULL) {
        for(Int32 i = 0; i < groups->GetLength(); i++) {
            AutoPtr<IInterface> obj = (*groups)[i];
            IWifiP2pGroup* wifiP2pGroup = IWifiP2pGroup::Probe(obj);
            Int32 netId;
            wifiP2pGroup->GetNetworkId(&netId);
            AutoPtr<ArrayOf<String> > p2pClientList = GetClientList(netId);
            if (p2pClientList == NULL) continue;
            for (Int32 j = 0; j < p2pClientList->GetLength(); j++) {
                if (deviceAddress.EqualsIgnoreCase((*p2pClientList)[j])) {
                    return netId;
                }
            }
        }
    }

    return -1;
}

AutoPtr<ArrayOf<String> > WifiP2pServiceImpl::P2pStateMachine::GetClientList(
    /* [in] */ Int32 netId)
{
    String p2pClients = mWifiNative->GetNetworkVariable(netId, String("p2p_client_list"));
    if (p2pClients.IsNull()) {
        return NULL;
    }

    AutoPtr< ArrayOf<String> > clients;
    StringUtils::Split(p2pClients, String(" "), (ArrayOf<String>**)&clients);
    return clients;
}

Boolean WifiP2pServiceImpl::P2pStateMachine::RemoveClientFromList(
    /* [in] */ Int32 netId,
    /* [in] */ const String& addr,
    /* [in] */ Boolean isRemovable)
{
    StringBuilder modifiedClientList;
    AutoPtr<ArrayOf<String> > currentClientList = GetClientList(netId);
    Boolean isClientRemoved = FALSE;
    if (currentClientList != NULL) {
        for (Int32 j = 0; j < currentClientList->GetLength(); j++) {
            if (!addr.EqualsIgnoreCase((*currentClientList)[j])) {
                modifiedClientList += " ";
                modifiedClientList += (*currentClientList)[j];
            }
            else {
                isClientRemoved = TRUE;
            }
        }
    }

    if (modifiedClientList.GetLength() == 0 && isRemovable) {
        // the client list is empty. so remove it.
        if (WifiP2pServiceImpl::DBG) Logd("Remove unknown network");
        mGroups->Remove(netId);
        return TRUE;
    }

    if (!isClientRemoved) {
        // specified p2p client is not found. already removed.
        return FALSE;
    }

    if (WifiP2pServiceImpl::DBG)
        Slogger::D(WifiP2pServiceImpl::TAG, "Modified client list: %s",
            modifiedClientList.ToString().string());
    if (modifiedClientList.GetLength() == 0) {
        modifiedClientList += "\"\"";
    }
    String str;
    modifiedClientList.ToString(&str);
    mWifiNative->SetNetworkVariable(netId, String("p2p_client_list"), str);
    mWifiNative->SaveConfig();
    return TRUE;
}

ECode WifiP2pServiceImpl::P2pStateMachine::SetWifiP2pInfoOnGroupFormation(
    /* [in] */ IInetAddress* serverInetAddress)
{
    mWifiP2pInfo->SetGroupFormed(TRUE);
    Boolean b;
    mGroup->IsGroupOwner(&b);
    mWifiP2pInfo->SetIsGroupOwner(b);
    mWifiP2pInfo->SetGroupOwnerAddress(serverInetAddress);
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::ResetWifiP2pInfo()
{
    mWifiP2pInfo->SetGroupFormed(FALSE);
    mWifiP2pInfo->SetIsGroupOwner(FALSE);
    mWifiP2pInfo->SetGroupOwnerAddress(NULL);
    return NOERROR;
}

String WifiP2pServiceImpl::P2pStateMachine::GetDeviceName(
    /* [in] */ const String& deviceAddress)
{
    AutoPtr<IWifiP2pDevice> d;
    mPeers->Get(deviceAddress, (IWifiP2pDevice**)&d);
    if (d != NULL) {
        String name;
        d->GetDeviceName(&name);
        if (name.Equals(String(""))) {
            return deviceAddress;
        }
        return name;
    }
    //Treat the address as name if there is no match
    return deviceAddress;
}

String WifiP2pServiceImpl::P2pStateMachine::GetPersistedDeviceName()
{
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    String deviceName;
    Settings::Global::GetString(cr, ISettingsGlobal::WIFI_P2P_DEVICE_NAME, &deviceName);
    if (deviceName.IsNull()) {
        AutoPtr<IResources> res;
        mHost->mContext->GetResources((IResources**)&res);
        String str;
        res->GetString(R::string::def_wifi_direct_name, &str);
        if (!TextUtils::IsEmpty(str)) {
            return str;
        }
        /* We use the 4 digits of the ANDROID_ID to have a friendly
         * default that has low likelihood of collision with a peer */
        String id;
        Settings::Secure::GetString(cr, ISettingsSecure::ANDROID_ID, &id);
        return String("Android_") + id.Substring(0,4);
    }
    return deviceName;
}

Boolean WifiP2pServiceImpl::P2pStateMachine::SetAndPersistDeviceName(
    /* [in] */ const String& devName)
{
    if (devName.IsNull()) return FALSE;

    Boolean b = mWifiNative->SetDeviceName(devName);
    if (!b) {
        Slogger::E(WifiP2pServiceImpl::TAG, "Failed to set device name %s", devName.string());
        return FALSE;
    }

    mHost->mThisDevice->SetDeviceName(devName);
    String name;
    mHost->mThisDevice->GetDeviceName(&name);
    mWifiNative->SetP2pSsidPostfix(String("-") + name);

    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    Boolean result;
    Settings::Global::PutString(cr, ISettingsGlobal::WIFI_P2P_DEVICE_NAME, devName, &result);
    SendThisDeviceChangedBroadcast();
    return TRUE;
}

Boolean WifiP2pServiceImpl::P2pStateMachine::SetWfdInfo(
    /* [in] */ IWifiP2pWfdInfo* wfdInfo)
{
    if (WifiP2pServiceImpl::DBG) {
        String info;
        IObject::Probe(wfdInfo)->ToString(&info);
        Slogger::D(WifiP2pServiceImpl::TAG, "P2pStateMachine::SetWfdInfo: %s", info.string());
    }

    Boolean success;
    Boolean b;
    wfdInfo->IsWfdEnabled(&b);
    if (!b) {
        success = mWifiNative->SetWfdEnable(FALSE);
    }
    else {
        success = mWifiNative->SetWfdEnable(TRUE);
        if (success) {
            String info;
            wfdInfo->GetDeviceInfoHex(&info);
            success = mWifiNative->SetWfdDeviceInfo(info);
            if (WifiP2pServiceImpl::DBG) {
                Slogger::D(WifiP2pServiceImpl::TAG, "P2pStateMachine::SetWfdDeviceInfo: %s", info.string());
            }
        }
    }

    if (!success) {
        Slogger::E(WifiP2pServiceImpl::TAG, "Failed to set wfd properties");
        return FALSE;
    }

    mHost->mThisDevice->SetWfdInfo(wfdInfo);
    SendThisDeviceChangedBroadcast();
    return TRUE;
}

ECode WifiP2pServiceImpl::P2pStateMachine::InitializeP2pSettings()
{
    mWifiNative->SetPersistentReconnect(TRUE);
    mHost->mThisDevice->SetDeviceName(GetPersistedDeviceName());
    String name, type;
    mHost->mThisDevice->GetDeviceName(&name);
    mWifiNative->SetDeviceName(name);
    mHost->mThisDevice->GetPrimaryDeviceType(&type);
    // DIRECT-XY-DEVICENAME (XY is randomly generated)
    mWifiNative->SetP2pSsidPostfix(String("-") + name);
    mWifiNative->SetDeviceType(type);
    // Supplicant defaults to using virtual display with display
    // which refers to a remote display. Use physical_display
    mWifiNative->SetConfigMethods(String("virtual_push_button physical_display keypad"));
    // STA has higher priority over P2P
    mWifiNative->SetConcurrencyPriority(String("sta"));

    String addr = mWifiNative->P2pGetDeviceAddress();
    mHost->mThisDevice->SetDeviceAddress(addr);
    UpdateThisDevice(IWifiP2pDevice::AVAILABLE);
    if (WifiP2pServiceImpl::DBG) {
        mHost->mThisDevice->GetDeviceAddress(&addr);
        Slogger::D(WifiP2pServiceImpl::TAG, "DeviceAddress: %s", addr.string());
    }

    mHost->mClientInfoList.Clear();
    mWifiNative->P2pFlush();
    mWifiNative->P2pServiceFlush();
    mHost->mServiceTransactionId = 0;
    mHost->mServiceDiscReqId = NULL;

    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    String countryCode;
    Settings::Global::GetString(cr, ISettingsGlobal::WIFI_COUNTRY_CODE, &countryCode);

    if (!countryCode.IsNullOrEmpty()) {
        mHost->mP2pStateMachine->SendMessage(SET_COUNTRY_CODE, TO_IINTERFACE(CoreUtils::Convert(countryCode)));
    }

    UpdatePersistentNetworks(RELOAD);
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::UpdateThisDevice(
    /* [in] */ Int32 status)
{
    mHost->mThisDevice->SetStatus(status);
    SendThisDeviceChangedBroadcast();
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::HandleGroupCreationFailure()
{
    ResetWifiP2pInfo();
    mHost->mNetworkInfo->SetDetailedState(NetworkInfoDetailedState_FAILED, String(NULL), String(NULL));
    SendP2pConnectionChangedBroadcast();
    // Remove only the peer we failed to connect to so that other devices discovered
    // that have not timed out still remain in list for connection
    Boolean peersChanged;
    mPeers->Remove(mPeersLostDuringConnection, &peersChanged);
    String deviceAddress;
    mSavedPeerConfig->GetDeviceAddress(&deviceAddress);
    AutoPtr<IWifiP2pDevice> dev;
    if (!deviceAddress.IsNull() &&
            (mPeers->Remove(deviceAddress, (IWifiP2pDevice**)&dev), dev) != NULL) {
        peersChanged = TRUE;
    }
    if (peersChanged) {
        SendPeersChangedBroadcast();
    }

    Boolean b;
    mPeersLostDuringConnection->Clear(&b);
    mHost->mServiceDiscReqId = NULL;
    SendMessage(IWifiP2pManager::DISCOVER_PEERS);
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::HandleGroupRemoved()
{
    Boolean b;
    mGroup->IsGroupOwner(&b);
    String name;
    mGroup->GetInterface(&name);
    if (b) {
        StopDhcpServer(name);
    }
    else {
        if (WifiP2pServiceImpl::DBG) Logd("stop DHCP client");
        IStateMachine::Probe(mHost->mDhcpStateMachine)->SendMessage(IDhcpStateMachine::CMD_STOP_DHCP);
        mHost->mDhcpStateMachine->DoQuit();
        mHost->mDhcpStateMachine = NULL;

        //try {
        String iface;
        mGroup->GetInterface(&iface);
        ECode ec = mHost->mNwService->RemoveInterfaceFromLocalNetwork(iface);
        if (FAILED(ec)) {
            Loge("Failed to remove iface from local network ");
        }
        //} catch (RemoteException e) {
        //    loge("Failed to remove iface from local network " + e);
        //} catch (IllegalStateException ie) {
        //    loge("Failed to remove iface from local network " + ie);
        ///}
    }

    // try {
    ECode ec = mHost->mNwService->ClearInterfaceAddresses(name);
    if (FAILED(ec)) {
        Slogger::E(WifiP2pServiceImpl::TAG, "Failed to clear addresses %s, ec=%08x", name.string(), ec);
    }
    // } catch (Exception e) {
    //     Loge("Failed to clear addresses " + e);
    // }
    Int32 result;
    NetworkUtils::ResetConnections(name, INetworkUtils::RESET_ALL_ADDRESSES, &result);

    // Clear any timeout that was set. This is essential for devices
    // that reuse the main p2p interface for a created group.
    mWifiNative->SetP2pGroupIdle(name, 0);

    Boolean peersChanged = FALSE;
    // Remove only peers part of the group, so that other devices discovered
    // that have not timed out still remain in list for connection
    AutoPtr<ICollection> clientList;
    mGroup->GetClientList((ICollection**)&clientList);
    Boolean bTemp;
    AutoPtr<ArrayOf<IInterface*> > array;
    clientList->ToArray((ArrayOf<IInterface*>**)&array);
    Int32 size = array->GetLength();
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj = (*array)[i];
        IWifiP2pDevice* d = IWifiP2pDevice::Probe(obj);
        if (mPeers->Remove(d, &bTemp)) peersChanged = TRUE;
    }
    AutoPtr<IWifiP2pDevice> owner;
    mGroup->GetOwner((IWifiP2pDevice**)&owner);
    if (mPeers->Remove(owner, &bTemp), bTemp) peersChanged = TRUE;
    if (mPeers->Remove(mPeersLostDuringConnection, &bTemp), bTemp) peersChanged = TRUE;
    if (peersChanged) {
        SendPeersChangedBroadcast();
    }

    mGroup = NULL;
    mPeersLostDuringConnection->Clear(&b);
    mHost->mServiceDiscReqId = NULL;

    if (mHost->mTemporarilyDisconnectedWifi) {
        mHost->mWifiChannel->SendMessage(WifiP2pServiceImpl::DISCONNECT_WIFI_REQUEST, 0);
        mHost->mTemporarilyDisconnectedWifi = FALSE;
    }
    return NOERROR;
}

//State machine initiated requests can have replyTo set to NULL indicating
//there are no recipients, we ignore those reply actions
ECode WifiP2pServiceImpl::P2pStateMachine::ReplyToMessage(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 what)
{
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    if (replyTo == NULL) return NOERROR;
    AutoPtr<IMessage> dstMsg = ObtainMessage(msg);
    dstMsg->SetWhat(what);
    mHost->mReplyChannel->ReplyToMessage(msg, dstMsg);
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::ReplyToMessage(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1)
{
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    if (replyTo == NULL) return NOERROR;
    AutoPtr<IMessage> dstMsg = ObtainMessage(msg);
    dstMsg->SetWhat(what);
    dstMsg->SetArg1(arg1);
    mHost->mReplyChannel->ReplyToMessage(msg, dstMsg);
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::ReplyToMessage(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    if (replyTo == NULL) return NOERROR;
    AutoPtr<IMessage> dstMsg = ObtainMessage(msg);
    dstMsg->SetWhat(what);
    dstMsg->SetObj(obj);
    mHost->mReplyChannel->ReplyToMessage(msg, dstMsg);
    return NOERROR;
}

/* arg2 on the source message has a hash code that needs to be retained in replies
 * see WifiP2pManager for details */
AutoPtr<IMessage> WifiP2pServiceImpl::P2pStateMachine::ObtainMessage(
    /* [in] */ IMessage* srcMsg)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    Int32 arg2 = 0;
    if (srcMsg != NULL) {
        srcMsg->GetArg2(&arg2);
    }
    msg->SetArg2(arg2);
    return msg;
}

ECode WifiP2pServiceImpl::P2pStateMachine::Logd(
    /* [in] */ const String& s)
{
    Slogger::D("WifiP2pServiceImpl::P2pStateMachine: %s", s.string());
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::Loge(
    /* [in] */ const String& s)
{
    Slogger::E("WifiP2pServiceImpl::P2pStateMachine: %s", s.string());
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::Logd(
    /* [in] */ const char* s)
{
    Slogger::D("WifiP2pServiceImpl::P2pStateMachine: %s", s);
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::Loge(
    /* [in] */ const char* s)
{
    Slogger::E("WifiP2pServiceImpl::P2pStateMachine: %s", s);
    return NOERROR;
}

/**
 * Update service discovery request to wpa_supplicant.
 */
Boolean WifiP2pServiceImpl::P2pStateMachine::UpdateSupplicantServiceRequest()
{
    ClearSupplicantServiceRequest();

    StringBuffer sb;
    HashMap<Int32, AutoPtr<IWifiP2pServiceRequest> >::Iterator ite;
    HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator it;
    for(it = mHost->mClientInfoList.Begin(); it != mHost->mClientInfoList.End(); ++it) {
        AutoPtr<IWifiP2pServiceRequest> req;
        for(ite = it->mSecond->mReqList.Begin(); ite != it->mSecond->mReqList.End(); ++ite) {
            req = ite->mSecond;
            if (req != NULL) {
                String str;
                req->GetSupplicantQuery(&str);
                sb += str;
            }
        }
    }

    if (sb.GetLength() == 0) {
        return FALSE;
    }

    String str = sb.ToString();
    String zero("00:00:00:00:00:00");
    mHost->mServiceDiscReqId = mWifiNative->P2pServDiscReq(zero, str);
    if (mHost->mServiceDiscReqId.IsNull()) {
        return FALSE;
    }
    return TRUE;
}

ECode WifiP2pServiceImpl::P2pStateMachine::ClearSupplicantServiceRequest()
{
    if (mHost->mServiceDiscReqId.IsNull()) return NOERROR;

    mWifiNative->P2pServDiscCancelReq(mHost->mServiceDiscReqId);
    mHost->mServiceDiscReqId = String(NULL);
    return NOERROR;
}

Boolean WifiP2pServiceImpl::P2pStateMachine::AddServiceRequest(
    /* [in] */ IMessenger* m,
    /* [in] */ IWifiP2pServiceRequest* req)
{
    ClearClientDeadChannels();
    AutoPtr<ClientInfo> clientInfo;
    GetClientInfo(m, TRUE, (ClientInfo**)&clientInfo);
    if (clientInfo == NULL) {
        return FALSE;
    }

    ++mHost->mServiceTransactionId;
    //The Wi-Fi p2p spec says transaction id should be non-zero
    if (mHost->mServiceTransactionId == 0) ++mHost->mServiceTransactionId;
    req->SetTransactionId(mHost->mServiceTransactionId);
    typedef typename HashMap<Int32, AutoPtr<IWifiP2pServiceRequest> >::ValueType PairValueType;
    clientInfo->mReqList.Insert(PairValueType(mHost->mServiceTransactionId, req));

    if (mHost->mServiceDiscReqId.IsNull()) {
        return TRUE;
    }

    return UpdateSupplicantServiceRequest();
}

ECode WifiP2pServiceImpl::P2pStateMachine::RemoveServiceRequest(
    /* [in] */ IMessenger* m,
    /* [in] */ IWifiP2pServiceRequest* req)
{
    AutoPtr<ClientInfo> clientInfo;
    GetClientInfo(m, FALSE, (ClientInfo**)&clientInfo);
    if (clientInfo == NULL) {
        return NOERROR;
    }

    //Application does not have transaction id information
    //go through stored requests to remove
    Boolean removed = FALSE;
    HashMap<Int32, AutoPtr<IWifiP2pServiceRequest> >::Iterator it;
    it = clientInfo->mReqList.Begin();
    for(; it != clientInfo->mReqList.End(); ++it) {
        Boolean b;
        IObject::Probe(it->mSecond)->Equals(req, &b);
        if (b) {
            removed = TRUE;
            clientInfo->mReqList.Erase(it);
            break;
        }
    }

    if (!removed) return NOERROR;

    if (clientInfo->mReqList.IsEmpty() && clientInfo->mServList.IsEmpty()) {
        if (WifiP2pServiceImpl::DBG) Logd("remove client information from framework");
        AutoPtr<IMessenger> messenger;
        clientInfo->GetMessenger((IMessenger**)&messenger);
        mHost->mClientInfoList.Erase(messenger);
    }

    if (mHost->mServiceDiscReqId.IsNull()) {
        return NOERROR;
    }

    UpdateSupplicantServiceRequest();
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::ClearServiceRequests(
    /* [in] */ IMessenger* m)
{
    AutoPtr<ClientInfo> clientInfo;
    GetClientInfo(m, FALSE, (ClientInfo**)&clientInfo);
    if (clientInfo == NULL) {
        return NOERROR;
    }

    if (clientInfo->mReqList.IsEmpty()) {
        return NOERROR;
    }

    clientInfo->mReqList.Clear();

    if (clientInfo->mServList.IsEmpty()) {
        if (WifiP2pServiceImpl::DBG) Logd("remove channel information from framework");
        AutoPtr<IMessenger> messenger;
        clientInfo->GetMessenger((IMessenger**)&messenger);
        mHost->mClientInfoList.Erase(messenger);
    }

    if (mHost->mServiceDiscReqId.IsNull()) {
        return NOERROR;
    }

    UpdateSupplicantServiceRequest();
    return NOERROR;
}

Boolean WifiP2pServiceImpl::P2pStateMachine::AddLocalService(
    /* [in] */ IMessenger* m,
    /* [in] */ IWifiP2pServiceInfo* servInfo)
{
    ClearClientDeadChannels();
    AutoPtr<ClientInfo> clientInfo;
    GetClientInfo(m, TRUE, (ClientInfo**)&clientInfo);
    if (clientInfo == NULL) {
        return FALSE;
    }

    clientInfo->mServList.PushBack(servInfo);

    if (!mWifiNative->P2pServiceAdd(servInfo)) {
        clientInfo->mServList.Remove(servInfo);
        return FALSE;
    }

    return TRUE;
}

ECode WifiP2pServiceImpl::P2pStateMachine::RemoveLocalService(
    /* [in] */ IMessenger* m,
    /* [in] */ IWifiP2pServiceInfo* servInfo)
{
    AutoPtr<ClientInfo> clientInfo;
    GetClientInfo(m, FALSE, (ClientInfo**)&clientInfo);
    if (clientInfo == NULL) {
        return NOERROR;
    }

    mWifiNative->P2pServiceDel(servInfo);

    AutoPtr<IWifiP2pServiceInfo> s = servInfo;
    List<AutoPtr<IWifiP2pServiceInfo> >::Iterator it =
        Find(clientInfo->mServList.Begin(), clientInfo->mServList.End(), s);
    if (it != clientInfo->mServList.End())
        clientInfo->mServList.Erase(it);

    if (clientInfo->mReqList.IsEmpty() && clientInfo->mServList.IsEmpty()) {
        if (WifiP2pServiceImpl::DBG) Logd("remove client information from framework");
        AutoPtr<IMessenger> messenger;
        clientInfo->GetMessenger((IMessenger**)&messenger);
        HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator ite;
        ite = mHost->mClientInfoList.Find(messenger);
        if (ite != mHost->mClientInfoList.End())
            mHost->mClientInfoList.Erase(ite);
    }
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::ClearLocalServices(
    /* [in] */ IMessenger* m)
{
    AutoPtr<ClientInfo> clientInfo;
    GetClientInfo(m, FALSE, (ClientInfo**)&clientInfo);
    if (clientInfo == NULL) {
        return NOERROR;
    }

    List<AutoPtr<IWifiP2pServiceInfo> >::Iterator it = clientInfo->mServList.Begin();
    for(; it != clientInfo->mServList.End(); ++it) {
        mWifiNative->P2pServiceDel(*it);
    }

    clientInfo->mServList.Clear();
    if (clientInfo->mReqList.IsEmpty()) {
        if (WifiP2pServiceImpl::DBG) Logd("remove client information from framework");
        AutoPtr<IMessenger> messenger;
        clientInfo->GetMessenger((IMessenger**)&messenger);
        HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator ite;
        ite = mHost->mClientInfoList.Find(messenger);
        if (ite != mHost->mClientInfoList.End())
            mHost->mClientInfoList.Erase(ite);
    }
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::ClearClientInfo(
    /* [in] */ IMessenger* m)
{
    ClearLocalServices(m);
    ClearServiceRequests(m);
    return NOERROR;
}

/**
 * Send the service response to the WifiP2pManager.Channel.
 *
 * @param resp
 */
ECode WifiP2pServiceImpl::P2pStateMachine::SendServiceResponse(
    /* [in] */ IWifiP2pServiceResponse* resp)
{
    ECode ec = NOERROR;
    Int32 id;
    resp->GetTransactionId(&id);

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator it;
    it = mHost->mClientInfoList.Begin();
    for (; it != mHost->mClientInfoList.End(); ++it) {
        HashMap<Int32, AutoPtr<IWifiP2pServiceRequest> >::Iterator ite;
        ite = it->mSecond->mReqList.Find(id);
        if (ite != it->mSecond->mReqList.End()) {
            AutoPtr<IMessage> msg;
            helper->Obtain((IMessage**)&msg);
            msg->SetWhat(IWifiP2pManager::RESPONSE_SERVICE);
            msg->SetArg1(0);
            msg->SetArg2(0);
            msg->SetObj(ite->mSecond.Get());
            ec = it->mSecond->mMessenger->Send(msg);
            if (FAILED(ec)) {
                if (WifiP2pServiceImpl::DBG) Logd("detect dead channel");
                return ClearClientInfo(it->mSecond->mMessenger);
            }
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::ClearClientDeadChannels()
{
    ECode ec = NOERROR;
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    List<AutoPtr<IMessenger> > deadClients;
    HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator it;
    it = mHost->mClientInfoList.Begin();
    for(; it != mHost->mClientInfoList.End(); ++it) {
        AutoPtr<IMessage> msg;
        helper->Obtain((IMessage**)&msg);
        msg->SetWhat(IWifiP2pManager::RESPONSE_SERVICE);
        msg->SetArg1(0);
        msg->SetArg2(0);
        msg->SetObj(NULL);
        ec = it->mSecond->mMessenger->Send(msg);
        if(FAILED(ec)) {
            if (WifiP2pServiceImpl::DBG) Logd("detect dead channel");
            deadClients.PushBack(it->mSecond->mMessenger);
        }
    }

    List<AutoPtr<IMessenger> >::Iterator ite = deadClients.Begin();
    for(; ite != deadClients.End(); ++ite) {
        ClearClientInfo(*ite);
    }

    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::GetClientInfo(
    /* [in] */ IMessenger* m,
    /* [in] */ Boolean createIfNotExist,
    /* [out] */ ClientInfo** result)
{
    AutoPtr<ClientInfo> clientInfo;
    HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator it;
    it = mHost->mClientInfoList.Find(m);
    if (it == mHost->mClientInfoList.End() && createIfNotExist) {
        if (WifiP2pServiceImpl::DBG) Logd("add a new client");
        clientInfo = new ClientInfo(m, mHost);
        AutoPtr<IMessenger> msger = m;
        typedef typename HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::ValueType PairValueType;
        mHost->mClientInfoList.Insert(PairValueType(msger, clientInfo));
    }
    else {
        clientInfo = it->mSecond;
    }

    *result = clientInfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

String WifiP2pServiceImpl::P2pStateMachine::CmdToString(
    /* [in] */ Int32 what)
{
    switch (what) {
        case AsyncChannel::CMD_CHANNEL_HALF_CONNECTED: return String("CMD_CHANNEL_HALF_CONNECTED");
        case AsyncChannel::CMD_CHANNEL_FULL_CONNECTION: return String("CMD_CHANNEL_FULL_CONNECTION");
        case AsyncChannel::CMD_CHANNEL_FULLY_CONNECTED: return String("CMD_CHANNEL_FULLY_CONNECTED");
        case AsyncChannel::CMD_CHANNEL_DISCONNECT: return String("CMD_CHANNEL_DISCONNECT");
        case AsyncChannel::CMD_CHANNEL_DISCONNECTED: return String("CMD_CHANNEL_DISCONNECTED");

        case IWifiP2pManager::DISCOVER_PEERS: return String("DISCOVER_PEERS");
        case IWifiP2pManager::DISCOVER_PEERS_FAILED: return String("DISCOVER_PEERS_FAILED");
        case IWifiP2pManager::DISCOVER_PEERS_SUCCEEDED: return String("DISCOVER_PEERS_SUCCEEDED");
        case IWifiP2pManager::STOP_DISCOVERY: return String("STOP_DISCOVERY");
        case IWifiP2pManager::STOP_DISCOVERY_FAILED: return String("STOP_DISCOVERY_FAILED");
        case IWifiP2pManager::STOP_DISCOVERY_SUCCEEDED: return String("STOP_DISCOVERY_SUCCEEDED");
        case IWifiP2pManager::CONNECT: return String("CONNECT");
        case IWifiP2pManager::CONNECT_FAILED: return String("CONNECT_FAILED");
        case IWifiP2pManager::CONNECT_SUCCEEDED: return String("CONNECT_SUCCEEDED");
        case IWifiP2pManager::CANCEL_CONNECT: return String("CANCEL_CONNECT");
        case IWifiP2pManager::CANCEL_CONNECT_FAILED: return String("CANCEL_CONNECT_FAILED");
        case IWifiP2pManager::CANCEL_CONNECT_SUCCEEDED: return String("CANCEL_CONNECT_SUCCEEDED");
        case IWifiP2pManager::CREATE_GROUP: return String("CREATE_GROUP");
        case IWifiP2pManager::CREATE_GROUP_FAILED: return String("CREATE_GROUP_FAILED");
        case IWifiP2pManager::CREATE_GROUP_SUCCEEDED: return String("CREATE_GROUP_SUCCEEDED");
        case IWifiP2pManager::REMOVE_GROUP: return String("REMOVE_GROUP");
        case IWifiP2pManager::REMOVE_GROUP_FAILED: return String("REMOVE_GROUP_FAILED");
        case IWifiP2pManager::REMOVE_GROUP_SUCCEEDED: return String("REMOVE_GROUP_SUCCEEDED");
        case IWifiP2pManager::REQUEST_PEERS: return String("REQUEST_PEERS");
        case IWifiP2pManager::RESPONSE_PEERS: return String("RESPONSE_PEERS");
        case IWifiP2pManager::REQUEST_CONNECTION_INFO: return String("REQUEST_CONNECTION_INFO");
        case IWifiP2pManager::RESPONSE_CONNECTION_INFO: return String("RESPONSE_CONNECTION_INFO");
        case IWifiP2pManager::REQUEST_GROUP_INFO: return String("REQUEST_GROUP_INFO");
        case IWifiP2pManager::RESPONSE_GROUP_INFO: return String("RESPONSE_GROUP_INFO");
        case IWifiP2pManager::ADD_LOCAL_SERVICE: return String("ADD_LOCAL_SERVICE");
        case IWifiP2pManager::ADD_LOCAL_SERVICE_FAILED: return String("ADD_LOCAL_SERVICE_FAILED");
        case IWifiP2pManager::ADD_LOCAL_SERVICE_SUCCEEDED: return String("ADD_LOCAL_SERVICE_SUCCEEDED");
        case IWifiP2pManager::REMOVE_LOCAL_SERVICE: return String("REMOVE_LOCAL_SERVICE");
        case IWifiP2pManager::REMOVE_LOCAL_SERVICE_FAILED: return String("REMOVE_LOCAL_SERVICE_FAILED");
        case IWifiP2pManager::REMOVE_LOCAL_SERVICE_SUCCEEDED: return String("REMOVE_LOCAL_SERVICE_SUCCEEDED");
        case IWifiP2pManager::CLEAR_LOCAL_SERVICES: return String("CLEAR_LOCAL_SERVICES");
        case IWifiP2pManager::CLEAR_LOCAL_SERVICES_FAILED: return String("CLEAR_LOCAL_SERVICES_FAILED");
        case IWifiP2pManager::CLEAR_LOCAL_SERVICES_SUCCEEDED: return String("CLEAR_LOCAL_SERVICES_SUCCEEDED");
        case IWifiP2pManager::ADD_SERVICE_REQUEST: return String("ADD_SERVICE_REQUEST");
        case IWifiP2pManager::ADD_SERVICE_REQUEST_FAILED: return String("ADD_SERVICE_REQUEST_FAILED");
        case IWifiP2pManager::ADD_SERVICE_REQUEST_SUCCEEDED: return String("ADD_SERVICE_REQUEST_SUCCEEDED");
        case IWifiP2pManager::REMOVE_SERVICE_REQUEST: return String("REMOVE_SERVICE_REQUEST");
        case IWifiP2pManager::REMOVE_SERVICE_REQUEST_FAILED: return String("REMOVE_SERVICE_REQUEST_FAILED");
        case IWifiP2pManager::REMOVE_SERVICE_REQUEST_SUCCEEDED: return String("REMOVE_SERVICE_REQUEST_SUCCEEDED");
        case IWifiP2pManager::CLEAR_SERVICE_REQUESTS: return String("CLEAR_SERVICE_REQUESTS");
        case IWifiP2pManager::CLEAR_SERVICE_REQUESTS_FAILED: return String("CLEAR_SERVICE_REQUESTS_FAILED");
        case IWifiP2pManager::CLEAR_SERVICE_REQUESTS_SUCCEEDED: return String("CLEAR_SERVICE_REQUESTS_SUCCEEDED");
        case IWifiP2pManager::DISCOVER_SERVICES: return String("DISCOVER_SERVICES");
        case IWifiP2pManager::DISCOVER_SERVICES_FAILED: return String("DISCOVER_SERVICES_FAILED");
        case IWifiP2pManager::DISCOVER_SERVICES_SUCCEEDED: return String("DISCOVER_SERVICES_SUCCEEDED");
        case IWifiP2pManager::PING: return String("PING");
        case IWifiP2pManager::RESPONSE_SERVICE: return String("RESPONSE_SERVICE");
        case IWifiP2pManager::SET_DEVICE_NAME: return String("SET_DEVICE_NAME");
        case IWifiP2pManager::SET_DEVICE_NAME_FAILED: return String("SET_DEVICE_NAME_FAILED");
        case IWifiP2pManager::SET_DEVICE_NAME_SUCCEEDED: return String("SET_DEVICE_NAME_SUCCEEDED");
        //case IWifiP2pManager::SET_DIALOG_LISTENER: return String("SET_DIALOG_LISTENER");
        //case IWifiP2pManager::DIALOG_LISTENER_DETACHED: return String("DIALOG_LISTENER_DETACHED");
        //case IWifiP2pManager::DIALOG_LISTENER_ATTACHED: return String("DIALOG_LISTENER_ATTACHED");
        //case IWifiP2pManager::CONNECTION_REQUESTED: return String("CONNECTION_REQUESTED");
        //case IWifiP2pManager::SHOW_PIN_REQUESTED: return String("SHOW_PIN_REQUESTED");
        case IWifiP2pManager::DELETE_PERSISTENT_GROUP: return String("DELETE_PERSISTENT_GROUP");
        case IWifiP2pManager::DELETE_PERSISTENT_GROUP_FAILED: return String("DELETE_PERSISTENT_GROUP_FAILED");
        case IWifiP2pManager::DELETE_PERSISTENT_GROUP_SUCCEEDED: return String("DELETE_PERSISTENT_GROUP_SUCCEEDED");
        case IWifiP2pManager::REQUEST_PERSISTENT_GROUP_INFO: return String("REQUEST_PERSISTENT_GROUP_INFO");
        case IWifiP2pManager::RESPONSE_PERSISTENT_GROUP_INFO: return String("RESPONSE_PERSISTENT_GROUP_INFO");
        case IWifiP2pManager::SET_WFD_INFO: return String("SET_WFD_INFO");
        case IWifiP2pManager::SET_WFD_INFO_FAILED: return String("SET_WFD_INFO_FAILED");
        case IWifiP2pManager::SET_WFD_INFO_SUCCEEDED: return String("SET_WFD_INFO_SUCCEEDED");
        //case IWifiP2pManager::SET_GO_PSK: return String("SET_GO_PSK");
        //case IWifiP2pManager::SET_GO_PSK_FAILED: return String("SET_GO_PSK_FAILED");
        //case IWifiP2pManager::SET_GO_PSK_SUCCEEDED: return String("SET_GO_PSK_SUCCEEDED");

        case WifiMonitor::SUP_CONNECTION_EVENT: return String("SUP_CONNECTION_EVENT");
        case WifiMonitor::SUP_DISCONNECTION_EVENT: return String("SUP_DISCONNECTION_EVENT");
        case WifiMonitor::NETWORK_CONNECTION_EVENT: return String("NETWORK_CONNECTION_EVENT");
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT: return String("NETWORK_DISCONNECTION_EVENT");
        case WifiMonitor::SCAN_RESULTS_EVENT: return String("SCAN_RESULTS_EVENT");
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT: return String("SUPPLICANT_STATE_CHANGE_EVENT");
        case WifiMonitor::AUTHENTICATION_FAILURE_EVENT: return String("AUTHENTICATION_FAILURE_EVENT");
        case WifiMonitor::WPS_SUCCESS_EVENT: return String("WPS_SUCCESS_EVENT");
        case WifiMonitor::WPS_FAIL_EVENT: return String("WPS_FAIL_EVENT");
        case WifiMonitor::WPS_OVERLAP_EVENT: return String("WPS_OVERLAP_EVENT");
        case WifiMonitor::WPS_TIMEOUT_EVENT: return String("WPS_TIMEOUT_EVENT");
        case WifiMonitor::DRIVER_HUNG_EVENT: return String("DRIVER_HUNG_EVENT");
        case WifiMonitor::P2P_DEVICE_FOUND_EVENT: return String("P2P_DEVICE_FOUND_EVENT");
        case WifiMonitor::P2P_DEVICE_LOST_EVENT: return String("P2P_DEVICE_LOST_EVENT");
        case WifiMonitor::P2P_GO_NEGOTIATION_REQUEST_EVENT: return String("P2P_GO_NEGOTIATION_REQUEST_EVENT");
        case WifiMonitor::P2P_GO_NEGOTIATION_SUCCESS_EVENT: return String("P2P_GO_NEGOTIATION_SUCCESS_EVENT");
        case WifiMonitor::P2P_GO_NEGOTIATION_FAILURE_EVENT: return String("P2P_GO_NEGOTIATION_FAILURE_EVENT");
        case WifiMonitor::P2P_GROUP_FORMATION_SUCCESS_EVENT: return String("P2P_GROUP_FORMATION_SUCCESS_EVENT");
        case WifiMonitor::P2P_GROUP_FORMATION_FAILURE_EVENT: return String("P2P_GROUP_FORMATION_FAILURE_EVENT");
        case WifiMonitor::P2P_GROUP_STARTED_EVENT: return String("P2P_GROUP_STARTED_EVENT");
        case WifiMonitor::P2P_GROUP_REMOVED_EVENT: return String("P2P_GROUP_REMOVED_EVENT");
        case WifiMonitor::P2P_INVITATION_RECEIVED_EVENT: return String("P2P_INVITATION_RECEIVED_EVENT");
        case WifiMonitor::P2P_INVITATION_RESULT_EVENT: return String("P2P_INVITATION_RESULT_EVENT");
        case WifiMonitor::P2P_PROV_DISC_PBC_REQ_EVENT: return String("P2P_PROV_DISC_PBC_REQ_EVENT");
        case WifiMonitor::P2P_PROV_DISC_PBC_RSP_EVENT: return String("P2P_PROV_DISC_PBC_RSP_EVENT");
        case WifiMonitor::P2P_PROV_DISC_ENTER_PIN_EVENT: return String("P2P_PROV_DISC_ENTER_PIN_EVENT");
        case WifiMonitor::P2P_PROV_DISC_SHOW_PIN_EVENT: return String("P2P_PROV_DISC_SHOW_PIN_EVENT");
        case WifiMonitor::P2P_FIND_STOPPED_EVENT: return String("P2P_FIND_STOPPED_EVENT");
        case WifiMonitor::P2P_SERV_DISC_RESP_EVENT: return String("P2P_SERV_DISC_RESP_EVENT");
        case WifiMonitor::P2P_PROV_DISC_FAILURE_EVENT: return String("P2P_PROV_DISC_FAILURE_EVENT");
        case WifiMonitor::AP_STA_DISCONNECTED_EVENT: return String("AP_STA_DISCONNECTED_EVENT");
        case WifiMonitor::AP_STA_CONNECTED_EVENT: return String("AP_STA_CONNECTED_EVENT");

        //case WifiStateMachine::CMD_LOAD_DRIVER: return String("CMD_LOAD_DRIVER");
        //case WifiStateMachine::CMD_UNLOAD_DRIVER: return String("CMD_UNLOAD_DRIVER");
        //case WifiStateMachine::CMD_LOAD_DRIVER_SUCCESS: return String("CMD_LOAD_DRIVER_SUCCESS");
        //case WifiStateMachine::CMD_LOAD_DRIVER_FAILURE: return String("CMD_LOAD_DRIVER_FAILURE");
        //case WifiStateMachine::CMD_UNLOAD_DRIVER_SUCCESS: return String("CMD_UNLOAD_DRIVER_SUCCESS");
        //case WifiStateMachine::CMD_UNLOAD_DRIVER_FAILURE: return String("CMD_UNLOAD_DRIVER_FAILURE");
        case WifiStateMachine::CMD_START_SUPPLICANT: return String("CMD_START_SUPPLICANT");
        case WifiStateMachine::CMD_STOP_SUPPLICANT: return String("CMD_STOP_SUPPLICANT");
        case WifiStateMachine::CMD_START_DRIVER: return String("CMD_START_DRIVER");
        case WifiStateMachine::CMD_STOP_DRIVER: return String("CMD_STOP_DRIVER");
        case WifiStateMachine::CMD_STATIC_IP_SUCCESS: return String("CMD_STATIC_IP_SUCCESS");
        case WifiStateMachine::CMD_STATIC_IP_FAILURE: return String("CMD_STATIC_IP_FAILURE");
        case WifiStateMachine::CMD_STOP_SUPPLICANT_FAILED: return String("CMD_STOP_SUPPLICANT_FAILED");
        case WifiStateMachine::CMD_DELAYED_STOP_DRIVER: return String("CMD_DELAYED_STOP_DRIVER");
        case WifiStateMachine::CMD_DRIVER_START_TIMED_OUT: return String("CMD_DRIVER_START_TIMED_OUT");
        //case WifiStateMachine::CMD_CAPTIVE_CHECK_COMPLETE: return String("CMD_CAPTIVE_CHECK_COMPLETE");
        case WifiStateMachine::CMD_START_AP: return String("CMD_START_AP");
        case WifiStateMachine::CMD_START_AP_SUCCESS: return String("CMD_START_AP_SUCCESS");
        case WifiStateMachine::CMD_START_AP_FAILURE: return String("CMD_START_AP_FAILURE");
        case WifiStateMachine::CMD_STOP_AP: return String("CMD_STOP_AP");
        case WifiStateMachine::CMD_SET_AP_CONFIG: return String("CMD_SET_AP_CONFIG");
        case WifiStateMachine::CMD_SET_AP_CONFIG_COMPLETED: return String("CMD_SET_AP_CONFIG_COMPLETED");
        case WifiStateMachine::CMD_REQUEST_AP_CONFIG: return String("CMD_REQUEST_AP_CONFIG");
        case WifiStateMachine::CMD_RESPONSE_AP_CONFIG: return String("CMD_RESPONSE_AP_CONFIG");
        case WifiStateMachine::CMD_TETHER_STATE_CHANGE: return String("CMD_TETHER_STATE_CHANGE");
        case WifiStateMachine::CMD_TETHER_NOTIFICATION_TIMED_OUT: return String("CMD_TETHER_NOTIFICATION_TIMED_OUT");
        case WifiStateMachine::CMD_BLUETOOTH_ADAPTER_STATE_CHANGE: return String("CMD_BLUETOOTH_ADAPTER_STATE_CHANGE");
        case WifiStateMachine::CMD_PING_SUPPLICANT: return String("CMD_PING_SUPPLICANT");
        case WifiStateMachine::CMD_ADD_OR_UPDATE_NETWORK: return String("CMD_ADD_OR_UPDATE_NETWORK");
        case WifiStateMachine::CMD_REMOVE_NETWORK: return String("CMD_REMOVE_NETWORK");
        case WifiStateMachine::CMD_ENABLE_NETWORK: return String("CMD_ENABLE_NETWORK");
        case WifiStateMachine::CMD_ENABLE_ALL_NETWORKS: return String("CMD_ENABLE_ALL_NETWORKS");
        case WifiStateMachine::CMD_BLACKLIST_NETWORK: return String("CMD_BLACKLIST_NETWORK");
        case WifiStateMachine::CMD_CLEAR_BLACKLIST: return String("CMD_CLEAR_BLACKLIST");
        case WifiStateMachine::CMD_SAVE_CONFIG: return String("CMD_SAVE_CONFIG");
        case WifiStateMachine::CMD_GET_CONFIGURED_NETWORKS: return String("CMD_GET_CONFIGURED_NETWORKS");
        case WifiStateMachine::CMD_START_SCAN: return String("CMD_START_SCAN");
        //case WifiStateMachine::CMD_SET_SCAN_MODE: return String("CMD_SET_SCAN_MODE");
        //case WifiStateMachine::CMD_SET_SCAN_TYPE: return String("CMD_SET_SCAN_TYPE");
        case WifiStateMachine::CMD_DISCONNECT: return String("CMD_DISCONNECT");
        case WifiStateMachine::CMD_RECONNECT: return String("CMD_RECONNECT");
        case WifiStateMachine::CMD_REASSOCIATE: return String("CMD_REASSOCIATE");
        case WifiStateMachine::CMD_SET_HIGH_PERF_MODE: return String("CMD_SET_HIGH_PERF_MODE");
        case WifiStateMachine::CMD_SET_COUNTRY_CODE: return String("CMD_SET_COUNTRY_CODE");
        case WifiStateMachine::CMD_ENABLE_RSSI_POLL: return String("CMD_ENABLE_RSSI_POLL");
        case WifiStateMachine::CMD_RSSI_POLL: return String("CMD_RSSI_POLL");
        case WifiStateMachine::CMD_START_PACKET_FILTERING: return String("CMD_START_PACKET_FILTERING");
        case WifiStateMachine::CMD_STOP_PACKET_FILTERING: return String("CMD_STOP_PACKET_FILTERING");
        case WifiStateMachine::CMD_SET_SUSPEND_OPT_ENABLED: return String("CMD_SET_SUSPEND_OPT_ENABLED");
        case WifiStateMachine::CMD_NO_NETWORKS_PERIODIC_SCAN: return String("CMD_NO_NETWORKS_PERIODIC_SCAN");
        case WifiStateMachine::CMD_SET_FREQUENCY_BAND: return String("CMD_SET_FREQUENCY_BAND");
        //case WifiStateMachine::CMD_ENABLE_BACKGROUND_SCAN: return String("CMD_ENABLE_BACKGROUND_SCAN");
        case WifiStateMachine::CMD_RESET_SUPPLICANT_STATE: return String("CMD_RESET_SUPPLICANT_STATE");
        case WifiStateMachine::CMD_ENABLE_P2P: return String("CMD_ENABLE_P2P");
        case WifiStateMachine::CMD_DISABLE_P2P_REQ: return String("CMD_DISABLE_P2P_REQ");
        case WifiStateMachine::CMD_DISABLE_P2P_RSP: return String("CMD_DISABLE_P2P_RSP");

        case WifiP2pServiceImpl::GROUP_CREATING_TIMED_OUT: return String("GROUP_CREATING_TIMED_OUT");
        case WifiP2pServiceImpl::PEER_CONNECTION_USER_ACCEPT: return String("PEER_CONNECTION_USER_ACCEPT");
        case WifiP2pServiceImpl::PEER_CONNECTION_USER_REJECT: return String("PEER_CONNECTION_USER_REJECT");
        case WifiP2pServiceImpl::DROP_WIFI_USER_ACCEPT: return String("DROP_WIFI_USER_ACCEPT");
        case WifiP2pServiceImpl::DROP_WIFI_USER_REJECT: return String("DROP_WIFI_USER_REJECT");
        case WifiP2pServiceImpl::DISABLE_P2P_TIMED_OUT: return String("DISABLE_P2P_TIMED_OUT");
        case WifiP2pServiceImpl::P2P_CONNECTION_CHANGED: return String("P2P_CONNECTION_CHANGED");
        case WifiP2pServiceImpl::DISCONNECT_WIFI_REQUEST: return String("DISCONNECT_WIFI_REQUEST");
        case WifiP2pServiceImpl::DISCONNECT_WIFI_RESPONSE: return String("DISCONNECT_WIFI_RESPONSE");
    }
    return String("unknown cmd.");
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::DefaultState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::DefaultState::DefaultState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::DefaultState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what, arg1;
    message->GetWhat(&what);
    message->GetArg1(&arg1);
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    AutoPtr<IMessenger> replyTo;
    message->GetReplyTo((IMessenger**)&replyTo);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
        case AsyncChannel::CMD_CHANNEL_HALF_CONNECTED:
            if (arg1 == AsyncChannel::STATUS_SUCCESSFUL) {
                if (WifiP2pServiceImpl::DBG)
                    Slogger::D(WifiP2pServiceImpl::TAG, "Full connection with WifiStateMachine established");
                mHost->mHost->mWifiChannel = (AsyncChannel*)IAsyncChannel::Probe(obj);
            }
            else {
                Slogger::E(WifiP2pServiceImpl::TAG, "Full connection failure, error = %d", arg1);
                mHost->mHost->mWifiChannel = NULL;
            }
            break;

        case AsyncChannel::CMD_CHANNEL_DISCONNECTED:
            if (arg1 == AsyncChannel::STATUS_SEND_UNSUCCESSFUL) {
                Slogger::E(WifiP2pServiceImpl::TAG, "Send failed, client connection lost");
            }
            else {
                Slogger::E(WifiP2pServiceImpl::TAG, "Client connection lost with reason: %d", arg1);
            }
            mHost->mHost->mWifiChannel = NULL;
            break;

        case AsyncChannel::CMD_CHANNEL_FULL_CONNECTION: {
            AutoPtr<AsyncChannel> ac = new AsyncChannel();
            AutoPtr<IHandler> handler;
            mHost->GetHandler((IHandler**)&handler);
            ac->Connect(mHost->mHost->mContext, handler, replyTo);
            break;
        }
        case BLOCK_DISCOVERY: {
            mHost->mHost->mDiscoveryBlocked = (arg1 == ENABLED ? TRUE : FALSE);
            // always reset this - we went to a state that doesn't support discovery so
            // it would have stopped regardless
            mHost->mHost->mDiscoveryPostponed = FALSE;
            if (mHost->mHost->mDiscoveryBlocked) {
                //try {
                AutoPtr<IStateMachine> m = IStateMachine::Probe(obj);
                Int32 arg2;
                message->GetArg2(&arg2);
                m->SendMessage(arg2);
                //} catch (Exception e) {
                //    loge("unable to send BLOCK_DISCOVERY response: " + e);
                //}
            }
            break;
        }
        case IWifiP2pManager::DISCOVER_PEERS:
            mHost->ReplyToMessage(message, IWifiP2pManager::DISCOVER_PEERS_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::STOP_DISCOVERY:
            mHost->ReplyToMessage(message, IWifiP2pManager::STOP_DISCOVERY_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::DISCOVER_SERVICES:
            mHost->ReplyToMessage(message, IWifiP2pManager::DISCOVER_SERVICES_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::CONNECT:
            mHost->ReplyToMessage(message, IWifiP2pManager::CONNECT_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::CANCEL_CONNECT:
            mHost->ReplyToMessage(message, IWifiP2pManager::CANCEL_CONNECT_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::CREATE_GROUP:
            mHost->ReplyToMessage(message, IWifiP2pManager::CREATE_GROUP_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::REMOVE_GROUP:
            mHost->ReplyToMessage(message, IWifiP2pManager::REMOVE_GROUP_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::ADD_LOCAL_SERVICE:
            mHost->ReplyToMessage(message, IWifiP2pManager::ADD_LOCAL_SERVICE_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::REMOVE_LOCAL_SERVICE:
            mHost->ReplyToMessage(message, IWifiP2pManager::REMOVE_LOCAL_SERVICE_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::CLEAR_LOCAL_SERVICES:
            mHost->ReplyToMessage(message, IWifiP2pManager::CLEAR_LOCAL_SERVICES_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::ADD_SERVICE_REQUEST:
            mHost->ReplyToMessage(message, IWifiP2pManager::ADD_SERVICE_REQUEST_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::REMOVE_SERVICE_REQUEST:
            mHost->ReplyToMessage(message,
                    IWifiP2pManager::REMOVE_SERVICE_REQUEST_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::CLEAR_SERVICE_REQUESTS:
            mHost->ReplyToMessage(message,
                    IWifiP2pManager::CLEAR_SERVICE_REQUESTS_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::SET_DEVICE_NAME:
            mHost->ReplyToMessage(message, IWifiP2pManager::SET_DEVICE_NAME_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::DELETE_PERSISTENT_GROUP:
            mHost->ReplyToMessage(message, IWifiP2pManager::DELETE_PERSISTENT_GROUP,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::SET_WFD_INFO:
            mHost->ReplyToMessage(message, IWifiP2pManager::SET_WFD_INFO_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::REQUEST_PEERS: {
            AutoPtr<IWifiP2pDeviceList> list;
            CWifiP2pDeviceList::New(mHost->mPeers, (IWifiP2pDeviceList**)&list);
            mHost->ReplyToMessage(message, IWifiP2pManager::RESPONSE_PEERS, list);
            break;
        }
        case IWifiP2pManager::REQUEST_CONNECTION_INFO: {
            AutoPtr<IWifiP2pInfo> info;
            CWifiP2pInfo::New(mHost->mWifiP2pInfo, (IWifiP2pInfo**)&info);
            mHost->ReplyToMessage(message, IWifiP2pManager::RESPONSE_CONNECTION_INFO, info);
            break;
        }
        case IWifiP2pManager::REQUEST_GROUP_INFO: {
            AutoPtr<IWifiP2pGroup> group;
            if (mHost->mGroup != NULL)
                CWifiP2pGroup::New(mHost->mGroup, (IWifiP2pGroup**)&group);
            mHost->ReplyToMessage(message, IWifiP2pManager::RESPONSE_GROUP_INFO, group);
            break;
        }
        case IWifiP2pManager::REQUEST_PERSISTENT_GROUP_INFO: {
            AutoPtr<IWifiP2pGroupList> groupList;
            CWifiP2pGroupList::New(mHost->mGroups, NULL, (IWifiP2pGroupList**)&groupList);
            mHost->ReplyToMessage(message, IWifiP2pManager::RESPONSE_PERSISTENT_GROUP_INFO, groupList);
            break;
        }
        case IWifiP2pManager::START_WPS: {
            mHost->ReplyToMessage(message, IWifiP2pManager::START_WPS_FAILED,
            IWifiP2pManager::BUSY);
            break;
        }
        case IWifiP2pManager::GET_HANDOVER_REQUEST:
        case IWifiP2pManager::GET_HANDOVER_SELECT:
            mHost->ReplyToMessage(message, IWifiP2pManager::RESPONSE_GET_HANDOVER_MESSAGE, (IInterface*)NULL);
            break;
        case IWifiP2pManager::INITIATOR_REPORT_NFC_HANDOVER:
        case IWifiP2pManager::RESPONDER_REPORT_NFC_HANDOVER:
            mHost->ReplyToMessage(message, IWifiP2pManager::REPORT_NFC_HANDOVER_FAILED,
            IWifiP2pManager::BUSY);
            break;
        case WifiMonitor::P2P_INVITATION_RESULT_EVENT:
        case WifiMonitor::SCAN_RESULTS_EVENT:
        case WifiMonitor::SUP_CONNECTION_EVENT:
        case WifiMonitor::SUP_DISCONNECTION_EVENT:
        case WifiMonitor::NETWORK_CONNECTION_EVENT:
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT:
        case WifiMonitor::SUPPLICANT_STATE_CHANGE_EVENT:
        case WifiMonitor::AUTHENTICATION_FAILURE_EVENT:
        case WifiMonitor::WPS_SUCCESS_EVENT:
        case WifiMonitor::WPS_FAIL_EVENT:
        case WifiMonitor::WPS_OVERLAP_EVENT:
        case WifiMonitor::WPS_TIMEOUT_EVENT:
        case WifiMonitor::P2P_GROUP_REMOVED_EVENT:
        case WifiMonitor::P2P_DEVICE_FOUND_EVENT:
        case WifiMonitor::P2P_DEVICE_LOST_EVENT:
        case WifiMonitor::P2P_FIND_STOPPED_EVENT:
        case WifiMonitor::P2P_SERV_DISC_RESP_EVENT:
        case PEER_CONNECTION_USER_ACCEPT:
        case PEER_CONNECTION_USER_REJECT:
        case DISCONNECT_WIFI_RESPONSE:
        case DROP_WIFI_USER_ACCEPT:
        case DROP_WIFI_USER_REJECT:
        case GROUP_CREATING_TIMED_OUT:
        case DISABLE_P2P_TIMED_OUT:
        case IDhcpStateMachine::CMD_PRE_DHCP_ACTION:
        case IDhcpStateMachine::CMD_POST_DHCP_ACTION:
        case IDhcpStateMachine::CMD_ON_QUIT:
        case WifiMonitor::P2P_PROV_DISC_FAILURE_EVENT:
        case SET_MIRACAST_MODE:
        case IWifiP2pManager::START_LISTEN:
        case IWifiP2pManager::STOP_LISTEN:
        case IWifiP2pManager::SET_CHANNEL:
        case SET_COUNTRY_CODE:
            break;
         case WifiStateMachine::CMD_ENABLE_P2P:
            // Enable is lazy and has no response
            break;
         case WifiStateMachine::CMD_DISABLE_P2P_REQ:
            // If we end up handling in default, p2p is not enabled
            mHost->mHost->mWifiChannel->SendMessage(WifiStateMachine::CMD_DISABLE_P2P_RSP);
            break;
            /* unexpected group created, remove */
        case WifiMonitor::P2P_GROUP_STARTED_EVENT: {
            mHost->mGroup = IWifiP2pGroup::Probe(obj);
            Slogger::E(WifiP2pServiceImpl::TAG, "Unexpected group creation, remove %p", mHost->mGroup.Get());
            String interfaceName;
            mHost->mGroup->GetInterface(&interfaceName);
            mHost->mWifiNative->P2pGroupRemove(interfaceName);
            break;
        }
        // A group formation failure is always followed by
        // a group removed event. Flushing things at group formation
        // failure causes supplicant issues. Ignore right now.
        case WifiMonitor::P2P_GROUP_FORMATION_FAILURE_EVENT:
            break;
        default:
            Slogger::E(WifiP2pServiceImpl::TAG, "Unhandled message %d, %s", what, P2pStateMachine::CmdToString(what).string());
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }

    *result = TRUE;  // HANDLED;
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::P2pNotSupportedState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::P2pNotSupportedState::P2pNotSupportedState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::P2pNotSupportedState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
       case IWifiP2pManager::DISCOVER_PEERS:
            mHost->ReplyToMessage(message, IWifiP2pManager::DISCOVER_PEERS_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::STOP_DISCOVERY:
            mHost->ReplyToMessage(message, IWifiP2pManager::STOP_DISCOVERY_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::DISCOVER_SERVICES:
            mHost->ReplyToMessage(message, IWifiP2pManager::DISCOVER_SERVICES_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::CONNECT:
            mHost->ReplyToMessage(message, IWifiP2pManager::CONNECT_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::CANCEL_CONNECT:
            mHost->ReplyToMessage(message, IWifiP2pManager::CANCEL_CONNECT_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
       case IWifiP2pManager::CREATE_GROUP:
            mHost->ReplyToMessage(message, IWifiP2pManager::CREATE_GROUP_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::REMOVE_GROUP:
            mHost->ReplyToMessage(message, IWifiP2pManager::REMOVE_GROUP_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::ADD_LOCAL_SERVICE:
            mHost->ReplyToMessage(message, IWifiP2pManager::ADD_LOCAL_SERVICE_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::REMOVE_LOCAL_SERVICE:
            mHost->ReplyToMessage(message, IWifiP2pManager::REMOVE_LOCAL_SERVICE_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::CLEAR_LOCAL_SERVICES:
            mHost->ReplyToMessage(message, IWifiP2pManager::CLEAR_LOCAL_SERVICES_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::ADD_SERVICE_REQUEST:
            mHost->ReplyToMessage(message, IWifiP2pManager::ADD_SERVICE_REQUEST_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::REMOVE_SERVICE_REQUEST:
            mHost->ReplyToMessage(message,
                    IWifiP2pManager::REMOVE_SERVICE_REQUEST_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::CLEAR_SERVICE_REQUESTS:
            mHost->ReplyToMessage(message,
                    IWifiP2pManager::CLEAR_SERVICE_REQUESTS_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::SET_DEVICE_NAME:
            mHost->ReplyToMessage(message, IWifiP2pManager::SET_DEVICE_NAME_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::DELETE_PERSISTENT_GROUP:
            mHost->ReplyToMessage(message, IWifiP2pManager::DELETE_PERSISTENT_GROUP,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::SET_WFD_INFO:
            mHost->ReplyToMessage(message, IWifiP2pManager::SET_WFD_INFO_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::START_WPS:
            mHost->ReplyToMessage(message, IWifiP2pManager::START_WPS_FAILED,
                IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::START_LISTEN:
            mHost->ReplyToMessage(message, IWifiP2pManager::START_LISTEN_FAILED,
                IWifiP2pManager::P2P_UNSUPPORTED);
            break;
        case IWifiP2pManager::STOP_LISTEN:
            mHost->ReplyToMessage(message, IWifiP2pManager::STOP_LISTEN_FAILED,
                    IWifiP2pManager::P2P_UNSUPPORTED);
            break;
       default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }
    *result = TRUE;  // HANDLED;
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::P2pDisablingState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::P2pDisablingState::P2pDisablingState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::P2pDisablingState::Enter()
{
    if (WifiP2pServiceImpl::DBG)
        Slogger::D(WifiP2pServiceImpl::TAG, " Enter %s", GetName().string());
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(DISABLE_P2P_TIMED_OUT, ++mDisableP2pTimeoutIndex, 0, (IMessage**)&msg);
    mHost->SendMessageDelayed(msg, DISABLE_P2P_WAIT_TIME_MS);
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::P2pDisablingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);
    Int32 arg1;
    message->GetArg1(&arg1);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
        case WifiMonitor::SUP_DISCONNECTION_EVENT:
            if (WifiP2pServiceImpl::DBG) mHost->Logd("p2p socket connection lost");
            mHost->TransitionTo(mHost->mP2pDisabledState);
            break;
        case WifiStateMachine::CMD_ENABLE_P2P:
        case WifiStateMachine::CMD_DISABLE_P2P_REQ:
            mHost->DeferMessage(message);
            break;
        case DISABLE_P2P_TIMED_OUT:
            if (mHost->mHost->mDisableP2pTimeoutIndex == arg1) {
                mHost->Loge("P2p disable timed out");
                mHost->TransitionTo(mHost->mP2pDisabledState);
            }
            break;
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }
    *result = TRUE;  // HANDLED;
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::P2pDisablingState::Exit()
{
    mHost->mHost->mWifiChannel->SendMessage(WifiStateMachine::CMD_DISABLE_P2P_RSP);
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::P2pDisabledState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::P2pDisabledState::P2pDisabledState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::P2pDisabledState::Enter()
{
    if (WifiP2pServiceImpl::DBG) Slogger::D(WifiP2pServiceImpl::TAG, "Enter %s", GetName().string());
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::P2pDisabledState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
        case WifiStateMachine::CMD_ENABLE_P2P: {
            // try {
            ECode ec = mHost->mHost->mNwService->SetInterfaceUp(mHost->mHost->mInterface);
            if (FAILED(ec)) {
                Slogger::E(WifiP2pServiceImpl::TAG, "Unable to change interface settings: %08x", ec);
            }
            // } catch (RemoteException re) {
            //     Loge("Unable to change interface settings: " + re);
            // } catch (IllegalStateException ie) {
            //     Loge("Unable to change interface settings: " + ie);
            // }
            mHost->mWifiMonitor->StartMonitoring();
            mHost->TransitionTo(mHost->mP2pEnablingState);
            break;
        }

        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }
    *result = TRUE;  // HANDLED;
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::P2pEnablingState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::P2pEnablingState::P2pEnablingState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::P2pEnablingState::Enter()
{
    if (WifiP2pServiceImpl::DBG) Slogger::D(WifiP2pServiceImpl::TAG, "Enter %s", GetName().string());
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::P2pEnablingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
        case WifiMonitor::SUP_CONNECTION_EVENT:
            if (WifiP2pServiceImpl::DBG) mHost->Logd("P2p socket connection successful");
            mHost->TransitionTo(mHost->mInactiveState);
            break;
        case WifiMonitor::SUP_DISCONNECTION_EVENT:
            mHost->Loge("P2p socket connection failed");
            mHost->TransitionTo(mHost->mP2pDisabledState);
            break;
        case WifiStateMachine::CMD_ENABLE_P2P:
        case WifiStateMachine::CMD_DISABLE_P2P_REQ:
            mHost->DeferMessage(message);
            break;
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }
    *result = TRUE;  // HANDLED;
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::P2pEnabledState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::P2pEnabledState::P2pEnabledState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::P2pEnabledState::Enter()
{
    if (WifiP2pServiceImpl::DBG) Slogger::D(WifiP2pServiceImpl::TAG, "Enter %s", GetName().string());
    mHost->SendP2pStateChangedBroadcast(TRUE);
    mHost->mHost->mNetworkInfo->SetIsAvailable(TRUE);
    mHost->SendP2pConnectionChangedBroadcast();
    mHost->InitializeP2pSettings();
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::P2pEnabledState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what, arg1;
    message->GetWhat(&what);
    message->GetArg1(&arg1);
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    AutoPtr<IMessenger> replyTo;
    message->GetReplyTo((IMessenger**)&replyTo);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    String addr, devAddr, address;
    String name;
    Boolean b;

    switch (what) {
        case WifiMonitor::SUP_DISCONNECTION_EVENT:
            mHost->Loge("Unexpected loss of p2p socket connection");
            mHost->TransitionTo(mHost->mP2pDisabledState);
            break;
        case WifiStateMachine::CMD_ENABLE_P2P:
            //Nothing to do
            break;
        case WifiStateMachine::CMD_DISABLE_P2P_REQ:
            mHost->mPeers->Clear(&b);
            if (b) mHost->SendPeersChangedBroadcast();
            mHost->mGroups->Clear(&b);
            if (b) mHost->SendP2pPersistentGroupsChangedBroadcast();

            mHost->mWifiMonitor->StopMonitoring();
            mHost->TransitionTo(mHost->mP2pDisablingState);
            break;
        case IWifiP2pManager::SET_DEVICE_NAME:
        {
            AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(obj);
            if (device != NULL) device->GetDeviceName(&name);
            if (device != NULL && mHost->SetAndPersistDeviceName(name)) {
                if (WifiP2pServiceImpl::DBG)
                    Slogger::D(WifiP2pServiceImpl::TAG, "P2pEnabledState set device name %s", name.string());
                mHost->ReplyToMessage(message, IWifiP2pManager::SET_DEVICE_NAME_SUCCEEDED);
            }
            else {
                mHost->ReplyToMessage(message, IWifiP2pManager::SET_DEVICE_NAME_FAILED,
                    IWifiP2pManager::ERROR);
            }
            break;
        }
        case IWifiP2pManager::SET_WFD_INFO:
        {
            AutoPtr<IWifiP2pWfdInfo> wfdInfo = IWifiP2pWfdInfo::Probe(obj);

            if (WifiP2pServiceImpl::DBG) {
                String temp("NULL");
                if (wfdInfo) IObject::Probe(wfdInfo)->ToString(&temp);
                String info("P2pEnabledState set WFD info:");
                info += temp;
                mHost->Logd(info.string());
            }

            if (wfdInfo != NULL && mHost->SetWfdInfo(wfdInfo)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::SET_WFD_INFO_SUCCEEDED);
            }
            else {
                mHost->ReplyToMessage(message, IWifiP2pManager::SET_WFD_INFO_FAILED,
                    IWifiP2pManager::ERROR);
            }
            break;
        }
        case BLOCK_DISCOVERY:
        {
            Boolean blocked = (arg1 == ENABLED ? TRUE : FALSE);
            if (mHost->mHost->mDiscoveryBlocked == blocked) break;
            mHost->mHost->mDiscoveryBlocked = blocked;
            if (blocked && mHost->mHost->mDiscoveryStarted) {
                mHost->mWifiNative->P2pStopFind();
                mHost->mHost->mDiscoveryPostponed = TRUE;
            }
            if (!blocked && mHost->mHost->mDiscoveryPostponed) {
                mHost->mHost->mDiscoveryPostponed = FALSE;
                mHost->mWifiNative->P2pFind(DISCOVER_TIMEOUT_S);
            }
            if (blocked) {
                //try {
                AutoPtr<IStateMachine> m = IStateMachine::Probe(obj);
                Int32 arg2;
                message->GetArg2(&arg2);
                m->SendMessage(arg2);
                //} catch (Exception e) {
                //    loge("unable to send BLOCK_DISCOVERY response: " + e);
                //}
            }
            break;
        }
        case IWifiP2pManager::DISCOVER_PEERS:
            if (mHost->mHost->mDiscoveryBlocked) {
                mHost->ReplyToMessage(message, IWifiP2pManager::DISCOVER_PEERS_FAILED,
                IWifiP2pManager::BUSY);
                break;
            }
            // do not send service discovery request while normal find operation.
            mHost->ClearSupplicantServiceRequest();
            if (mHost->mWifiNative->P2pFind(DISCOVER_TIMEOUT_S)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::DISCOVER_PEERS_SUCCEEDED);
                mHost->SendP2pDiscoveryChangedBroadcast(TRUE);
            }
            else {
                mHost->ReplyToMessage(message, IWifiP2pManager::DISCOVER_PEERS_FAILED,
                    IWifiP2pManager::ERROR);
            }
            break;
        case WifiMonitor::P2P_FIND_STOPPED_EVENT:
            mHost->SendP2pDiscoveryChangedBroadcast(FALSE);
            break;
        case IWifiP2pManager::STOP_DISCOVERY:
            if (mHost->mWifiNative->P2pStopFind()) {
                mHost->ReplyToMessage(message, IWifiP2pManager::STOP_DISCOVERY_SUCCEEDED);
            } else {
                mHost->ReplyToMessage(message, IWifiP2pManager::STOP_DISCOVERY_FAILED,
                    IWifiP2pManager::ERROR);
            }
            break;
        case IWifiP2pManager::DISCOVER_SERVICES:
            if (mHost->mHost->mDiscoveryBlocked) {
                mHost->ReplyToMessage(message, IWifiP2pManager::DISCOVER_SERVICES_FAILED,
                IWifiP2pManager::BUSY);
                break;
            }
            if (WifiP2pServiceImpl::DBG) mHost->Logd("P2pEnabledState discover services");
            if (!mHost->UpdateSupplicantServiceRequest()) {
                mHost->ReplyToMessage(message, IWifiP2pManager::DISCOVER_SERVICES_FAILED,
                    IWifiP2pManager::NO_SERVICE_REQUESTS);
                break;
            }
            if (mHost->mWifiNative->P2pFind(DISCOVER_TIMEOUT_S)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::DISCOVER_SERVICES_SUCCEEDED);
            }
            else {
                mHost->ReplyToMessage(message, IWifiP2pManager::DISCOVER_SERVICES_FAILED,
                    IWifiP2pManager::ERROR);
            }
            break;
        case WifiMonitor::P2P_DEVICE_FOUND_EVENT: {
            AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(obj);
            assert(device != NULL);

            if (WifiP2pServiceImpl::DBG) {
                String temp;
                IObject::Probe(device)->ToString(&temp);
                String info("P2pEnabledState found device:\n");
                info += temp;
                mHost->Logd(info.string());
            }

            mHost->mHost->mThisDevice->GetDeviceAddress(&addr);
            device->GetDeviceAddress(&devAddr);
            if (addr.Equals(devAddr)) break;
            mHost->mPeers->UpdateSupplicantDetails(device);
            mHost->SendPeersChangedBroadcast();
            break;
        }
        case WifiMonitor::P2P_DEVICE_LOST_EVENT: {
            AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(obj);
            AutoPtr<IWifiP2pDevice> d2;
            String deviceName;
            device->GetDeviceName(&deviceName);
            mHost->mPeers->Remove(deviceName, (IWifiP2pDevice**)&d2);
            if (d2 != NULL) mHost->SendPeersChangedBroadcast();
            break;
        }
        case IWifiP2pManager::ADD_LOCAL_SERVICE: {
            if (WifiP2pServiceImpl::DBG) mHost->Logd("P2pEnabledState add service");
            AutoPtr<IWifiP2pServiceInfo> servInfo = IWifiP2pServiceInfo::Probe(obj);
            if (mHost->AddLocalService(replyTo, servInfo)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::ADD_LOCAL_SERVICE_SUCCEEDED);
            } else {
                mHost->ReplyToMessage(message, IWifiP2pManager::ADD_LOCAL_SERVICE_FAILED);
            }
            break;
        }
        case IWifiP2pManager::REMOVE_LOCAL_SERVICE: {
            if (WifiP2pServiceImpl::DBG) mHost->Logd("P2pEnabledState remove service");
            AutoPtr<IWifiP2pServiceInfo> servInfo = IWifiP2pServiceInfo::Probe(obj);
            mHost->RemoveLocalService(replyTo, servInfo);
            mHost->ReplyToMessage(message, IWifiP2pManager::REMOVE_LOCAL_SERVICE_SUCCEEDED);
            break;
        }
        case IWifiP2pManager::CLEAR_LOCAL_SERVICES:
            if (WifiP2pServiceImpl::DBG) mHost->Logd("P2pEnabledState clear service");
            mHost->ClearLocalServices(replyTo);
            mHost->ReplyToMessage(message, IWifiP2pManager::CLEAR_LOCAL_SERVICES_SUCCEEDED);
            break;
        case IWifiP2pManager::ADD_SERVICE_REQUEST: {
            if (WifiP2pServiceImpl::DBG) mHost->Logd("P2pEnabledState add service request");
            AutoPtr<IWifiP2pServiceRequest> request = IWifiP2pServiceRequest::Probe(obj);
            if (!mHost->AddServiceRequest(replyTo, request)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::ADD_SERVICE_REQUEST_FAILED);
                break;
            }
            mHost->ReplyToMessage(message, IWifiP2pManager::ADD_SERVICE_REQUEST_SUCCEEDED);
            break;
        }
        case IWifiP2pManager::REMOVE_SERVICE_REQUEST: {
            if (WifiP2pServiceImpl::DBG) mHost->Logd("P2pEnabledState remove service request");
            AutoPtr<IWifiP2pServiceRequest> request = IWifiP2pServiceRequest::Probe(obj);
            mHost->RemoveServiceRequest(replyTo, request);
            mHost->ReplyToMessage(message, IWifiP2pManager::REMOVE_SERVICE_REQUEST_SUCCEEDED);
            break;
        }
        case IWifiP2pManager::CLEAR_SERVICE_REQUESTS:
            if (WifiP2pServiceImpl::DBG) mHost->Logd("P2pEnabledState clear service request");
            mHost->ClearServiceRequests(replyTo);
            mHost->ReplyToMessage(message, IWifiP2pManager::CLEAR_SERVICE_REQUESTS_SUCCEEDED);
            break;
        case WifiMonitor::P2P_SERV_DISC_RESP_EVENT: {
            if (WifiP2pServiceImpl::DBG) mHost->Logd("P2pEnabledState receive service response");
            AutoPtr<IList> container = IList::Probe(obj);
            assert(obj != NULL);
            Int32 size;
            container->GetSize(&size);
            AutoPtr<IWifiP2pServiceResponse> resp;
            AutoPtr<IWifiP2pDevice> device;
            for(Int32 i = 0; i < size; ++i) {
                AutoPtr<IInterface> current;
                container->Get(i, (IInterface**)&current);
                resp = IWifiP2pServiceResponse::Probe(current);
                device = NULL;
                resp->GetSrcDevice((IWifiP2pDevice**)&device);
                device->GetDeviceAddress(&address);
                device = NULL;
                mHost->mPeers->Get(address, (IWifiP2pDevice**)&device);
                resp->SetSrcDevice(device);
                mHost->SendServiceResponse(resp);
            }
            break;
        }
        case IWifiP2pManager::DELETE_PERSISTENT_GROUP:
            if (WifiP2pServiceImpl::DBG) mHost->Logd("P2pEnabledState delete persistent group");
            mHost->mGroups->Remove(arg1);
            mHost->ReplyToMessage(message, IWifiP2pManager::DELETE_PERSISTENT_GROUP_SUCCEEDED);
            break;
        case SET_MIRACAST_MODE:
            mHost->mWifiNative->SetMiracastMode(arg1);
            mHost->SendMiracastModeChanged(arg1);
            break;
        case IWifiP2pManager::START_LISTEN:
            if (WifiP2pServiceImpl::DBG) mHost->Logd("P2pEnabledState start listen mode");
            mHost->mWifiNative->P2pFlush();
            if (mHost->mWifiNative->P2pExtListen(TRUE, 500, 500)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::START_LISTEN_SUCCEEDED);
            }
            else {
                mHost->ReplyToMessage(message, IWifiP2pManager::START_LISTEN_FAILED);
            }
            break;
        case IWifiP2pManager::STOP_LISTEN:
            if (WifiP2pServiceImpl::DBG) mHost->Logd("P2pEnabledState stop listen mode");
            if (mHost->mWifiNative->P2pExtListen(FALSE, 0, 0)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::STOP_LISTEN_SUCCEEDED);
            }
            else {
                mHost->ReplyToMessage(message, IWifiP2pManager::STOP_LISTEN_FAILED);
            }
            mHost->mWifiNative->P2pFlush();
            break;
        case IWifiP2pManager::SET_CHANNEL: {
            AutoPtr<IBundle> p2pChannels = IBundle::Probe(obj);
            Int32 lc;
            p2pChannels->GetInt32(String("lc"), 0, &lc);
            Int32 oc;
            p2pChannels->GetInt32(String("oc"), 0, &oc);
            if (WifiP2pServiceImpl::DBG) mHost->Logd("P2pEnabledState set listen and operating channel");
            if (mHost->mWifiNative->P2pSetChannel(lc, oc)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::SET_CHANNEL_SUCCEEDED);
            } else {
                mHost->ReplyToMessage(message, IWifiP2pManager::SET_CHANNEL_FAILED);
            }
            break;
        }
        case SET_COUNTRY_CODE: {
            String countryCode;
            ICharSequence::Probe(obj)->ToString(&countryCode);
            countryCode = countryCode.ToUpperCase(/*Locale.ROOT*/);
            if (mHost->mHost->mLastSetCountryCode == NULL ||
                countryCode.Equals(mHost->mHost->mLastSetCountryCode) == FALSE) {
                if (mHost->mWifiNative->SetCountryCode(countryCode)) {
                    mHost->mHost->mLastSetCountryCode = countryCode;
                }
            }
            break;
        }
        case IWifiP2pManager::GET_HANDOVER_REQUEST: {
            AutoPtr<IBundle> requestBundle;
            CBundle::New((IBundle**)&requestBundle);
            requestBundle->PutString(IWifiP2pManager::EXTRA_HANDOVER_MESSAGE,
                                    mHost->mWifiNative->GetNfcHandoverRequest());
            mHost->ReplyToMessage(message, IWifiP2pManager::RESPONSE_GET_HANDOVER_MESSAGE, TO_IINTERFACE(requestBundle));
            break;
        }
        case IWifiP2pManager::GET_HANDOVER_SELECT: {
            AutoPtr<IBundle> selectBundle;
            CBundle::New((IBundle**)&selectBundle);
            selectBundle->PutString(IWifiP2pManager::EXTRA_HANDOVER_MESSAGE,
                                    mHost->mWifiNative->GetNfcHandoverSelect());
            mHost->ReplyToMessage(message, IWifiP2pManager::RESPONSE_GET_HANDOVER_MESSAGE, TO_IINTERFACE(selectBundle));
            break;
        }
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }
    *result = TRUE;  // HANDLED;
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::P2pEnabledState::Exit()
{
    mHost->SendP2pDiscoveryChangedBroadcast(FALSE);
    mHost->SendP2pStateChangedBroadcast(FALSE);
    mHost->mHost->mNetworkInfo->SetIsAvailable(FALSE);
    mHost->mHost->mLastSetCountryCode = String(NULL);
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::InactiveState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::InactiveState::InactiveState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::InactiveState::Enter()
{
    if (WifiP2pServiceImpl::DBG) Slogger::D(WifiP2pServiceImpl::TAG, "Enter %s", GetName().string());
    mHost->mHost->mIsInvite = FALSE;
    mHost->mSavedPeerConfig->Invalidate();
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::InactiveState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    String addr;
    String devAddr;
    String address;
    String name;
    Int32 id;
    Int32 netId;
    Boolean ret = FALSE;
    Boolean b;
    Boolean b1,b2,b3;

    switch (what) {
        case IWifiP2pManager::CONNECT: {
            if (WifiP2pServiceImpl::DBG) mHost->Logd("InactiveState  sending connect");
            AutoPtr<IWifiP2pConfig> config = IWifiP2pConfig::Probe(obj);

            if (mHost->IsConfigInvalid(config)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::CONNECT_FAILED);
                break;
            }

            mHost->mHost->mAutonomousGroup = FALSE;
            mHost->mWifiNative->P2pStopFind();
            if (mHost->ReinvokePersistentGroup(config)) {
                mHost->TransitionTo(mHost->mGroupNegotiationState);
            } else {
                mHost->TransitionTo(mHost->mProvisionDiscoveryState);
            }
            mHost->mSavedPeerConfig = config;

            mHost->mSavedPeerConfig->GetDeviceAddress(&devAddr);
            mHost->mPeers->UpdateStatus(devAddr, IWifiP2pDevice::INVITED);

            mHost->SendPeersChangedBroadcast();
            mHost->ReplyToMessage(message, IWifiP2pManager::CONNECT_SUCCEEDED);
            break;
        }
        case IWifiP2pManager::STOP_DISCOVERY:
            b = mHost->mWifiNative->P2pStopFind();
            if (b) {
                // When discovery stops in inactive state, flush to clear
                // state peer data
                mHost->mWifiNative->P2pFlush();
                mHost->mHost->mServiceDiscReqId = NULL;
                mHost->ReplyToMessage(message, IWifiP2pManager::STOP_DISCOVERY_SUCCEEDED);
            } else {
                mHost->ReplyToMessage(message, IWifiP2pManager::STOP_DISCOVERY_FAILED,
                    IWifiP2pManager::ERROR);
            }
            break;
        case WifiMonitor::P2P_GO_NEGOTIATION_REQUEST_EVENT: {
            AutoPtr<IWifiP2pConfig> config = IWifiP2pConfig::Probe(obj);
            if (mHost->IsConfigInvalid(config)) {
                mHost->Loge("Dropping GO neg request ");// + config;
                break;
            }
            mHost->mSavedPeerConfig = config;
            mHost->mHost->mAutonomousGroup = FALSE;
            mHost->mHost->mJoinExistingGroup = FALSE;

            mHost->TransitionTo(mHost->mUserAuthorizingNegotiationRequestState);
            break;
        }
        case WifiMonitor::P2P_INVITATION_RECEIVED_EVENT: {
            AutoPtr<IWifiP2pGroup> group = IWifiP2pGroup::Probe(obj);
            AutoPtr<IWifiP2pDevice> owner;
            group->GetOwner((IWifiP2pDevice**)&owner);
            if (owner == NULL) {
                if (WifiP2pServiceImpl::DBG) mHost->Loge("Ignored invitation from NULL owner");
                break;
            }

            AutoPtr<IWifiP2pConfig> config;
            CWifiP2pConfig::New((IWifiP2pConfig**)&config);
            owner->GetDeviceAddress(&address);
            config->SetDeviceAddress(address);
            if (mHost->IsConfigInvalid(config)) {
                mHost->Loge("Dropping invitation request ");// + config;
                break;
            }

            mHost->mSavedPeerConfig = config;

            //Check if we have the owner in peer list and use appropriate
            //wps method. Default is to use PBC.
            owner = NULL;
            mHost->mPeers->Get(address, (IWifiP2pDevice**)&owner);
            if (owner != NULL) {
                owner->WpsPbcSupported(&b1);
                owner->WpsKeypadSupported(&b2);
                owner->WpsDisplaySupported(&b3);
                AutoPtr<IWpsInfo> wps;
                mHost->mSavedPeerConfig->GetWps((IWpsInfo**)&wps);
                if (b1) {
                    wps->SetSetup(IWpsInfo::PBC);
                } else if (b2) {
                    wps->SetSetup(IWpsInfo::KEYPAD);
                } else if (b3) {
                    wps->SetSetup(IWpsInfo::DISPLAY);
                }
            }

            mHost->mHost->mAutonomousGroup = FALSE;
            mHost->mHost->mJoinExistingGroup = TRUE;
            mHost->mHost->mIsInvite = TRUE;

            mHost->TransitionTo(mHost->mUserAuthorizingInviteRequestState);
            break;
        }
        case WifiMonitor::P2P_PROV_DISC_PBC_REQ_EVENT:
        case WifiMonitor::P2P_PROV_DISC_ENTER_PIN_EVENT:
            //We let the supplicant handle the provision discovery response
            //and wait instead for the GO_NEGOTIATION_REQUEST_EVENT.
            //Handling provision discovery and issuing a p2p_connect before
            //group negotiation comes through causes issues
           break;
        case WifiMonitor::P2P_PROV_DISC_SHOW_PIN_EVENT: {
               AutoPtr<IWifiP2pProvDiscEvent> provDisc = IWifiP2pProvDiscEvent::Probe(obj);
               AutoPtr<IWifiP2pDevice> device;
               provDisc->GetDevice((IWifiP2pDevice**)&device);;
               if (device == NULL) {
                   Slogger::D(TAG, "Device entry is null");
                   break;
               }
               String pin;
               provDisc->GetPin(&pin);
               String deviceAddress;
               device->GetDeviceAddress(&deviceAddress);
               mHost->NotifyP2pProvDiscShowPinRequest(pin, deviceAddress);
               mHost->mPeers->UpdateStatus(deviceAddress, IWifiP2pDevice::INVITED);
               mHost->SendPeersChangedBroadcast();
               mHost->TransitionTo(mHost->mGroupNegotiationState);
               break;
        }
        case IWifiP2pManager::CREATE_GROUP:
            mHost->mHost->mAutonomousGroup = TRUE;
            message->GetArg1(&netId);
            if (netId == IWifiP2pGroup::PERSISTENT_NET_ID) {
                // check if the go persistent group is present.
                mHost->mHost->mThisDevice->GetDeviceAddress(&addr);
                mHost->mGroups->GetNetworkId(addr, &netId);
                if (netId != -1) {
                    ret = mHost->mWifiNative->P2pGroupAdd(netId);
                } else {
                    ret = mHost->mWifiNative->P2pGroupAdd(TRUE);
                }
            } else {
                ret = mHost->mWifiNative->P2pGroupAdd(FALSE);
            }

            if (ret) {
                mHost->ReplyToMessage(message, IWifiP2pManager::CREATE_GROUP_SUCCEEDED);
                mHost->TransitionTo(mHost->mGroupNegotiationState);
            } else {
                mHost->ReplyToMessage(message, IWifiP2pManager::CREATE_GROUP_FAILED,
                    IWifiP2pManager::ERROR);
                // remain at this state.
            }
            break;
        case WifiMonitor::P2P_GROUP_STARTED_EVENT:
            mHost->mGroup = IWifiP2pGroup::Probe(obj);
            if (WifiP2pServiceImpl::DBG) mHost->Logd("InactiveState  group started");

            // We hit this scenario when a persistent group is reinvoked
            mHost->mGroup->GetNetworkId(&id);
            if (id == IWifiP2pGroup::PERSISTENT_NET_ID) {
                mHost->mHost->mAutonomousGroup = FALSE;
                mHost->DeferMessage(message);
                mHost->TransitionTo(mHost->mGroupNegotiationState);
            }
            else {
                String info;
                IObject::Probe(mHost->mGroup)->ToString(&info);
                Slogger::E(WifiP2pServiceImpl::TAG, "Unexpected group creation, remove %s", info.string());
                mHost->mGroup->GetInterface(&name);
                mHost->mWifiNative->P2pGroupRemove(name);
            }
            break;
        case IWifiP2pManager::START_LISTEN:
            if (WifiP2pServiceImpl::DBG) mHost->Logd("InactiveState start listen mode");
            mHost->mWifiNative->P2pFlush();
            if (mHost->mWifiNative->P2pExtListen(TRUE, 500, 500)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::START_LISTEN_SUCCEEDED);
            } else {
                mHost->ReplyToMessage(message, IWifiP2pManager::START_LISTEN_FAILED);
            }
            break;
        case IWifiP2pManager::STOP_LISTEN:
            if (WifiP2pServiceImpl::DBG) mHost->Logd("InactiveState stop listen mode");
            if (mHost->mWifiNative->P2pExtListen(FALSE, 0, 0)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::STOP_LISTEN_SUCCEEDED);
            } else {
                mHost->ReplyToMessage(message, IWifiP2pManager::STOP_LISTEN_FAILED);
            }
            mHost->mWifiNative->P2pFlush();
            break;
        case IWifiP2pManager::SET_CHANNEL: {
            AutoPtr<IBundle> p2pChannels = IBundle::Probe(obj);
            Int32 lc;
            p2pChannels->GetInt32(String("lc"), 0, &lc);
            Int32 oc;
            p2pChannels->GetInt32(String("oc"), 0, &oc);
            if (WifiP2pServiceImpl::DBG) mHost->Logd("InactiveState set listen and operating channel");
            if (mHost->mWifiNative->P2pSetChannel(lc, oc)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::SET_CHANNEL_SUCCEEDED);
            } else {
                mHost->ReplyToMessage(message, IWifiP2pManager::SET_CHANNEL_FAILED);
            }
            break;
        }
        case IWifiP2pManager::INITIATOR_REPORT_NFC_HANDOVER: {
            String handoverSelect;

            if (obj != NULL) {
                AutoPtr<IBundle> bundle = IBundle::Probe(obj);
                bundle->GetString(IWifiP2pManager::EXTRA_HANDOVER_MESSAGE, &handoverSelect);
            }
            if (!handoverSelect.IsNull()
                && mHost->mWifiNative->InitiatorReportNfcHandover(handoverSelect)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::REPORT_NFC_HANDOVER_SUCCEEDED);
                mHost->TransitionTo(mHost->mGroupCreatingState);
            } else {
                mHost->ReplyToMessage(message, IWifiP2pManager::REPORT_NFC_HANDOVER_FAILED);
            }
            break;
        }
        case IWifiP2pManager::RESPONDER_REPORT_NFC_HANDOVER: {
            String handoverRequest;
            if (obj != NULL) {
                AutoPtr<IBundle> bundle = IBundle::Probe(obj);
                bundle->GetString(IWifiP2pManager::EXTRA_HANDOVER_MESSAGE, &handoverRequest);
            }
            if (!handoverRequest.IsNull()
                && mHost->mWifiNative->ResponderReportNfcHandover(handoverRequest)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::REPORT_NFC_HANDOVER_SUCCEEDED);
                mHost->TransitionTo(mHost->mGroupCreatingState);
            } else {
                mHost->ReplyToMessage(message, IWifiP2pManager::REPORT_NFC_HANDOVER_FAILED);
            }
            break;
        }
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }
    *result = TRUE;  // HANDLED;
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::GroupCreatingState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::GroupCreatingState::GroupCreatingState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::GroupCreatingState::Enter()
{
    if (WifiP2pServiceImpl::DBG) Slogger::D(WifiP2pServiceImpl::TAG, "Enter %s", GetName().string());
    AutoPtr<IMessage> msg;
    mHost->ObtainMessage(GROUP_CREATING_TIMED_OUT, ++mGroupCreatingTimeoutIndex, 0, (IMessage**)&msg);
    mHost->SendMessageDelayed(msg, GROUP_CREATING_WAIT_TIME_MS);
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::GroupCreatingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Boolean ret = TRUE;  // HANDLED;
    Int32 what, arg1;
    message->GetWhat(&what);
    message->GetArg1(&arg1);
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    String addr;
    String devAddr;

    switch (what) {
        case GROUP_CREATING_TIMED_OUT:
            if (mGroupCreatingTimeoutIndex == arg1) {
                if (WifiP2pServiceImpl::DBG) mHost->Logd("Group negotiation timed out");
                mHost->HandleGroupCreationFailure();
                mHost->TransitionTo(mHost->mInactiveState);
            }
            break;
        case WifiMonitor::P2P_DEVICE_LOST_EVENT: {
            AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(obj);
            device->GetDeviceAddress(&addr);

            if (mHost->mSavedPeerConfig != NULL) {
                mHost->mSavedPeerConfig->GetDeviceAddress(&devAddr);
                if(!devAddr.Equals(addr)) {
                    if (WifiP2pServiceImpl::DBG) {
                        Slogger::D("mHost->mSavedPeerConfig %s, device %s", devAddr.string(), addr.string());
                    }
                    // Do the regular device lost handling
                    ret = FALSE;  // NOT_HANDLED;
                }
            }
            // Do nothing
            if (WifiP2pServiceImpl::DBG) Slogger::D(WifiP2pServiceImpl::TAG, "Add device to lost list %d", device.Get());
            mHost->mPeersLostDuringConnection->UpdateSupplicantDetails(device);
            break;
        }
        case IWifiP2pManager::DISCOVER_PEERS:
            /* Discovery will break negotiation */
            mHost->ReplyToMessage(message, IWifiP2pManager::DISCOVER_PEERS_FAILED,
                    IWifiP2pManager::BUSY);
            break;
        case IWifiP2pManager::CANCEL_CONNECT:
            //Do a supplicant p2p_cancel which only cancels an ongoing
            //group negotiation. This will fail for a pending provision
            //discovery or for a pending user action, but at the framework
            //level, we always treat cancel as succeeded and enter
            //an inactive state
            mHost->mWifiNative->P2pCancelConnect();
            mHost->HandleGroupCreationFailure();
            mHost->TransitionTo(mHost->mInactiveState);
            mHost->ReplyToMessage(message, IWifiP2pManager::CANCEL_CONNECT_SUCCEEDED);
            break;
        case WifiMonitor::P2P_GO_NEGOTIATION_SUCCESS_EVENT:
            // We hit this scenario when NFC handover is invoked.
            mHost->mHost->mAutonomousGroup = FALSE;
            mHost->TransitionTo(mHost->mGroupNegotiationState);
            break;
        default:
            ret = FALSE;  // NOT_HANDLED;
            break;
    }

    *result = ret;
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingNegotiationRequestState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingNegotiationRequestState::UserAuthorizingNegotiationRequestState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingNegotiationRequestState::Enter()
{
    if (WifiP2pServiceImpl::DBG) Slogger::D(WifiP2pServiceImpl::TAG, "Enter %s", GetName().string());
    mHost->NotifyInvitationReceived();
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingNegotiationRequestState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    *result = TRUE;  // HANDLED;
    Int32 what;
    message->GetWhat(&what);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
        case PEER_CONNECTION_USER_ACCEPT: {
            mHost->mWifiNative->P2pStopFind();
            mHost->P2pConnectWithPinDisplay(mHost->mSavedPeerConfig);
            String deviceAddr;
            mHost->mSavedPeerConfig->GetDeviceAddress(&deviceAddr);
            mHost->mPeers->UpdateStatus(deviceAddr, IWifiP2pDevice::INVITED);

            mHost->SendPeersChangedBroadcast();
            mHost->TransitionTo(mHost->mGroupNegotiationState);
            break;
        }
        case PEER_CONNECTION_USER_REJECT:
            if (WifiP2pServiceImpl::DBG) {
                String info;
                if (mHost->mSavedPeerConfig) IObject::Probe(mHost->mSavedPeerConfig)->ToString(&info);
                Slogger::D(WifiP2pServiceImpl::TAG, "User rejected negotiation %s", info.string());
            }
            mHost->TransitionTo(mHost->mInactiveState);
            break;
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingNegotiationRequestState::Exit()
{
    //TODO: dismiss dialog if not already done
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingInviteRequestState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingInviteRequestState::UserAuthorizingInviteRequestState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingInviteRequestState::Enter()
{
    if (WifiP2pServiceImpl::DBG) Slogger::D(WifiP2pServiceImpl::TAG, "Enter %s", GetName().string());
    mHost->NotifyInvitationReceived();
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingInviteRequestState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    *result = TRUE;  // HANDLED;
    Int32 what;
    message->GetWhat(&what);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
        case PEER_CONNECTION_USER_ACCEPT: {
            mHost->mWifiNative->P2pStopFind();
            if (!mHost->ReinvokePersistentGroup(mHost->mSavedPeerConfig)) {
                // Do negotiation when persistence fails
                mHost->P2pConnectWithPinDisplay(mHost->mSavedPeerConfig);
            }

            String deviceAddr;
            mHost->mSavedPeerConfig->GetDeviceAddress(&deviceAddr);
            mHost->mPeers->UpdateStatus(deviceAddr, IWifiP2pDevice::INVITED);

            mHost->SendPeersChangedBroadcast();
            mHost->TransitionTo(mHost->mGroupNegotiationState);
            break;
        }
        case PEER_CONNECTION_USER_REJECT:
            if (WifiP2pServiceImpl::DBG) {
                String info;
                if (mHost->mSavedPeerConfig) IObject::Probe(mHost->mSavedPeerConfig)->ToString(&info);
                Slogger::D(WifiP2pServiceImpl::TAG, "User rejected negotiation %s", info.string());
            }
            mHost->TransitionTo(mHost->mInactiveState);
            break;
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingInviteRequestState::Exit()
{
    //TODO: dismiss dialog if not already done
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::ProvisionDiscoveryState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::ProvisionDiscoveryState::ProvisionDiscoveryState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::ProvisionDiscoveryState::Enter()
{
    if (WifiP2pServiceImpl::DBG) Slogger::D(WifiP2pServiceImpl::TAG, "Enter %s", GetName().string());
    mHost->mWifiNative->P2pProvisionDiscovery(mHost->mSavedPeerConfig);
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::ProvisionDiscoveryState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    String addr, devAddr;
    Int32 setup;
    String pin;

    switch (what) {
        case WifiMonitor::P2P_PROV_DISC_PBC_RSP_EVENT: {
            AutoPtr<IWifiP2pProvDiscEvent> provDisc = IWifiP2pProvDiscEvent::Probe(obj);
            AutoPtr<IWifiP2pDevice> device;
            provDisc->GetDevice((IWifiP2pDevice**)&device);
            device->GetDeviceAddress(&addr);
            mHost->mSavedPeerConfig->GetDeviceAddress(&devAddr);
            if (!addr.Equals(devAddr)) break;

            AutoPtr<IWpsInfo> wps;
            mHost->mSavedPeerConfig->GetWps((IWpsInfo**)&wps);
            wps->GetSetup(&setup);
            if (setup == IWpsInfo::PBC) {
                if (WifiP2pServiceImpl::DBG)
                    Slogger::D(WifiP2pServiceImpl::TAG, "Found a match %p", mHost->mSavedPeerConfig.Get());
                mHost->P2pConnectWithPinDisplay(mHost->mSavedPeerConfig);
                mHost->TransitionTo(mHost->mGroupNegotiationState);
            }
            break;
        }
        case WifiMonitor::P2P_PROV_DISC_ENTER_PIN_EVENT: {
            AutoPtr<IWifiP2pProvDiscEvent> provDisc = IWifiP2pProvDiscEvent::Probe(obj);
            AutoPtr<IWifiP2pDevice> device;
            provDisc->GetDevice((IWifiP2pDevice**)&device);
            device->GetDeviceAddress(&addr);
            mHost->mSavedPeerConfig->GetDeviceAddress(&devAddr);
            if (!addr.Equals(devAddr)) break;

            AutoPtr<IWpsInfo> wps;
            mHost->mSavedPeerConfig->GetWps((IWpsInfo**)&wps);
            wps->GetSetup(&setup);
            if (setup == IWpsInfo::KEYPAD) {
                if (WifiP2pServiceImpl::DBG)
                    Slogger::D(WifiP2pServiceImpl::TAG, "Found a match %p", mHost->mSavedPeerConfig.Get());
                /* we already have the pin */
                wps->GetPin(&pin);
                if (!pin.IsEmpty()) {
                    mHost->P2pConnectWithPinDisplay(mHost->mSavedPeerConfig);
                    mHost->TransitionTo(mHost->mGroupNegotiationState);
                }
                else {
                    mHost->mHost->mJoinExistingGroup = FALSE;
                    mHost->TransitionTo(mHost->mUserAuthorizingNegotiationRequestState);
                }
            }
            break;
        }
        case WifiMonitor::P2P_PROV_DISC_SHOW_PIN_EVENT: {
            AutoPtr<IWifiP2pProvDiscEvent> provDisc = IWifiP2pProvDiscEvent::Probe(obj);
            AutoPtr<IWifiP2pDevice> device;
            provDisc->GetDevice((IWifiP2pDevice**)&device);
            device->GetDeviceAddress(&addr);
            mHost->mSavedPeerConfig->GetDeviceAddress(&devAddr);
            if (!addr.Equals(devAddr)) break;

            AutoPtr<IWpsInfo> wps;
            mHost->mSavedPeerConfig->GetWps((IWpsInfo**)&wps);
            wps->GetSetup(&setup);
            if (setup == IWpsInfo::DISPLAY) {
                if (WifiP2pServiceImpl::DBG)
                    Slogger::D(WifiP2pServiceImpl::TAG, "Found a match %p", mHost->mSavedPeerConfig.Get());
                provDisc->GetPin(&pin);
                wps->SetPin(pin);
                mHost->P2pConnectWithPinDisplay(mHost->mSavedPeerConfig);
                mHost->NotifyInvitationSent(pin, addr);
                mHost->TransitionTo(mHost->mGroupNegotiationState);
            }
            break;
        }
        case WifiMonitor::P2P_PROV_DISC_FAILURE_EVENT:
            mHost->Loge("provision discovery failed");
            mHost->HandleGroupCreationFailure();
            mHost->TransitionTo(mHost->mInactiveState);
            break;
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }
    *result = TRUE;  // HANDLED;
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::GroupNegotiationState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::GroupNegotiationState::GroupNegotiationState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::GroupNegotiationState::Enter()
{
    if (WifiP2pServiceImpl::DBG) Slogger::D(WifiP2pServiceImpl::TAG, "Enter %s", GetName().string());
    mHost->mPendingReformGroupIndication = FALSE;
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::GroupNegotiationState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    String devAddr;
    String netName;
    String name;
    String addr;
    Int32 netId;
    Int32 id;
    Boolean b;

    switch (what) {
        // We ignore these right now, since we get a GROUP_STARTED notification
        // afterwards
        case WifiMonitor::P2P_GO_NEGOTIATION_SUCCESS_EVENT:
        case WifiMonitor::P2P_GROUP_FORMATION_SUCCESS_EVENT:
            if (WifiP2pServiceImpl::DBG) mHost->Logd("GroupNegotiationState go success");
            break;
        // Action of removing and reforming group will be taken
        // when we enter in GroupCreatedState
        case WifiMonitor::P2P_REMOVE_AND_REFORM_GROUP_EVENT:
            mHost->Logd("P2P_REMOVE_AND_REFORM_GROUP_EVENT event received in GroupNegotiationState state");
            mHost->mPendingReformGroupIndication = TRUE;
            break;
        case WifiMonitor::P2P_GROUP_STARTED_EVENT:
            mHost->mGroup = IWifiP2pGroup::Probe(obj);
            if (WifiP2pServiceImpl::DBG) mHost->Logd("GroupNegotiationState group started");
            mHost->mGroup->GetNetworkId(&id);
            if (id == IWifiP2pGroup::PERSISTENT_NET_ID) {
                /*
                 * update cache information and set network id to mGroup.
                 */
                mHost->UpdatePersistentNetworks(NO_RELOAD);
                AutoPtr<IWifiP2pDevice> dev;
                mHost->mGroup->GetOwner((IWifiP2pDevice**)&dev);
                dev->GetDeviceAddress(&devAddr);
                mHost->mGroup->GetNetworkName(&netName);
                mHost->mGroups->GetNetworkId(devAddr, netName, &netId);
                mHost->mGroup->SetNetworkId(netId);
            }

            mHost->mGroup->IsGroupOwner(&b);
            mHost->mGroup->GetInterface(&name);
            if (b) {
                /* Setting an idle time out on GO causes issues with certain scenarios
                 * on clients where it can be off-channel for longer and with the power
                 * save modes used.
                 *
                 * TODO: Verify multi-channel scenarios and supplicant behavior are
                 * better before adding a time out in future
                 */
                //Set group idle timeout of 10 sec, to avoid GO beaconing incase of any
                //failure during 4-way Handshake.
                if (!mHost->mHost->mAutonomousGroup) {
                    mHost->mWifiNative->SetP2pGroupIdle(name, GROUP_IDLE_TIME_S);
                }
                mHost->StartDhcpServer(name);
            }
            else {
                mHost->mWifiNative->SetP2pGroupIdle(name, GROUP_IDLE_TIME_S);
                AutoPtr<IDhcpStateMachineHelper> dhcpsmHelper;
                CDhcpStateMachineHelper::AcquireSingleton((IDhcpStateMachineHelper**)&dhcpsmHelper);
                dhcpsmHelper->MakeDhcpStateMachine(
                    mHost->mHost->mContext, mHost, name, (IDhcpStateMachine**)&(mHost->mHost->mDhcpStateMachine));
                // TODO: We should use DHCP state machine PRE message like WifiStateMachine
                mHost->mWifiNative->SetP2pPowerSave(name, FALSE);
                IStateMachine::Probe(mHost->mHost->mDhcpStateMachine)->SendMessage(IDhcpStateMachine::CMD_START_DHCP);
                AutoPtr<IWifiP2pDevice> groupOwner;
                mHost->mGroup->GetOwner((IWifiP2pDevice**)&groupOwner);
                groupOwner->GetDeviceAddress(&addr);
                AutoPtr<IWifiP2pDevice> peer;
                mHost->mPeers->Get(addr, (IWifiP2pDevice**)&peer);
                if (peer != NULL) {
                    /* update group owner details with the ones found at discovery */
                    groupOwner->UpdateSupplicantDetails(peer);
                    mHost->mPeers->UpdateStatus(addr, IWifiP2pDevice::CONNECTED);
                    mHost->SendPeersChangedBroadcast();
                }
                else {
                    // A supplicant bug can lead to reporting an invalid
                    // group owner address (all zeroes) at times. Avoid a
                    // crash, but continue group creation since it is not
                    // essential.
                    mHost->Logw(String("Unknown group owner "));// + groupOwner;
                }
            }

            mHost->TransitionTo(mHost->mGroupCreatedState);
            break;
        case WifiMonitor::P2P_GO_NEGOTIATION_FAILURE_EVENT: {
            AutoPtr<IInteger32> iobj = IInteger32::Probe(obj);
            assert(obj != NULL);
            Int32 ival;
            iobj->GetValue(&ival);
            P2pStatus status = (P2pStatus)ival;
            if (status == P2pStatus_NO_COMMON_CHANNEL) {
                mHost->TransitionTo(mHost->mFrequencyConflictState);
                break;
            }
            /* continue with group removal handling */
        }
        case WifiMonitor::P2P_GROUP_REMOVED_EVENT:
            if (WifiP2pServiceImpl::DBG) mHost->Logd("GroupNegotiationState go failure");
            mHost->HandleGroupCreationFailure();
            mHost->TransitionTo(mHost->mInactiveState);
            break;
        // A group formation failure is always followed by
        // a group removed event. Flushing things at group formation
        // failure causes supplicant issues. Ignore right now.
        case WifiMonitor::P2P_GROUP_FORMATION_FAILURE_EVENT: {
            AutoPtr<IInteger32> iobj = IInteger32::Probe(obj);
            assert(obj != NULL);
            Int32 ival;
            iobj->GetValue(&ival);
            P2pStatus status = (P2pStatus)ival;

            if (status == P2pStatus_NO_COMMON_CHANNEL) {
                mHost->TransitionTo(mHost->mFrequencyConflictState);
                break;
            }
            break;
        }
        case WifiMonitor::P2P_INVITATION_RESULT_EVENT: {
            AutoPtr<IInteger32> iobj = IInteger32::Probe(obj);
            assert(obj != NULL);
            Int32 ival;
            iobj->GetValue(&ival);
            P2pStatus status = (P2pStatus)ival;
            if (status == P2pStatus_SUCCESS) {
                // invocation was succeeded.
                // wait P2P_GROUP_STARTED_EVENT.
                break;
            }
            mHost->Loge(String("Invitation result %d"));//, status;
            if (status == P2pStatus_UNKNOWN_P2P_GROUP) {
                // target device has already removed the credential.
                // So, remove this credential accordingly.
                mHost->mSavedPeerConfig->GetNetId(&netId);
                if (netId >= 0) {
                    if (WifiP2pServiceImpl::DBG) mHost->Logd("Remove unknown client from the list");
                    mHost->mSavedPeerConfig->GetDeviceAddress(&addr);
                    mHost->RemoveClientFromList(netId, addr, TRUE);
                }

                //Reinvocation has failed, try group negotiation
                mHost->mSavedPeerConfig->SetNetId(IWifiP2pGroup::PERSISTENT_NET_ID);
                mHost->TransitionTo(mHost->mProvisionDiscoveryState);
            }
            else if (status == P2pStatus_INFORMATION_IS_CURRENTLY_UNAVAILABLE) {

                // Devices setting persistent_reconnect to 0 in wpa_supplicant
                // always defer the invocation request and return
                // "information is currently unable" error.
                // So, try another way to connect for interoperability.
                mHost->mSavedPeerConfig->SetNetId(IWifiP2pGroup::PERSISTENT_NET_ID);
                mHost->P2pConnectWithPinDisplay(mHost->mSavedPeerConfig);
            }
            else if (status == P2pStatus_NO_COMMON_CHANNEL) {
                mHost->TransitionTo(mHost->mFrequencyConflictState);
            }
            else {
                mHost->HandleGroupCreationFailure();
                mHost->TransitionTo(mHost->mInactiveState);
            }
            break;
        }
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }
    *result = TRUE;  // HANDLED;
    return NOERROR;
}


//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::PositiveButtonListener
//==================================================================
CAR_INTERFACE_IMPL(WifiP2pServiceImpl::P2pStateMachine::PositiveButtonListener, Object, IDialogInterfaceOnClickListener)

WifiP2pServiceImpl::P2pStateMachine::PositiveButtonListener::PositiveButtonListener(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{}

ECode WifiP2pServiceImpl::P2pStateMachine::PositiveButtonListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mHost->SendMessage(WifiP2pServiceImpl::DROP_WIFI_USER_ACCEPT);
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::NegativeButtonListener
//==================================================================
CAR_INTERFACE_IMPL(WifiP2pServiceImpl::P2pStateMachine::NegativeButtonListener, Object, IDialogInterfaceOnClickListener)

WifiP2pServiceImpl::P2pStateMachine::NegativeButtonListener::NegativeButtonListener(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{}

ECode WifiP2pServiceImpl::P2pStateMachine::NegativeButtonListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mHost->SendMessage(WifiP2pServiceImpl::DROP_WIFI_USER_REJECT);
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::DialogCancelListener
//==================================================================
CAR_INTERFACE_IMPL(WifiP2pServiceImpl::P2pStateMachine::DialogCancelListener, Object, IDialogInterfaceOnCancelListener)

WifiP2pServiceImpl::P2pStateMachine::DialogCancelListener::DialogCancelListener(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{}

ECode WifiP2pServiceImpl::P2pStateMachine::DialogCancelListener::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    mHost->SendMessage(WifiP2pServiceImpl::DROP_WIFI_USER_REJECT);
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::NotifyP2pProvDiscShowPinRequestOnClickListener
//==================================================================
CAR_INTERFACE_IMPL(WifiP2pServiceImpl::P2pStateMachine::NotifyP2pProvDiscShowPinRequestOnClickListener,
        Object, IDialogInterfaceOnClickListener)

WifiP2pServiceImpl::P2pStateMachine::NotifyP2pProvDiscShowPinRequestOnClickListener::NotifyP2pProvDiscShowPinRequestOnClickListener(
    /* [in] */ P2pStateMachine* host,
    /* [in] */ const String& address,
    /* [in] */ const String& pin)
    : mHost(host)
    , mAddress(address)
    , mPin(pin)
{}

ECode WifiP2pServiceImpl::P2pStateMachine::NotifyP2pProvDiscShowPinRequestOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mHost->mSavedPeerConfig = NULL;
    CWifiP2pConfig::New((IWifiP2pConfig**)&mHost->mSavedPeerConfig);
    mHost->mSavedPeerConfig->SetDeviceAddress(mAddress);
    AutoPtr<IWpsInfo> wps;
    mHost->mSavedPeerConfig->GetWps((IWpsInfo**)&wps);
    wps->SetSetup(IWpsInfo::DISPLAY);
    wps->SetPin(mPin);
    mHost->mWifiNative->P2pConnect(mHost->mSavedPeerConfig, FORM_GROUP);
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedPositiveButtonListener
//==================================================================
CAR_INTERFACE_IMPL(WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedPositiveButtonListener,
        Object, IDialogInterfaceOnClickListener)

WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedPositiveButtonListener::NotifyInvitationReceivedPositiveButtonListener(
    /* [in] */ P2pStateMachine* host,
    /* [in] */ IWpsInfo* wpsInfo,
    /* [in] */ IEditText* pin)
    : mHost(host)
    , mWpsInfo(wpsInfo)
    , mPin(pin)
{}

ECode WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedPositiveButtonListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    Int32 setup;
    mWpsInfo->GetSetup(&setup);
    if (setup == IWpsInfo::KEYPAD) {
        AutoPtr<IWpsInfo> wps;
        mHost->mSavedPeerConfig->GetWps((IWpsInfo**)&wps);
        AutoPtr<ICharSequence> cs;
        ITextView::Probe(mPin)->GetText((ICharSequence**)&cs);
        String text;
        cs->ToString(&text);
        wps->SetPin(text);
    }
    //if (WifiP2pServiceImpl::DBG) Logd(getName() + " accept invitation " + mSavedPeerConfig);
    mHost->SendMessage(WifiP2pServiceImpl::PEER_CONNECTION_USER_ACCEPT);
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedNegativeButtonListener
//==================================================================
CAR_INTERFACE_IMPL(WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedNegativeButtonListener,
        Object, IDialogInterfaceOnClickListener)

WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedNegativeButtonListener::NotifyInvitationReceivedNegativeButtonListener(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{}

ECode WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedNegativeButtonListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (WifiP2pServiceImpl::DBG) mHost->Logd("NotifyInvitationReceivedNegativeButtonListener ignore connect");
    mHost->SendMessage(PEER_CONNECTION_USER_REJECT);
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedDialogCancelListener
//==================================================================
CAR_INTERFACE_IMPL(WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedDialogCancelListener,
        Object, IDialogInterfaceOnCancelListener)

WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedDialogCancelListener::NotifyInvitationReceivedDialogCancelListener(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{}

ECode WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedDialogCancelListener::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    if (WifiP2pServiceImpl::DBG) mHost->Logd("NotifyInvitationReceivedDialogCancelListener ignore connect");
    mHost->SendMessage(PEER_CONNECTION_USER_REJECT);
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedDialogKeyListener
//==================================================================
CAR_INTERFACE_IMPL(WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedDialogKeyListener,
        Object, IDialogInterfaceOnKeyListener)

WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedDialogKeyListener::NotifyInvitationReceivedDialogKeyListener(
    /* [in] */ P2pStateMachine* host,
    /* [in] */ IAlertDialog* dialog)
    : mHost(host)
    , mDialog(dialog)
{}

ECode WifiP2pServiceImpl::P2pStateMachine::NotifyInvitationReceivedDialogKeyListener::OnKey(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* consumed)
{
    VALIDATE_NOT_NULL(consumed);
    // TODO: make the actual key come from a config value.
    if (keyCode == IKeyEvent::KEYCODE_VOLUME_MUTE) {
        mHost->SendMessage(PEER_CONNECTION_USER_ACCEPT);
        IDialogInterface::Probe(mDialog)->Dismiss();
        *consumed = TRUE;
        return NOERROR;
    }
    *consumed = FALSE;
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::FrequencyConflictState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::FrequencyConflictState::FrequencyConflictState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::FrequencyConflictState::Enter()
{
    if (WifiP2pServiceImpl::DBG) Slogger::D(WifiP2pServiceImpl::TAG, "Enter %s", GetName().string());
    NotifyFrequencyConflict();
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::FrequencyConflictState::NotifyFrequencyConflict()
{
    mHost->Logd("Notify frequency conflict");
    AutoPtr<IResourcesHelper> resHelper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&resHelper);
    AutoPtr<IResources> r;
    resHelper->GetSystem((IResources**)&r);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(mHost->mHost->mContext, (IAlertDialogBuilder**)&builder);

    String addr, str;
    mHost->mSavedPeerConfig->GetDeviceAddress(&addr);
    AutoPtr<ICharSequence> seq;
    CString::New(addr, (ICharSequence**)&seq);
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
    array->Set(0, seq->Probe(EIID_IInterface));
    r->GetString(R::string::wifi_p2p_frequency_conflict_message, array, &str);
    AutoPtr<ICharSequence> csMsg;
    CString::New(str, (ICharSequence**)&csMsg);
    builder->SetMessage(csMsg);

    AutoPtr<IDialogInterfaceOnClickListener> listener;
    listener = new PositiveButtonListener(mHost);
    r->GetString(R::string::dlg_ok, &str);
    csMsg = NULL;
    CString::New(str, (ICharSequence**)&csMsg);
    builder->SetPositiveButton(csMsg, listener);

    listener = new NegativeButtonListener(mHost);
    r->GetString(R::string::decline, &str);
    csMsg = NULL;
    CString::New(str, (ICharSequence**)&csMsg);
    builder->SetNegativeButton(csMsg, listener);

    AutoPtr<IDialogInterfaceOnCancelListener> cancelListener;
    cancelListener = new DialogCancelListener(mHost);
    builder->SetOnCancelListener(cancelListener);

    AutoPtr<IAlertDialog> dialog;
    builder->Create((IAlertDialog**)&dialog);

    AutoPtr<IWindow> win;
    IDialog::Probe(dialog)->GetWindow((IWindow**)&win);
    win->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    IDialog::Probe(dialog)->Show();
    mFrequencyConflictDialog = dialog;
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::FrequencyConflictState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
        case WifiMonitor::P2P_GO_NEGOTIATION_SUCCESS_EVENT:
        case WifiMonitor::P2P_GROUP_FORMATION_SUCCESS_EVENT:
            mHost->Loge("FrequencyConflictState group sucess during freq conflict!");
            break;
        case WifiMonitor::P2P_GROUP_STARTED_EVENT:
            mHost->Loge("FrequencyConflictState group started after freq conflict, handle anyway");
            mHost->DeferMessage(message);
            mHost->TransitionTo(mHost->mGroupNegotiationState);
            break;
        case WifiMonitor::P2P_GO_NEGOTIATION_FAILURE_EVENT:
        case WifiMonitor::P2P_GROUP_REMOVED_EVENT:
        case WifiMonitor::P2P_GROUP_FORMATION_FAILURE_EVENT:
            // Ignore failures since we retry again
            break;
        case DROP_WIFI_USER_REJECT:
            // User rejected dropping wifi in favour of p2p
            mHost->HandleGroupCreationFailure();
            mHost->TransitionTo(mHost->mInactiveState);
            break;
        case DROP_WIFI_USER_ACCEPT:
            // User accepted dropping wifi in favour of p2p
            mHost->mHost->mWifiChannel->SendMessage(WifiP2pServiceImpl::DISCONNECT_WIFI_REQUEST, 1);
            mHost->mHost->mTemporarilyDisconnectedWifi = TRUE;
            break;
        case DISCONNECT_WIFI_RESPONSE:
            // Got a response from wifistatemachine, retry p2p
            if (WifiP2pServiceImpl::DBG) mHost->Logd("FrequencyConflictState Wifi disconnected, retry p2p");
            mHost->TransitionTo(mHost->mInactiveState);
            mHost->SendMessage(IWifiP2pManager::CONNECT, mHost->mSavedPeerConfig);
            break;
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }
    *result = TRUE;  // HANDLED;
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::FrequencyConflictState::Exit()
{
    if (mFrequencyConflictDialog != NULL) {
        IDialogInterface::Probe(mFrequencyConflictDialog)->Dismiss();
        mFrequencyConflictDialog = NULL;
    }
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::GroupCreatedState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::GroupCreatedState::GroupCreatedState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::GroupCreatedState::Enter()
{
    if (WifiP2pServiceImpl::DBG) Slogger::D(WifiP2pServiceImpl::TAG, "Enter %s", GetName().string());

    if (mHost->mPendingReformGroupIndication) {
        mHost->mPendingReformGroupIndication = FALSE;
        String interfaceName;
        mHost->mGroup->GetInterface(&interfaceName);
        if (mHost->mWifiNative->P2pGroupRemove(interfaceName)) {
            Slogger::D(WifiP2pServiceImpl::TAG, "Removed P2P group successfully");
            mHost->TransitionTo(mHost->mOngoingGroupRemovalState);
        } else {
            Slogger::D(WifiP2pServiceImpl::TAG, "Failed to remove the P2P group");
            mHost->HandleGroupRemoved();
            mHost->TransitionTo(mHost->mInactiveState);
        }
        if (mHost->mHost->mAutonomousGroup) {
            Slogger::D(WifiP2pServiceImpl::TAG, "AutonomousGroup is set, reform P2P Group");
            mHost->SendMessage(IWifiP2pManager::CREATE_GROUP);
        } else {
            Slogger::D(WifiP2pServiceImpl::TAG, "AutonomousGroup is not set, will not reform P2P Group");
        }
    } else {
        // Once connected, peer config details are invalid
        mHost->mSavedPeerConfig->Invalidate();
        String nullStr;
        mHost->mHost->mNetworkInfo->SetDetailedState(
                Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED, nullStr, nullStr);

        mHost->UpdateThisDevice(IWifiP2pDevice::CONNECTED);

        //DHCP server has already been started if I am a group owner
        Boolean b;
        mHost->mGroup->IsGroupOwner(&b);
        if (b) {
            AutoPtr<IInetAddress> iAddr;
            NetworkUtils::NumericToInetAddress(WifiP2pServiceImpl::SERVER_ADDRESS, (IInetAddress**)&iAddr);
            mHost->SetWifiP2pInfoOnGroupFormation(iAddr);
        }

        // In case of a negotiation group, connection changed is sent
        // after a client joins. For autonomous, send now
        if (mHost->mHost->mAutonomousGroup) {
            mHost->SendP2pConnectionChangedBroadcast();
        }
    }
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::GroupCreatedState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what, arg1;
    message->GetWhat(&what);
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    message->GetArg1(&arg1);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    String deviceAddress;
    String devAddr;
    String addr;
    String name;
    String pin;
    String emptyStr("");
    Int32 netId;
    Boolean b;

    switch (what) {
        case WifiMonitor::AP_STA_CONNECTED_EVENT: {
            AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(obj);
            device->GetDeviceAddress(&deviceAddress);
            // Clear timeout that was set when group was started.
            String interfaceName;
            mHost->mGroup->GetInterface(&interfaceName);
            mHost->mWifiNative->SetP2pGroupIdle(interfaceName, 0);
            if (!deviceAddress.IsNull()) {

                AutoPtr<IWifiP2pDevice> dev;
                mHost->mPeers->Get(deviceAddress, (IWifiP2pDevice**)&dev);
                if (dev != NULL) {
                    mHost->mGroup->AddClient(dev);
                }
                else {
                    mHost->mGroup->AddClient(deviceAddress);
                }
                mHost->mPeers->UpdateStatus(deviceAddress, IWifiP2pDevice::CONNECTED);

                if (WifiP2pServiceImpl::DBG) mHost->Logd("GroupCreatedState ap sta connected");
                mHost->SendPeersChangedBroadcast();
            }
            else {
                mHost->Loge("Connect on NULL device address, ignore");
            }
            mHost->SendP2pConnectionChangedBroadcast();
            break;
        }
        case WifiMonitor::AP_STA_DISCONNECTED_EVENT: {
            AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(obj);
            device->GetDeviceAddress(&deviceAddress);
            if (!deviceAddress.IsNull()) {

                mHost->mPeers->UpdateStatus(deviceAddress, IWifiP2pDevice::AVAILABLE);

                mHost->mGroup->RemoveClient(deviceAddress, &b);
                if (b) {
                    if (WifiP2pServiceImpl::DBG)
                        Slogger::D(WifiP2pServiceImpl::TAG, "Removed client %s", deviceAddress.string());
                    mHost->mGroup->IsClientListEmpty(&b);
                    if (!mHost->mHost->mAutonomousGroup && b) {
                        Slogger::D(WifiP2pServiceImpl::TAG, "Client list empty, remove non-persistent p2p group");
                        mHost->mGroup->GetInterface(&name);
                        mHost->mWifiNative->P2pGroupRemove(name);
                        // We end up sending connection changed broadcast
                        // when this happens at exit()
                    }
                    else {
                        // Notify when a client disconnects from group
                        mHost->SendP2pConnectionChangedBroadcast();
                    }
                }
                else {
                    if (WifiP2pServiceImpl::DBG) {
                        Slogger::D(WifiP2pServiceImpl::TAG, "Failed to remove client %s", deviceAddress.string());
                        AutoPtr<ArrayOf<IInterface*> > clients;
                        AutoPtr<ICollection> list;
                        mHost->mGroup->GetClientList((ICollection**)&list);
                        list->ToArray((ArrayOf<IInterface*>**)&clients);
                        if (clients) {
                            for (Int32 i = 0; i < clients->GetLength(); ++i) {
                                IWifiP2pDevice* c = IWifiP2pDevice::Probe((*clients)[i]);
                                c->GetDeviceAddress(&deviceAddress);
                                Slogger::D(WifiP2pServiceImpl::TAG, "client %d: %s", i, deviceAddress.string());
                            }
                        }
                    }
                }
                mHost->SendPeersChangedBroadcast();
                if (WifiP2pServiceImpl::DBG) mHost->Logd("GroupCreatedState ap sta disconnected");
            }
            else {
                String info;
                IObject::Probe(device)->ToString(&info);
                Slogger::E(WifiP2pServiceImpl::TAG, "Disconnect on unknown device: %s", info.string());
            }
            break;
        }
        case WifiMonitor::P2P_REMOVE_AND_REFORM_GROUP_EVENT: {
            /* First remove p2p group and then restart only if
             * autonoums group formation is set to true
             */
            Slogger::D(WifiP2pServiceImpl::TAG, "Received event P2P_REMOVE_AND_REFORM_GROUP, remove P2P group");
            String interfaceName;
            mHost->mGroup->GetInterface(&interfaceName);
            if (mHost->mWifiNative->P2pGroupRemove(interfaceName)) {
                Slogger::D(TAG, "Removed P2P group successfully");
                mHost->TransitionTo(mHost->mOngoingGroupRemovalState);
                mHost->ReplyToMessage(message, IWifiP2pManager::REMOVE_GROUP_SUCCEEDED);
            } else {
                Slogger::D(TAG, "Failed to remove the P2P group");
                mHost->HandleGroupRemoved();
                mHost->TransitionTo(mHost->mInactiveState);
                mHost->ReplyToMessage(message, IWifiP2pManager::REMOVE_GROUP_FAILED, IWifiP2pManager::ERROR);
            }
            if (mHost->mHost->mAutonomousGroup) {
                Slogger::D(TAG, "AutonomousGroup is set, reform P2P Group");
                mHost->SendMessage(IWifiP2pManager::CREATE_GROUP);
            } else {
                Slogger::D(TAG, "AutonomousGroup is not set, will not reform P2P Group");
            }
            break;
        }
        case IDhcpStateMachine::CMD_POST_DHCP_ACTION: {
            AutoPtr<IDhcpResults> dhcpResults = IDhcpResults::Probe(obj);
            if (arg1 == IDhcpStateMachine::DHCP_SUCCESS &&
                    dhcpResults != NULL) {
                if (WifiP2pServiceImpl::DBG) mHost->Logd("DhcpResults: TODO");// + dhcpResults;
                AutoPtr<IInetAddress> iAddress;
                dhcpResults->GetServerAddress((IInetAddress**)&iAddress);
                mHost->SetWifiP2pInfoOnGroupFormation(iAddress);

                mHost->SendP2pConnectionChangedBroadcast();
                //Turn on power save on client
                mHost->mGroup->GetInterface(&name);
                mHost->mWifiNative->SetP2pPowerSave(name, TRUE);

                //try {
                String iface;
                mHost->mGroup->GetInterface(&iface);
                AutoPtr<IList> list;
                IStaticIpConfiguration::Probe(dhcpResults)->GetRoutes(iface, (IList**)&list);
                mHost->mHost->mNwService->AddInterfaceToLocalNetwork(iface, list);
                //} catch (RemoteException e) {
                //    loge("Failed to add iface to local network " + e);
                //} catch (IllegalStateException ie) {
                //    loge("Failed to add iface to local network " + ie);
                //}
            }
            else {
                mHost->Loge("DHCP failed");
                mHost->mWifiNative->P2pGroupRemove(name);
            }
            break;
        }
        case IWifiP2pManager::REMOVE_GROUP:
            if (WifiP2pServiceImpl::DBG) mHost->Logd("GroupCreatedState remove group");
            mHost->mGroup->GetInterface(&name);
            if (mHost->mWifiNative->P2pGroupRemove(name)) {
                mHost->TransitionTo(mHost->mOngoingGroupRemovalState);
                mHost->ReplyToMessage(message, IWifiP2pManager::REMOVE_GROUP_SUCCEEDED);
            }
            else {
                mHost->HandleGroupRemoved();
                mHost->TransitionTo(mHost->mInactiveState);
                mHost->ReplyToMessage(message, IWifiP2pManager::REMOVE_GROUP_FAILED,
                    IWifiP2pManager::ERROR);
            }
            break;
        /* We do not listen to NETWORK_DISCONNECTION_EVENT for group removal
         * handling since supplicant actually tries to reconnect after a temporary
         * disconnect until group idle time out. Eventually, a group removal event
         * will come when group has been removed.
         *
         * When there are connectivity issues during temporary disconnect, the application
         * will also just remove the group.
         *
         * Treating network disconnection as group removal causes race conditions since
         * supplicant would still maintain the group at that stage.
         */
        case WifiMonitor::P2P_GROUP_REMOVED_EVENT: {
            if (WifiP2pServiceImpl::DBG) mHost->Logd("GroupCreatedState group removed");

            mHost->HandleGroupRemoved();
            mHost->mWifiNative->P2pFlush();
            mHost->TransitionTo(mHost->mInactiveState);
            break;
        }
        case WifiMonitor::P2P_DEVICE_LOST_EVENT: {
            AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(obj);
            //Device loss for a connected device indicates it is not in discovery any more
            mHost->mGroup->Contains(device, &b);
            if (b) {
                if (WifiP2pServiceImpl::DBG) {
                    String info;
                    IObject::Probe(device)->ToString(&info);
                    Slogger::D(WifiP2pServiceImpl::TAG, "Add device to lost list %s", info.string());
                }
                mHost->mPeersLostDuringConnection->UpdateSupplicantDetails(device);
                *result = TRUE;  // HANDLED;
                return NOERROR;
            }
            // Do the regular device lost handling
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
        }
        case WifiStateMachine::CMD_DISABLE_P2P_REQ:
            mHost->SendMessage(IWifiP2pManager::REMOVE_GROUP);
            mHost->DeferMessage(message);
            break;
            // This allows any client to join the GO during the
            // WPS window
        case IWifiP2pManager::START_WPS: {
            AutoPtr<IWpsInfo> wps = IWpsInfo::Probe(obj);
            if (wps == NULL) {
                mHost->ReplyToMessage(message, IWifiP2pManager::START_WPS_FAILED);
                break;
            }
            Boolean ret = TRUE;
            Int32 setup;
            wps->GetSetup(&setup);
            String iface;
            mHost->mGroup->GetInterface(&iface);
            if (setup == IWpsInfo::PBC) {
                ret = mHost->mWifiNative->StartWpsPbc(iface, String(NULL));
            } else {
                String pin;
                wps->GetPin(&pin);
                if (pin.IsNull()) {
                    pin = mHost->mWifiNative->StartWpsPinDisplay(iface);
                    //try {
                        Int32 intValue;
                        ECode ec = StringToIntegral::Parse(pin, 10, &intValue);
                        if (FAILED(ec)) {
                            ret = FALSE;
                        }
                        else {
                            mHost->NotifyInvitationSent(pin, String("any"));
                        }
                    //} catch (NumberFormatException ignore) {
                    //    ret = false;
                    //}
                } else {
                    ret = mHost->mWifiNative->StartWpsPinKeypad(iface, pin);
                }
            }
            mHost->ReplyToMessage(message, ret ? IWifiP2pManager::START_WPS_SUCCEEDED :
                                                 IWifiP2pManager::START_WPS_FAILED);
            break;
        }
        case IWifiP2pManager::CONNECT: {
            AutoPtr<IWifiP2pConfig> config = IWifiP2pConfig::Probe(obj);

            if (mHost->IsConfigInvalid(config)) {
                mHost->Loge("Dropping connect requeset ");// + config;
                mHost->ReplyToMessage(message, IWifiP2pManager::CONNECT_FAILED);
                break;
            }
            config->GetDeviceAddress(&addr);
            Slogger::D(WifiP2pServiceImpl::TAG, "Inviting device : %s", addr.string());
            mHost->mSavedPeerConfig = config;
            if (mHost->mWifiNative->P2pInvite(mHost->mGroup, addr)) {
                mHost->mPeers->UpdateStatus(addr, IWifiP2pDevice::INVITED);
                mHost->SendPeersChangedBroadcast();
                mHost->ReplyToMessage(message, IWifiP2pManager::CONNECT_SUCCEEDED);
            }
            else {
                mHost->ReplyToMessage(message, IWifiP2pManager::CONNECT_FAILED,
                    IWifiP2pManager::ERROR);
            }
            // TODO: figure out updating the status to declined when invitation is rejected
            break;
        }
        case WifiMonitor::P2P_INVITATION_RESULT_EVENT: {
            AutoPtr<IInteger32> iobj = IInteger32::Probe(obj);
            Int32 ival;
            iobj->GetValue(&ival);
            P2pStatus status = (P2pStatus)ival;
            Slogger::D(WifiP2pServiceImpl::TAG, "===> INVITATION RESULT EVENT : %d", status);
            if (status == P2pStatus_SUCCESS) {
                // invocation was succeeded.
                break;
            }
            mHost->Loge("Invitation result TODO");//, status;
            if (status == P2pStatus_UNKNOWN_P2P_GROUP) {
                // target device has already removed the credential.
                // So, remove this credential accordingly.
                mHost->mGroup->GetNetworkId(&netId);
                if (netId >= 0) {
                    if (WifiP2pServiceImpl::DBG) mHost->Logd("Remove unknown client from the list");
                    mHost->mSavedPeerConfig->GetDeviceAddress(&addr);
                    if (!mHost->RemoveClientFromList(netId, addr, FALSE)) {
                        // not found the client on the list
                        Slogger::E(WifiP2pServiceImpl::TAG, "Already removed the client, ignore");
                    }
                    // try invitation.
                    mHost->SendMessage(IWifiP2pManager::CONNECT, mHost->mSavedPeerConfig);
                }
            }
            break;
        }
        case WifiMonitor::P2P_PROV_DISC_PBC_REQ_EVENT:
        case WifiMonitor::P2P_PROV_DISC_ENTER_PIN_EVENT:
        case WifiMonitor::P2P_PROV_DISC_SHOW_PIN_EVENT: {
            AutoPtr<IWifiP2pProvDiscEvent> provDisc = IWifiP2pProvDiscEvent::Probe(obj);
            AutoPtr<IWifiP2pDevice> device;
            provDisc->GetDevice((IWifiP2pDevice**)&device);
            device->GetDeviceAddress(&addr);

            mHost->mSavedPeerConfig = NULL;
            CWifiP2pConfig::New((IWifiP2pConfig**)&mHost->mSavedPeerConfig);
            mHost->mSavedPeerConfig->SetDeviceAddress(addr);
            AutoPtr<IWpsInfo> wps;
            mHost->mSavedPeerConfig->GetWps((IWpsInfo**)&wps);
            if (what == WifiMonitor::P2P_PROV_DISC_ENTER_PIN_EVENT) {
                wps->SetSetup(IWpsInfo::KEYPAD);
            }
            else if (what == WifiMonitor::P2P_PROV_DISC_SHOW_PIN_EVENT) {
                wps->SetSetup(IWpsInfo::DISPLAY);
                provDisc->GetPin(&pin);
                wps->SetPin(pin);
            }
            else {
                wps->SetSetup(IWpsInfo::PBC);
            }

            Boolean b;
            mHost->mGroup->IsGroupOwner(&b);
            if (WifiP2pServiceImpl::DBG) mHost->Logd("mGroup.isGroupOwner() TODO");//, b;
            if (b) {
                if (WifiP2pServiceImpl::DBG)
                    mHost->Logd("Local device is Group Owner, transiting to mUserAuthorizingJoinState");
                mHost->TransitionTo(mHost->mUserAuthorizingJoinState);
            }
            break;
        }
        case WifiMonitor::P2P_GROUP_STARTED_EVENT:
            Slogger::E(WifiP2pServiceImpl::TAG, "Duplicate group creation event notice, ignore");
            break;
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }

    *result = TRUE;  // HANDLED;
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::GroupCreatedState::Exit()
{
    mHost->UpdateThisDevice(IWifiP2pDevice::AVAILABLE);
    mHost->ResetWifiP2pInfo();
    String nullStr;
    mHost->mHost->mNetworkInfo->SetDetailedState(NetworkInfoDetailedState_DISCONNECTED, nullStr, nullStr);
    mHost->SendP2pConnectionChangedBroadcast();
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingJoinState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingJoinState::UserAuthorizingJoinState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingJoinState::Enter()
{
    if (WifiP2pServiceImpl::DBG) Slogger::D(WifiP2pServiceImpl::TAG, "Enter %s", GetName().string());
    mHost->NotifyInvitationReceived();
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingJoinState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
        case WifiMonitor::P2P_PROV_DISC_PBC_REQ_EVENT:
        case WifiMonitor::P2P_PROV_DISC_ENTER_PIN_EVENT:
        case WifiMonitor::P2P_PROV_DISC_SHOW_PIN_EVENT:
            //Ignore more client requests
            break;
        case PEER_CONNECTION_USER_ACCEPT: {
            //Stop discovery to avoid failure due to channel switch
            mHost->mWifiNative->P2pStopFind();
            String interfaceName;
            mHost->mGroup->GetInterface(&interfaceName);
            AutoPtr<IWpsInfo> info;
            mHost->mSavedPeerConfig->GetWps((IWpsInfo**)&info);
            Int32 setup;
            info->GetSetup(&setup);
            String pin;
            info->GetPin(&pin);
            if (setup == IWpsInfo::PBC) {
                mHost->mWifiNative->StartWpsPbc(interfaceName, String(NULL));
            }
            else {
                mHost->mWifiNative->StartWpsPinKeypad(interfaceName, pin);
            }
            mHost->TransitionTo(mHost->mGroupCreatedState);
            break;
        }
        case PEER_CONNECTION_USER_REJECT:
            if (WifiP2pServiceImpl::DBG) mHost->Logd("User rejected incoming request");
            mHost->TransitionTo(mHost->mGroupCreatedState);
            break;
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }

    *result = TRUE;  // HANDLED;
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::UserAuthorizingJoinState::Exit()
{
    //TODO: dismiss dialog if not already done
    return NOERROR;
}

//==================================================================
// WifiP2pServiceImpl::P2pStateMachine::OngoingGroupRemovalState
//==================================================================
WifiP2pServiceImpl::P2pStateMachine::OngoingGroupRemovalState::OngoingGroupRemovalState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode WifiP2pServiceImpl::P2pStateMachine::OngoingGroupRemovalState::Enter()
{
    if (WifiP2pServiceImpl::DBG) Slogger::D(WifiP2pServiceImpl::TAG, "Enter %s", GetName().string());
    return NOERROR;
}

ECode WifiP2pServiceImpl::P2pStateMachine::OngoingGroupRemovalState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);

    if (WifiP2pServiceImpl::DBG) {
        Slogger::D(WifiP2pServiceImpl::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
        // Group removal ongoing. Multiple calls
        // end up removing persisted network. Do nothing.
        case IWifiP2pManager::REMOVE_GROUP:
            mHost->ReplyToMessage(message, IWifiP2pManager::REMOVE_GROUP_SUCCEEDED);
            break;
        // Parent state will transition out of this state
        // when removal is complete
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }

    *result = TRUE;  // HANDLED;
    return NOERROR;
}

//////////////////////
//  WifiP2pServiceImpl::ClientHandler
/////////////////////
WifiP2pServiceImpl::ClientHandler::ClientHandler(
    /* [in] */ WifiP2pServiceImpl* host,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mHost(host)
{
}

ECode WifiP2pServiceImpl::ClientHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case IWifiP2pManager::SET_DEVICE_NAME:
        case IWifiP2pManager::SET_WFD_INFO:
        case IWifiP2pManager::DISCOVER_PEERS:
        case IWifiP2pManager::STOP_DISCOVERY:
        case IWifiP2pManager::CONNECT:
        case IWifiP2pManager::CANCEL_CONNECT:
        case IWifiP2pManager::CREATE_GROUP:
        case IWifiP2pManager::REMOVE_GROUP:
        case IWifiP2pManager::START_LISTEN:
        case IWifiP2pManager::STOP_LISTEN:
        case IWifiP2pManager::SET_CHANNEL:
        case IWifiP2pManager::START_WPS:
        case IWifiP2pManager::ADD_LOCAL_SERVICE:
        case IWifiP2pManager::REMOVE_LOCAL_SERVICE:
        case IWifiP2pManager::CLEAR_LOCAL_SERVICES:
        case IWifiP2pManager::DISCOVER_SERVICES:
        case IWifiP2pManager::ADD_SERVICE_REQUEST:
        case IWifiP2pManager::REMOVE_SERVICE_REQUEST:
        case IWifiP2pManager::CLEAR_SERVICE_REQUESTS:
        case IWifiP2pManager::REQUEST_PEERS:
        case IWifiP2pManager::REQUEST_CONNECTION_INFO:
        case IWifiP2pManager::REQUEST_GROUP_INFO:
        case IWifiP2pManager::DELETE_PERSISTENT_GROUP:
        case IWifiP2pManager::REQUEST_PERSISTENT_GROUP_INFO: {
            AutoPtr<IMessage> message;
            AutoPtr<IMessageHelper> helper;
            CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
            helper->Obtain(msg, (IMessage**)&message);
            mHost->mP2pStateMachine->SendMessage(message);
            break;
        }
        default:
            Slogger::D(WifiP2pServiceImpl::TAG, "ClientHandler.handleMessage ignoring msg=%d", msg);
            break;
    }
    return NOERROR;
}

} // namespace P2p
} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

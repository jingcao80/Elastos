#include "elastos/droid/wifi/p2p/CWifiP2pService.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/Pair.h>

#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/net/CNetworkInfo.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/net/CLinkAddress.h"
#include "elastos/droid/net/DhcpStateMachine.h"
#include "elastos/droid/net/wifi/WifiNative.h"
#include "elastos/droid/net/wifi/WifiMonitor.h"
#include "elastos/droid/net/wifi/WifiStateMachine.h"
#include "elastos/droid/net/wifi/p2p/CWifiP2pDevice.h"
#include "elastos/droid/net/wifi/p2p/CWifiP2pGroup.h"
#include "elastos/droid/net/wifi/p2p/CWifiP2pInfo.h"
#include "elastos/droid/net/wifi/p2p/CWifiP2pConfig.h"
#include "elastos/droid/net/wifi/p2p/CWifiP2pDeviceList.h"
#include "elastos/droid/net/wifi/p2p/CWifiP2pGroupList.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/os/CMessenger.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/app/CAlertDialogBuilder.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"

using Elastos::Utility::Etl::Pair;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::IInteger32;
using Elastos::Net::IInetAddress;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::R;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::DhcpStateMachine;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Net::CLinkAddress;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED;
using Elastos::Droid::Net::NetworkInfoDetailedState_FAILED;
using Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED;
using Elastos::Droid::Wifi::WifiNative;
using Elastos::Droid::Wifi::WifiMonitor;
using Elastos::Droid::Wifi::WifiStateMachine;
using Elastos::Droid::Wifi::P2p::IWifiP2pInfo;
using Elastos::Droid::Wifi::P2p::CWifiP2pConfig;
using Elastos::Droid::Wifi::P2p::CWifiP2pDevice;
using Elastos::Droid::Wifi::P2p::CWifiP2pDeviceList;
using Elastos::Droid::Wifi::P2p::CWifiP2pGroup;
using Elastos::Droid::Wifi::P2p::CWifiP2pGroupList;
using Elastos::Droid::Wifi::P2p::P2pStatus;
using Elastos::Droid::Wifi::P2p::Nsd::EIID_IWifiP2pServiceInfo;
using Elastos::Droid::Wifi::P2p::Nsd::EIID_IWifiP2pServiceRequest;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IActivityManagerRunningTaskInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {
namespace P2p {

const String CWifiP2pService::TAG("CWifiP2pService");
const Boolean CWifiP2pService::DBG = FALSE;
const String CWifiP2pService::NETWORKTYPE("WIFI_P2P");

const Boolean CWifiP2pService::JOIN_GROUP = TRUE;
const Boolean CWifiP2pService::FORM_GROUP = FALSE;

const Boolean CWifiP2pService::TRY_REINVOCATION = TRUE;
const Boolean CWifiP2pService::NO_REINVOCATION = FALSE;

const Boolean CWifiP2pService::RELOAD = TRUE;
const Boolean CWifiP2pService::NO_RELOAD = FALSE;

const Int32 CWifiP2pService::CONNECT_FAILURE = -1;
const Int32 CWifiP2pService::CONNECT_SUCCESS = 0;
const Int32 CWifiP2pService::NEEDS_PROVISION_REQ = 1;

/* Two minutes comes from the wpa_supplicant setting */
const Int32 CWifiP2pService::GROUP_CREATING_WAIT_TIME_MS = 120 * 1000;

const Int32 CWifiP2pService::DISABLE_P2P_WAIT_TIME_MS = 5 * 1000;

/* Set a two minute discover timeout to avoid STA scans from being blocked */
const Int32 CWifiP2pService::DISCOVER_TIMEOUT_S = 120;

/* Idle time after a peer is gone when the group is torn down */
const Int32 CWifiP2pService::GROUP_IDLE_TIME_S = 10;

const Int32 CWifiP2pService::BASE = IProtocol::BASE_WIFI_P2P_SERVICE;

/* User accepted a peer request */
const Int32 CWifiP2pService::PEER_CONNECTION_USER_ACCEPT    = IProtocol::BASE_WIFI_P2P_SERVICE + 2;
/* User rejected a peer request */
const Int32 CWifiP2pService::PEER_CONNECTION_USER_REJECT    = IProtocol::BASE_WIFI_P2P_SERVICE + 3;
/* User wants to disconnect wifi in favour of p2p */
const Int32 CWifiP2pService::DROP_WIFI_USER_ACCEPT          = IProtocol::BASE_WIFI_P2P_SERVICE + 4;
/* User wants to keep his wifi connection and drop p2p */
const Int32 CWifiP2pService::DROP_WIFI_USER_REJECT          = IProtocol::BASE_WIFI_P2P_SERVICE + 5;

static AutoPtr<ArrayOf<String> > InitServiceDHCPRange()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    array->Set(0, String("192.168.49.2"));
    array->Set(0, String("192.168.49.254"));
    return array;
}

/* Is chosen as a unique range to avoid conflict with
   the range defined in Tethering.java */
const AutoPtr<ArrayOf<String> > CWifiP2pService::DHCP_RANGE = InitServiceDHCPRange();
const String CWifiP2pService::SERVER_ADDRESS("192.168.49.1");
const String CWifiP2pService::SERVER_ADDRESS_TETHER("192.168.43.1");

const String CWifiP2pService::WFD_P2P_ROLE("wlan.wfdp2p.role");
const String CWifiP2pService::WFD_DNSMASQ_PEER("wlan.wfddnsmasq.peer");
const String CWifiP2pService::WFD_P2P_DEVICE_ADDR("wlan.wfdp2p.addr");

Int32 CWifiP2pService::mGroupCreatingTimeoutIndex = 0;
Int32 CWifiP2pService::mDisableP2pTimeoutIndex = 0;

//==================================================================
// CWifiP2pService::ClientInfo
//==================================================================
CWifiP2pService::ClientInfo::ClientInfo(
    /* [in] */ IMessenger* m,
    /* [in] */ CWifiP2pService* host)
    : mMessenger(m)
    , mHost(host)
{
}

ECode CWifiP2pService::ClientInfo::GetMessenger(
    /*[out] */ IMessenger** messenger)
{
    VALIDATE_NOT_NULL(messenger);
    *messenger = mMessenger;
    REFCOUNT_ADD(*messenger);
    return NOERROR;
}

ECode CWifiP2pService::ClientInfo::SetMessenger(
    /*[in] */ IMessenger* messenger)
{
    mMessenger = messenger;
    return NOERROR;
}

//==================================================================
// CWifiP2pService
//==================================================================
CWifiP2pService::CWifiP2pService()
    : mP2pSupported(FALSE)
    , mAutonomousGroup(FALSE)
    , mJoinExistingGroup(FALSE)
    , mDiscoveryStarted(FALSE)
    , mTempoarilyDisconnectedWifi(FALSE)
    , mServiceTransactionId(0)
{
    mReplyChannel = new AsyncChannel();
    CWifiP2pDevice::New((IWifiP2pDevice**)&mThisDevice);
}

CWifiP2pService::~CWifiP2pService()
{
}

ECode CWifiP2pService::constructor(
    /* [in] */ IContext *context)
{
    mContext = context;

    //STOPSHIP: get this from native side
    mInterface = String("p2p0");

    AutoPtr<IInterface> obj;
    context->GetSystemService(IActivity::ACTIVITY_SERVICE, (IInterface**)&obj);
    mActivityMgr = IActivityManager::Probe(obj);
    assert(mActivityMgr != NULL);
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

    //set wfd p2p role
    String emptyStr("");
    SystemProperties::Set(WFD_P2P_ROLE, emptyStr);//empty
    SystemProperties::Set(WFD_DNSMASQ_PEER, emptyStr);//empty
    SystemProperties::Set(WFD_P2P_DEVICE_ADDR, emptyStr);//empty

    Slogger::I(TAG, " WifiP2pService start! P2pSupported %d, device type: %s", mP2pSupported, str.string());
    mP2pStateMachine = new P2pStateMachine(TAG, mP2pSupported, this);
    mP2pStateMachine->Start();
    return NOERROR;
}

P2pStatus CWifiP2pService::ValueOf(
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

ECode CWifiP2pService::ConnectivityServiceReady()
{
    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::NETWORKMANAGEMENT_SERVICE);
    mNwService = IINetworkManagementService::Probe(obj);
    return NOERROR;
}

ECode CWifiP2pService::GetMessenger(
    /* [out] */ IMessenger** msg)
{
    VALIDATE_NOT_NULL(msg);

    FAIL_RETURN(EnforceAccessPermission());
    FAIL_RETURN(EnforceChangePermission());

    AutoPtr<IHandler> handler = mP2pStateMachine->GetHandler();
    return CMessenger::New(handler, msg);
}

ECode CWifiP2pService::EnforceAccessPermission()
{
    return mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::ACCESS_WIFI_STATE,
        String("WifiP2pService"));
}

ECode CWifiP2pService::EnforceChangePermission()
{
    return mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::CHANGE_WIFI_STATE,
        String("WifiP2pService"));
}

//==================================================================
// CWifiP2pService::P2pStateMachine::GroupDeleteListener
//==================================================================
CAR_INTERFACE_IMPL(CWifiP2pService::P2pStateMachine::GroupDeleteListener, IGroupDeleteListener)

CWifiP2pService::P2pStateMachine::GroupDeleteListener::GroupDeleteListener(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::GroupDeleteListener::OnDeleteGroup(
    /* [in] */ Int32 netId)
{
    if (CWifiP2pService::DBG)
        Slogger::E(CWifiP2pService::TAG, "called onDeleteGroup() netId=%d", netId);

    mHost->mWifiNative->RemoveNetwork(netId);
    mHost->mWifiNative->SaveConfig();
    mHost->SendP2pPersistentGroupsChangedBroadcast();
    return NOERROR;
}

//==================================================================
// CWifiP2pService::P2pStateMachine
//==================================================================
CWifiP2pService::P2pStateMachine::P2pStateMachine(
    /* [in] */ const String& name,
    /* [in] */ Boolean p2pSupported,
    /* [in] */ CWifiP2pService* host)
    : StateMachine(name)
    , mHost(host)
{
    mDefaultState = new DefaultState(this);
    mP2pNotSupportedState = new P2pNotSupportedState(this);
    mP2pDisablingState = new P2pDisablingState(this);
    mP2pDisabledState = new P2pDisabledState(this);
    mP2pEnablingState = new P2pEnablingState(this);
    mP2pEnabledState = new P2pEnabledState(this);
    // Inactive is when p2p is enabled with no connectivity
    mInactiveState = new InactiveState(this);
    mGroupCreatingState = new GroupCreatingState(this);
    mUserAuthorizingInvitationState = new UserAuthorizingInvitationState(this);
    mProvisionDiscoveryState = new ProvisionDiscoveryState(this);
    mGroupNegotiationState = new GroupNegotiationState(this);
    mFrequencyConflictState = new FrequencyConflictState(this);

    mGroupCreatedState = new GroupCreatedState(this);
    mUserAuthorizingJoinState = new UserAuthorizingJoinState(this);
    mOngoingGroupRemovalState = new OngoingGroupRemovalState(this);

    mWifiNative = new WifiNative(mHost->mInterface);
    mWifiMonitor = new WifiMonitor(this, mWifiNative);

    CWifiP2pDeviceList::New((IWifiP2pDeviceList**)&mPeers);
    CWifiP2pDeviceList::New((IWifiP2pDeviceList**)&mNotP2pDevice);

    CWifiP2pDeviceList::New((IWifiP2pDeviceList**)&mPeersLostDuringConnection);
    AutoPtr<IGroupDeleteListener> listener = new GroupDeleteListener(this);
    CWifiP2pGroupList::New(NULL, listener, (IWifiP2pGroupList**)&mGroups);
    CWifiP2pInfo::New((IWifiP2pInfo**)&mWifiP2pInfo);

    AddState(mDefaultState);
    AddState(mP2pNotSupportedState, mDefaultState);
    AddState(mP2pDisablingState, mDefaultState);
    AddState(mP2pDisabledState, mDefaultState);
    AddState(mP2pEnablingState, mDefaultState);
    AddState(mP2pEnabledState, mDefaultState);
    AddState(mInactiveState, mP2pEnabledState);
    AddState(mGroupCreatingState, mP2pEnabledState);
    AddState(mUserAuthorizingInvitationState, mGroupCreatingState);
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
}

ECode CWifiP2pService::P2pStateMachine::SendP2pStateChangedBroadcast(
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

ECode CWifiP2pService::P2pStateMachine::SendP2pDiscoveryChangedBroadcast(
    /* [in] */ Boolean started)
{
    if (mHost->mDiscoveryStarted == started) return NOERROR;
    mHost->mDiscoveryStarted = started;

    if (CWifiP2pService::DBG)
        Slogger::D(CWifiP2pService::TAG, "discovery change broadcast %d", started);

    AutoPtr<IIntent> intent;
    CIntent::New(IWifiP2pManager::WIFI_P2P_DISCOVERY_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(IWifiP2pManager::EXTRA_DISCOVERY_STATE, started ?
        IWifiP2pManager::WIFI_P2P_DISCOVERY_STARTED : IWifiP2pManager::WIFI_P2P_DISCOVERY_STOPPED);
    return mHost->mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

ECode CWifiP2pService::P2pStateMachine::SendThisDeviceChangedBroadcast()
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

ECode CWifiP2pService::P2pStateMachine::SendP2pPeersChangedBroadcast()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiP2pManager::WIFI_P2P_PEERS_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    return mHost->mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
}

ECode CWifiP2pService::P2pStateMachine::SendP2pConnectionChangedBroadcast()
{
    if (CWifiP2pService::DBG) Logd("sending p2p connection changed broadcast");
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

    AutoPtr<IWifiP2pDevice> device;
    if (mWifiP2pDevice != NULL) {
        CWifiP2pDevice::New(mWifiP2pDevice, (IWifiP2pDevice**)&device);
    }
    else {
        CWifiP2pDevice::New((IWifiP2pDevice**)&device);
    }
    intent->PutExtra(
        IWifiP2pManager::EXTRA_WIFI_P2P_DEVICE,
        IParcelable::Probe(device.Get()));

    mHost->mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);

    AutoPtr<INetworkInfo> networkInfo;
    CNetworkInfo::New(mHost->mNetworkInfo, (INetworkInfo**)&networkInfo);
    mHost->mWifiChannel->SendMessage(
        IWifiP2pService::P2P_CONNECTION_CHANGED,
        networkInfo->Probe(EIID_IInterface));
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::SendP2pPersistentGroupsChangedBroadcast()
{
    if (CWifiP2pService::DBG) Logd("sending p2p persistent groups changed broadcast");
    AutoPtr<IIntent> intent;
    CIntent::New(IWifiP2pManager::WIFI_P2P_PERSISTENT_GROUPS_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    return mHost->mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

ECode CWifiP2pService::P2pStateMachine::StartDhcpServer(
    /* [in] */ const String& intf)
{
    String serverAddr;
    if (mHost->mAutonomousGroup)
        serverAddr = CWifiP2pService::SERVER_ADDRESS_TETHER;
    else
        serverAddr = CWifiP2pService::SERVER_ADDRESS;
    // try {
    AutoPtr<IInterfaceConfiguration> ifcg;
    mHost->mNwService->GetInterfaceConfig(intf, (IInterfaceConfiguration**)&ifcg);

    AutoPtr<IInetAddress> addr;
    NetworkUtils::NumericToInetAddress(serverAddr, (IInetAddress**)&addr);
    AutoPtr<ILinkAddress> linkAddr;
    CLinkAddress::New(addr, 24, (ILinkAddress**)&linkAddr);
    ifcg->SetLinkAddress(linkAddr);
    ifcg->SetInterfaceUp();
    mHost->mNwService->SetInterfaceConfig(intf, ifcg);
    /* This starts the dnsmasq server */
    // try {
    ECode ec = mHost->mNwService->StartTethering(DHCP_RANGE);
    if (FAILED(ec)) {
        Slogger::E(CWifiP2pService::TAG, "Error startTethering %s, ec=%08x", intf.string(), ec);
    }
    // } catch (Exception e) {
    //         loge("Error startTethering " + intf + ", :" + e);
    // }

    //tether mode with p2p, just tether when user call createGroupOwner
    //if negotiate self,just keep the old style.
    if (mHost->mAutonomousGroup) {
        if (mHost->mConnectivityManager == NULL) {
            AutoPtr<IInterface> svrObj;
            mHost->mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&svrObj);
            mHost->mConnectivityManager = IConnectivityManager::Probe(svrObj);
        }
        Logd("p2p: mAutonomousGroup is true, set tether mode, share internet.");
        Int32 result;
        mHost->mConnectivityManager->Tether(intf, &result);
        if (result != IConnectivityManager::TETHER_ERROR_NO_ERROR) {
            Slogger::E(CWifiP2pService::TAG, "p2p: Error tethering on %s", intf.string());
            return NOERROR;
        }
        Slogger::D(CWifiP2pService::TAG, "p2p: mTetherInterfaceName :%s", intf.string());
        mHost->mTetherInterfaceName = intf;
    }
    else {
        /*  */
        Logd("p2p: mAutonomousGroup is false, do not set tether mode.");
        /* This starts the dnsmasq server */
        return mHost->mNwService->StartTethering(DHCP_RANGE);
    }
    // } catch (Exception e) {
    //     Loge("Error configuring interface " + intf + ", :" + e);
    //     return;
    // }

    Slogger::D(CWifiP2pService::TAG, "Started Dhcp server on %s", intf.string());
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::StopDhcpServer(
    /* [in] */ const String& intf)
{
    // try {
    ECode ec = NOERROR;
    //tether mode with p2p
    if (mHost->mAutonomousGroup) {
        Logd("p2p: mAutonomousGroup is true, untether.");
        if (mHost->mConnectivityManager == NULL) {
            AutoPtr<IInterface> obj;
            mHost->mContext->GetSystemService(
                IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
            mHost->mConnectivityManager = IConnectivityManager::Probe(obj);
        }
        Int32 result;
        ec = mHost->mConnectivityManager->Untether(mHost->mTetherInterfaceName, &result);
        if (result != IConnectivityManager::TETHER_ERROR_NO_ERROR) {
            Logd("p2p: Untether initiate failed!");
        }
    }
    else {
        Logd("p2p: mAutonomousGroup is false, untether.");
        ec = mHost->mNwService->StopTethering();
    }

    if (FAILED(ec)) {
        Slogger::E(CWifiP2pService::TAG, "Error stopping Dhcp server, ec=%08x", ec);
        return NOERROR;
    }
    // } catch (Exception e) {
    //     Loge("Error stopping Dhcp server" + e);
    //     return;
    // }

    Logd("Stopped Dhcp server");
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::NotifyP2pEnableFailure()
{
    Logd("p2p: NotifyP2pEnableFailure.");
    AutoPtr<IResources> r = CResources::GetSystem();

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
    dialog->GetWindow((IWindow**)&win);
    win->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    return dialog->Show();
}

ECode CWifiP2pService::P2pStateMachine::AddRowToDialog(
    /* [in] */ IViewGroup* group,
    /* [in] */ Int32 stringId,
    /* [in] */ const String& value)
{
    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "p2p: AddRowToDialog: %s.", value.string());
    }

    AutoPtr<IResources> r = CResources::GetSystem();
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

ECode CWifiP2pService::P2pStateMachine::NotifyInvitationSent(
    /* [in] */ const String& pin,
    /* [in] */ const String& peerAddress)
{
    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "p2p: AddRowToDialog: pin: %s, address: %s.",
            pin.string(), peerAddress.string());
    }

    AutoPtr<IResources> r = CResources::GetSystem();
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
    dialog->GetWindow((IWindow**)&win);
    win->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    return dialog->Show();
}

ECode CWifiP2pService::P2pStateMachine::NotifyInvitationReceived()
{
    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "p2p: NotifyInvitationReceived");
    }

    AutoPtr<IResources> r = CResources::GetSystem();
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

/*
    AutoPtr<IWpsInfo> wps;
    mSavedPeerConfig->GetWps((IWpsInfo**)&wps);

    AutoPtr<Elastos::Droid::Widget::IEditText> pin;
    textEntryView->FindViewById(Elastos::Droid::R::id::wifi_p2p_wps_pin, (IView**)&pin);

    AutoPtr<IAlertDialog> dialog;
    AutoPtr<IAlertDialogBuilder> builder;
    // CAlertDialogBuilder::New(mHost->mContext, (IAlertDialogBuilder**)&builder);

    String title, accept;
    r->GetString(Elastos::Droid::R::string::wifi_p2p_invitation_to_connect_title, &title);
    r->GetString(Elastos::Droid::R::string::accept, &accept);
    AutoPtr<ICharSequence> csTitle;
    CString::New(title, (ICharSequence**)&csTitle);
    builder->SetTitle(csTitle);
    builder->SetView(textEntryView);
//TODO: Need
    //builder->setPositiveButton(r->GetString(R.string.accept), new OnClickListener() {
    //                 public void onClick(DialogInterface dialog, int which) {
    //                     if (wps.setup == WpsInfo.KEYPAD) {
    //                         mSavedPeerConfig.wps.pin = pin.getText().toString();
    //                     }
    //                     if (CWifiP2pService::DBG) Logd(getName() + " accept invitation " + mSavedPeerConfig);
    //                     sendMessage(PEER_CONNECTION_USER_ACCEPT);
    //                 }
    //             })
    //builder->setNegativeButton(r->GetString(R.string.decline), new OnClickListener() {
    //                 @Override
    //                 public void onClick(DialogInterface dialog, int which) {
    //                     if (CWifiP2pService::DBG) Logd(getName() + " ignore connect");
    //                     sendMessage(PEER_CONNECTION_USER_REJECT);
    //                 }
    //             })
    //builder->setOnCancelListener(new DialogInterface.OnCancelListener() {
    //                 @Override
    //                 public void onCancel(DialogInterface arg0) {
    //                     if (CWifiP2pService::DBG) Logd(getName() + " ignore connect");
    //                     sendMessage(PEER_CONNECTION_USER_REJECT);
    //                 }
    //             })
    builder->Create((IAlertDialog**)&dialog);

    //make the enter pin area or the display pin area visible
    Int32 setup;
    wps->GetSetup(&setup);
    AutoPtr<IView> v;
    String pinStr;
    switch (setup) {
        case IWpsInfo::KEYPAD:
            // if (CWifiP2pService::DBG) Logd("Enter pin section visible");
            textEntryView->FindViewById(Elastos::Droid::R::id::enter_pin_section, (IView**)&v);
            v->SetVisibility(IView::VISIBLE);
            break;
        case IWpsInfo::DISPLAY:
            // if (CWifiP2pService::DBG) Logd("Shown pin section visible");
            wps->GetPin(&pinStr);
            AddRowToDialog(group, Elastos::Droid::R::string::wifi_p2p_show_pin_message, pinStr);
            break;
        default:
            break;
    }

    AutoPtr<IWindow> win;
    dialog->GetWindow((IWindow**)&win);
    win->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    return dialog->Show();
    */
    SendMessage(PEER_CONNECTION_USER_ACCEPT);
    return NOERROR;
}

/**
 * Synchronize the persistent group list between
 * wpa_supplicant and mGroups.
 */
ECode CWifiP2pService::P2pStateMachine::UpdatePersistentNetworks(
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
        ECode ec = StringUtils::ParseInt32((*result)[0], 10, &netId);
        if (FAILED(ec)) {
            Slogger::E(CWifiP2pService::TAG, "NumberFormatException %s", (*result)[0].string());
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
            if (CWifiP2pService::DBG)
                Slogger::D(CWifiP2pService::TAG, "clean up the unused persistent group. netId=%d", netId);
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

Int32 CWifiP2pService::P2pStateMachine::Connect(
    /* [in] */ IWifiP2pConfig* config,
    /* [in] */ Boolean tryInvocation)
{
    if (config == NULL) {
        Loge("config is NULL");
        return CONNECT_FAILURE;
    }

    String addr, devAddr;
    config->GetDeviceAddress(&addr);
    if (mSavedPeerConfig != NULL)
        mSavedPeerConfig->GetDeviceAddress(&devAddr);
    Boolean isResp = (mSavedPeerConfig != NULL && addr.Equals(devAddr));
    mSavedPeerConfig = config;

    AutoPtr<IWifiP2pDevice> dev;
    mPeers->Get(addr, (IWifiP2pDevice**)&dev);
    if (dev == NULL) {
        Slogger::E(CWifiP2pService::TAG, "target device not found %s", addr.string());
        return CONNECT_FAILURE;
    }

    Boolean join;
    dev->IsGroupOwner(&join);
    String address,ssid;
    dev->GetDeviceAddress(&address);
    ssid = mWifiNative->P2pGetSsid(address);
    if (CWifiP2pService::DBG)
        Slogger::D(CWifiP2pService::TAG, "target ssid is %s join %d", ssid.string(), join);

    Boolean b;
    dev->IsGroupLimit(&b);
    if (join && b) {
        if (CWifiP2pService::DBG) Logd("target device reaches group limit.");

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
                return CONNECT_FAILURE;
            }
            return CONNECT_SUCCESS;
        }
    }

    dev->IsDeviceLimit(&b);
    if (!join && b) {
        Loge("target device reaches the device limit.");
        return CONNECT_FAILURE;
    }

    dev->IsInvitationCapable(&b);
    if (!join && tryInvocation && b) {
        Int32 netId = IWifiP2pGroup::PERSISTENT_NET_ID;
        Int32 id;
        config->GetNetId(&id);
        if (id >= 0) {
            String oAddr;
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

        if (CWifiP2pService::DBG)
            Slogger::D("netId related with %s = %d", address.string(), netId);

        if (netId >= 0) {
            // Invoke the persistent group.
            if (mWifiNative->P2pReinvoke(netId, address)) {
                // Save network id. It'll be used when an invitation result event is received.
                mSavedPeerConfig->SetNetId(netId);
                return CONNECT_SUCCESS;
            }
            else {
                Loge("p2pReinvoke() failed, update networks");
                UpdatePersistentNetworks(RELOAD);
                // continue with negotiation
            }
        }
    }

    //Stop discovery before issuing connect
    mWifiNative->P2pStopFind();

    if (!isResp) {
        return NEEDS_PROVISION_REQ;
    }

    P2pConnectWithPinDisplay(config);
    return CONNECT_SUCCESS;
}

Int32 CWifiP2pService::P2pStateMachine::GetNetworkIdFromClientList(
    /* [in] */ const String& deviceAddress)
{
    if (deviceAddress == NULL) return -1;

    AutoPtr<ArrayOf<IWifiP2pGroup*> > groups;
    mGroups->GetGroupList((ArrayOf<IWifiP2pGroup*>**)&groups);
    if (groups != NULL) {
        for(Int32 i = 0; i < groups->GetLength(); i++) {
            Int32 netId;
            (*groups)[i]->GetNetworkId(&netId);
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

AutoPtr<ArrayOf<String> > CWifiP2pService::P2pStateMachine::GetClientList(
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

Boolean CWifiP2pService::P2pStateMachine::RemoveClientFromList(
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
        if (CWifiP2pService::DBG) Logd("Remove unknown network");
        mGroups->Remove(netId);
        return TRUE;
    }

    if (!isClientRemoved) {
        // specified p2p client is not found. already removed.
        return FALSE;
    }

    if (CWifiP2pService::DBG)
        Slogger::D(CWifiP2pService::TAG, "Modified client list: %s",
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

ECode CWifiP2pService::P2pStateMachine::SetWifiP2pInfoOnGroupFormation(
    /* [in] */ const String& serverAddress)
{
    mWifiP2pInfo->SetGroupFormed(TRUE);
    Boolean b;
    mGroup->IsGroupOwner(&b);
    mWifiP2pInfo->SetIsGroupOwner(b);
    AutoPtr<IInetAddress> addr;
    NetworkUtils::NumericToInetAddress(serverAddress, (IInetAddress**)&addr);
    mWifiP2pInfo->SetGroupOwnerAddress(addr);
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::ResetWifiP2pInfo()
{
    mWifiP2pInfo->SetGroupFormed(FALSE);
    mWifiP2pInfo->SetIsGroupOwner(FALSE);
    mWifiP2pInfo->SetGroupOwnerAddress(NULL);
    return NOERROR;
}

String CWifiP2pService::P2pStateMachine::GetDeviceName(
    /* [in] */ const String& deviceAddress)
{
    AutoPtr<IWifiP2pDevice> d;
    mPeers->Get(deviceAddress, (IWifiP2pDevice**)&d);
    if (d != NULL) {
        String name;
        d->GetDeviceName(&name);
            return name;
    }
    //Treat the address as name if there is no match
    return deviceAddress;
}

ECode CWifiP2pService::P2pStateMachine::P2pConnectWithPinDisplay(
    /* [in] */ IWifiP2pConfig* config)
{
    AutoPtr<IWifiP2pDevice> dev;
    String addr;
    config->GetDeviceAddress(&addr);
    mPeers->Get(addr, (IWifiP2pDevice**)&dev);
    if (dev == NULL) {
        Slogger::E(CWifiP2pService::TAG, "target device is not found %s", addr.string());
        return NOERROR;
    }

    Boolean b;
    dev->IsGroupOwner(&b);
    String pin = mWifiNative->P2pConnect(config, b);
    // try {
    Int32 ival;
    ECode ec = StringUtils::ParseInt32(pin, 10, &ival);
    if (SUCCEEDED(ec)) {
        if (!SendShowPinReqToFrontApp(pin)) {
            return NotifyInvitationSent(pin, addr);
        }
    }
    else {
        Slogger::W(CWifiP2pService::TAG, "do nothing if p2pConnect did not return a pin");
    }
    // } catch (NumberFormatException ignore) {
    //     // do nothing if p2pConnect did not return a pin
    // }
    return NOERROR;
}

String CWifiP2pService::P2pStateMachine::GetPersistedDeviceName()
{
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    String deviceName;
    Settings::Global::GetString(cr, ISettingsGlobal::WIFI_P2P_DEVICE_NAME, &deviceName);
    if (deviceName.IsNull()) {
        /* We use the 4 digits of the ANDROID_ID to have a friendly
         * default that has low likelihood of collision with a peer */
        String id;
        Settings::Secure::GetString(cr, ISettingsSecure::ANDROID_ID, &id);
        return String("Android_") + id.Substring(0,4);
    }
    return deviceName;
}

Boolean CWifiP2pService::P2pStateMachine::SetAndPersistDeviceName(
    /* [in] */ const String& devName)
{
    if (devName.IsNull()) return FALSE;

    Boolean b = mWifiNative->SetDeviceName(devName);
    if (!b) {
        Slogger::E(CWifiP2pService::TAG, "Failed to set device name %s", devName.string());
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

Boolean CWifiP2pService::P2pStateMachine::SetWfdInfo(
    /* [in] */ IWifiP2pWfdInfo* wfdInfo)
{
    if (CWifiP2pService::DBG) {
        String info;
        wfdInfo->ToString(&info);
        Slogger::D(CWifiP2pService::TAG, "P2pStateMachine::SetWfdInfo: %s", info.string());
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
            if (CWifiP2pService::DBG) {
                Slogger::D(CWifiP2pService::TAG, "P2pStateMachine::SetWfdDeviceInfo: %s", info.string());
            }
        }
    }

    if (!success) {
        Slogger::E(CWifiP2pService::TAG, "Failed to set wfd properties");
        return FALSE;
    }

    mHost->mThisDevice->SetWfdInfo(wfdInfo);
    SendThisDeviceChangedBroadcast();
    return TRUE;
}

ECode CWifiP2pService::P2pStateMachine::InitializeP2pSettings()
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
    if (CWifiP2pService::DBG) {
        mHost->mThisDevice->GetDeviceAddress(&addr);
        Slogger::D(CWifiP2pService::TAG, "DeviceAddress: %s", addr.string());
    }

    mWifiNative->SetWfdEnable(TRUE); // set wfd enable default

    mHost->mClientInfoList.Clear();
    mWifiNative->P2pFlush();
    mWifiNative->P2pServiceFlush();
    mHost->mServiceTransactionId = 0;
    mHost->mServiceDiscReqId = NULL;

    UpdatePersistentNetworks(RELOAD);
    return NOERROR;
}

Boolean CWifiP2pService::P2pStateMachine::SetGroupOwnerPsk(
    /* [in] */ const String& goPsk)
{
    if (goPsk.IsNull()) return FALSE;

    if (!mWifiNative->SetGroupOwnerPsk(goPsk)) {
        Slogger::E(CWifiP2pService::TAG, "Failed to set GroupOwner psk: [%s]", goPsk.string());
        return FALSE;
    }
    return TRUE;
}

ECode CWifiP2pService::P2pStateMachine::UpdateThisDevice(
    /* [in] */ Int32 status)
{
    mHost->mThisDevice->SetStatus(status);
    SendThisDeviceChangedBroadcast();
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::HandleGroupCreationFailure()
{
    ResetWifiP2pInfo();
    mHost->mNetworkInfo->SetDetailedState(NetworkInfoDetailedState_FAILED, String(NULL), String(NULL));
    SendP2pConnectionChangedBroadcast();
    mSavedPeerConfig = NULL;
    /* After cancelling group formation, new connections on existing peers can fail
     * at supplicant. Flush and restart discovery */
    mWifiNative->P2pFlush();
    Boolean b;
    mPeers->Remove(mPeersLostDuringConnection, &b);
    if (b) SendP2pPeersChangedBroadcast();
    mPeersLostDuringConnection->Clear(&b);
    mHost->mServiceDiscReqId = NULL;
    SendMessage(IWifiP2pManager::DISCOVER_PEERS);
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::HandleGroupRemoved()
{
    AutoPtr<ArrayOf<IWifiP2pDevice*> > devices;
    mGroup->GetClientList((ArrayOf<IWifiP2pDevice*>**)&devices);
    Boolean changed = FALSE;

    AutoPtr<ArrayOf<IWifiP2pDevice*> > list;
    mPeers->GetDeviceList((ArrayOf<IWifiP2pDevice*>**)&list);
    if (list != NULL) {
        for (Int32 i = 0; i < list->GetLength(); i++) {
            AutoPtr<IWifiP2pDevice> d = (*list)[i];
            Boolean b = FALSE;
            if (devices != NULL) {
                b = devices->Contains(d);
            }
            if (!b) {
                AutoPtr<IWifiP2pDevice> owner;
                mGroup->GetOwner((IWifiP2pDevice**)&owner);
                owner->Equals(d, &b);
            }

            if (b) {
                d->SetStatus(IWifiP2pDevice::AVAILABLE);
                changed = TRUE;
            }
        }
    }

    Boolean b;
    mGroup->IsGroupOwner(&b);
    String name;
    mGroup->GetInterface(&name);
    if (b) {
        StopDhcpServer(name);
    }
    else {
        if (CWifiP2pService::DBG) Logd("stop DHCP client");
        mHost->mDhcpStateMachine->SendMessage(DhcpStateMachine::CMD_STOP_DHCP);
        mHost->mDhcpStateMachine->DoQuit();
        mHost->mDhcpStateMachine = NULL;
    }

    // try {
    ECode ec = mHost->mNwService->ClearInterfaceAddresses(name);
    if (FAILED(ec)) {
        Slogger::E(CWifiP2pService::TAG, "Failed to clear addresses %s, ec=%08x", name.string(), ec);
    }
    // } catch (Exception e) {
    //     Loge("Failed to clear addresses " + e);
    // }
    NetworkUtils::ResetConnections(name, NetworkUtils::RESET_ALL_ADDRESSES);

    // Clear any timeout that was set. This is essential for devices
    // that reuse the main p2p interface for a created group.
    mWifiNative->SetP2pGroupIdle(name, 0);

    mGroup = NULL;
    mWifiNative->P2pFlush();
    mPeers->Remove(mPeersLostDuringConnection, &b);
    if (b) SendP2pPeersChangedBroadcast();
    mPeersLostDuringConnection->Clear(&b);
    mHost->mServiceDiscReqId = NULL;
    if (changed) SendP2pPeersChangedBroadcast();

    if (mHost->mTempoarilyDisconnectedWifi) {
        mHost->mWifiChannel->SendMessage(IWifiP2pService::DISCONNECT_WIFI_REQUEST, 0);
        mHost->mTempoarilyDisconnectedWifi = FALSE;
    }
    return NOERROR;
}

//State machine initiated requests can have replyTo set to NULL indicating
//there are no recipients, we ignore those reply actions
ECode CWifiP2pService::P2pStateMachine::ReplyToMessage(
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

ECode CWifiP2pService::P2pStateMachine::ReplyToMessage(
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

ECode CWifiP2pService::P2pStateMachine::ReplyToMessage(
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
AutoPtr<IMessage> CWifiP2pService::P2pStateMachine::ObtainMessage(
    /* [in] */ IMessage* srcMsg)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    Int32 arg2;
    if (srcMsg != NULL)
        srcMsg->GetArg2(&arg2);
    msg->SetArg2(arg2);
    return msg;
}

ECode CWifiP2pService::P2pStateMachine::Logd(
    /* [in] */ const String& s)
{
    Slogger::D("CWifiP2pService::P2pStateMachine: %s", s.string());
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::Loge(
    /* [in] */ const String& s)
{
    Slogger::E("CWifiP2pService::P2pStateMachine: %s", s.string());
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::Logd(
    /* [in] */ const char* s)
{
    Slogger::D("CWifiP2pService::P2pStateMachine: %s", s);
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::Loge(
    /* [in] */ const char* s)
{
    Slogger::E("CWifiP2pService::P2pStateMachine: %s", s);
    return NOERROR;
}

AutoPtr<IWifiP2pDevice> CWifiP2pService::P2pStateMachine::GetWifiP2pDeviceFromPeers(
    /* [in] */ const String& deviceAddress)
{
    AutoPtr<ArrayOf<IWifiP2pDevice*> > list;
    mPeers->GetDeviceList((ArrayOf<IWifiP2pDevice*>**)&list);
    if (list != NULL) {
        String addr;
        for (Int32 i = 0; i < list->GetLength(); ++i) {
            AutoPtr<IWifiP2pDevice> d = (*list)[i];
            d->GetDeviceAddress(&addr);
            if (ComparedMacAddr(addr, deviceAddress)) {
                return d;
            }
        }
    }

    return NULL;
}

Boolean CWifiP2pService::P2pStateMachine::ComparedMacAddr(
    /* [in] */ const String& inputa,
    /* [in] */ const String& inputb)
{
    //42:fc:89:a8:96:09
    if (inputa.IsNull() || inputb.IsNull())
        return FALSE;

    return inputa.Equals(inputb);
}

/**
 * Update service discovery request to wpa_supplicant.
 */
Boolean CWifiP2pService::P2pStateMachine::UpdateSupplicantServiceRequest()
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

ECode CWifiP2pService::P2pStateMachine::ClearSupplicantServiceRequest()
{
    if (mHost->mServiceDiscReqId.IsNull()) return NOERROR;

    mWifiNative->P2pServDiscCancelReq(mHost->mServiceDiscReqId);
    mHost->mServiceDiscReqId = String(NULL);
    return NOERROR;
}

Boolean CWifiP2pService::P2pStateMachine::AddServiceRequest(
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

ECode CWifiP2pService::P2pStateMachine::RemoveServiceRequest(
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
        it->mSecond->Equals(req, &b);
        if (b) {
            removed = TRUE;
            clientInfo->mReqList.Erase(it);
            break;
        }
    }

    if (!removed) return NOERROR;

    if (clientInfo->mReqList.IsEmpty() && clientInfo->mServList.IsEmpty()) {
        if (CWifiP2pService::DBG) Logd("remove client information from framework");
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

ECode CWifiP2pService::P2pStateMachine::ClearServiceRequests(
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
        if (CWifiP2pService::DBG) Logd("remove channel information from framework");
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

Boolean CWifiP2pService::P2pStateMachine::AddLocalService(
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

ECode CWifiP2pService::P2pStateMachine::RemoveLocalService(
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
        if (CWifiP2pService::DBG) Logd("remove client information from framework");
        AutoPtr<IMessenger> messenger;
        clientInfo->GetMessenger((IMessenger**)&messenger);
        HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator ite;
        ite = mHost->mClientInfoList.Find(messenger);
        if (ite != mHost->mClientInfoList.End())
            mHost->mClientInfoList.Erase(ite);
    }
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::ClearLocalServices(
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
        if (CWifiP2pService::DBG) Logd("remove client information from framework");
        AutoPtr<IMessenger> messenger;
        clientInfo->GetMessenger((IMessenger**)&messenger);
        HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator ite;
        ite = mHost->mClientInfoList.Find(messenger);
        if (ite != mHost->mClientInfoList.End())
            mHost->mClientInfoList.Erase(ite);
    }
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::ClearClientInfo(
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
ECode CWifiP2pService::P2pStateMachine::SendServiceResponse(
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
                if (CWifiP2pService::DBG) Logd("detect dead channel");
                return ClearClientInfo(it->mSecond->mMessenger);
            }
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::ClearClientDeadChannels()
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
            if (CWifiP2pService::DBG) Logd("detect dead channel");
            deadClients.PushBack(it->mSecond->mMessenger);
        }
    }

    List<AutoPtr<IMessenger> >::Iterator ite = deadClients.Begin();
    for(; ite != deadClients.End(); ++ite) {
        ClearClientInfo(*ite);
    }

    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::GetClientInfo(
    /* [in] */ IMessenger* m,
    /* [in] */ Boolean createIfNotExist,
    /* [out] */ ClientInfo** result)
{
    AutoPtr<ClientInfo> clientInfo;
    HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator it;
    it = mHost->mClientInfoList.Find(m);
    if (it == mHost->mClientInfoList.End() && createIfNotExist) {
        if (CWifiP2pService::DBG) Logd("add a new client");
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

ECode CWifiP2pService::P2pStateMachine::SendDetachedMsg(
    /* [in] */ Int32 reason)
{
    if (mHost->mForegroundAppMessenger == NULL) return NOERROR;

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    msg->SetWhat(IWifiP2pManager::DIALOG_LISTENER_DETACHED);
    msg->SetArg1(reason);
    // try {
    mHost->mForegroundAppMessenger->Send(msg);
    // } catch (RemoteException e) {
    // }
    mHost->mForegroundAppMessenger = NULL;
    mHost->mForegroundAppPkgName = NULL;
    return NOERROR;
}

Boolean CWifiP2pService::P2pStateMachine::SendShowPinReqToFrontApp(
    /* [in] */ const String& pin)
{
    if (!IsForegroundApp(mHost->mForegroundAppPkgName)) {
        SendDetachedMsg(IWifiP2pManager::NOT_IN_FOREGROUND);
        return FALSE;
    }

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    msg->SetWhat(IWifiP2pManager::SHOW_PIN_REQUESTED);
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutString(IWifiP2pManager::WPS_PIN_BUNDLE_KEY, pin);
    msg->SetData(bundle);
    return SendDialogMsgToFrontApp(msg);
}

Boolean CWifiP2pService::P2pStateMachine::SendConnectNoticeToApp(
    /* [in] */ IWifiP2pDevice* inDev,
    /* [in] */ IWifiP2pConfig* config)
{
    AutoPtr<IWifiP2pDevice> dev = inDev;
    if (dev == NULL) {
        String addr;
        config->GetDeviceAddress(&addr);
        CWifiP2pDevice::New(addr, (IWifiP2pDevice**)&dev);
    }

    if (!IsForegroundApp(mHost->mForegroundAppPkgName)) {
        if (CWifiP2pService::DBG) Logd("application is NOT foreground");
        SendDetachedMsg(IWifiP2pManager::NOT_IN_FOREGROUND);
        return FALSE;
    }

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    msg->SetWhat(IWifiP2pManager::CONNECTION_REQUESTED);
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutParcelable(IWifiP2pManager::P2P_DEV_BUNDLE_KEY, IParcelable::Probe(dev));
    bundle->PutParcelable(IWifiP2pManager::P2P_CONFIG_BUNDLE_KEY, IParcelable::Probe(config));
    msg->SetData(bundle);
    return SendDialogMsgToFrontApp(msg);
}

Boolean CWifiP2pService::P2pStateMachine::SendDialogMsgToFrontApp(
    /* [in] */ IMessage* msg)
{
    if (FAILED(mHost->mForegroundAppMessenger->Send(msg))) {
        mHost->mForegroundAppMessenger = NULL;
        mHost->mForegroundAppPkgName = NULL;
        return FALSE;
    }
    return TRUE;
}

Boolean CWifiP2pService::P2pStateMachine::SetDialogListenerApp(
    /* [in] */ IMessenger* m,
    /* [in] */ const String& appPkgName,
    /* [in] */ Boolean isReset)
{
    if (mHost->mForegroundAppPkgName != String(NULL) && !mHost->mForegroundAppPkgName.Equals(appPkgName)) {
        if (IsForegroundApp(mHost->mForegroundAppPkgName)) {
            // The current dialog listener is foreground app's.
            if (CWifiP2pService::DBG) Logd("application is NOT foreground");
            return FALSE;
        }
        // detach an old listener.
        SendDetachedMsg(IWifiP2pManager::NOT_IN_FOREGROUND);
    }

    if (isReset) {
        if (CWifiP2pService::DBG) Logd("reset dialog listener");
        mHost->mForegroundAppMessenger = NULL;
        mHost->mForegroundAppPkgName = NULL;
        return TRUE;
    }

    if (!IsForegroundApp(appPkgName)) {
        return FALSE;
    }

    mHost->mForegroundAppMessenger = m;
    mHost->mForegroundAppPkgName = appPkgName;
    if (CWifiP2pService::DBG)
        Slogger::E(CWifiP2pService::TAG, "set dialog listener. app= %s", appPkgName.string());
    return TRUE;
}

Boolean CWifiP2pService::P2pStateMachine::IsForegroundApp(
    /* [in] */ const String& pkgName)
{
    if (pkgName.IsNull()) return FALSE;

    AutoPtr<IObjectContainer> container;
    mHost->mActivityMgr->GetRunningTasks(1, (IObjectContainer**)&container);
    if (container == NULL) {
        return FALSE;
    }

    Int32 count;
    container->GetObjectCount(&count);
    if (count == 0) {
        return FALSE;
    }

    AutoPtr<IObjectEnumerator> enumerator;
    container->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    Boolean hasNext = FALSE;
    while (enumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IInterface> current;
        enumerator->Current((IInterface**)&current);
        AutoPtr<IActivityManagerRunningTaskInfo> taskInfo =
            IActivityManagerRunningTaskInfo::Probe(current);

        AutoPtr<IComponentName> name;
        taskInfo->GetBaseActivity((IComponentName**)&name);
        String pName;
        name->GetPackageName(&pName);
        return pkgName.Equals(pName);
    }

    return FALSE;
}

String CWifiP2pService::P2pStateMachine::CmdToString(
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
        case IWifiP2pManager::SET_DIALOG_LISTENER: return String("SET_DIALOG_LISTENER");
        case IWifiP2pManager::DIALOG_LISTENER_DETACHED: return String("DIALOG_LISTENER_DETACHED");
        case IWifiP2pManager::DIALOG_LISTENER_ATTACHED: return String("DIALOG_LISTENER_ATTACHED");
        case IWifiP2pManager::CONNECTION_REQUESTED: return String("CONNECTION_REQUESTED");
        case IWifiP2pManager::SHOW_PIN_REQUESTED: return String("SHOW_PIN_REQUESTED");
        case IWifiP2pManager::DELETE_PERSISTENT_GROUP: return String("DELETE_PERSISTENT_GROUP");
        case IWifiP2pManager::DELETE_PERSISTENT_GROUP_FAILED: return String("DELETE_PERSISTENT_GROUP_FAILED");
        case IWifiP2pManager::DELETE_PERSISTENT_GROUP_SUCCEEDED: return String("DELETE_PERSISTENT_GROUP_SUCCEEDED");
        case IWifiP2pManager::REQUEST_PERSISTENT_GROUP_INFO: return String("REQUEST_PERSISTENT_GROUP_INFO");
        case IWifiP2pManager::RESPONSE_PERSISTENT_GROUP_INFO: return String("RESPONSE_PERSISTENT_GROUP_INFO");
        case IWifiP2pManager::SET_WFD_INFO: return String("SET_WFD_INFO");
        case IWifiP2pManager::SET_WFD_INFO_FAILED: return String("SET_WFD_INFO_FAILED");
        case IWifiP2pManager::SET_WFD_INFO_SUCCEEDED: return String("SET_WFD_INFO_SUCCEEDED");
        case IWifiP2pManager::SET_GO_PSK: return String("SET_GO_PSK");
        case IWifiP2pManager::SET_GO_PSK_FAILED: return String("SET_GO_PSK_FAILED");
        case IWifiP2pManager::SET_GO_PSK_SUCCEEDED: return String("SET_GO_PSK_SUCCEEDED");

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

        case WifiStateMachine::CMD_LOAD_DRIVER: return String("CMD_LOAD_DRIVER");
        case WifiStateMachine::CMD_UNLOAD_DRIVER: return String("CMD_UNLOAD_DRIVER");
        case WifiStateMachine::CMD_LOAD_DRIVER_SUCCESS: return String("CMD_LOAD_DRIVER_SUCCESS");
        case WifiStateMachine::CMD_LOAD_DRIVER_FAILURE: return String("CMD_LOAD_DRIVER_FAILURE");
        case WifiStateMachine::CMD_UNLOAD_DRIVER_SUCCESS: return String("CMD_UNLOAD_DRIVER_SUCCESS");
        case WifiStateMachine::CMD_UNLOAD_DRIVER_FAILURE: return String("CMD_UNLOAD_DRIVER_FAILURE");
        case WifiStateMachine::CMD_START_SUPPLICANT: return String("CMD_START_SUPPLICANT");
        case WifiStateMachine::CMD_STOP_SUPPLICANT: return String("CMD_STOP_SUPPLICANT");
        case WifiStateMachine::CMD_START_DRIVER: return String("CMD_START_DRIVER");
        case WifiStateMachine::CMD_STOP_DRIVER: return String("CMD_STOP_DRIVER");
        case WifiStateMachine::CMD_STATIC_IP_SUCCESS: return String("CMD_STATIC_IP_SUCCESS");
        case WifiStateMachine::CMD_STATIC_IP_FAILURE: return String("CMD_STATIC_IP_FAILURE");
        case WifiStateMachine::CMD_STOP_SUPPLICANT_FAILED: return String("CMD_STOP_SUPPLICANT_FAILED");
        case WifiStateMachine::CMD_DELAYED_STOP_DRIVER: return String("CMD_DELAYED_STOP_DRIVER");
        case WifiStateMachine::CMD_DRIVER_START_TIMED_OUT: return String("CMD_DRIVER_START_TIMED_OUT");
        case WifiStateMachine::CMD_CAPTIVE_CHECK_COMPLETE: return String("CMD_CAPTIVE_CHECK_COMPLETE");
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
        case WifiStateMachine::CMD_SET_SCAN_MODE: return String("CMD_SET_SCAN_MODE");
        case WifiStateMachine::CMD_SET_SCAN_TYPE: return String("CMD_SET_SCAN_TYPE");
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
        case WifiStateMachine::CMD_ENABLE_BACKGROUND_SCAN: return String("CMD_ENABLE_BACKGROUND_SCAN");
        case WifiStateMachine::CMD_RESET_SUPPLICANT_STATE: return String("CMD_RESET_SUPPLICANT_STATE");
        case WifiStateMachine::CMD_ENABLE_P2P: return String("CMD_ENABLE_P2P");
        case WifiStateMachine::CMD_DISABLE_P2P_REQ: return String("CMD_DISABLE_P2P_REQ");
        case WifiStateMachine::CMD_DISABLE_P2P_RSP: return String("CMD_DISABLE_P2P_RSP");

        case CWifiP2pService::GROUP_CREATING_TIMED_OUT: return String("GROUP_CREATING_TIMED_OUT");
        case CWifiP2pService::PEER_CONNECTION_USER_ACCEPT: return String("PEER_CONNECTION_USER_ACCEPT");
        case CWifiP2pService::PEER_CONNECTION_USER_REJECT: return String("PEER_CONNECTION_USER_REJECT");
        case CWifiP2pService::DROP_WIFI_USER_ACCEPT: return String("DROP_WIFI_USER_ACCEPT");
        case CWifiP2pService::DROP_WIFI_USER_REJECT: return String("DROP_WIFI_USER_REJECT");
        case CWifiP2pService::DISABLE_P2P_TIMED_OUT: return String("DISABLE_P2P_TIMED_OUT");
        case CWifiP2pService::P2P_CONNECTION_CHANGED: return String("P2P_CONNECTION_CHANGED");
        case CWifiP2pService::DISCONNECT_WIFI_REQUEST: return String("DISCONNECT_WIFI_REQUEST");
        case CWifiP2pService::DISCONNECT_WIFI_RESPONSE: return String("DISCONNECT_WIFI_RESPONSE");
    }
    return String("unknown cmd.");
}

//==================================================================
// CWifiP2pService::P2pStateMachine::DefaultState
//==================================================================
CWifiP2pService::P2pStateMachine::DefaultState::DefaultState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::DefaultState::ProcessMessage(
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

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
        case AsyncChannel::CMD_CHANNEL_HALF_CONNECTED:
            if (arg1 == AsyncChannel::STATUS_SUCCESSFUL) {
                if (CWifiP2pService::DBG)
                    Slogger::D(CWifiP2pService::TAG, "Full connection with WifiStateMachine established");
                mHost->mHost->mWifiChannel = (AsyncChannel*)obj.Get();
            }
            else {
                Slogger::E(CWifiP2pService::TAG, "Full connection failure, error = %d", arg1);
                mHost->mHost->mWifiChannel = NULL;
            }
            break;

        case AsyncChannel::CMD_CHANNEL_DISCONNECTED:
            if (arg1 == AsyncChannel::STATUS_SEND_UNSUCCESSFUL) {
                Slogger::E(CWifiP2pService::TAG, "Send failed, client connection lost");
            }
            else {
                Slogger::E(CWifiP2pService::TAG, "Client connection lost with reason: %d", arg1);
            }
            mHost->mHost->mWifiChannel = NULL;
            break;

        case AsyncChannel::CMD_CHANNEL_FULL_CONNECTION: {
            AutoPtr<AsyncChannel> ac = new AsyncChannel();
            AutoPtr<IHandler> handler = mHost->GetHandler();
            ac->Connect(mHost->mHost->mContext, handler, replyTo);
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
        case IWifiP2pManager::SET_GO_PSK:
            mHost->ReplyToMessage(message, IWifiP2pManager::SET_GO_PSK_FAILED,
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
        case IWifiP2pManager::SET_DIALOG_LISTENER: {
            AutoPtr<IBundle> bundle;
            message->GetData((IBundle**)&bundle);
            String appPkgName;
            bundle->GetString(IWifiP2pManager::APP_PKG_BUNDLE_KEY, &appPkgName);
            Boolean isReset;
            bundle->GetBoolean(IWifiP2pManager::RESET_DIALOG_LISTENER_BUNDLE_KEY, &isReset);
            if (mHost->SetDialogListenerApp(replyTo, appPkgName, isReset)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::DIALOG_LISTENER_ATTACHED);
            }
            else {
                mHost->ReplyToMessage(message, IWifiP2pManager::DIALOG_LISTENER_DETACHED,
                    IWifiP2pManager::NOT_IN_FOREGROUND);
            }
            break;
        }
            // Ignore
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
        case DhcpStateMachine::CMD_PRE_DHCP_ACTION:
        case DhcpStateMachine::CMD_POST_DHCP_ACTION:
        case DhcpStateMachine::CMD_ON_QUIT:
        case WifiMonitor::P2P_PROV_DISC_FAILURE_EVENT:
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
            Slogger::E(CWifiP2pService::TAG, "Unexpected group creation, remove %p", mHost->mGroup.Get());
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
            Slogger::E(CWifiP2pService::TAG, "Unhandled message %d, %s", what, P2pStateMachine::CmdToString(what).string());
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }

    *result = TRUE;  // HANDLED;
    return NOERROR;
}

//==================================================================
// CWifiP2pService::P2pStateMachine::P2pNotSupportedState
//==================================================================
CWifiP2pService::P2pStateMachine::P2pNotSupportedState::P2pNotSupportedState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::P2pNotSupportedState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
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
        case IWifiP2pManager::SET_DIALOG_LISTENER:
            mHost->ReplyToMessage(message, IWifiP2pManager::DIALOG_LISTENER_DETACHED,
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
        case IWifiP2pManager::SET_GO_PSK:
            mHost->ReplyToMessage(message, IWifiP2pManager::SET_GO_PSK_FAILED,
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
// CWifiP2pService::P2pStateMachine::P2pDisablingState
//==================================================================
CWifiP2pService::P2pStateMachine::P2pDisablingState::P2pDisablingState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::P2pDisablingState::Enter()
{
    if (CWifiP2pService::DBG)
        Slogger::D(CWifiP2pService::TAG, " Enter %s", GetName().string());
    AutoPtr<IMessage> msg = mHost->ObtainMessage(DISABLE_P2P_TIMED_OUT,
        ++mDisableP2pTimeoutIndex, 0);
    mHost->SendMessageDelayed(msg, DISABLE_P2P_WAIT_TIME_MS);
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::P2pDisablingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);
    Int32 arg1;
    message->GetArg1(&arg1);

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
        case WifiMonitor::SUP_DISCONNECTION_EVENT:
            if (CWifiP2pService::DBG) mHost->Logd("p2p socket connection lost");
            mHost->TransitionTo(mHost->mP2pDisabledState);
            break;
        case WifiStateMachine::CMD_ENABLE_P2P:
        case WifiStateMachine::CMD_DISABLE_P2P_REQ:
            mHost->DeferMessage(message);
            break;
        case DISABLE_P2P_TIMED_OUT:
            if (mHost->mHost->mGroupCreatingTimeoutIndex == arg1) {
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

ECode CWifiP2pService::P2pStateMachine::P2pDisablingState::Exit()
{
    mHost->mHost->mWifiChannel->SendMessage(WifiStateMachine::CMD_DISABLE_P2P_RSP);
    return NOERROR;
}

//==================================================================
// CWifiP2pService::P2pStateMachine::P2pDisabledState
//==================================================================
CWifiP2pService::P2pStateMachine::P2pDisabledState::P2pDisabledState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::P2pDisabledState::Enter()
{
    if (CWifiP2pService::DBG) Slogger::D(CWifiP2pService::TAG, "Enter %s", GetName().string());
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::P2pDisabledState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
        case WifiStateMachine::CMD_ENABLE_P2P: {
            // try {
            ECode ec = mHost->mHost->mNwService->SetInterfaceUp(mHost->mHost->mInterface);
            if (FAILED(ec)) {
                Slogger::E(CWifiP2pService::TAG, "Unable to change interface settings: %08x", ec);
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
// CWifiP2pService::P2pStateMachine::P2pEnablingState
//==================================================================
CWifiP2pService::P2pStateMachine::P2pEnablingState::P2pEnablingState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::P2pEnablingState::Enter()
{
    if (CWifiP2pService::DBG) Slogger::D(CWifiP2pService::TAG, "Enter %s", GetName().string());
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::P2pEnablingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
        case WifiMonitor::SUP_CONNECTION_EVENT:
            if (CWifiP2pService::DBG) mHost->Logd("P2p socket connection successful");
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
// CWifiP2pService::P2pStateMachine::P2pEnabledState
//==================================================================
CWifiP2pService::P2pStateMachine::P2pEnabledState::P2pEnabledState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::P2pEnabledState::Enter()
{
    if (CWifiP2pService::DBG) Slogger::D(CWifiP2pService::TAG, "Enter %s", GetName().string());
    mHost->SendP2pStateChangedBroadcast(TRUE);
    mHost->mHost->mNetworkInfo->SetIsAvailable(TRUE);
    mHost->SendP2pConnectionChangedBroadcast();
    mHost->InitializeP2pSettings();
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::P2pEnabledState::ProcessMessage(
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

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
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
            if (b) mHost->SendP2pPeersChangedBroadcast();
            mHost->mGroups->Clear(&b);
            if (b) mHost->SendP2pPersistentGroupsChangedBroadcast();

            mHost->mWifiNative->CloseSupplicantConnection();
            mHost->TransitionTo(mHost->mP2pDisablingState);
            break;
        case IWifiP2pManager::SET_DEVICE_NAME:
        {
            AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(obj);
            if (device != NULL) device->GetDeviceName(&name);
            if (device != NULL && mHost->SetAndPersistDeviceName(name)) {
                if (CWifiP2pService::DBG)
                    Slogger::D(CWifiP2pService::TAG, "P2pEnabledState set device name %s", name.string());
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

            if (CWifiP2pService::DBG) {
                String temp("NULL");
                if (wfdInfo) wfdInfo->ToString(&temp);
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
        case IWifiP2pManager::SET_GO_PSK:
        {
            AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(obj);
            if (device) device->GetDeviceName(&name);
            if (!name.IsNull() && (name.GetLength() < 13) && (name.GetLength() > 7)
                && mHost->SetGroupOwnerPsk(name)) {
                if (CWifiP2pService::DBG)
                    Slogger::D(CWifiP2pService::TAG, "set GroupOnwer psk to [%s]", name.string());
                mHost->ReplyToMessage(message, IWifiP2pManager::SET_GO_PSK_SUCCEEDED);
            }
            else {
                mHost->ReplyToMessage(message, IWifiP2pManager::SET_GO_PSK_FAILED,
                    IWifiP2pManager::ERROR);
            }
            break;
        }
        case IWifiP2pManager::DISCOVER_PEERS:
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
            if (CWifiP2pService::DBG) mHost->Logd("P2pEnabledState discover services");
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

            if (CWifiP2pService::DBG) {
                String temp;
                device->ToString(&temp);
                String info("P2pEnabledState found device:\n");
                info += temp;
                mHost->Logd(info.string());
            }

            mHost->mHost->mThisDevice->GetDeviceAddress(&addr);
            device->GetDeviceAddress(&devAddr);
            if (addr.Equals(devAddr)) break;
            mHost->mPeers->Update(device);
            mHost->SendP2pPeersChangedBroadcast();
            break;
        }
        case WifiMonitor::P2P_DEVICE_LOST_EVENT: {
            AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(obj);
            mHost->mPeers->Remove(device, &b);
            if (b) mHost->SendP2pPeersChangedBroadcast();
            break;
        }
        case IWifiP2pManager::ADD_LOCAL_SERVICE: {
            if (CWifiP2pService::DBG) mHost->Logd("P2pEnabledState add service");
            AutoPtr<IWifiP2pServiceInfo> servInfo = IWifiP2pServiceInfo::Probe(obj);
            if (mHost->AddLocalService(replyTo, servInfo)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::ADD_LOCAL_SERVICE_SUCCEEDED);
            } else {
                mHost->ReplyToMessage(message, IWifiP2pManager::ADD_LOCAL_SERVICE_FAILED);
            }
            break;
        }
        case IWifiP2pManager::REMOVE_LOCAL_SERVICE: {
            if (CWifiP2pService::DBG) mHost->Logd("P2pEnabledState remove service");
            AutoPtr<IWifiP2pServiceInfo> servInfo = IWifiP2pServiceInfo::Probe(obj);
            mHost->RemoveLocalService(replyTo, servInfo);
            mHost->ReplyToMessage(message, IWifiP2pManager::REMOVE_LOCAL_SERVICE_SUCCEEDED);
            break;
        }
        case IWifiP2pManager::CLEAR_LOCAL_SERVICES:
            if (CWifiP2pService::DBG) mHost->Logd("P2pEnabledState clear service");
            mHost->ClearLocalServices(replyTo);
            mHost->ReplyToMessage(message, IWifiP2pManager::CLEAR_LOCAL_SERVICES_SUCCEEDED);
            break;
        case IWifiP2pManager::ADD_SERVICE_REQUEST: {
            if (CWifiP2pService::DBG) mHost->Logd("P2pEnabledState add service request");
            AutoPtr<IWifiP2pServiceRequest> request = IWifiP2pServiceRequest::Probe(obj);
            if (!mHost->AddServiceRequest(replyTo, request)) {
                mHost->ReplyToMessage(message, IWifiP2pManager::ADD_SERVICE_REQUEST_FAILED);
                break;
            }
            mHost->ReplyToMessage(message, IWifiP2pManager::ADD_SERVICE_REQUEST_SUCCEEDED);
            break;
        }
        case IWifiP2pManager::REMOVE_SERVICE_REQUEST: {
            if (CWifiP2pService::DBG) mHost->Logd("P2pEnabledState remove service request");
            AutoPtr<IWifiP2pServiceRequest> request = IWifiP2pServiceRequest::Probe(obj);
            mHost->RemoveServiceRequest(replyTo, request);
            mHost->ReplyToMessage(message, IWifiP2pManager::REMOVE_SERVICE_REQUEST_SUCCEEDED);
            break;
        }
        case IWifiP2pManager::CLEAR_SERVICE_REQUESTS:
            if (CWifiP2pService::DBG) mHost->Logd("P2pEnabledState clear service request");
            mHost->ClearServiceRequests(replyTo);
            mHost->ReplyToMessage(message, IWifiP2pManager::CLEAR_SERVICE_REQUESTS_SUCCEEDED);
            break;
        case WifiMonitor::P2P_SERV_DISC_RESP_EVENT: {
            if (CWifiP2pService::DBG) mHost->Logd("P2pEnabledState receive service response");
            AutoPtr<IObjectContainer> container = IObjectContainer::Probe(obj);
            assert(obj != NULL);
            AutoPtr<IObjectEnumerator> enumerator;
            container->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
            Boolean hasNext = FALSE;
            AutoPtr<IWifiP2pServiceResponse> resp;
            AutoPtr<IWifiP2pDevice> device;
            while (enumerator->MoveNext(&hasNext), hasNext) {
                AutoPtr<IInterface> current;
                enumerator->Current((IInterface**)&current);
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
            if (CWifiP2pService::DBG) mHost->Logd("P2pEnabledState delete persistent group");
            mHost->mGroups->Remove(arg1);
            mHost->ReplyToMessage(message, IWifiP2pManager::DELETE_PERSISTENT_GROUP_SUCCEEDED);
            break;
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }
    *result = TRUE;  // HANDLED;
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::P2pEnabledState::Exit()
{
    mHost->SendP2pStateChangedBroadcast(FALSE);
    mHost->mHost->mNetworkInfo->SetIsAvailable(FALSE);
    return NOERROR;
}

//==================================================================
// CWifiP2pService::P2pStateMachine::InactiveState
//==================================================================
CWifiP2pService::P2pStateMachine::InactiveState::InactiveState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::InactiveState::Enter()
{
    if (CWifiP2pService::DBG) Slogger::D(CWifiP2pService::TAG, "Enter %s", GetName().string());
    //Start listening every time we get inactive
    //TODO: Fix listen after driver behavior is fixed
    //mHost->mWifiNative->P2pListen();
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::InactiveState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    String addr;
    String devAddr;
    String address;
    String name;
    Int32 id;
    Int32 gc;
    Int32 connectRet;
    Int32 netId;
    Boolean ret = FALSE;
    Boolean b;
    Boolean b1,b2,b3;

    switch (what) {
        case IWifiP2pManager::CONNECT: {
            if (CWifiP2pService::DBG) mHost->Logd("InactiveState  sending connect");
            AutoPtr<IWifiP2pConfig> config = IWifiP2pConfig::Probe(obj);
            mHost->mHost->mAutonomousGroup = FALSE;

            /* Update group capability before connect */
            config->GetDeviceAddress(&addr);
            gc = mHost->mWifiNative->GetGroupCapability(addr);
            mHost->mPeers->UpdateGroupCapability(addr, gc);
            connectRet = mHost->Connect(config, TRY_REINVOCATION);
            if (connectRet == CONNECT_FAILURE) {
                mHost->ReplyToMessage(message, IWifiP2pManager::CONNECT_FAILED);
                break;
            }
            mHost->mSavedPeerConfig->GetDeviceAddress(&devAddr);
            mHost->mPeers->UpdateStatus(devAddr, IWifiP2pDevice::INVITED);

            //1. add start,user kick to connect
            mHost->mWifiP2pDevice = mHost->GetWifiP2pDeviceFromPeers(devAddr);
            //add end

            mHost->SendP2pPeersChangedBroadcast();
            mHost->ReplyToMessage(message, IWifiP2pManager::CONNECT_SUCCEEDED);
            if (connectRet == NEEDS_PROVISION_REQ) {
                if (CWifiP2pService::DBG) mHost->Logd("Sending prov disc");
                mHost->TransitionTo(mHost->mProvisionDiscoveryState);
                break;
            }
            mHost->TransitionTo(mHost->mGroupNegotiationState);
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
            mHost->mSavedPeerConfig = IWifiP2pConfig::Probe(obj);
            mHost->mHost->mAutonomousGroup = FALSE;
            mHost->mHost->mJoinExistingGroup = FALSE;
            mHost->mSavedPeerConfig->GetDeviceAddress(&devAddr);
            AutoPtr<IWifiP2pDevice> dev;
            mHost->mPeers->Get(devAddr, (IWifiP2pDevice**)&dev);
            if (!mHost->SendConnectNoticeToApp(dev, mHost->mSavedPeerConfig)) {
                mHost->TransitionTo(mHost->mUserAuthorizingInvitationState);
            }
            break;
        }
        case WifiMonitor::P2P_INVITATION_RECEIVED_EVENT: {
            AutoPtr<IWifiP2pGroup> group = IWifiP2pGroup::Probe(obj);
            AutoPtr<IWifiP2pDevice> owner;
            group->GetOwner((IWifiP2pDevice**)&owner);
            if (owner == NULL) {
                if (CWifiP2pService::DBG) mHost->Loge("Ignored invitation from NULL owner");
                break;
            }

            mHost->mSavedPeerConfig = NULL;
            CWifiP2pConfig::New((IWifiP2pConfig**)&mHost->mSavedPeerConfig);
            owner->GetDeviceAddress(&address);
            mHost->mSavedPeerConfig->SetDeviceAddress(address);

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
            //TODO In the p2p client case, we should set source address correctly.
            mHost->mSavedPeerConfig->GetDeviceAddress(&devAddr);
            AutoPtr<IWifiP2pDevice> dev;
            mHost->mPeers->Get(devAddr, (IWifiP2pDevice**)&dev);
            if (!mHost->SendConnectNoticeToApp(dev, mHost->mSavedPeerConfig)) {
                mHost->TransitionTo(mHost->mUserAuthorizingInvitationState);
            }
            break;
        }
        case WifiMonitor::P2P_PROV_DISC_PBC_REQ_EVENT:
        case WifiMonitor::P2P_PROV_DISC_ENTER_PIN_EVENT:
        case WifiMonitor::P2P_PROV_DISC_SHOW_PIN_EVENT:
            //We let the supplicant handle the provision discovery response
            //and wait instead for the GO_NEGOTIATION_REQUEST_EVENT.
            //Handling provision discovery and issuing a p2p_connect before
            //group negotiation comes through causes issues
           break;
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
            if (CWifiP2pService::DBG) mHost->Logd("InactiveState  group started");

            // We hit this scenario when a persistent group is reinvoked
            mHost->mGroup->GetNetworkId(&id);
            if (id == IWifiP2pGroup::PERSISTENT_NET_ID) {
                mHost->mHost->mAutonomousGroup = FALSE;
                mHost->DeferMessage(message);
                mHost->TransitionTo(mHost->mGroupNegotiationState);
            }
            else {
                String info;
                mHost->mGroup->ToString(&info);
                Slogger::E(CWifiP2pService::TAG, "Unexpected group creation, remove %s", info.string());
                mHost->mGroup->GetInterface(&name);
                mHost->mWifiNative->P2pGroupRemove(name);
            }
            break;
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }
    *result = TRUE;  // HANDLED;
    return NOERROR;
}

//==================================================================
// CWifiP2pService::P2pStateMachine::GroupCreatingState
//==================================================================
CWifiP2pService::P2pStateMachine::GroupCreatingState::GroupCreatingState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::GroupCreatingState::Enter()
{
    if (CWifiP2pService::DBG) Slogger::D(CWifiP2pService::TAG, "Enter %s", GetName().string());
    AutoPtr<IMessage> msg = mHost->ObtainMessage(GROUP_CREATING_TIMED_OUT,
        ++mGroupCreatingTimeoutIndex, 0);
    mHost->SendMessageDelayed(msg, GROUP_CREATING_WAIT_TIME_MS);
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::GroupCreatingState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Boolean ret = TRUE;  // HANDLED;
    Int32 what, arg1;
    message->GetWhat(&what);
    message->GetArg1(&arg1);
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    String addr;
    String devAddr;

    switch (what) {
        case GROUP_CREATING_TIMED_OUT:
            if (mGroupCreatingTimeoutIndex == arg1) {
                if (CWifiP2pService::DBG) mHost->Logd("Group negotiation timed out");
                mHost->HandleGroupCreationFailure();
                mHost->TransitionTo(mHost->mInactiveState);
            }
            break;
        case WifiMonitor::P2P_DEVICE_LOST_EVENT: {
            AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(obj);
            device->GetDeviceAddress(&addr);
            // If we lose a device during an autonomous group creation,
            // mHost->mSavedPeerConfig can be empty
            if (mHost->mSavedPeerConfig != NULL) {
                mHost->mSavedPeerConfig->GetDeviceAddress(&devAddr);
                if(!devAddr.Equals(addr)) {
                    if (CWifiP2pService::DBG) {
                        Slogger::D("mHost->mSavedPeerConfig %s, device %s", devAddr.string(), addr.string());
                    }
                    // Do the regular device lost handling
                    ret = FALSE;  // NOT_HANDLED;
                }
            }
            // Do nothing
            if (CWifiP2pService::DBG) Slogger::D(CWifiP2pService::TAG, "Add device to lost list %d", device.Get());
            mHost->mPeersLostDuringConnection->Update(device);
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
        default:
            ret = FALSE;  // NOT_HANDLED;
            break;
    }

    *result = ret;
    return NOERROR;
}

//==================================================================
// CWifiP2pService::P2pStateMachine::UserAuthorizingInvitationState
//==================================================================
CWifiP2pService::P2pStateMachine::UserAuthorizingInvitationState::UserAuthorizingInvitationState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::UserAuthorizingInvitationState::Enter()
{
    if (CWifiP2pService::DBG) Slogger::D(CWifiP2pService::TAG, "Enter %s", GetName().string());
    mHost->NotifyInvitationReceived();
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::UserAuthorizingInvitationState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    *result = TRUE;  // HANDLED;
    Int32 what;
    message->GetWhat(&what);

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    String addr;
    switch (what) {
        case PEER_CONNECTION_USER_ACCEPT:
            if (mHost->Connect(mHost->mSavedPeerConfig, TRY_REINVOCATION) == CONNECT_FAILURE) {
                mHost->HandleGroupCreationFailure();
                mHost->TransitionTo(mHost->mInactiveState);
                break;
            }
            mHost->mSavedPeerConfig->GetDeviceAddress(&addr);
            mHost->mPeers->UpdateStatus(addr, IWifiP2pDevice::INVITED);
            //2.add start,
            mHost->mWifiP2pDevice = mHost->GetWifiP2pDeviceFromPeers(addr);
            //add end
            mHost->SendP2pPeersChangedBroadcast();
            mHost->TransitionTo(mHost->mGroupNegotiationState);
            break;
        case PEER_CONNECTION_USER_REJECT:
            if (CWifiP2pService::DBG) {
                String info;
                if (mHost->mSavedPeerConfig) mHost->mSavedPeerConfig->ToString(&info);
                Slogger::D(CWifiP2pService::TAG, "User rejected invitation %s", info.string());
            }
            mHost->mSavedPeerConfig = NULL;
            mHost->TransitionTo(mHost->mInactiveState);
            break;
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::UserAuthorizingInvitationState::Exit()
{
    //TODO: dismiss dialog if not already done
    return NOERROR;
}

//==================================================================
// CWifiP2pService::P2pStateMachine::ProvisionDiscoveryState
//==================================================================
CWifiP2pService::P2pStateMachine::ProvisionDiscoveryState::ProvisionDiscoveryState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::ProvisionDiscoveryState::Enter()
{
    if (CWifiP2pService::DBG) Slogger::D(CWifiP2pService::TAG, "Enter %s", GetName().string());
    mHost->mWifiNative->P2pProvisionDiscovery(mHost->mSavedPeerConfig);
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::ProvisionDiscoveryState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
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
                if (CWifiP2pService::DBG)
                    Slogger::D(CWifiP2pService::TAG, "Found a match %p", mHost->mSavedPeerConfig.Get());
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
                if (CWifiP2pService::DBG)
                    Slogger::D(CWifiP2pService::TAG, "Found a match %p", mHost->mSavedPeerConfig.Get());
                /* we already have the pin */
                wps->GetPin(&pin);
                if (!pin.IsEmpty()) {
                    mHost->P2pConnectWithPinDisplay(mHost->mSavedPeerConfig);
                    mHost->TransitionTo(mHost->mGroupNegotiationState);
                }
                else {
                    mHost->mHost->mJoinExistingGroup = FALSE;
                    AutoPtr<IWifiP2pDevice> dev;
                    mHost->mPeers->Get(devAddr, (IWifiP2pDevice**)&dev);
                    if (!mHost->SendConnectNoticeToApp(dev, mHost->mSavedPeerConfig)) {
                        mHost->TransitionTo(mHost->mUserAuthorizingInvitationState);
                    }
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
                if (CWifiP2pService::DBG)
                    Slogger::D(CWifiP2pService::TAG, "Found a match %p", mHost->mSavedPeerConfig.Get());
                provDisc->GetPin(&pin);
                wps->SetPin(pin);
                mHost->P2pConnectWithPinDisplay(mHost->mSavedPeerConfig);
                if (!mHost->SendShowPinReqToFrontApp(pin)) {
                    mHost->NotifyInvitationSent(pin, addr);
                }
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
// CWifiP2pService::P2pStateMachine::GroupNegotiationState
//==================================================================
CWifiP2pService::P2pStateMachine::GroupNegotiationState::GroupNegotiationState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::GroupNegotiationState::Enter()
{
    if (CWifiP2pService::DBG) Slogger::D(CWifiP2pService::TAG, "Enter %s", GetName().string());
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::GroupNegotiationState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
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
            if (CWifiP2pService::DBG) mHost->Logd("GroupNegotiationState go success");
            break;
        case WifiMonitor::P2P_GROUP_STARTED_EVENT:
            mHost->mGroup = IWifiP2pGroup::Probe(obj);
            if (CWifiP2pService::DBG) mHost->Logd("GroupNegotiationState group started");
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
                mHost->StartDhcpServer(name);
            }
            else {
                mHost->mWifiNative->SetP2pGroupIdle(name, GROUP_IDLE_TIME_S);
                mHost->mHost->mDhcpStateMachine = DhcpStateMachine::MakeDhcpStateMachine(
                    mHost->mHost->mContext, mHost, name);
                mHost->mHost->mDhcpStateMachine->SendMessage(DhcpStateMachine::CMD_START_DHCP);
                AutoPtr<IWifiP2pDevice> groupOwner;
                mHost->mGroup->GetOwner((IWifiP2pDevice**)&groupOwner);
                /* update group owner details with the ones found at discovery */
                groupOwner->GetDeviceAddress(&addr);
                AutoPtr<IWifiP2pDevice> dev;
                mHost->mPeers->Get(addr, (IWifiP2pDevice**)&dev);
                groupOwner->Update(dev);
                mHost->mPeers->UpdateStatus(addr, IWifiP2pDevice::CONNECTED);
                //3.add start,user kick to connect
                mHost->mWifiP2pDevice = mHost->GetWifiP2pDeviceFromPeers(addr);
                //add end
                mHost->SendP2pPeersChangedBroadcast();
            }
            mHost->mSavedPeerConfig = NULL;
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
            if (CWifiP2pService::DBG) mHost->Logd("GroupNegotiationState go failure");
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
            else if (status == P2pStatus_UNKNOWN_P2P_GROUP) {
                // target device has already removed the credential.
                // So, remove this credential accordingly.
                mHost->mSavedPeerConfig->GetNetId(&netId);
                if (netId >= 0) {
                    if (CWifiP2pService::DBG) mHost->Logd("Remove unknown client from the list");
                    mHost->mSavedPeerConfig->GetDeviceAddress(&addr);
                    mHost->RemoveClientFromList(netId, addr, TRUE);
                }

                // invocation is failed or deferred. Try another way to connect.
                mHost->mSavedPeerConfig->SetNetId(IWifiP2pGroup::PERSISTENT_NET_ID);
                if (mHost->Connect(mHost->mSavedPeerConfig, NO_REINVOCATION) == CONNECT_FAILURE) {
                    mHost->HandleGroupCreationFailure();
                    mHost->TransitionTo(mHost->mInactiveState);
                }
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
// CWifiP2pService::P2pStateMachine::PositiveButtonListener
//==================================================================
CAR_INTERFACE_IMPL(CWifiP2pService::P2pStateMachine::PositiveButtonListener, IDialogInterfaceOnClickListener)

CWifiP2pService::P2pStateMachine::PositiveButtonListener::PositiveButtonListener(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{}

ECode CWifiP2pService::P2pStateMachine::PositiveButtonListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mHost->SendMessage(CWifiP2pService::DROP_WIFI_USER_ACCEPT);
    return NOERROR;
}

//==================================================================
// CWifiP2pService::P2pStateMachine::NegativeButtonListener
//==================================================================
CAR_INTERFACE_IMPL(CWifiP2pService::P2pStateMachine::NegativeButtonListener, IDialogInterfaceOnClickListener)

CWifiP2pService::P2pStateMachine::NegativeButtonListener::NegativeButtonListener(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{}

ECode CWifiP2pService::P2pStateMachine::NegativeButtonListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mHost->SendMessage(CWifiP2pService::DROP_WIFI_USER_REJECT);
    return NOERROR;
}

//==================================================================
// CWifiP2pService::P2pStateMachine::DialogCancelListener
//==================================================================
CAR_INTERFACE_IMPL(CWifiP2pService::P2pStateMachine::DialogCancelListener, IDialogInterfaceOnCancelListener)

CWifiP2pService::P2pStateMachine::DialogCancelListener::DialogCancelListener(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{}

ECode CWifiP2pService::P2pStateMachine::DialogCancelListener::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    mHost->SendMessage(CWifiP2pService::DROP_WIFI_USER_REJECT);
    return NOERROR;
}

//==================================================================
// CWifiP2pService::P2pStateMachine::FrequencyConflictState
//==================================================================
CWifiP2pService::P2pStateMachine::FrequencyConflictState::FrequencyConflictState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::FrequencyConflictState::Enter()
{
    if (CWifiP2pService::DBG) Slogger::D(CWifiP2pService::TAG, "Enter %s", GetName().string());
    NotifyFrequencyConflict();
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::FrequencyConflictState::NotifyFrequencyConflict()
{
    mHost->Logd("Notify frequency conflict");
    AutoPtr<IResources> r = CResources::GetSystem();

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
    dialog->GetWindow((IWindow**)&win);
    win->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    dialog->Show();
    mFrequencyConflictDialog = dialog;
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::FrequencyConflictState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
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
            mHost->mHost->mWifiChannel->SendMessage(IWifiP2pService::DISCONNECT_WIFI_REQUEST, 1);
            mHost->mHost->mTempoarilyDisconnectedWifi = TRUE;
            break;
        case DISCONNECT_WIFI_RESPONSE:
            // Got a response from wifistatemachine, retry p2p
            if (CWifiP2pService::DBG) mHost->Logd("FrequencyConflictState Wifi disconnected, retry p2p");
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

ECode CWifiP2pService::P2pStateMachine::FrequencyConflictState::Exit()
{
    if (mFrequencyConflictDialog != NULL) {
        mFrequencyConflictDialog->Dismiss();
        mFrequencyConflictDialog = NULL;
    }
    return NOERROR;
}

//==================================================================
// CWifiP2pService::P2pStateMachine::GroupCreatedState
//==================================================================
CWifiP2pService::P2pStateMachine::GroupCreatedState::GroupCreatedState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::GroupCreatedState::Enter()
{
    if (CWifiP2pService::DBG) Slogger::D(CWifiP2pService::TAG, "Enter %s", GetName().string());
    String nullStr;
    mHost->mHost->mNetworkInfo->SetDetailedState(
        Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED, nullStr, nullStr);

    mHost->UpdateThisDevice(IWifiP2pDevice::CONNECTED);

    //DHCP server has already been started if I am a group owner
    Boolean b;
    mHost->mGroup->IsGroupOwner(&b);
    if (b) {
        // mHost->SetWifiP2pInfoOnGroupFormation(SERVER_ADDRESS);
        String serverAddr;
        if (mHost->mHost->mAutonomousGroup)
            serverAddr = CWifiP2pService::SERVER_ADDRESS_TETHER;
        else
            serverAddr = CWifiP2pService::SERVER_ADDRESS;
        mHost->SetWifiP2pInfoOnGroupFormation(serverAddr);
    }

    // In case of a negotiation group, connection changed is sent
    // after a client joins. For autonomous, send now
    if (mHost->mHost->mAutonomousGroup) {
        mHost->SendP2pConnectionChangedBroadcast();
    }
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::GroupCreatedState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what, arg1;
    message->GetWhat(&what);
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    message->GetArg1(&arg1);

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    String deviceAddress;
    String devAddr;
    String addr;
    String name;
    String pin;
    String emptyStr("");
    Int32 setup;
    Int32 netId;
    Boolean b;

    switch (what) {
        case WifiMonitor::AP_STA_CONNECTED_EVENT: {
            AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(obj);
            device->GetDeviceAddress(&deviceAddress);
            Boolean notP2pDevice = TRUE;
            if (!deviceAddress.IsNull()) {
                if (mHost->mSavedProvDiscDevice != NULL) {
                    mHost->mSavedProvDiscDevice->GetDeviceAddress(&devAddr);
                    if (deviceAddress.Equals(devAddr))  {
                        mHost->mSavedProvDiscDevice = NULL;
                        notP2pDevice = FALSE;
                        String devInfo;
                        device->ToString(&devInfo);
                        mHost->Loge("GroupCreatedState: p2p device connect action............! device info:");
                        mHost->Loge(devInfo.string());
                    }
                }

                AutoPtr<IWifiP2pWfdInfo> info;
                device->GetWfdInfo((IWifiP2pWfdInfo**)&info);
                if (notP2pDevice) {
                    String devInfo;
                    device->ToString(&devInfo);
                    if (info != NULL) {
                        notP2pDevice = FALSE;
                        mHost->Loge("GroupCreatedState: p2p device connect action............001! device info:");
                        mHost->Loge(devInfo.string());
                    }
                    else {
                        mHost->mNotP2pDevice->Update(device);
                        mHost->Loge("GroupCreatedState: not p2p device connect action........! device info:");
                        mHost->Loge(devInfo.string());
                        break;
                    }
                }

                AutoPtr<IWifiP2pDevice> dev;
                mHost->mPeers->Get(deviceAddress, (IWifiP2pDevice**)&dev);
                if (dev != NULL) {
                    mHost->mGroup->AddClient(dev);
                }
                else {
                    mHost->mGroup->AddClient(deviceAddress);
                }
                mHost->mPeers->UpdateStatus(deviceAddress, IWifiP2pDevice::CONNECTED);

                //2.add start,user kick to connect
                mHost->mWifiP2pDevice = mHost->GetWifiP2pDeviceFromPeers(deviceAddress);
                if (mHost->mWifiP2pDevice == NULL && info != NULL) {
                    mHost->mWifiP2pDevice = device;
                }
                SystemProperties::Set(WFD_P2P_ROLE, String("1"));//GO
                SystemProperties::Set(WFD_P2P_DEVICE_ADDR, deviceAddress);//
                //add end

                if (CWifiP2pService::DBG) mHost->Logd("GroupCreatedState ap sta connected");
                mHost->SendP2pPeersChangedBroadcast();
                mHost->SendP2pConnectionChangedBroadcast();//miracast give longer time to waiting for getPeeripAddress.
            }
            else {
                mHost->Loge("Connect on NULL device address, ignore");
            }
            mHost->Loge("GroupCreatedState: AP_STA_CONNECTED_EVENT end!!!!");
            break;
        }
        case WifiMonitor::AP_STA_DISCONNECTED_EVENT: {
            AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(obj);
            device->GetDeviceAddress(&deviceAddress);
            if (!deviceAddress.IsNull()) {
                AutoPtr<IWifiP2pDevice> dev;
                mHost->mNotP2pDevice->Get(deviceAddress, (IWifiP2pDevice**)&dev);
                if (NULL != dev) {
                    mHost->Loge("GroupCreatedState: AP_STA_DISCONNECTED_EVENT not p2p device break;!!!!");
                    Boolean result;
                    mHost->mNotP2pDevice->Remove(device, &result);
                    break;
                }

                mHost->mPeers->UpdateStatus(deviceAddress, IWifiP2pDevice::AVAILABLE);

                SystemProperties::Set(WFD_P2P_ROLE, emptyStr);//empty
                SystemProperties::Set(WFD_DNSMASQ_PEER, emptyStr);//empty
                SystemProperties::Set(WFD_P2P_DEVICE_ADDR, emptyStr);//empty
                //2.add start,user kick to connect
                mHost->mWifiP2pDevice = NULL;
                //add end

                mHost->mGroup->RemoveClient(deviceAddress, &b);
                if (b) {
                    if (CWifiP2pService::DBG)
                        Slogger::D(CWifiP2pService::TAG, "Removed client %s", deviceAddress.string());
                    mHost->mGroup->IsClientListEmpty(&b);
                    if (!mHost->mHost->mAutonomousGroup && b) {
                        Slogger::D(CWifiP2pService::TAG, "Client list empty, remove non-persistent p2p group");
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
                    if (CWifiP2pService::DBG) {
                        Slogger::D(CWifiP2pService::TAG, "Failed to remove client %s", deviceAddress.string());
                        AutoPtr<ArrayOf<IWifiP2pDevice*> > clients;
                        mHost->mGroup->GetClientList((ArrayOf<IWifiP2pDevice*> **)&clients);
                        if (clients) {
                            for (Int32 i = 0; i < clients->GetLength(); ++i) {
                                IWifiP2pDevice* c = (*clients)[i];
                                c->GetDeviceAddress(&deviceAddress);
                                Slogger::D(CWifiP2pService::TAG, "client %d: %s", i, deviceAddress.string());
                            }
                        }
                    }
                }
                mHost->SendP2pPeersChangedBroadcast();
                if (CWifiP2pService::DBG) mHost->Logd("GroupCreatedState ap sta disconnected");
            }
            else {
                String info;
                device->ToString(&info);
                Slogger::E(CWifiP2pService::TAG, "Disconnect on unknown device: %s", info.string());
            }
            break;
        }
        case DhcpStateMachine::CMD_POST_DHCP_ACTION: {
            AutoPtr<DhcpInfoInternal> dhcpInfo = (DhcpInfoInternal*)obj.Get();
            if (arg1 == DhcpStateMachine::DHCP_SUCCESS && dhcpInfo != NULL) {
                if (CWifiP2pService::DBG)
                    Slogger::D(CWifiP2pService::TAG, "DhcpInfo: %s", dhcpInfo->ToString().string());
                addr = dhcpInfo->mServerAddress;
                mHost->SetWifiP2pInfoOnGroupFormation(addr);

                SystemProperties::Set(WFD_P2P_ROLE, String("2"));//client
                AutoPtr<ISystemProperties> sp;
                CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
                String gateway;
                sp->Get(String("dhcp.p2p.gateway"), &gateway);
                if (!gateway.IsNull())
                    Slogger::D(CWifiP2pService::TAG, "gateway=[%s]",  gateway.string());
                else
                    mHost->Loge("gateway not set in the property yet.");

                String ipaddress;
                sp->Get(String("dhcp.p2p.ipaddress"), &ipaddress);
                if (!ipaddress.IsNull())
                    Slogger::D(CWifiP2pService::TAG, "ipaddress=[%s]",  ipaddress.string());
                else
                    mHost->Loge("gateway not set in the property yet.");

                mHost->SendP2pConnectionChangedBroadcast();
                //Turn on power save on client
                mHost->mGroup->GetInterface(&name);
                mHost->mWifiNative->SetP2pPowerSave(name, TRUE);
            }
            else {
                mHost->Loge("DHCP failed");
                mHost->mWifiNative->P2pGroupRemove(name);
            }
            break;
        }
        case IWifiP2pManager::REMOVE_GROUP:
        case WifiMonitor::NETWORK_DISCONNECTION_EVENT://client receive disconnect will wait too long,idle timeout
            if (CWifiP2pService::DBG) mHost->Logd("GroupCreatedState remove group");
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
            if (CWifiP2pService::DBG) mHost->Logd("GroupCreatedState group removed");
            /*reset wfd p2p role to empty*/
            SystemProperties::Set(WFD_P2P_ROLE, emptyStr);
            SystemProperties::Set(WFD_DNSMASQ_PEER, emptyStr);//empty

            AutoPtr<ISystemProperties> sp;
            CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
            String gateway;
            sp->Get(String("dhcp.p2p.gateway"), &gateway);
            if (!gateway.IsNull()) {
                SystemProperties::Set(String("dhcp.p2p.gateway"), emptyStr);
            }
            else {
                mHost->Loge("gateway not set in the property yet.");
            }

            String ipaddress;
            sp->Get(String("dhcp.p2p.ipaddress"), &ipaddress);
            if (!ipaddress.IsNull()) {
                SystemProperties::Set(String("dhcp.p2p.ipaddress"), emptyStr);
            }
            else {
                mHost->Loge("ipaddress not set in the property yet.");
            }

            /*=== clear ip add start "p2p0" 2013-01-08 14:34 ===*/
            Slogger::D(CWifiP2pService::TAG, "clear interface ip %s", mHost->mHost->mInterface.string());
            // try {
            mHost->mHost->mNwService->ClearInterfaceAddresses(mHost->mHost->mInterface);
            mHost->mGroup->GetInterface(&name);
            if (FAILED(mHost->mHost->mNwService->DisableIpv6(name))) {
                Slogger::E(CWifiP2pService::TAG, "Failed to clear addresses or disable ipv6 %s", name.string());
            }
            // } catch (Exception e) {
            //
            // }
            /*=== clear ip add end ===*/
            mHost->mWifiP2pDevice = NULL;

            mHost->HandleGroupRemoved();
            mHost->TransitionTo(mHost->mInactiveState);
            break;
        }
        case WifiMonitor::P2P_DEVICE_LOST_EVENT: {
            AutoPtr<IWifiP2pDevice> device = IWifiP2pDevice::Probe(obj);
            //Device loss for a connected device indicates it is not in discovery any more
            mHost->mGroup->Contains(device, &b);
            if (b) {
                if (CWifiP2pService::DBG) {
                    String info;
                    device->ToString(&info);
                    Slogger::D(CWifiP2pService::TAG, "Add device to lost list %s", info.string());
                }
                mHost->mPeersLostDuringConnection->Update(device);
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
        case IWifiP2pManager::CONNECT: {
            AutoPtr<IWifiP2pConfig> config = IWifiP2pConfig::Probe(obj);
            config->GetDeviceAddress(&addr);
            if (mHost->mSavedProvDiscDevice != NULL) {
                mHost->mSavedProvDiscDevice->GetDeviceAddress(&devAddr);
            }

            if (addr.IsNull() || (mHost->mSavedProvDiscDevice != NULL && devAddr.Equals(addr))) {
                AutoPtr<IWpsInfo> wps;
                config->GetWps((IWpsInfo**)&wps);
                wps->GetSetup(&setup);
                String wpsPin;
                wps->GetPin(&wpsPin);
                mHost->mGroup->GetInterface(&name);
                if (setup == IWpsInfo::PBC) {
                    mHost->mWifiNative->StartWpsPbc(name, String(NULL));
                }
                else {
                    if (wpsPin.IsNull()) {
                        pin = mHost->mWifiNative->StartWpsPinDisplay(name);
                        Int32 value;
                        ECode ec = StringUtils::ParseInt32(pin, 10, &value);
                        if (SUCCEEDED(ec)) {
                            if (mHost->SendShowPinReqToFrontApp(pin)) {
                                mHost->NotifyInvitationSent(
                                    pin, !addr.IsNull() ? addr : String("any"));
                            }
                        }
                        else {
                            // do nothing if pin is invalid
                        }
                    }
                    else {
                        mHost->mWifiNative->StartWpsPinKeypad(name, pin);
                    }
                }
                if (!addr.IsNull()) {
                    mHost->mPeers->UpdateStatus(addr, IWifiP2pDevice::INVITED);
                    //add start
                    mHost->mWifiP2pDevice = mHost->GetWifiP2pDeviceFromPeers(addr);
                    //add end
                    mHost->SendP2pPeersChangedBroadcast();
                }
                mHost->ReplyToMessage(message, IWifiP2pManager::CONNECT_SUCCEEDED);
            }
            else {
                Slogger::D(CWifiP2pService::TAG, "Inviting device : %s", addr.string());
                mHost->mSavedPeerConfig = config;
                if (mHost->mWifiNative->P2pInvite(mHost->mGroup, addr)) {
                    mHost->mPeers->UpdateStatus(addr, IWifiP2pDevice::INVITED);
                    //add start
                    mHost->mWifiP2pDevice = mHost->GetWifiP2pDeviceFromPeers(addr);
                    //add end
                    mHost->SendP2pPeersChangedBroadcast();
                    mHost->ReplyToMessage(message, IWifiP2pManager::CONNECT_SUCCEEDED);
                }
                else {
                    mHost->ReplyToMessage(message, IWifiP2pManager::CONNECT_FAILED,
                        IWifiP2pManager::ERROR);
                    mHost->mWifiP2pDevice = NULL;
                }
            }
            // TODO: figure out updating the status to declined when invitation is rejected
            break;
        }
        case WifiMonitor::P2P_INVITATION_RESULT_EVENT: {
            AutoPtr<IInteger32> iobj = IInteger32::Probe(obj);
            Int32 ival;
            iobj->GetValue(&ival);
            P2pStatus status = (P2pStatus)ival;
            Slogger::D(CWifiP2pService::TAG, "===> INVITATION RESULT EVENT : %d", status);
            if (status == P2pStatus_SUCCESS) {
                // invocation was succeeded.
                break;
            }
            else if (status == P2pStatus_UNKNOWN_P2P_GROUP) {
                // target device has already removed the credential.
                // So, remove this credential accordingly.
                mHost->mGroup->GetNetworkId(&netId);
                if (netId >= 0) {
                    if (CWifiP2pService::DBG) mHost->Logd("Remove unknown client from the list");
                    mHost->mSavedPeerConfig->GetDeviceAddress(&addr);
                    if (!mHost->RemoveClientFromList(netId, addr, FALSE)) {
                        // not found the client on the list
                        Slogger::E(CWifiP2pService::TAG, "Already removed the client, ignore");
                        break;
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
            mHost->mSavedProvDiscDevice = NULL;
            provDisc->GetDevice((IWifiP2pDevice**)&mHost->mSavedProvDiscDevice);
            mHost->mSavedProvDiscDevice->GetDeviceAddress(&addr);

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

            if (!mHost->SendConnectNoticeToApp(mHost->mSavedProvDiscDevice, mHost->mSavedPeerConfig)) {
                mHost->TransitionTo(mHost->mUserAuthorizingJoinState);
            }
            break;
        }
        case WifiMonitor::P2P_GROUP_STARTED_EVENT:
            Slogger::E(CWifiP2pService::TAG, "Duplicate group creation event notice, ignore");
            break;
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }

    *result = TRUE;  // HANDLED;
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::GroupCreatedState::Exit()
{
    mHost->UpdateThisDevice(IWifiP2pDevice::AVAILABLE);
    mHost->ResetWifiP2pInfo();
    String nullStr;
    mHost->mHost->mNetworkInfo->SetDetailedState(NetworkInfoDetailedState_DISCONNECTED, nullStr, nullStr);
    mHost->mWifiP2pDevice = NULL;
    mHost->SendP2pConnectionChangedBroadcast();
    return NOERROR;
}

//==================================================================
// CWifiP2pService::P2pStateMachine::UserAuthorizingJoinState
//==================================================================
CWifiP2pService::P2pStateMachine::UserAuthorizingJoinState::UserAuthorizingJoinState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::UserAuthorizingJoinState::Enter()
{
    if (CWifiP2pService::DBG) Slogger::D(CWifiP2pService::TAG, "Enter %s", GetName().string());
    mHost->NotifyInvitationReceived();
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::UserAuthorizingJoinState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
            GetName().string(), P2pStateMachine::CmdToString(what).string());
    }

    switch (what) {
        case WifiMonitor::P2P_PROV_DISC_PBC_REQ_EVENT:
        case WifiMonitor::P2P_PROV_DISC_ENTER_PIN_EVENT:
        case WifiMonitor::P2P_PROV_DISC_SHOW_PIN_EVENT:
            //Ignore more client requests
            break;
        case PEER_CONNECTION_USER_ACCEPT: {
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
            mHost->mSavedPeerConfig = NULL;
            mHost->TransitionTo(mHost->mGroupCreatedState);
            break;
        }
        case PEER_CONNECTION_USER_REJECT:
            if (CWifiP2pService::DBG) mHost->Logd("User rejected incoming request");
            mHost->mSavedPeerConfig = NULL;
            mHost->TransitionTo(mHost->mGroupCreatedState);
            break;
        default:
            *result = FALSE;  // NOT_HANDLED;
            return NOERROR;
    }

    *result = TRUE;  // HANDLED;
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::UserAuthorizingJoinState::Exit()
{
    //TODO: dismiss dialog if not already done
    return NOERROR;
}

//==================================================================
// CWifiP2pService::P2pStateMachine::OngoingGroupRemovalState
//==================================================================
CWifiP2pService::P2pStateMachine::OngoingGroupRemovalState::OngoingGroupRemovalState(
    /* [in] */ P2pStateMachine* host)
    : mHost(host)
{
}

ECode CWifiP2pService::P2pStateMachine::OngoingGroupRemovalState::Enter()
{
    if (CWifiP2pService::DBG) Slogger::D(CWifiP2pService::TAG, "Enter %s", GetName().string());
    return NOERROR;
}

ECode CWifiP2pService::P2pStateMachine::OngoingGroupRemovalState::ProcessMessage(
    /* [in] */ IMessage* message,
    /* [out] */ Boolean* result)
{
    Int32 what;
    message->GetWhat(&what);

    if (CWifiP2pService::DBG) {
        Slogger::D(CWifiP2pService::TAG, "%s ProcessMessage : %s",
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

} // namespace P2p
} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

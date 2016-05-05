
#include "elastos/droid/server/CConnectivityService.h"
#include "elastos/droid/server/CNetworkPolicyListener.h"
#include "elastos/droid/server/CDataActivityObserver.h"
#include "elastos/droid/server/connectivity/CNat464Xlat.h"
#include "elastos/droid/server/connectivity/NetworkAgentInfo.h"
#include "elastos/droid/server/connectivity/NetworkMonitor.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/etl/Algorithm.h>
#include <elastos/utility/logging/Slogger.h>
#include <cutils/properties.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Text.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.KeyStore.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::CNotification;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IIpPrefix;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::ILinkCapabilities;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::ILinkPropertiesHelper;
using Elastos::Droid::Net::CLinkPropertiesHelper;
using Elastos::Droid::Net::ILinkPropertiesCompareResult;
using Elastos::Droid::Net::CLinkPropertiesCompareResult;
using Elastos::Droid::Net::ISamplingDataTrackerHelper;
using Elastos::Droid::Net::CSamplingDataTrackerHelper;
using Elastos::Droid::Net::ISamplingDataTrackerSamplingSnapshot;
using Elastos::Droid::Net::CProxyInfo;
using Elastos::Droid::Net::IRouteInfoHelper;
using Elastos::Droid::Net::CRouteInfoHelper;
using Elastos::Droid::Net::IConnectivityManagerHelper;
using Elastos::Droid::Net::CConnectivityManagerHelper;
using Elastos::Droid::Net::EIID_IIConnectivityManager;
using Elastos::Droid::Net::IMobileDataStateTracker;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Net::CNetwork;
using Elastos::Droid::Net::CNetworkMisc;
using Elastos::Droid::Net::CNetworkCapabilities;
using Elastos::Droid::Net::CNetworkRequest;
using Elastos::Droid::Net::CNetworkConfig;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Net::CNetworkState;
using Elastos::Droid::Net::INetworkFactory;
using Elastos::Droid::Net::INetworkPolicyManager;
using Elastos::Droid::Net::NetworkInfoState;
using Elastos::Droid::Net::NetworkInfoState_CONNECTED;
using Elastos::Droid::Net::NetworkInfoState_DISCONNECTED;
using Elastos::Droid::Net::NetworkInfoState_SUSPENDED;
using Elastos::Droid::Net::NetworkInfoState_CONNECTING;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED;
using Elastos::Droid::Net::NetworkInfoDetailedState_BLOCKED;
using Elastos::Droid::Net::NetworkInfoDetailedState_FAILED;
using Elastos::Droid::Net::NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED;
using Elastos::Droid::Internal::Net::IVpnProfileHelper;
using Elastos::Droid::Internal::Net::CVpnProfileHelper;
using Elastos::Droid::Internal::Net::INetworkStatsFactoryHelper;
using Elastos::Droid::Internal::Net::CNetworkStatsFactoryHelper;
using Elastos::Droid::Internal::Utility::XmlUtils;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Internal::Telephony::IDctConstants;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::KeyStore::Security::ICredentials;
using Elastos::Droid::KeyStore::Security::IKeyStoreHelper;
//TODO using Elastos::Droid::KeyStore::Security::CKeyStoreHelper;
using Elastos::Droid::Server::Connectivity::CNat464Xlat;
using Elastos::Droid::Server::Connectivity::NetworkAgentInfo;
using Elastos::Droid::Server::Connectivity::NetworkMonitor;

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::IThread;
using Elastos::IO::IPrintWriter;
using Elastos::IO::CFile;
using Elastos::IO::IReader;
using Elastos::IO::IFileReader;
using Elastos::IO::CFileReader;
using Elastos::IO::ICloseable;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetAddressHelper;
using Elastos::Net::CInetAddressHelper;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ISet;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Server {

const String CConnectivityService::TAG("ConnectivityService");

const Boolean CConnectivityService::DBG = FALSE;
const Boolean CConnectivityService::VDBG = FALSE;
const Boolean CConnectivityService::SAMPLE_DBG = FALSE;
const Boolean CConnectivityService::LOGD_RULES = FALSE;

const Int32 CConnectivityService::RESTORE_DEFAULT_NETWORK_DELAY = 1 * 60 * 1000;
const String CConnectivityService::NETWORK_RESTORE_DELAY_PROP_NAME("android.telephony.apn-restore");


const Int32 CConnectivityService::DEFAULT_FAIL_FAST_TIME_MS = 1 * 60 * 1000;
const String CConnectivityService::FAIL_FAST_TIME_MS("persist.radio.fail_fast_time_ms");
const String CConnectivityService::ACTION_PKT_CNT_SAMPLE_INTERVAL_ELAPSED \
    ("android.net.ConnectivityService.action.PKT_CNT_SAMPLE_INTERVAL_ELAPSED");

// Set network sampling interval at 12 minutes, this way, even if the timers get
// aggregated, it will fire at around 15 minutes, which should allow us to
// aggregate this timer with other timers (specially the socket keep alive timers)
const Int32 CConnectivityService::DEFAULT_SAMPLING_INTERVAL_IN_SECONDS = (SAMPLE_DBG ? 30 : 12 * 60);

// start network sampling a minute after booting ...
const Int32 CConnectivityService::DEFAULT_START_SAMPLING_INTERVAL_IN_SECONDS = (SAMPLE_DBG ? 30 : 60);

const String CConnectivityService::NETID_UPDATE("org.codeaurora.NETID_UPDATE");

const String CConnectivityService::EXTRA_NETWORK_TYPE("netType");

const String CConnectivityService::EXTRA_NETID("netID");

const Int32 CConnectivityService::EVENT_DEFAULT_NETWORK_SWITCH = 540670;

const Int32 CConnectivityService::SAMPLE_INTERVAL_ELAPSED_REQUEST_CODE = 0;

const Int32 CConnectivityService::ENABLED  = 1;
const Int32 CConnectivityService::DISABLED = 0;

const Int32 CConnectivityService::EVENT_CHANGE_MOBILE_DATA_ENABLED = 2;
const Int32 CConnectivityService::EVENT_CLEAR_NET_TRANSITION_WAKELOCK = 8;
const Int32 CConnectivityService::EVENT_APPLY_GLOBAL_HTTP_PROXY = 9;
const Int32 CConnectivityService::EVENT_SET_DEPENDENCY_MET = 10;
const Int32 CConnectivityService::EVENT_SEND_STICKY_BROADCAST_INTENT = 11;
const Int32 CConnectivityService::EVENT_SET_POLICY_DATA_ENABLE = 12;
const Int32 CConnectivityService::EVENT_ENABLE_FAIL_FAST_MOBILE_DATA = 14;
const Int32 CConnectivityService::EVENT_SAMPLE_INTERVAL_ELAPSED = 15;
const Int32 CConnectivityService::EVENT_PROXY_HAS_CHANGED = 16;
const Int32 CConnectivityService::EVENT_REGISTER_NETWORK_FACTORY = 17;
const Int32 CConnectivityService::EVENT_REGISTER_NETWORK_AGENT = 18;
const Int32 CConnectivityService::EVENT_REGISTER_NETWORK_REQUEST = 19;
const Int32 CConnectivityService::EVENT_TIMEOUT_NETWORK_REQUEST = 20;
const Int32 CConnectivityService::EVENT_REGISTER_NETWORK_LISTENER = 21;
const Int32 CConnectivityService::EVENT_RELEASE_NETWORK_REQUEST = 22;
const Int32 CConnectivityService::EVENT_UNREGISTER_NETWORK_FACTORY = 23;
const Int32 CConnectivityService::EVENT_EXPIRE_NET_TRANSITION_WAKELOCK = 24;
const Int32 CConnectivityService::EVENT_SYSTEM_READY = 25;

const Int32 CConnectivityService::INET_CONDITION_LOG_MAX_SIZE = 15;

const Int32 CConnectivityService::MIN_NET_ID = 100; // some reserved marks
const Int32 CConnectivityService::MAX_NET_ID = 65535;

const String CConnectivityService::DEFAULT_TCP_BUFFER_SIZES("4096,87380,110208,4096,16384,110208");

const String CConnectivityService::PROVISIONING_URL_PATH("/data/misc/radio/provisioning_urls.xml");

/** XML tag for root element. */
const String CConnectivityService::TAG_PROVISIONING_URLS("provisioningUrls");
/** XML tag for individual url */
const String CConnectivityService::TAG_PROVISIONING_URL("provisioningUrl");
/** XML tag for redirected url */
const String CConnectivityService::TAG_REDIRECTED_URL("redirectedUrl");
/** XML attribute for mcc */
const String CConnectivityService::ATTR_MCC("mcc");
/** XML attribute for mnc */
const String CConnectivityService::ATTR_MNC("mnc");

const Int32 CConnectivityService::REDIRECTED_PROVISIONING = 1;
const Int32 CConnectivityService::PROVISIONING = 2;

const String CConnectivityService::NOTIFICATION_ID("CaptivePortal.Notification");

static Boolean IsNetworkTypeValid(
    /* [in] */ Int32 networkType,
    /* [out] */ Boolean* result)
{
    AutoPtr<IConnectivityManagerHelper> helper;
    CConnectivityManagerHelper::AcquireSingleton((IConnectivityManagerHelper**)&helper);
    return helper->IsNetworkTypeValid(networkType, result);
}

//==============================================================================
// CConnectivityService::LegacyTypeTracker
//==============================================================================
const Boolean CConnectivityService::LegacyTypeTracker::DBG = FALSE;
const Boolean CConnectivityService::LegacyTypeTracker::VDBG = FALSE;
const String CConnectivityService::LegacyTypeTracker::TAG("CConnectivityService::LegacyTypeTracker");

CConnectivityService::LegacyTypeTracker::LegacyTypeTracker(
    /* [in] */ CConnectivityService* host)
{
    mTypeLists = ArrayOf<IArrayList*>::Alloc(IConnectivityManager::MAX_NETWORK_TYPE + 1);
}

ECode CConnectivityService::LegacyTypeTracker::AddSupportedType(
    /* [in] */ Int32 type)
{
    if ((&mTypeLists)[type] != NULL) {
        Slogger::E(TAG, "legacy list for type %d already initialized", type);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    mTypeLists->Set(type, list);
    return NOERROR;
}

Boolean CConnectivityService::LegacyTypeTracker::IsTypeSupported(
    /* [in] */ Int32 type)
{
    Boolean bval;
    IsNetworkTypeValid(type, &bval);
    return bval && (*mTypeLists)[type] != NULL;
}

AutoPtr<INetworkAgentInfo> CConnectivityService::LegacyTypeTracker::GetNetworkForType(
    /* [in] */ Int32 type)
{
    AutoPtr<INetworkAgentInfo> info;
    Boolean empty;
    if (IsTypeSupported(type) && ((*mTypeLists)[type]->IsEmpty(&empty), !empty)) {
        AutoPtr<IInterface> obj;
        (*mTypeLists)[type]->Get(0, (IInterface**)&obj);
        info = INetworkAgentInfo::Probe(obj);
    }

    return info;
}

void CConnectivityService::LegacyTypeTracker::MaybeLogBroadcast(
    /* [in] */ INetworkAgentInfo* nai,
    /* [in] */ Boolean connected,
    /* [in] */ Int32 type)
{
    if (DBG) {
        String name;
        nai->Name(&name);
        Slogger::D(TAG, "Sending %s broadcast for type %d %s IsDefaultNetwork=%d.",
            connected ? "connected" : "disconnected", type, name.string(), mHost->IsDefaultNetwork(nai));
    }
}

/** Adds the given network to the specified legacy type list. */
void CConnectivityService::LegacyTypeTracker::Add(
    /* [in] */ Int32 type,
    /* [in] */ INetworkAgentInfo* nai)
{
    if (!IsTypeSupported(type)) {
        return;  // Invalid network type.
    }
    if (VDBG) {
        Slogger::D(TAG, "Adding agent %s for legacy network type %d",
            TO_CSTR(nai), type);
    }

    AutoPtr<IArrayList> list = (*mTypeLists)[type];
    Boolean bval;
    IInterface* naiObj = TO_IINTERFACE(nai);
    list->Contains(naiObj, &bval);
    if (bval) {
        Slogger::E(TAG, "Attempting to register duplicate agent for type %d: %s",
            type, TO_CSTR(nai));
        return;
    }

    list->Add(naiObj);

    // Send a broadcast if this is the first network of its type or if it's the default.
    Int32 ival;
    list->GetSize(&ival);
    if (ival == 1 || mHost->IsDefaultNetwork(nai)) {
        MaybeLogBroadcast(nai, TRUE, type);
        mHost->SendLegacyNetworkBroadcast(nai, TRUE, type);
    }
}

void CConnectivityService::LegacyTypeTracker::Remove(
    /* [in] */ Int32 type,
    /* [in] */ INetworkAgentInfo* nai)
{
    AutoPtr<IArrayList> list = (*mTypeLists)[type];
    Boolean bval;
    if (list == NULL || list->IsEmpty(&bval)) {
        return;
    }

    AutoPtr<IInterface> obj;
    list->Get(0, (IInterface**)&obj);

    AutoPtr<IInterface> naiObj = TO_IINTERFACE(nai);
    Boolean wasFirstNetwork = Object::Equals(obj, naiObj);

    list->Remove(naiObj, &bval);
    if (!bval) {
        return;
    }

    if (wasFirstNetwork || mHost->IsDefaultNetwork(nai)) {
        MaybeLogBroadcast(nai, FALSE, type);
        mHost->SendLegacyNetworkBroadcast(nai, FALSE, type);
    }

    list->IsEmpty(&bval);
    if (!bval && wasFirstNetwork) {
        if (DBG) {
            Slogger::D(TAG, "Other network available for type %d, sending connected broadcast",
                type);
        }

        obj = NULL;
        list->Get(0, (IInterface**)&obj);
        INetworkAgentInfo* info = INetworkAgentInfo::Probe(obj);
        MaybeLogBroadcast(info, FALSE, type);
        mHost->SendLegacyNetworkBroadcast(info, FALSE, type);
    }
}

void CConnectivityService::LegacyTypeTracker::Remove(
    /* [in] */ INetworkAgentInfo* nai)
{
    if (VDBG) Slogger::D(TAG, "Removing agent %s", TO_CSTR(nai));
    for (Int32 type = 0; type < mTypeLists->GetLength(); type++) {
        Remove(type, nai);
    }
}

String CConnectivityService::LegacyTypeTracker::NaiToString(
    /* [in] */ INetworkAgentInfo* nai)
{
    String name("NULL");
    if (nai != NULL) {
        nai->Name(&name);
    }

    NetworkAgentInfo* info = (NetworkAgentInfo*)nai;
    String stateStr("???/???");
    if (info->mNetworkInfo != NULL) {
        NetworkInfoState state;
        NetworkInfoDetailedState detailedState;
        info->mNetworkInfo->GetState(&state);
        info->mNetworkInfo->GetDetailedState(&detailedState);
        StringBuilder sb;
        sb += (Int32)state;
        sb += "/";
        sb += (Int32)detailedState;
        stateStr = sb.ToString();
    }

    StringBuilder sb(name);
    sb += " ";
    sb += stateStr;
    return sb.ToString();
}

ECode CConnectivityService::LegacyTypeTracker::Dump(
    /* [in] */ IIndentingPrintWriter* pw)
{
    IPrintWriter* writer = IPrintWriter::Probe(pw);
    Int32 size;
    for (Int32 type = 0; type < mTypeLists->GetLength(); type++) {
        if ((*mTypeLists)[type] == NULL) continue;
        writer->Print(type);
        writer->Print(String(" "));
        pw->IncreaseIndent();
        if ((*mTypeLists)[type]->GetSize(&size) == 0) writer->Println(String("none"));
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            (*mTypeLists)[type]->Get(i, (IInterface**)&obj);
            INetworkAgentInfo* nai = INetworkAgentInfo::Probe(obj);
            writer->Println(NaiToString(nai));
        }
        pw->DecreaseIndent();
    }
    return NOERROR;
}

void CConnectivityService::LegacyTypeTracker::Log(
    /* [in] */ const String& s)
{
    Slogger::D(TAG, s.string());
}


//==============================================================================
// CConnectivityService::NetworkRequestInfo
//==============================================================================

const Boolean CConnectivityService::NetworkRequestInfo::REQUEST = TRUE;
const Boolean CConnectivityService::NetworkRequestInfo::LISTEN = FALSE;

CAR_INTERFACE_IMPL(CConnectivityService::NetworkRequestInfo, Object, IProxyDeathRecipient)

CConnectivityService::NetworkRequestInfo::NetworkRequestInfo(
    /* [in] */ IMessenger* m,
    /* [in] */ INetworkRequest* r,
    /* [in] */ IBinder* binder,
    /* [in] */ Boolean isRequest,
    /* [in] */ CConnectivityService* host)
    : mHost(host)
{
    mMessenger = m;
    mRequest = r;
    mBinder = binder;
    mPid = Binder::GetCallingPid();
    mUid = Binder::GetCallingUid();
    mIsRequest = isRequest;

    AutoPtr<IProxy> proxy = (IProxy*)mBinder->Probe(EIID_IProxy);
    if (proxy != NULL) {
        ECode ec = proxy->LinkToDeath(this, 0);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            ProxyDied();
        }
    }
}

void CConnectivityService::NetworkRequestInfo::UnlinkDeathRecipient()
{
    AutoPtr<IProxy> proxy = (IProxy*)mBinder->Probe(EIID_IProxy);
    if (proxy != NULL) {
        Boolean bval;
        proxy->UnlinkToDeath(this, 0, &bval);
    }
}

ECode CConnectivityService::NetworkRequestInfo::ProxyDied()
{
    Slogger::D("CConnectivityService", "ConnectivityService NetworkRequestInfo binderDied(%s, %)",
        TO_CSTR(mRequest), TO_CSTR(mBinder));
    mHost->ReleaseNetworkRequest(mRequest);
    return NOERROR;
}

ECode CConnectivityService::NetworkRequestInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb(mIsRequest ? "Request" : "Listen");
    sb += " from uid/pid:";
    sb += mUid;
    sb += "/";
    sb += mPid;
    sb += " for ";
    sb += TO_CSTR(mRequest);
    return NOERROR;
}

//==============================================================================
// CConnectivityService::UserIntentReceiver
//==============================================================================
CConnectivityService::UserIntentReceiver::UserIntentReceiver(
    /* [in] */ CConnectivityService* host)
    : mHost(host)
{}

ECode CConnectivityService::UserIntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Int32 userId;
    intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, IUserHandle::USER_NULL, &userId);
    if (userId == IUserHandle::USER_NULL) return NOERROR;

    if (IIntent::ACTION_USER_STARTING.Equals(action)) {
        mHost->OnUserStart(userId);
    }
    else if (IIntent::ACTION_USER_STOPPING.Equals(action)) {
        mHost->OnUserStop(userId);
    }
    return NOERROR;
}

//==============================================================================
// CConnectivityService::InternalBroadcastReceiver
//==============================================================================
ECode CConnectivityService::InternalBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // Try creating lockdown tracker, since user present usually means
    // unlocked keystore.
    Boolean update;
    if ((mHost->UpdateLockdownVpn(&update), update)) {
        mHost->mContext->UnregisterReceiver(this);
    }
    return NOERROR;
}

//==============================================================================
// CConnectivityService::PktCntSampleIntervalElapsedeceiver
//==============================================================================

CConnectivityService::PktCntSampleIntervalElapsedeceiver::PktCntSampleIntervalElapsedeceiver(
    /* [in] */ CConnectivityService* host)
    : mHost(host)
{}

//
ECode CConnectivityService::PktCntSampleIntervalElapsedeceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(CConnectivityService::ACTION_PKT_CNT_SAMPLE_INTERVAL_ELAPSED)) {
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(CConnectivityService::EVENT_SAMPLE_INTERVAL_ELAPSED, (IMessage**)&msg);
        Boolean bval;
        mHost->mHandler->SendMessage(msg, &bval);
    }
    return NOERROR;
}

//==============================================================================
// CConnectivityService::InternalHandler
//==============================================================================

ECode CConnectivityService::InternalHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case CConnectivityService::EVENT_EXPIRE_NET_TRANSITION_WAKELOCK:
        case CConnectivityService::EVENT_CLEAR_NET_TRANSITION_WAKELOCK: {
            String causedBy;
            ISynchronize* syncObj = (ISynchronize*)mHost->Probe(EIID_ISynchronize);
            synchronized(syncObj) {
                Boolean bval;
                if (arg1 == mHost->mNetTransitionWakeLockSerialNumber
                    && (mHost->mNetTransitionWakeLock->IsHeld(&bval), bval)) {
                    mHost->mNetTransitionWakeLock->ReleaseLock();
                    causedBy = mHost->mNetTransitionWakeLockCausedBy;
                }
                else {
                    break;
                }
            }
            if (what == CConnectivityService::EVENT_EXPIRE_NET_TRANSITION_WAKELOCK) {
                Slogger::D("CConnectivityService", "Failed to find a new network"
                    " - expiring NetTransition Wakelock");
            }
            else {
                Slogger::D("CConnectivityService", "NetTransition Wakelock (%s)"
                    " cleared because we found a replacement network",
                    causedBy.IsNull() ? "unknown" : causedBy.string());
            }
            break;
        }
        case CConnectivityService::EVENT_APPLY_GLOBAL_HTTP_PROXY: {
            mHost->HandleDeprecatedGlobalHttpProxy();
            break;
        }
        case CConnectivityService::EVENT_SET_DEPENDENCY_MET: {
            Boolean met = (arg1 == CConnectivityService::ENABLED);
            mHost->HandleSetDependencyMet(arg2, met);
            break;
        }
        case CConnectivityService::EVENT_SEND_STICKY_BROADCAST_INTENT: {
            AutoPtr<IIntent> intent = IIntent::Probe(obj);
            mHost->SendStickyBroadcast(intent);
            break;
        }
        case CConnectivityService::EVENT_SET_POLICY_DATA_ENABLE: {
            Int32 networkType = arg1;
            Boolean enabled = arg2 == CConnectivityService::ENABLED;
            mHost->HandleSetPolicyDataEnable(networkType, enabled);
            break;
        }
        case CConnectivityService::EVENT_ENABLE_FAIL_FAST_MOBILE_DATA: {
            Int32 tag;
            mHost->mEnableFailFastMobileDataTag->Get(&tag);
            if (arg1 == tag) {
                AutoPtr<IMobileDataStateTracker> mobileDst;
                mobileDst = IMobileDataStateTracker::Probe(
                    (*mHost->mNetTrackers)[IConnectivityManager::TYPE_MOBILE]);
                if (mobileDst != NULL) {
                    mobileDst->SetEnableFailFastMobileData(arg2);
                }
            } else {
                Slogger::D("CConnectivityService", "EVENT_ENABLE_FAIL_FAST_MOBILE_DATA: stale arg1:%d != %d",
                    arg1, tag);
            }
            break;
        }
        case CConnectivityService::EVENT_SAMPLE_INTERVAL_ELAPSED: {
            mHost->HandleNetworkSamplingTimeout();
            break;
        }
        case CConnectivityService::EVENT_PROXY_HAS_CHANGED: {
            IProxyInfo* pi = IProxyInfo::Probe(obj);
            mHost->HandleApplyDefaultProxy(pi);
            break;
        }
        case CConnectivityService::EVENT_REGISTER_NETWORK_FACTORY: {
            NetworkFactoryInfo* nfi = (NetworkFactoryInfo*)IObject::Probe(obj);
            mHost->HandleRegisterNetworkFactory(nfi);
            break;
        }
        case CConnectivityService::EVENT_UNREGISTER_NETWORK_FACTORY: {
            IMessenger* msger = IMessenger::Probe(obj);
            mHost->HandleUnregisterNetworkFactory(msger);
            break;
        }
        case CConnectivityService::EVENT_REGISTER_NETWORK_AGENT: {
            INetworkAgentInfo* nai = INetworkAgentInfo::Probe(obj);
            mHost->HandleRegisterNetworkAgent(nai);
            break;
        }
        case CConnectivityService::EVENT_REGISTER_NETWORK_REQUEST:
        case CConnectivityService::EVENT_REGISTER_NETWORK_LISTENER: {
            mHost->HandleRegisterNetworkRequest(msg);
            break;
        }
        case CConnectivityService::EVENT_RELEASE_NETWORK_REQUEST: {
            INetworkRequest* nr = INetworkRequest::Probe(obj);
            mHost->HandleReleaseNetworkRequest(nr, arg1);
            break;
        }
        case CConnectivityService::EVENT_SYSTEM_READY: {
            AutoPtr<ICollection> values;
            mHost->mNetworkAgentInfos->GetValues((ICollection**)&values);
            AutoPtr<IIterator> it;
            values->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(obj);
                nai->mNetworkMonitor->mSystemReady = TRUE;
            }
            break;
        }
    }
    return NOERROR;
}

//==============================================================================
// CConnectivityService::NetworkStateTrackerHandler
//==============================================================================

ECode CConnectivityService::NetworkStateTrackerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<INetworkInfo> info = INetworkInfo::Probe(obj);
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IMessenger> replyToMsger;
    msg->GetReplyTo((IMessenger**)&replyToMsger);
    IInterface* replyTo = (IInterface*)replyToMsger;

    switch (what) {
        case IAsyncChannel::CMD_CHANNEL_HALF_CONNECTED: {
            mHost->HandleAsyncChannelHalfConnect(msg);
            break;
        }
        case IAsyncChannel::CMD_CHANNEL_DISCONNECT: {
            AutoPtr<IInterface> naiObj;
            mHost->mNetworkAgentInfos->Get(replyTo, (IInterface**)&naiObj);
            NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(naiObj);
            if (nai != NULL) nai->mAsyncChannel->Disconnect();
            break;
        }
        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED: {
            mHost->HandleAsyncChannelDisconnected(msg);
            break;
        }
        case INetworkAgent::EVENT_NETWORK_CAPABILITIES_CHANGED: {
            AutoPtr<IInterface> naiObj;
            mHost->mNetworkAgentInfos->Get(replyTo, (IInterface**)&naiObj);
            NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(naiObj);
            if (nai == NULL) {
                Slogger::E("CConnectivityService", "EVENT_NETWORK_CAPABILITIES_CHANGED from unknown NetworkAgent");
            }
            else {
                mHost->UpdateCapabilities(nai, INetworkCapabilities::Probe(obj));
            }
            break;
        }
        case EVENT_DEFAULT_NETWORK_SWITCH: {
            mHost->HandleDefaultNetworkSwitch();
            break;
        }
        case INetworkAgent::EVENT_NETWORK_PROPERTIES_CHANGED: {
            AutoPtr<IInterface> naiObj;
            mHost->mNetworkAgentInfos->Get(replyTo, (IInterface**)&naiObj);
            NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(naiObj);
            if (nai == NULL) {
                Slogger::E("CConnectivityService", "NetworkAgent not found for EVENT_NETWORK_PROPERTIES_CHANGED");
            }
            else {
                if (VDBG) {
                    Slogger::D("CConnectivityService", "Update of LinkProperties for %s; created=",
                        nai->Name().string(), nai->mCreated);
                }
                AutoPtr<ILinkProperties> oldLp = nai->mLinkProperties;
                synchronized(nai) {
                    nai->mLinkProperties = ILinkProperties::Probe(obj);
                }
                if (nai->mCreated) {
                    mHost->UpdateLinkProperties(nai, oldLp);
                    mHost->NotifyNetworkCallbacks(nai, IConnectivityManager::CALLBACK_IP_CHANGED);
                }
            }
            break;
        }
        case INetworkAgent::EVENT_NETWORK_INFO_CHANGED: {
            AutoPtr<IInterface> naiObj;
            mHost->mNetworkAgentInfos->Get(replyTo, (IInterface**)&naiObj);
            NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(naiObj);
            if (nai == NULL) {
                Slogger::E("CConnectivityService", "EVENT_NETWORK_INFO_CHANGED from unknown NetworkAgent");
                break;
            }
            info = INetworkInfo::Probe(obj);
            mHost->UpdateNetworkInfo(nai, info);
            break;
        }
        case INetworkAgent::EVENT_NETWORK_SCORE_CHANGED: {
            AutoPtr<IInterface> naiObj;
            mHost->mNetworkAgentInfos->Get(replyTo, (IInterface**)&naiObj);
            NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(naiObj);
            if (nai == NULL) {
                Slogger::E("CConnectivityService", "EVENT_NETWORK_SCORE_CHANGED from unknown NetworkAgent");
                break;
            }
            IInteger32* score = IInteger32::Probe(obj);
            if (score != NULL) {
                Int32 ival;
                score->GetValue(&ival);
                mHost->UpdateNetworkScore(nai, ival);
            }
            break;
        }
        case INetworkAgent::EVENT_UID_RANGES_ADDED: {
            AutoPtr<IInterface> naiObj;
            mHost->mNetworkAgentInfos->Get(replyTo, (IInterface**)&naiObj);
            NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(naiObj);
            if (nai == NULL) {
                Slogger::E("CConnectivityService", "EVENT_UID_RANGES_ADDED from unknown NetworkAgent");
                break;
            }
            Int32 id;
            nai->mNetwork->GetNetId(&id);
            AutoPtr<ArrayOf<IUidRange*> > array;
            IArrayOf* arrObj = IArrayOf::Probe(obj);
            if (arrObj) {
                Int32 size;
                arrObj->GetLength(&size);
                array = ArrayOf<IUidRange*>::Alloc(size);
                for (Int32 i = 0; i < size; ++i) {
                    AutoPtr<IInterface> o;
                    arrObj->Get(i, (IInterface**)&o);
                    array->Set(i, IUidRange::Probe(o));
                }
            }
            ECode ec = mHost->mNetd->AddVpnUidRanges(id, array);
            if (FAILED(ec)) {
                // Never crash!
                Slogger::E("CConnectivityService", "Exception in addVpnUidRanges: %08x", ec);
            }
            break;
        }
        case INetworkAgent::EVENT_UID_RANGES_REMOVED: {
            AutoPtr<IInterface> naiObj;
            mHost->mNetworkAgentInfos->Get(replyTo, (IInterface**)&naiObj);
            NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(naiObj);
            if (nai == NULL) {
                Slogger::E("CConnectivityService", "EVENT_UID_RANGES_REMOVED from unknown NetworkAgent");
                break;
            }

            Int32 id;
            nai->mNetwork->GetNetId(&id);
            AutoPtr<ArrayOf<IUidRange*> > array;
            IArrayOf* arrObj = IArrayOf::Probe(obj);
            if (arrObj) {
                Int32 size;
                arrObj->GetLength(&size);
                array = ArrayOf<IUidRange*>::Alloc(size);
                for (Int32 i = 0; i < size; ++i) {
                    AutoPtr<IInterface> o;
                    arrObj->Get(i, (IInterface**)&o);
                    array->Set(i, IUidRange::Probe(o));
                }
            }
            ECode ec = mHost->mNetd->RemoveVpnUidRanges(id, array);
            if (FAILED(ec)) {
                // Never crash!
                Slogger::E("CConnectivityService", "Exception in removeVpnUidRanges:　%08x", ec);
            }
            break;
        }
        case INetworkAgent::EVENT_SET_EXPLICITLY_SELECTED: {
            AutoPtr<IInterface> naiObj;
            mHost->mNetworkAgentInfos->Get(replyTo, (IInterface**)&naiObj);
            NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(naiObj);
            if (nai == NULL) {
                Slogger::E("CConnectivityService", "EVENT_SET_EXPLICITLY_SELECTED from unknown NetworkAgent");
                break;
            }
            Boolean bval;
            nai->mNetworkMisc->GetExplicitlySelected(&bval);
            if (nai->mCreated && !bval) {
                Slogger::E("CConnectivityService", "ERROR: created network explicitly selected.");
            }
            nai->mNetworkMisc->SetExplicitlySelected(TRUE);
            break;
        }
        case NetworkMonitor::EVENT_NETWORK_TESTED: {
            AutoPtr<IInterface> naiObj;
            mHost->mNetworkAgentInfos->Get(replyTo, (IInterface**)&naiObj);
            NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(naiObj);
            if (mHost->IsLiveNetworkAgent(nai, String("EVENT_NETWORK_VALIDATED"))) {
                Boolean valid = (arg1 == NetworkMonitor::NETWORK_TEST_RESULT_VALID);
                if (valid) {
                    if (DBG) Slogger::D("CConnectivityService", "Validated %s", nai->Name().string());
                    Boolean previouslyValidated = nai->mValidated;
                    Int32 previousScore, curScore;
                    nai->GetCurrentScore(&previousScore);
                    nai->mValidated = TRUE;
                    mHost->RematchNetworkAndRequests(nai, !previouslyValidated);
                    // If score has changed, rebroadcast to NetworkFactories. b/17726566
                    nai->GetCurrentScore(&curScore);
                    if (curScore != previousScore) {
                        mHost->SendUpdatedScoreToFactories(nai);
                    }
                }
                mHost->UpdateInetCondition(nai, valid);
                // Let the NetworkAgent know the state of its network
                nai->mAsyncChannel->SendMessage(
                    INetworkAgent::CMD_REPORT_NETWORK_STATUS,
                    (valid ? INetworkAgent::VALID_NETWORK : INetworkAgent::INVALID_NETWORK),
                    0, NULL);
            }
            break;
        }
        case NetworkMonitor::EVENT_NETWORK_LINGER_COMPLETE: {
            AutoPtr<IInterface> naiObj;
            mHost->mNetworkAgentInfos->Get(replyTo, (IInterface**)&naiObj);
            NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(naiObj);
            if (mHost->IsLiveNetworkAgent(nai, String("EVENT_NETWORK_LINGER_COMPLETE"))) {
                mHost->HandleLingerComplete(nai);
            }
            break;
        }
        case NetworkMonitor::EVENT_PROVISIONING_NOTIFICATION: {
            if (arg1 == 0) {
                mHost->SetProvNotificationVisibleIntent(FALSE, arg2, 0, String(NULL), NULL);
            }
            else {
                AutoPtr<NetworkAgentInfo> nai;
                ISynchronize* syncObj = (ISynchronize*)mHost->mNetworkForNetId->Probe(EIID_ISynchronize);
                synchronized(syncObj) {
                    AutoPtr<IInterface> obj;
                    mHost->mNetworkForNetId->Get(arg2, (IInterface**)&obj);;
                    nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(obj);
                }
                if (nai == NULL) {
                    Slogger::E("CConnectivityService", "EVENT_PROVISIONING_NOTIFICATION from unknown NetworkMonitor");
                    break;
                }

                Int32 type;
                nai->mNetworkInfo->GetType(&type);
                String extraInfo;
                nai->mNetworkInfo->GetExtraInfo(&extraInfo);

                mHost->SetProvNotificationVisibleIntent(
                    TRUE, arg2, type, extraInfo, IPendingIntent::Probe(obj));
            }
            break;
        }
        case INetworkStateTracker::EVENT_STATE_CHANGED: {
            info = INetworkInfo::Probe(obj);
            NetworkInfoState state;
            info->GetState(&state);

            if (VDBG || (state == NetworkInfoState_CONNECTED) ||
                    (state == NetworkInfoState_DISCONNECTED) ||
                    (state == NetworkInfoState_SUSPENDED)) {
                String name;
                info->GetTypeName(&name);
                NetworkInfoDetailedState detailedState;
                info->GetDetailedState(&detailedState);
                Slogger::D("CConnectivityService", "ConnectivityChange for %s: %d/%%d",
                    name.string(), state, detailedState);
            }

            // EventLogTags.writeConnectivityStateChanged(
            //         info.GetType(), info.GetSubtype(), info.GetDetailedState().ordinal());

            Boolean bval;
            info->IsConnectedToProvisioningNetwork(&bval);
            if (bval) {
                /**
                 * TODO: Create ConnectivityManager.TYPE_MOBILE_PROVISIONING
                 * for now its an in between network, its a network that
                 * is actually a default network but we don't want it to be
                 * announced as such to keep background applications from
                 * trying to use it. It turns out that some still try so we
                 * take the additional step of clearing any default routes
                 * to the link that may have incorrectly setup by the lower
                 * levels.
                 */
                Int32 type;
                info->GetType(&type);
                AutoPtr<ILinkProperties> lp = mHost->GetLinkPropertiesForTypeInternal(type);
                if (DBG) {
                    Slogger::D("CConnectivityService", "EVENT_STATE_CHANGED: connected to provisioning network, lp=%s",
                        TO_CSTR(lp));
                }

                // Clear any default routes setup by the radio so
                // any activity by applications trying to use this
                // connection will fail until the provisioning network
                // is enabled.
                /*
                for (RouteInfo r : lp.GetRoutes()) {
                    removeRoute(lp, r, TO_DEFAULT_TABLE,
                                mNetTrackers[info.GetType()].GetNetwork().netId);
                }
                */
            } else if (state == NetworkInfoState_DISCONNECTED) {
            } else if (state == NetworkInfoState_SUSPENDED) {
            } else if (state == NetworkInfoState_CONNECTED) {
            //    handleConnect(info);
            }
            mHost->NotifyLockdownVpn(NULL);
            break;
        }
        case INetworkStateTracker::EVENT_CONFIGURATION_CHANGED: {
            // TODO: Temporary allowing network configuration
            //       change not resetting sockets.
            //       @see bug/4455071
            /*
            info = INetworkInfo::Probe(obj);
            handleConnectivityChange(info.GetType(), mCurrentLinkProperties[info.GetType()],
                    FALSE);
            */
            break;
        }
    }
    return NOERROR;
}

//==============================================================================
// CConnectivityService::SettingsObserver
//==============================================================================
CConnectivityService::SettingsObserver::SettingsObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 what,
    /* [in] */ CConnectivityService* host)
    : mHandler(handler)
    , mWhat(what)
    , mHost(host)
{}

ECode CConnectivityService::SettingsObserver::Observe(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUri> uri;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetUriFor(ISettingsGlobal::HTTP_PROXY, (IUri**)&uri);
    return resolver->RegisterContentObserver(uri, FALSE, this);
}

ECode CConnectivityService::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(mWhat, (IMessage**)&msg);
    return msg->SendToTarget();
}

//==============================================================================
// CConnectivityService
//==============================================================================

CAR_INTERFACE_IMPL_2(CConnectivityService, Object, IIConnectivityManager, IBinder)

CAR_OBJECT_IMPL(CConnectivityService)

CConnectivityService::CConnectivityService()
    : mLockdownEnabled(FALSE)
    , mNetworkPreference(0)
    , mActiveDefaultNetwork(IConnectivityManager::TYPE_NONE)
    , mDefaultInetConditionPublished(0)
    , mNumDnsEntries(0)
    , mTestMode(FALSE)
    , mSystemReady(FALSE)
    , mNetTransitionWakeLockCausedBy("")
    , mNetTransitionWakeLockSerialNumber(0)
    , mNetTransitionWakeLockTimeout(0)
    , mDefaultProxyDisabled(FALSE)
    , mNetworksDefined(0)
    , mNextNetId(MIN_NET_ID)
    , mNextNetworkRequestId(-1)
    , mIsNotificationVisible(FALSE)
{
}

CConnectivityService::~CConnectivityService()
{
    mInetLog = NULL;
}

ECode CConnectivityService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IINetworkManagementService* netManager,
    /* [in] */ IINetworkStatsService* statsService,
    /* [in] */ IINetworkPolicyManager* policyManager)
{
    if (DBG) Slogger::D(TAG, "ConnectivityService starting up!");

    mLegacyTypeTracker = new LegacyTypeTracker(this);

    CFile::New(PROVISIONING_URL_PATH, (IFile**)&mProvisioningUrlFile);
    CAtomicInteger32::New(0, (IAtomicInteger32**)&mEnableFailFastMobileDataTag);

    CHashMap::New((IHashMap**)&mNetworkFactoryInfos);
    CHashMap::New((IHashMap**)&mNetworkRequests);

    CSparseArray::New((ISparseArray**)&mNetworkForRequestId);
    CSparseArray::New((ISparseArray**)&mNetworkForNetId);

    CHashMap::New((IHashMap**)&mNetworkAgentInfos);

    CNetworkPolicyListener::New(this, (IINetworkPolicyListener**)&mPolicyListener);

    mUserPresentReceiver = new InternalBroadcastReceiver(this);
    CDataActivityObserver::New(this, (IINetworkManagementEventObserver**)&mDataActivityObserver);

    AutoPtr<INetworkCapabilities> netCap;
    CNetworkCapabilities::New((INetworkCapabilities**)&netCap);
    netCap->AddCapability(INetworkCapabilities::NET_CAPABILITY_INTERNET);
    netCap->AddCapability(INetworkCapabilities::NET_CAPABILITY_NOT_RESTRICTED);
    CNetworkRequest::New(netCap, IConnectivityManager::TYPE_NONE,
        NextNetworkRequestId(), (INetworkRequest**)&mDefaultRequest);
    AutoPtr<IBinder> binder;
    CBinder::New((IBinder**)&binder);
    AutoPtr<NetworkRequestInfo> nri = new NetworkRequestInfo(
        NULL, mDefaultRequest, binder,  NetworkRequestInfo::REQUEST, this);
    mNetworkRequests->Put(TO_IINTERFACE(mDefaultRequest), TO_IINTERFACE(nri));

    AutoPtr<IHandlerThread> handlerThread;
    CHandlerThread::New(String("ConnectivityServiceThread"), (IHandlerThread**)&handlerThread);
    IThread::Probe(handlerThread)->Start();
    AutoPtr<ILooper> looper;
    handlerThread->GetLooper((ILooper**)&looper);
    mHandler = new InternalHandler(looper, this);
    mTrackerHandler = new NetworkStateTrackerHandler(looper, this);

    // setup our unique device name
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String prop;
    sysProp->Get(String("net.hostname"), &prop);
    String hostname;
    AutoPtr<ISettingsSecure> sysSec;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&sysSec);
    sysSec->GetString(resolver, ISettingsSecure::DEVICE_HOSTNAME, &hostname);
    if (TextUtils::IsEmpty(hostname) &&
        TextUtils::IsEmpty(prop)) {
        String id;
        sysSec->GetString(resolver, ISettingsSecure::ANDROID_ID, &id);
        if (!id.IsNullOrEmpty()) {
            String name("android-");
            name += id;
            sysProp->Set(String("net.hostname"), name);
        }
    }
    else {
        sysProp->Set(String("net.hostname"), hostname);
    }

    // read our default dns server ip
    String dns;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetString(resolver, ISettingsGlobal::DEFAULT_DNS_SERVER, &dns);
    if (dns.IsNullOrEmpty()) {
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        resources->GetString(R::string::config_default_dns_server, &dns);
    }
    ECode ec = NetworkUtils::NumericToInetAddress(dns, (IInetAddress**)&mDefaultDns);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Error setting defaultDns using %s", dns.string());
    }

    mContext = context;
    assert(mContext != NULL);
    mNetd = netManager;
    assert(mNetd != NULL);
    mPolicyManager = policyManager;
    assert(mPolicyManager != NULL);
    AutoPtr<IKeyStoreHelper> ksHelper;
    //TODO CKeyStoreHelper::AcquireSingleton((IKeyStoreHelper**)&ksHelper);
    //TODO ksHelper->GetInstance((IKeyStore**)&mKeyStore);
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&service);
    mTelephonyManager = ITelephonyManager::Probe(service);

    ec = mPolicyManager->RegisterListener(mPolicyListener);
    if (FAILED(ec)) {
        // ouch, no rules updates means some processes may never get network
        Slogger::E(TAG, "unable to register IINetworkPolicyListener");
    }

    service = NULL;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    AutoPtr<IPowerManager> powerManager = IPowerManager::Probe(service);
    powerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String(TAG), (IPowerManagerWakeLock**)&mNetTransitionWakeLock);
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    resources->GetInteger(R::integer::config_networkTransitionTimeout, &mNetTransitionWakeLockTimeout);

    mNetTrackers = ArrayOf<INetworkStateTracker*>::Alloc(IConnectivityManager::MAX_NETWORK_TYPE + 1);

    mNetConfigs = ArrayOf<INetworkConfig*>::Alloc(IConnectivityManager::MAX_NETWORK_TYPE + 1);

    // TODO: What is the "correct" way to do determine if this is a wifi only device?
    Boolean wifiOnly, bval;
    sysProp->GetBoolean(String("ro.radio.noril"), FALSE, &wifiOnly);
    Slogger::D(TAG, "wifiOnly=%d", wifiOnly);

    AutoPtr<IConnectivityManagerHelper> cmHelper;
    CConnectivityManagerHelper::AcquireSingleton((IConnectivityManagerHelper**)&cmHelper);
    AutoPtr<ArrayOf<String> > naStrings;
    resources->GetStringArray(R::array::networkAttributes, (ArrayOf<String>**)&naStrings);
    Int32 nType;
    for (Int32 i = 0; i < naStrings->GetLength(); i++) {
        String naString = (*naStrings)[i];
        AutoPtr<INetworkConfig> n;
        CNetworkConfig::New(naString, (INetworkConfig**)&n);
        if (VDBG) Slogger::D(TAG, "naString=%s config=%s",
            naString.string(), TO_CSTR(n));

        n->GetType(&nType);
        if (nType > IConnectivityManager::MAX_NETWORK_TYPE) {
            Slogger::E(TAG, "Error in networkAttributes - ignoring attempt to define type %d", nType);
            continue;
        }

        if (wifiOnly && (cmHelper->IsNetworkTypeMobile(nType, &bval), bval)) {
            Slogger::D(TAG, "networkAttributes - ignoring mobile as this dev is wifiOnly %d", nType);
            continue;
        }

        if ((*mNetConfigs)[nType] != NULL) {
            Slogger::E(TAG, "Error in networkAttributes - ignoring attempt to redefine type %d", nType);
            continue;
        }

        mLegacyTypeTracker->AddSupportedType(nType);

        mNetConfigs->Set(nType, n);
        mNetworksDefined++;
    }

    if (VDBG) Slogger::D(TAG, "mNetworksDefined=" + mNetworksDefined);

    AutoPtr< ArrayOf<Int32> > protectedNetworks;
    resources->GetInt32Array(R::array::config_protectedNetworks, (ArrayOf<Int32>**)&protectedNetworks);
    for (Int32 i = 0; i < protectedNetworks->GetLength(); i++) {
        Int32 p = (*protectedNetworks)[i];
        if ((*mNetConfigs)[p] != NULL &&
                (Find(mProtectedNetworks.Begin(), mProtectedNetworks.End(), p) == mProtectedNetworks.End())) {
                mProtectedNetworks.PushBack(p);
        }
        else {
            if (DBG) Slogger::E(TAG, "Ignoring protectedNetwork %d", p);
        }
    }

    String modeStr, typeStr;
    sysProp->Get(String("cm.test.mode"), &modeStr);
    sysProp->Get(String("ro.build.type"), &typeStr);
    mTestMode = modeStr.Equals("TRUE") && typeStr.Equals("eng");

    looper = NULL;
    mHandler->GetLooper((ILooper**)&looper);
    CTethering::NewByFriend(mContext, mNetd, statsService, looper, (CTethering**)&mTethering);

    mPermissionMonitor = new PermissionMonitor(mContext, mNetd);

    //set up the listener for user state for creating user VPNs
    mUserIntentReceiver = new UserIntentReceiver(this);
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_USER_STARTING);
    intentFilter->AddAction(IIntent::ACTION_USER_STOPPING);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiverAsUser(
        mUserIntentReceiver, UserHandle::ALL, intentFilter,
        String(NULL), NULL, (IIntent**)&intent);
    AutoPtr<IINetworkManagementEventObserver> clat;
    CNat464Xlat::New(mContext, mNetd, this, mTrackerHandler,
        (IINetworkManagementEventObserver**)&clat);
    mClat = (Nat464Xlat*)clat.Get();

    //try {
    mNetd->RegisterObserver(mTethering);
    mNetd->RegisterObserver(mDataActivityObserver);
    mNetd->RegisterObserver(mClat);
    //} catch (RemoteException e) {
    //    Loge("Error registering observer :" + e);
    //}

    if (DBG) {
        mInetLog = new List<String>();
    }

    mSettingsObserver = new SettingsObserver(mHandler, EVENT_APPLY_GLOBAL_HTTP_PROXY, this);
    mSettingsObserver->constructor(mHandler);
    mSettingsObserver->Observe(mContext);

    mDataConnectionStats = new DataConnectionStats(mContext);
    mDataConnectionStats->StartMonitoring();

    service = NULL;
    context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&service);
    mAlarmManager = IAlarmManager::Probe(service);

    AutoPtr<IIntentFilter> filter, newFilter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(CConnectivityService::ACTION_PKT_CNT_SAMPLE_INTERVAL_ELAPSED);
    AutoPtr<IBroadcastReceiver> receiver = new PktCntSampleIntervalElapsedeceiver(this);
    CIntentFilter::New(filter, (IIntentFilter**)&newFilter);
    AutoPtr<IIntent> stickyIntent;
    mContext->RegisterReceiver(receiver, newFilter, (IIntent**)&stickyIntent);

    mPacManager = new PacManager(mContext, mHandler, EVENT_PROXY_HAS_CHANGED);

    service = NULL;
    context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&service);
    mUserManager = IUserManager::Probe(service);
    return NOERROR;
}

Int32 CConnectivityService::NextNetworkRequestId()
{
    AutoLock lock(this);
    return mNextNetworkRequestId++;
}

ECode CConnectivityService::AssignNextNetId(
    /* [in] */ INetworkAgentInfo* naiObj)
{
    NetworkAgentInfo* nai = (NetworkAgentInfo*)naiObj;
    ISynchronize* syncObj = (ISynchronize*)mNetworkForNetId->Probe(EIID_ISynchronize);
    synchronized(syncObj) {
        for (Int32 i = MIN_NET_ID; i <= MAX_NET_ID; i++) {
            Int32 netId = mNextNetId;
            if (++mNextNetId > MAX_NET_ID) mNextNetId = MIN_NET_ID;
            // Make sure NetID unused.  http://b/16815182
            AutoPtr<IInterface> obj;
            mNetworkForNetId->Get(netId, (IInterface**)&obj);
            if (obj == NULL) {
                nai->mNetwork = NULL;
                CNetwork::New(netId, (INetwork**)&nai->mNetwork);
                mNetworkForNetId->Put(netId, naiObj);
                return NOERROR;
            }
        }
    }

    Slogger::E(TAG, "IllegalStateException: No free netIds");
    return E_ILLEGAL_STATE_EXCEPTION;
}

Int32 CConnectivityService::GetConnectivityChangeDelay()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    /** Check system properties for the default value then use secure settings value, if any. */
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    Int32 defaultDelay;
    sysProp->GetInt32(String("conn.") + ISettingsGlobal::CONNECTIVITY_CHANGE_DELAY,
            IConnectivityManager::CONNECTIVITY_CHANGE_DELAY_DEFAULT, &defaultDelay);
    Int32 val;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetInt32(cr, ISettingsGlobal::CONNECTIVITY_CHANGE_DELAY,
            defaultDelay, &val);
    return val;
}

Boolean CConnectivityService::Teardown(
    /* [in] */ INetworkStateTracker* netTracker)
{
    Boolean bval;
    if (netTracker->Teardown(&bval), bval) {
        netTracker->SetTeardownRequested(TRUE);
        return TRUE;
    }

    return FALSE;
}

Boolean CConnectivityService::IsNetworkBlocked(
    /* [in] */ Int32 networkType,
    /* [in] */ Int32 uid)
{
    AutoPtr<ILinkProperties> lp;
    GetLinkPropertiesForType(networkType, (ILinkProperties**)&lp);
    return IsNetworkWithLinkPropertiesBlocked(lp, uid);
}

Boolean CConnectivityService::IsNetworkBlocked(
    /* [in] */ INetworkAgentInfo* naiObj,
    /* [in] */ Int32 uid)
{
    NetworkAgentInfo* nai = (NetworkAgentInfo*)naiObj;
    return IsNetworkWithLinkPropertiesBlocked(nai->mLinkProperties, uid);
}

Boolean CConnectivityService::IsNetworkWithLinkPropertiesBlocked(
    /* [in] */ ILinkProperties* lp,
    /* [in] */ Int32 uid)
{
    Boolean networkCostly = FALSE;
    Int32 uidRules;

    String iface("");
    if (lp != NULL) {
        lp->GetInterfaceName(&iface);
    }

    {
        AutoLock lock(mRulesLock);

        networkCostly = !iface.IsNull() && mMeteredIfaces.Find(iface) != mMeteredIfaces.End();
        HashMap<Int32, Int32>::Iterator it = mUidRules.Find(uid);
        uidRules = (it != mUidRules.End()) ? it->mSecond : INetworkPolicyManager::RULE_ALLOW_ALL;
    }

    if (networkCostly && (uidRules & INetworkPolicyManager::RULE_REJECT_METERED) != 0) {
        return TRUE;
    }

    // no restrictive rules; network is visible
    return FALSE;
}

AutoPtr<INetworkInfo> CConnectivityService::GetFilteredNetworkInfo(
    /* [in] */ Int32 networkType,
    /* [in] */ Int32 uid)
{
    AutoPtr<INetworkInfo> info = GetNetworkInfoForType(networkType);
    return GetFilteredNetworkInfo(info, networkType, uid);
}

AutoPtr<INetworkInfo> CConnectivityService::GetFilteredNetworkInfo(
    /* [in] */ INetworkInfo* info,
    /* [in] */ Int32 networkType,
    /* [in] */ Int32 uid)
{
    AutoPtr<INetworkInfo> networkInfo = info;
    if (IsNetworkBlocked(networkType, uid)) {
        // network is blocked; clone and override state
        networkInfo = NULL;
        CNetworkInfo::New(info, (INetworkInfo**)&networkInfo);
        String nullStr;
        info->SetDetailedState(NetworkInfoDetailedState_BLOCKED, nullStr, nullStr);
        if (VDBG) Slogger::D(TAG, "returning Blocked NetworkInfo");
    }
    if (mLockdownTracker != NULL) {
        networkInfo = mLockdownTracker->AugmentNetworkInfo(info);
        if (VDBG) Slogger::D(TAG, "returning Locked NetworkInfo");
    }
    return info;
}

AutoPtr<INetworkInfo> CConnectivityService::GetFilteredNetworkInfo(
    /* [in] */ INetworkAgentInfo* naiObj,
    /* [in] */ Int32 uid)
{
    NetworkAgentInfo* nai = (NetworkAgentInfo*)naiObj;
    AutoPtr<INetworkInfo> info = nai->mNetworkInfo;
    if (IsNetworkBlocked(naiObj, uid)) {
        // network is blocked; clone and override state
        AutoPtr<INetworkInfo> networkInfo = info;
        info = NULL;
        CNetworkInfo::New(networkInfo, (INetworkInfo**)&info);
        String nullStr;
        info->SetDetailedState(NetworkInfoDetailedState_BLOCKED, nullStr, nullStr);

        if (DBG) Slogger::D(TAG, "returning Blocked NetworkInfo");
    }

    if (mLockdownTracker != NULL) {
        AutoPtr<INetworkInfo> networkInfo = info;
        info = mLockdownTracker->AugmentNetworkInfo(networkInfo);
        if (DBG) Slogger::D(TAG, "returning Locked NetworkInfo");
    }
    return info;
}

ECode CConnectivityService::GetActiveNetworkInfo(
    /* [out] */ INetworkInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(EnforceInternetPermission())
    Int32 uid = Binder::GetCallingUid();
    return GetNetworkInfo(mActiveDefaultNetwork, uid, result);
}

AutoPtr<INetworkInfo> CConnectivityService::GetProvisioningNetworkInfo()
{
    if (FAILED(EnforceInternetPermission())) {
        return NULL;
    }

    // Find the first Provisioning Network
    AutoPtr<INetworkInfo> provNi;
    AutoPtr< ArrayOf<INetworkInfo*> > infos;
    GetAllNetworkInfo((ArrayOf<INetworkInfo*>**)&infos);
    Boolean bval;
    for (Int32 i = 0; i < infos->GetLength(); ++i) {
        INetworkInfo* ni = (*infos)[i];
        if (ni->IsConnectedToProvisioningNetwork(&bval), bval) {
            provNi = ni;
            break;
        }
    }
    if (DBG) Slogger::D(TAG, "GetProvisioningNetworkInfo: X provNi=%s",
        TO_CSTR(provNi));
    return provNi;
}

ECode CConnectivityService::GetProvisioningOrActiveNetworkInfo(
    /* [out] */ INetworkInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = NULL;
    FAIL_RETURN(EnforceInternetPermission())

    AutoPtr<INetworkInfo> provNi = GetProvisioningNetworkInfo();
    if (provNi == NULL) {
        Int32 uid = Binder::GetCallingUid();
        GetNetworkInfo(mActiveDefaultNetwork, uid, (INetworkInfo**)&provNi);
    }
    if (DBG) Slogger::D(TAG, "getProvisioningOrActiveNetworkInfo: X provNi=%s",
         TO_CSTR(provNi));
    *info = provNi;
    REFCOUNT_ADD(*info)
    return NOERROR;
}

ECode CConnectivityService::GetActiveNetworkInfoUnfiltered(
    /* [out] */ INetworkInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    FAIL_RETURN(EnforceInternetPermission())
    Boolean isValid = FALSE;
    IsNetworkTypeValid(mActiveDefaultNetwork, &isValid);
    if (isValid) {
        AutoPtr<INetworkInfo> ni = GetNetworkInfoForType(mActiveDefaultNetwork);
        *info = ni;
        REFCOUNT_ADD(*info)
    }

    return NOERROR;
}

ECode CConnectivityService::GetActiveNetworkInfoForUid(
    /* [in] */ Int32 uid,
    /* [out] */ INetworkInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    FAIL_RETURN(EnforceConnectivityInternalPermission())
    return GetNetworkInfo(mActiveDefaultNetwork, uid, info);
}

ECode CConnectivityService::GetNetworkInfo(
    /* [in] */ Int32 networkType,
    /* [out] */ INetworkInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(EnforceInternetPermission())
    Int32 uid = Binder::GetCallingUid();
    return GetNetworkInfo(networkType, uid, result);
}

ECode CConnectivityService::GetNetworkInfo(
    /* [in] */ Int32 networkType,
    /* [in] */ Int32 uid,
    /* [out] */ INetworkInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    Boolean isValid = FALSE;
    IsNetworkTypeValid(networkType, &isValid);
    if (isValid) {
        if (GetNetworkInfoForType(networkType) != NULL) {
            AutoPtr<INetworkInfo> ni = GetFilteredNetworkInfo(networkType, uid);
            *info = ni;
            REFCOUNT_ADD(*info)
        }
    }
    return NOERROR;
}

ECode CConnectivityService::GetNetworkInfoForNetwork(
    /* [in] */ INetwork* network,
    /* [out] */ INetworkInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = NULL;

    FAIL_RETURN(EnforceInternetPermission())
    if (network == NULL) return NOERROR;

    Int32 uid = Binder::GetCallingUid();
    AutoPtr<INetworkAgentInfo> nai;
    ISynchronize* syncObj = (ISynchronize*)mNetworkForNetId->Probe(EIID_ISynchronize);
    synchronized(syncObj) {
        Int32 id;
        network->GetNetId(&id);
        AutoPtr<IInterface> obj;
        mNetworkForNetId->Get(id, (IInterface**)&obj);
        nai = INetworkAgentInfo::Probe(obj);
    }
    if (nai == NULL) return NOERROR;
    NetworkAgentInfo* naiObj = (NetworkAgentInfo*)nai.Get();
    synchronized(naiObj) {
        if (naiObj->mNetworkInfo == NULL) return NOERROR;
        AutoPtr<INetworkInfo> ni = GetFilteredNetworkInfo(nai, uid);
        *info = ni;
        REFCOUNT_ADD(*info)
    }
    return NOERROR;
}

ECode CConnectivityService::GetAllNetworkInfo(
    /* [out, callee] */ ArrayOf<INetworkInfo*>** allInfo)
{
    VALIDATE_NOT_NULL(allInfo);
    *allInfo = NULL;

    FAIL_RETURN(EnforceInternetPermission())
    Int32 uid = Binder::GetCallingUid();
    List< AutoPtr<INetworkInfo> > result;

    for (Int32 networkType = 0; networkType <= IConnectivityManager::MAX_NETWORK_TYPE;
            networkType++) {
        if (GetNetworkInfoForType(networkType) != NULL) {
            AutoPtr<INetworkInfo> ni = GetFilteredNetworkInfo(networkType, uid);
            result.PushBack(ni);
        }
    }

    AutoPtr< ArrayOf<INetworkInfo*> > infoArray = ArrayOf<INetworkInfo*>::Alloc(result.GetSize());
    List< AutoPtr<INetworkInfo> >::Iterator it = result.Begin();
    for (Int32 i = 0; it != result.End(); ++it, ++i) {
        AutoPtr<INetworkInfo> netInfo = *it;
        infoArray->Set(i, netInfo);
    }
    *allInfo = infoArray;
    REFCOUNT_ADD(*allInfo);
    return NOERROR;
}

ECode CConnectivityService::GetNetworkForType(
    /* [in] */ Int32 networkType,
    /* [out] */ INetwork** network)
{
    VALIDATE_NOT_NULL(network)
    *network = NULL;

    FAIL_RETURN(EnforceInternetPermission())
    Int32 uid = Binder::GetCallingUid();
    if (IsNetworkBlocked(networkType, uid)) {
        return NOERROR;
    }

    AutoPtr<INetworkAgentInfo> nai = mLegacyTypeTracker->GetNetworkForType(networkType);
    if (nai != NULL) {
        NetworkAgentInfo* naiObj = (NetworkAgentInfo*)nai.Get();
        *network = naiObj->mNetwork;
        REFCOUNT_ADD(*network)
    }
    return NOERROR;
}

ECode CConnectivityService::GetAllNetworks(
    /* [out, callee] */ ArrayOf<INetwork*>** networks)
{
    VALIDATE_NOT_NULL(networks)
    FAIL_RETURN(EnforceInternetPermission())

    AutoPtr< ArrayOf<INetwork*> > array;
    ISynchronize* syncObj = (ISynchronize*)mNetworkForNetId->Probe(EIID_ISynchronize);
    synchronized(syncObj) {
        Int32 size = 0;
        mNetworkForNetId->GetSize(&size);
        array = ArrayOf<INetwork*>::Alloc(size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mNetworkForNetId->ValueAt(i, (IInterface**)&obj);
            NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(obj);
            AutoPtr<INetwork> network;
            CNetwork::New(nai->mNetwork, (INetwork**)&network);
            array->Set(i, network);
        }
    }

    *networks = array;
    REFCOUNT_ADD(*networks)
    return NOERROR;
}

ECode CConnectivityService::IsNetworkSupported(
    /* [in] */ Int32 networkType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    FAIL_RETURN(EnforceInternetPermission())

    Boolean isValid = FALSE;
    IsNetworkTypeValid(networkType, &isValid);
    AutoPtr<INetworkInfo> info = GetNetworkInfoForType(networkType);
    *result = (isValid && info != NULL);
    return NOERROR;
}

ECode CConnectivityService::GetActiveLinkProperties(
    /* [out] */ ILinkProperties** properties)
{
    return GetLinkPropertiesForType(mActiveDefaultNetwork, properties);
}

ECode CConnectivityService::GetLinkPropertiesForType(
    /* [in] */ Int32 networkType,
    /* [out] */ ILinkProperties** properties)
{
    VALIDATE_NOT_NULL(properties)
    *properties = NULL;
    FAIL_RETURN(EnforceInternetPermission())
    Boolean bval;
    IsNetworkTypeValid(networkType, &bval);
    if (bval) {
        AutoPtr<ILinkProperties> lp = GetLinkPropertiesForTypeInternal(networkType);
        *properties = lp;
        REFCOUNT_ADD(*properties)
    }
    return NOERROR;
}

ECode CConnectivityService::GetLinkProperties(
    /* [in] */ INetwork* network,
    /* [out] */ ILinkProperties** properties)
{
    VALIDATE_NOT_NULL(properties)
    *properties = NULL;

    FAIL_RETURN(EnforceInternetPermission())
    AutoPtr<NetworkAgentInfo> nai;
    ISynchronize* syncObj = (ISynchronize*)mNetworkForNetId->Probe(EIID_ISynchronize);
    synchronized(syncObj) {
        Int32 id;
        network->GetNetId(&id);
        AutoPtr<IInterface> obj;
        mNetworkForNetId->Get(id, (IInterface**)&obj);
        nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(obj);
    }

    if (nai != NULL) {
        synchronized(nai) {
            return CLinkProperties::New(nai->mLinkProperties, properties);
        }
    }
    return NOERROR;
}

ECode CConnectivityService::GetNetworkCapabilities(
    /* [in] */ INetwork* network,
    /* [out] */ INetworkCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    FAIL_RETURN(EnforceInternetPermission())

    AutoPtr<NetworkAgentInfo> nai;
    ISynchronize* syncObj = (ISynchronize*)mNetworkForNetId->Probe(EIID_ISynchronize);
    synchronized(syncObj) {
        Int32 id;
        network->GetNetId(&id);
        AutoPtr<IInterface> obj;
        mNetworkForNetId->Get(id, (IInterface**)&obj);
        nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(obj);
    }
    if (nai != NULL) {
        synchronized(nai) {
            return CNetworkCapabilities::New(nai->mNetworkCapabilities, result);
        }
    }
    return NOERROR;
}

ECode CConnectivityService::GetAllNetworkState(
    /* [out, callee] */ ArrayOf<INetworkState*>** allStates)
{
    VALIDATE_NOT_NULL(allStates);
    *allStates = NULL;

    FAIL_RETURN(EnforceInternetPermission())
    Int32 uid = Binder::GetCallingUid();

    List<AutoPtr<INetworkState> > result;
    AutoPtr<INetworkInfo> info;
    AutoPtr<ILinkProperties> lp;
    AutoPtr<INetworkCapabilities> netcap;
    for (Int32 networkType = 0; networkType <= IConnectivityManager::MAX_NETWORK_TYPE; networkType++) {
        info = GetNetworkInfoForType(networkType);
        if (info != NULL) {
            info = GetFilteredNetworkInfo(networkType, uid);
            lp = GetLinkPropertiesForTypeInternal(networkType);
            netcap = GetNetworkCapabilitiesForType(networkType);
            AutoPtr<INetworkState> state;
            CNetworkState::New(info, lp, netcap, (INetworkState**)&state);
            result.PushBack(state);
        }
    }

    AutoPtr< ArrayOf<INetworkState*> > stateArray = ArrayOf<INetworkState*>::Alloc(result.GetSize());
    List< AutoPtr<INetworkState> >::Iterator it = result.Begin();
    for (Int32 i = 0; it != result.End(); ++it, ++i) {
        AutoPtr<INetworkState> netState = *it;
        stateArray->Set(i, netState);
    }
    *allStates = stateArray;
    REFCOUNT_ADD(*allStates);
    return NOERROR;
}

AutoPtr<INetworkState> CConnectivityService::GetNetworkStateUnchecked(
    /* [in] */ Int32 networkType)
{
    Boolean isValid = FALSE;
    IsNetworkTypeValid(networkType, &isValid);
    if (isValid) {
        AutoPtr<INetworkInfo> info = GetNetworkInfoForType(networkType);
        if (info != NULL) {
            AutoPtr<ILinkProperties> lp = GetLinkPropertiesForTypeInternal(networkType);
            AutoPtr<INetworkCapabilities> netcap = GetNetworkCapabilitiesForType(networkType);
            AutoPtr<INetworkState> state;
            CNetworkState::New(info, lp, netcap, (INetworkState**)&state);
            return state;
        }
    }
    return NULL;
}

ECode CConnectivityService::GetActiveNetworkQuotaInfo(
    /* [out] */ INetworkQuotaInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;
    FAIL_RETURN(EnforceInternetPermission())

    Int64 token = Binder::ClearCallingIdentity();
    AutoPtr<INetworkState> state = GetNetworkStateUnchecked(mActiveDefaultNetwork);
    if (state != NULL) {
        ECode ec = mPolicyManager->GetNetworkQuotaInfo(state, info);
        Binder::RestoreCallingIdentity(token);
        return ec;
    }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CConnectivityService::IsActiveNetworkMetered(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    FAIL_RETURN(EnforceInternetPermission())
    Int64 token = Binder::ClearCallingIdentity();
    *result = IsNetworkMeteredUnchecked(mActiveDefaultNetwork);
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

Boolean CConnectivityService::IsNetworkMeteredUnchecked(
    /* [in] */ Int32 networkType)
{
    AutoPtr<INetworkState> state = GetNetworkStateUnchecked(networkType);
    if (state != NULL) {
        Boolean result;
        ECode ec = mPolicyManager->IsNetworkMetered(state, &result);
        if (SUCCEEDED(ec)) return result;
    }
    return FALSE;
}

ECode CConnectivityService::RequestRouteToHostAddress(
    /* [in] */ Int32 networkType,
    /* [in] */ ArrayOf<Byte>* hostAddress,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    FAIL_RETURN(EnforceChangePermission())

    if (Find(mProtectedNetworks.Begin(), mProtectedNetworks.End(), networkType)
            != mProtectedNetworks.End()) {
        FAIL_RETURN(EnforceConnectivityInternalPermission())
    }

    AutoPtr<IInetAddressHelper> iaHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&iaHelper);
    AutoPtr<IInetAddress> addr;
    ECode ec = iaHelper->GetByAddress(hostAddress, (IInetAddress**)&addr);
    if (ec == (ECode)E_UNKNOWN_HOST_EXCEPTION) {
        if (DBG) Slogger::D(TAG, "requestRouteToHostAddress got E_UNKNOWN_HOST_EXCEPTION");
        return FALSE;
    }

    Boolean isValid;
    IsNetworkTypeValid(networkType, &isValid);
    if (!isValid) {
        if (DBG) Slogger::D(TAG, "requestRouteToHostAddress on invalid network: %d", networkType);
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<INetworkAgentInfo> nai = mLegacyTypeTracker->GetNetworkForType(networkType);
    if (nai == NULL) {
        if (mLegacyTypeTracker->IsTypeSupported(networkType) == FALSE) {
            if (DBG) Slogger::D(TAG, "requestRouteToHostAddress on unsupported network: %d", networkType);
        }
        else {
            if (DBG) Slogger::D(TAG, "requestRouteToHostAddress on down network: %d", networkType);
        }
        return FALSE;
    }

    NetworkInfoDetailedState netState;
    NetworkAgentInfo* naiObj = (NetworkAgentInfo*)nai.Get();
    synchronized(naiObj) {
        naiObj->mNetworkInfo->GetDetailedState(&netState);
    }

    if (netState != NetworkInfoDetailedState_CONNECTED
        && netState != NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK) {
        if (VDBG) {
            Slogger::D(TAG, "requestRouteToHostAddress on down network "
                "(%d) - dropped  netState=%d", networkType, netState);
        }
        return FALSE;
    }

    Int32 uid = Binder::GetCallingUid();
    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<ILinkProperties> lp;
    Int32 netId;
    synchronized(naiObj) {
        lp = naiObj->mLinkProperties;
        naiObj->mNetwork->GetNetId(&netId);
    }
    Boolean ok = AddLegacyRouteToHost(lp, addr, netId, uid);
    if (DBG) Slogger::D(TAG, "requestRouteToHostAddress ok=%d", ok);
    *result = ok;

    Binder::RestoreCallingIdentity(token);

    return NOERROR;
}

Boolean CConnectivityService::AddLegacyRouteToHost(
    /* [in] */ ILinkProperties* lp,
    /* [in] */ IInetAddress* addr,
    /* [in] */ Int32 netId,
    /* [in] */ Int32 uid)
{
    AutoPtr<IRouteInfo> bestRoute;
    AutoPtr<IRouteInfoHelper> riHelper;
    CRouteInfoHelper::AcquireSingleton((IRouteInfoHelper**)&riHelper);
    AutoPtr<IList> routes;
    lp->GetAllRoutes((IList**)&routes);
    riHelper->SelectBestRoute(ICollection::Probe(routes), addr, (IRouteInfo**)&bestRoute);
    if (bestRoute == NULL) {
        String iface;
        lp->GetInterfaceName(&iface);
        riHelper->MakeHostRoute(addr, iface, (IRouteInfo**)&bestRoute);
    }
    else {
        String iface;
        bestRoute->GetInterface(&iface);
        AutoPtr<IInetAddress> gateway;
        bestRoute->GetGateway((IInetAddress**)&gateway);
        if (Object::Equals(gateway, addr)) {
            // if there is no better route, add the implied hostroute for our gateway
            riHelper->MakeHostRoute(addr, iface, (IRouteInfo**)&bestRoute);
        }
        else {
            // if we will connect to this through another route, add a direct route
            // to it's gateway
            riHelper->MakeHostRoute(addr, gateway, iface, (IRouteInfo**)&bestRoute);
        }
    }
    if (DBG) {
        String iface;
        bestRoute->GetInterface(&iface);
        Slogger::D(TAG, "Adding %s for interface %s",
            TO_CSTR(bestRoute), iface.string());
    }

    ECode ec = mNetd->AddLegacyRouteForNetId(netId, bestRoute, uid);
    if (FAILED(ec)) {
        // never crash - catch them all
        if (DBG) Slogger::E(TAG, "Exception trying to add a route: error code = %08x", ec);
        return FALSE;
    }
    return TRUE;
}

ECode CConnectivityService::SetDataDependency(
    /* [in] */ Int32 networkType,
    /* [in] */ Boolean met)
{
    FAIL_RETURN(EnforceConnectivityInternalPermission())

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_SET_DEPENDENCY_MET,
        (met ? ENABLED : DISABLED), networkType, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

void CConnectivityService::HandleAsyncChannelHalfConnect(
    /* [in] */ IMessage* msg)
{
    Int32 arg1, arg2;
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    AutoPtr<IAsyncChannel> ac = IAsyncChannel::Probe(obj);
    Boolean bval;
    mNetworkFactoryInfos->ContainsKey(TO_IINTERFACE(replyTo), &bval);
    if (bval) {
        if (arg1 == IAsyncChannel::STATUS_SUCCESSFUL) {
            if (VDBG) Slogger::D(TAG, "NetworkFactory connected");
            AutoPtr<IInterface> p;
            mNetworkFactoryInfos->Get(replyTo, (IInterface**)&p);
            AutoPtr<NetworkFactoryInfo> _p = (NetworkFactoryInfo*)IObject::Probe(p);
            _p->mAsyncChannel->SendMessage(IAsyncChannel::CMD_CHANNEL_FULL_CONNECTION);
            // A network factory has connected.  Send it all current NetworkRequests.
            AutoPtr<ICollection> values;
            mNetworkRequests->GetValues((ICollection**)&values);
            AutoPtr<IIterator> it;
            values->GetIterator((IIterator**)&it);
            Boolean hasNext;
            Int32 requestId, score;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> tmp;
                it->GetNext((IInterface**)&tmp);
                AutoPtr<NetworkRequestInfo> nri = (NetworkRequestInfo*)IProxyDeathRecipient::Probe(tmp);
                if (nri->mIsRequest == FALSE) continue;
                nri->mRequest->GetRequestId(&requestId);
                tmp = NULL;
                mNetworkForRequestId->Get(requestId, (IInterface**)&tmp);
                NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(tmp);
                score = 0;
                if (nai != NULL) {
                    nai->GetCurrentScore(&score);
                }
                ac->SendMessage(INetworkFactory::CMD_REQUEST_NETWORK,
                    score, 0, nri->mRequest);
            }
        }
        else {
            Slogger::E(TAG, "Error connecting NetworkFactory");
            mNetworkFactoryInfos->Remove(obj);
        }
    }
    else if (mNetworkAgentInfos->ContainsKey(replyTo, &bval), bval) {
        if (arg1 == IAsyncChannel::STATUS_SUCCESSFUL) {
            if (VDBG) Slogger::D(TAG, "NetworkAgent connected");
            // A network agent has requested a connection.  Establish the connection.
            AutoPtr<IInterface> tmp;
            mNetworkAgentInfos->Get(TO_IINTERFACE(replyTo), (IInterface**)&tmp);
            NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(tmp);
            nai->mAsyncChannel->SendMessage(IAsyncChannel::CMD_CHANNEL_FULL_CONNECTION);
        }
        else {
            Slogger::E(TAG, "Error connecting NetworkAgent");
            AutoPtr<IInterface> tmp;
            mNetworkAgentInfos->Remove(replyTo, (IInterface**)&tmp);
            NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(tmp);
            if (nai != NULL) {
                ISynchronize* syncObj = (ISynchronize*)mNetworkForNetId->Probe(EIID_ISynchronize);
                synchronized(syncObj) {
                    Int32 id;
                    nai->mNetwork->GetNetId(&id);
                    mNetworkForNetId->Remove(id);
                }
                // Just in case.
                mLegacyTypeTracker->Remove(nai);
            }
        }
    }
}

void CConnectivityService::HandleAsyncChannelDisconnected(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    AutoPtr<IInterface> tmp;
    mNetworkAgentInfos->Get(TO_IINTERFACE(replyTo), (IInterface**)&tmp);
    NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(tmp);
    if (nai != NULL) {
        if (DBG) {
            Int32 size;
            nai->mNetworkRequests->GetSize(&size);
            Slogger::D(TAG, "%s got DISCONNECTED, was satisfying %d",
                nai->Name().string(), size);
        }
        // A network agent has disconnected.
        if (nai->mCreated) {
            // Tell netd to clean up the configuration for this network
            // (routing rules, DNS, etc).
            Int32 id;
            nai->mNetwork->GetNetId(&id);
            ECode ec =  mNetd->RemoveNetwork(id);
            if (FAILED(ec)) {
                Slogger::E(TAG, "Exception removing network: %08x", ec);
            }
        }
        // TODO - if we move the logic to the network agent (have them disconnect
        // because they lost all their requests or because their score isn't good)
        // then they would disconnect organically, report their new state and then
        // disconnect the channel.
        Boolean bval;
        if (nai->mNetworkInfo->IsConnected(&bval), bval) {
            String nullStr;
            nai->mNetworkInfo->SetDetailedState(NetworkInfoDetailedState_DISCONNECTED, nullStr, nullStr);
        }
        if (IsDefaultNetwork(nai)) {
            mDefaultInetConditionPublished = 0;
        }
        NotifyNetworkCallbacks(nai, IConnectivityManager::CALLBACK_LOST);
        nai->mNetworkMonitor->SendMessage(NetworkMonitor::CMD_NETWORK_DISCONNECTED);
        mNetworkAgentInfos->Remove(TO_IINTERFACE(replyTo));
        UpdateClat(NULL, nai->mLinkProperties, nai);
        mLegacyTypeTracker->Remove(nai);
        Int32 id ;
        nai->mNetwork->GetNetId(&id);
        ISynchronize* syncObj = (ISynchronize*)mNetworkForNetId->Probe(EIID_ISynchronize);
        synchronized(syncObj) {
            mNetworkForNetId->Remove(id);
        }

        // Since we've lost the network, go through all the requests that
        // it was satisfying and see if any other factory can satisfy them.
        // TODO: This logic may be better replaced with a call to RematchAllNetworksAndRequests
        List<AutoPtr<NetworkAgentInfo> > toActivate;
        List<AutoPtr<NetworkAgentInfo> >::Iterator lit;
        Int32 size = 0, netId, requestId;
        nai->mNetworkRequests->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            nai->mNetworkRequests->ValueAt(i, (IInterface**)&obj);
            AutoPtr<INetworkRequest> request = INetworkRequest::Probe(obj);
            request->GetRequestId(&requestId);
            AutoPtr<IInterface> tmpObj;
            mNetworkForRequestId->Get(requestId, (IInterface**)&tmpObj);
            AutoPtr<NetworkAgentInfo> currentNetwork = (NetworkAgentInfo*)INetworkAgentInfo::Probe(tmpObj);
            if (currentNetwork != NULL && (currentNetwork->mNetwork->GetNetId(&netId), netId) == id) {
                if (DBG) {
                    Slogger::D(TAG, "Checking for replacement network to handle request %s",
                        TO_CSTR(request));
                }
                mNetworkForRequestId->Remove(requestId);
                SendUpdatedScoreToFactories(request, 0);
                AutoPtr<NetworkAgentInfo> alternative;
                AutoPtr<ISet> set;
                mNetworkAgentInfos->GetEntrySet((ISet**)&set);
                AutoPtr<IIterator> it;
                set->GetIterator((IIterator**)&it);
                Boolean hasNext;
                Int32 score1, score2;
                AutoPtr<INetworkCapabilities> netcap;
                request->GetNetworkCapabilities((INetworkCapabilities**)&netcap);
                while (it->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> mpObj, valueObj;
                    it->GetNext((IInterface**)&mpObj);
                    IMapEntry* entry = IMapEntry::Probe(mpObj);
                    entry->GetValue((IInterface**)&valueObj);
                    AutoPtr<NetworkAgentInfo> existing = (NetworkAgentInfo*)INetworkAgentInfo::Probe(valueObj);
                    existing->mNetworkInfo->IsConnected(&bval);
                    if (bval) {
                        netcap->SatisfiedByNetworkCapabilities(existing->mNetworkCapabilities, &bval);
                        if (bval) {
                            existing->GetCurrentScore(&score2);
                            if (alternative == NULL || (alternative->GetCurrentScore(&score1), score1) < score2) {
                                alternative = existing;
                            }
                        }
                    }
                }
                if (alternative != NULL) {
                    if (DBG) {
                        Slogger::D(TAG, " found replacement in %s", alternative->Name().string());
                    }
                    lit = Find(toActivate.Begin(), toActivate.End(), alternative);
                    if (lit == toActivate.End()) {
                        toActivate.PushBack(alternative);
                    }
                }
            }
        }

        mDefaultRequest->GetRequestId(&requestId);
        AutoPtr<IInterface> tmpObj;
        nai->mNetworkRequests->Get(requestId, (IInterface**)&tmpObj);
        if (tmpObj != NULL) {
            RemoveDataActivityTracking(nai);
            mActiveDefaultNetwork = IConnectivityManager::TYPE_NONE;
            NotifyLockdownVpn(nai);
            RequestNetworkTransitionWakelock(nai->Name());
        }

        for (lit = toActivate.Begin(); lit != toActivate.End(); ++lit) {
            NetworkAgentInfo* networkToActivate = *lit;
            networkToActivate->mNetworkLingered->Clear();
            networkToActivate->mNetworkMonitor->SendMessage(NetworkMonitor::CMD_NETWORK_CONNECTED);
            RematchNetworkAndRequests((INetworkAgentInfo*)networkToActivate, FALSE);
        }
    }
}

void CConnectivityService::HandleRegisterNetworkRequest(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<NetworkRequestInfo> nri = (NetworkRequestInfo*)IProxyDeathRecipient::Probe(obj);
    AutoPtr<INetworkCapabilities> newCap;
    nri->mRequest->GetNetworkCapabilities((INetworkCapabilities**)&newCap);

    // Check for the best currently alive network that satisfies this request
    AutoPtr<NetworkAgentInfo> bestNetwork;

    AutoPtr<ICollection> values;
    mNetworkRequests->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext, bval;
    Int32 score, bestScore;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> tmp;
        it->GetNext((IInterface**)&tmp);
        NetworkAgentInfo* network = (NetworkAgentInfo*)INetworkAgentInfo::Probe(tmp);

        if (DBG) Slogger::D(TAG, "handleRegisterNetworkRequest checking %s", network->Name().string());
        if (newCap->SatisfiedByNetworkCapabilities(network->mNetworkCapabilities, &bval), bval) {
            network->GetCurrentScore(&score);
            if (DBG) {
                Slogger::D(TAG, "apparently satisfied.  currentScore=%d", score);
            }
            if ((bestNetwork == NULL)
                || (bestNetwork->GetCurrentScore(&bestScore), bestScore) < score) {
                if (!nri->mIsRequest) {
                    // Not setting bestNetwork here as a listening NetworkRequest may be
                    // satisfied by multiple Networks.  Instead the request is added to
                    // each satisfying Network and notified about each.
                    network->AddRequest(nri->mRequest);
                    NotifyNetworkCallback(network, nri);
                } else {
                    bestNetwork = network;
                }
            }
        }
    }
    Int32 id = 0;
    nri->mRequest->GetRequestId(&id);
    AutoPtr<IInterface> pId;
    mNetworkForRequestId->Get(id, (IInterface**)&pId);
    if (pId != NULL) {
        if (DBG) {
            Slogger::E(TAG, "ignoring duplicate request");
        }
    }
    else if (bestNetwork != NULL) {
        if (VDBG) Slogger::D(TAG, "using %s", bestNetwork->Name().string());
        Boolean bval;
        bestNetwork->mNetworkInfo->IsConnected(&bval);
        if (nri->mIsRequest && bval) {
            // Cancel any lingering so the linger timeout doesn't teardown this network
            // even though we have a request for it.
            bestNetwork->mNetworkLingered->Clear();
            bestNetwork->mNetworkMonitor->SendMessage(NetworkMonitor::CMD_NETWORK_CONNECTED);
        }
        bestNetwork->AddRequest(nri->mRequest);
        nri->mRequest->GetRequestId(&id);
        mNetworkForRequestId->Put(id, TO_IINTERFACE(bestNetwork));
        NotifyNetworkCallback((INetworkAgentInfo*)bestNetwork.Get(), nri);
        bestNetwork->GetCurrentScore(&score);
        Int32 type;
        nri->mRequest->GetLegacyType(&type);
        if (type != IConnectivityManager::TYPE_NONE) {
            //To support legacy calls for network request
            mLegacyTypeTracker->Add(type, (INetworkAgentInfo*)bestNetwork.Get());
        }
    }
    mNetworkRequests->Put((IInterface*)(nri->mRequest.Get()), TO_IINTERFACE(nri));
    if (nri->mIsRequest) {
        if (DBG) Slogger::D(TAG, "sending new NetworkRequest to factories");

        values = NULL;
        it = NULL;
        mNetworkFactoryInfos->GetValues((ICollection**)&values);
        values->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> tmp;
            it->GetNext((IInterface**)&tmp);
            NetworkFactoryInfo* nfi = (NetworkFactoryInfo*)IObject::Probe(tmp);
            nfi->mAsyncChannel->SendMessage(INetworkFactory::CMD_REQUEST_NETWORK, score, 0, nri->mRequest);
        }
    }
}

void CConnectivityService::HandleReleaseNetworkRequest(
    /* [in] */ INetworkRequest* request,
    /* [in] */ Int32 callingUid)
{
    AutoPtr<IInterface> obj;
    mNetworkRequests->Get(request, (IInterface**)&obj);
    AutoPtr<NetworkRequestInfo> nri = (NetworkRequestInfo*)IProxyDeathRecipient::Probe(obj);
    if (nri != NULL) {
        if (IProcess::SYSTEM_UID != callingUid && nri->mUid != callingUid) {
            if (DBG) Slogger::D(TAG, "Attempt to release unowned NetworkRequest %s",
                TO_CSTR(request));
            return;
        }
        if (DBG) Slogger::D(TAG, "releasing NetworkRequest %s",
            TO_CSTR(request));

        nri->UnlinkDeathRecipient();
        Int32 requestId, size;
        nri->mRequest->GetRequestId(&requestId);

        mNetworkRequests->Remove(request);
        if (nri->mIsRequest) {
            // Find all networks that are satisfying this request and remove the request
            // from their request lists.
            AutoPtr<ICollection> values;
            mNetworkRequests->GetValues((ICollection**)&values);
            AutoPtr<IIterator> it;
            values->GetIterator((IIterator**)&it);
            Boolean hasNext, keep;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> tmp, requestObj;
                it->GetNext((IInterface**)&tmp);
                NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(tmp);
                nai->mNetworkRequests->Get(requestId, (IInterface**)&requestObj);
                if (requestObj != NULL) {
                    nai->mNetworkRequests->Remove(requestId);
                    nai->mNetworkRequests->GetSize(&size);
                    if (DBG) {
                        Slogger::D(TAG, " Removing from current network %s, leaving %d requests.",
                            nai->Name().string(), size);
                    }
                    // check if has any requests remaining and if not,
                    // disconnect (unless it's a VPN).
                    nai->IsVPN(&keep);
                    for (Int32 i = 0; i < size && !keep; i++) {
                        AutoPtr<IInterface> nrObj;
                        nai->mNetworkRequests->ValueAt(i, (IInterface**)&nrObj);
                        AutoPtr<INetworkRequest> r = INetworkRequest::Probe(nrObj);
                        if (IsRequest(r)) keep = TRUE;
                    }
                    if (!keep) {
                        if (DBG) Slogger::D(TAG, "no live requests for %s; disconnecting",
                            nai->Name().string());
                        nai->mAsyncChannel->Disconnect();
                    }
                }
            }

            // Maintain the illusion.  When this request arrived, we might have preteneded
            // that a network connected to serve it, even though the network was already
            // connected.  Now that this request has gone away, we might have to pretend
            // that the network disconnected.  LegacyTypeTracker will generate that
            // phatom disconnect for this type.
            AutoPtr<IInterface> naiObj;
            mNetworkForRequestId->Get(requestId, (IInterface**)&naiObj);
            AutoPtr<INetworkAgentInfo> nai = INetworkAgentInfo::Probe(naiObj);
            if (nai != NULL) {
                mNetworkForRequestId->Remove(requestId);
                Int32 type;
                nri->mRequest->GetLegacyType(&type);
                if (type != IConnectivityManager::TYPE_NONE) {
                    mLegacyTypeTracker->Remove(type, nai);
                }
            }

            values = NULL;
            it = NULL;
            mNetworkFactoryInfos->GetValues((ICollection**)&values);
            values->GetIterator((IIterator**)&it);
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> tmp;
                it->GetNext((IInterface**)&tmp);
                NetworkFactoryInfo* nfi = (NetworkFactoryInfo*)IObject::Probe(tmp);
                nfi->mAsyncChannel->SendMessage(INetworkFactory::CMD_CANCEL_REQUEST, nri->mRequest);
            }
        }
        else {
            // listens don't have a singular affectedNetwork.  Check all networks to see
            // if this listen request applies and remove it.
            AutoPtr<ICollection> values;
            mNetworkAgentInfos->GetValues((ICollection**)&values);
            AutoPtr<IIterator> it;
            values->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> tmp;
                it->GetNext((IInterface**)&tmp);
                NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(tmp);
                nai->mNetworkRequests->Remove(requestId);
            }
        }
        CallCallbackForRequest(nri, NULL, IConnectivityManager::CALLBACK_RELEASED);
    }
}

void CConnectivityService::HandleSetDependencyMet(
    /* [in] */ Int32 networkType,
    /* [in] */ Boolean met)
{
    if ((*mNetTrackers)[networkType] != NULL) {
        if (DBG) {
           Slogger::D(TAG, "HandleSetDependencyMet(%d, %s)", networkType, met ? "TRUE" : "FALSE");
        }
        (*mNetTrackers)[networkType]->SetDependencyMet(met);
    }
}

ECode CConnectivityService::SetPolicyDataEnable(
    /* [in] */ Int32 networkType,
    /* [in] */ Boolean enabled)
{
    // only someone like NPMS should only be calling us
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::MANAGE_NETWORK_POLICY, TAG));

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_SET_POLICY_DATA_ENABLE,
        networkType, (enabled ? ENABLED : DISABLED), (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

void CConnectivityService::HandleSetPolicyDataEnable(
    /* [in] */ Int32 networkType,
    /* [in] */ Boolean enabled)
{
    // TODO - handle this passing to factories
    // Boolean isValid = FALSE;
    // if ((IsNetworkTypeValid(networkType, &isValid), isValid)) {
    //     AutoPtr<INetworkStateTracker> tracker = (*mNetTrackers)[networkType];
    //     if (tracker != NULL) {
    //         tracker->SetPolicyDataEnable(enabled);
    //     }
    // }
}

ECode CConnectivityService::EnforceInternetPermission()
{
    return mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::INTERNET,
            String("ConnectivityService"));
}

ECode CConnectivityService::EnforceAccessPermission()
{
    return mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::ACCESS_NETWORK_STATE,
            String("ConnectivityService"));
}

ECode CConnectivityService::EnforceChangePermission()
{
    return mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CHANGE_NETWORK_STATE,
            String("ConnectivityService"));
}

ECode CConnectivityService::EnforceTetherAccessPermission()
{
    return mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::ACCESS_NETWORK_STATE,
            String("ConnectivityService"));
}

ECode CConnectivityService::EnforceConnectivityInternalPermission()
{
    return mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL,
            String("ConnectivityService"));
}

ECode CConnectivityService::SendConnectedBroadcast(
    /* [in] */ INetworkInfo* info)
{
    FAIL_RETURN(EnforceConnectivityInternalPermission())
    SendGeneralBroadcast(info, IConnectivityManager::CONNECTIVITY_ACTION_IMMEDIATE);
    SendGeneralBroadcast(info, IConnectivityManager::CONNECTIVITY_ACTION);
    return NOERROR;
}

void CConnectivityService::SendConnectedBroadcastDelayed(
    /* [in] */ INetworkInfo* info,
    /* [in] */ Int32 delayMs)
{
    SendGeneralBroadcast(info, IConnectivityManager::CONNECTIVITY_ACTION_IMMEDIATE);
    SendGeneralBroadcastDelayed(info, IConnectivityManager::CONNECTIVITY_ACTION, delayMs);
}

void CConnectivityService::SendInetConditionBroadcast(
    /* [in] */ INetworkInfo* info)
{
    SendGeneralBroadcast(info, IConnectivityManager::INET_CONDITION_ACTION);
}

AutoPtr<IIntent> CConnectivityService::MakeGeneralIntent(
    /* [in] */ INetworkInfo* info,
    /* [in] */ const String& bcastType)
{
    AutoPtr<INetworkInfo> egressInfo = info;
    if (mLockdownTracker != NULL) {
        egressInfo = mLockdownTracker->AugmentNetworkInfo(egressInfo);
    }

    AutoPtr<IIntent> intent;
    CIntent::New(bcastType, (IIntent**)&intent);
    AutoPtr<INetworkInfo> infoNew;
    CNetworkInfo::New(egressInfo, (INetworkInfo**)&infoNew);
    intent->PutExtra(IConnectivityManager::EXTRA_NETWORK_INFO, IParcelable::Probe(infoNew));
    Int32 type;
    egressInfo->GetType(&type);
    intent->PutExtra(IConnectivityManager::EXTRA_NETWORK_TYPE, type);

    Boolean failover;
    if ((egressInfo->IsFailover(&failover), failover)) {
        intent->PutBooleanExtra(IConnectivityManager::EXTRA_IS_FAILOVER, TRUE);
        egressInfo->SetFailover(FALSE);
    }
    String reason;
    if (egressInfo->GetReason(&reason), !reason.IsNull()) {
        intent->PutExtra(IConnectivityManager::EXTRA_REASON, reason);
    }
    String extraInfo;
    if (egressInfo->GetExtraInfo(&extraInfo), !extraInfo.IsNull()) {
        intent->PutExtra(IConnectivityManager::EXTRA_EXTRA_INFO, extraInfo);
    }
    Int32 requestId = 0;
    mDefaultRequest->GetRequestId(&requestId);
    AutoPtr<IInterface> tmpObj;
    mNetworkForRequestId->Get(requestId, (IInterface**)&tmpObj);
    AutoPtr<NetworkAgentInfo> def = (NetworkAgentInfo*)INetworkAgentInfo::Probe(tmpObj);
    Boolean isDefault = FALSE;
    Boolean bHasTransport = FALSE;
    def->mNetworkCapabilities->HasTransport(INetworkCapabilities::TRANSPORT_CELLULAR, &bHasTransport);
    if((type == IConnectivityManager::TYPE_MOBILE) &&
        bHasTransport) {
        isDefault = TRUE;
    }

    def->mNetworkCapabilities->HasTransport(INetworkCapabilities::TRANSPORT_WIFI, &bHasTransport);
    if((type == IConnectivityManager::TYPE_WIFI) &&
        bHasTransport) {
        isDefault = TRUE;
    }
    intent->PutExtra(String("isDefault"), isDefault);
    intent->PutExtra(IConnectivityManager::EXTRA_INET_CONDITION, mDefaultInetConditionPublished);
    return intent;
}

void CConnectivityService::SendGeneralBroadcast(
    /* [in] */ INetworkInfo* info,
    /* [in] */ const String& bcastType)
{
    AutoPtr<IIntent> intent = MakeGeneralIntent(info, bcastType);
    SendStickyBroadcast(intent);
}

void CConnectivityService::SendGeneralBroadcastDelayed(
    /* [in] */ INetworkInfo* info,
    /* [in] */ const String& bcastType,
    /* [in] */ Int32 delayMs)
{
    AutoPtr<IIntent> intent = MakeGeneralIntent(info, bcastType);
    SendStickyBroadcastDelayed(intent, delayMs);
}

void CConnectivityService::SendDataActivityBroadcast(
    /* [in] */ Int32 deviceType,
    /* [in] */ Boolean active,
    /* [in] */ Int64 tsNanos)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IConnectivityManager::ACTION_DATA_ACTIVITY_CHANGE, (IIntent**)&intent);
    intent->PutExtra(IConnectivityManager::EXTRA_DEVICE_TYPE, deviceType);
    intent->PutBooleanExtra(IConnectivityManager::EXTRA_IS_ACTIVE, active);
    intent->PutExtra(IConnectivityManager::EXTRA_REALTIME_NS, tsNanos);
    Int64 ident = Binder::ClearCallingIdentity();
    mContext->SendOrderedBroadcastAsUser(intent, UserHandle::ALL,
            Elastos::Droid::Manifest::permission::RECEIVE_DATA_ACTIVITY_CHANGE,
            NULL, NULL, 0, String(NULL), NULL);
    Binder::RestoreCallingIdentity(ident);
}

void CConnectivityService::SendStickyBroadcast(
    /* [in] */ IIntent* intent)
{
    ISynchronize* syncObj = (ISynchronize*)Probe(EIID_ISynchronize);
    AutoLock lock(syncObj);
    if (!mSystemReady) {
        mInitialBroadcast = NULL;
        CIntent::New(intent, (IIntent**)&mInitialBroadcast);
    }
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    if (DBG) {
       String action;
       intent->GetAction(&action);
       Slogger::V(TAG, "SendStickyBroadcast: action=%s", action.string());
    }

    Int64 ident = Binder::ClearCallingIdentity();
    mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
    Binder::RestoreCallingIdentity(ident);
}

void CConnectivityService::SendStickyBroadcastDelayed(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 delayMs)
{
    if (delayMs <= 0) {
        SendStickyBroadcast(intent);
    }
    else {
        if (VDBG) {
           String action;
           intent->GetAction(&action);
           Slogger::V(TAG, "SendStickyBroadcastDelayed: delayMs=%d, action=%s", delayMs, action.string());
        }

        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(EVENT_SEND_STICKY_BROADCAST_INTENT,
            intent, (IMessage**)&msg);
        Boolean result;
        mHandler->SendMessageDelayed(msg, delayMs, &result);
    }
}

ECode CConnectivityService::SystemReady()
{
    // start network sampling ..
    AutoPtr<IIntent> intent;
    CIntent::New(ACTION_PKT_CNT_SAMPLE_INTERVAL_ELAPSED, (IIntent**)&intent);
    String pkgName;
    mContext->GetPackageName(&pkgName);
    intent->SetPackage(pkgName);

    AutoPtr<IPendingIntentHelper> piHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);
    mSampleIntervalElapsedIntent = NULL;
    piHelper->GetBroadcast(mContext, SAMPLE_INTERVAL_ELAPSED_REQUEST_CODE, intent, 0,
        (IPendingIntent**)&mSampleIntervalElapsedIntent);
    SetAlarm(DEFAULT_START_SAMPLING_INTERVAL_IN_SECONDS * 1000, mSampleIntervalElapsedIntent);

    LoadGlobalProxy();

    {
        ISynchronize* syncObj = (ISynchronize*)Probe(EIID_ISynchronize);
        AutoLock lock(syncObj);
        mSystemReady = TRUE;
        if (mInitialBroadcast != NULL) {
            mContext->SendStickyBroadcastAsUser(mInitialBroadcast, UserHandle::ALL);
            mInitialBroadcast = NULL;
        }
    }
    // load the global proxy at startup
    //send EVENT_APPLY_GLOBAL_HTTP_PROXY
    Boolean result;
    mHandler->SendEmptyMessage(EVENT_APPLY_GLOBAL_HTTP_PROXY, &result);

    // Try bringing up tracker, but if KeyStore isn't ready yet, wait
    // for user to unlock device.
    if ((UpdateLockdownVpn(&result), !result)) {
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New(IIntent::ACTION_USER_PRESENT, (IIntentFilter**)&filter);
        AutoPtr<IIntent> intent;
        mContext->RegisterReceiver(mUserPresentReceiver, filter, (IIntent**)&intent);
    }

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_SYSTEM_READY, (IMessage**)&msg);
    mHandler->SendMessage(msg, &result);
    mPermissionMonitor->StartMonitoring();
    return NOERROR;
}

ECode CConnectivityService::CaptivePortalCheckCompleted(
    /* [in] */ INetworkInfo* info,
    /* [in] */ Boolean isCaptivePortal)
{
    FAIL_RETURN(EnforceConnectivityInternalPermission())
    if (DBG) {
        Slogger::D(TAG, "captivePortalCheckCompleted: ni=%s captive=%d",
            TO_CSTR(info), isCaptivePortal);
    }
//        mNetTrackers[info.GetType()].captivePortalCheckCompleted(isCaptivePortal);
    return NOERROR;
}

void CConnectivityService::SetupDataActivityTracking(
    /* [in] */ INetworkAgentInfo* nai)
{
    NetworkAgentInfo* networkAgent = (NetworkAgentInfo* )nai;
    String iface;
    networkAgent->mLinkProperties->GetInterfaceName(&iface);

    Int32 timeout;
    Int32 type = IConnectivityManager::TYPE_NONE;

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Boolean bval;
    if (networkAgent->mNetworkCapabilities->HasTransport(
        INetworkCapabilities::TRANSPORT_CELLULAR, &bval), bval) {
        sg->GetInt32(cr, ISettingsGlobal::DATA_ACTIVITY_TIMEOUT_MOBILE, 5, &timeout);
        type = IConnectivityManager::TYPE_MOBILE;
    }
    else if (networkAgent->mNetworkCapabilities->HasTransport(
        INetworkCapabilities::TRANSPORT_WIFI, &bval), &bval) {
        sg->GetInt32(cr, ISettingsGlobal::DATA_ACTIVITY_TIMEOUT_WIFI, 0, &timeout);
        type = IConnectivityManager::TYPE_WIFI;
    }
    else {
        // do not track any other networks
        timeout = 0;
    }

    if (timeout > 0 && iface != NULL && type != IConnectivityManager::TYPE_NONE) {
        ECode ec = mNetd->AddIdleTimer(iface, timeout, type);
        if (FAILED(ec)) {
            // You shall not crash!
            Slogger::E(TAG, "Exception in SetupDataActivityTracking %08x", ec);
        }
    }
}

void CConnectivityService::RemoveDataActivityTracking(
    /* [in] */ INetworkAgentInfo* nai)
{
    NetworkAgentInfo* networkAgent = (NetworkAgentInfo*)nai;
    String iface;
    networkAgent->mLinkProperties->GetInterfaceName(&iface);
    AutoPtr<INetworkCapabilities> caps = networkAgent->mNetworkCapabilities;

    if (iface != NULL) {
        Boolean b1, b2;
        caps->HasTransport(INetworkCapabilities::TRANSPORT_CELLULAR, &b1);
        caps->HasTransport(INetworkCapabilities::TRANSPORT_WIFI, &b2);
        if (b1 || b2) {
            // the call fails silently if no idletimer setup for this interface
            ECode ec = mNetd->RemoveIdleTimer(iface);
            if (FAILED(ec)) {
                Slogger::E(TAG, "Exception in RemoveDataActivityTracking %08x", ec);
            }
        }
    }
}

void CConnectivityService::UpdateMtu(
    /* [in] */ ILinkProperties* newLp,
    /* [in] */ ILinkProperties* oldLp)
{
    String iface;
    newLp->GetInterfaceName(&iface);
    Int32 mtu;
    newLp->GetMtu(&mtu);
    Boolean bval;
    if (oldLp != NULL && (newLp->IsIdenticalMtu(oldLp, &bval), bval)) {
        if (VDBG) Slogger::D(TAG, "identical MTU - not setting");
        return;
    }

    AutoPtr<ILinkPropertiesHelper> lpHelper;
    CLinkPropertiesHelper::AcquireSingleton((ILinkPropertiesHelper**)&lpHelper);

    Boolean isMtu = FALSE;
    newLp->HasGlobalIPv6Address(&bval);
    lpHelper->IsValidMtu(mtu, bval, &isMtu);
    if (isMtu == FALSE) {
        Slogger::E(TAG, "Unexpected mtu value: %d, %s", mtu, iface.string());
        return;
    }

    // Cannot set MTU without interface name
    if (TextUtils::IsEmpty(iface)) {
        Slogger::E(TAG, "Setting MTU size with NULL iface.");
        return;
    }

    if (DBG) Slogger::D(TAG, "Setting MTU size: %s, %d", iface.string(), mtu);
    ECode ec = mNetd->SetMtu(iface, mtu);
    if (FAILED(ec)) {
        Slogger::E(TAG, "exception in setMtu()");
    }
}

void CConnectivityService::UpdateTcpBufferSizes(
    /* [in] */ INetworkAgentInfo* networkAgent)
{
    if (IsDefaultNetwork(networkAgent) == FALSE) {
        return;
    }

    NetworkAgentInfo* nai = (NetworkAgentInfo*)networkAgent;
    String tcpBufferSizes;
    nai->mLinkProperties->GetTcpBufferSizes(&tcpBufferSizes);
    AutoPtr<ArrayOf<String> > values;
    if (tcpBufferSizes != NULL) {
        StringUtils::Split(tcpBufferSizes, ",", (ArrayOf<String>**)&values);
    }

    if (values == NULL || values->GetLength() != 6) {
        if (DBG) {
            Slogger::D(TAG, "Invalid tcpBufferSizes string: %s, using defaults",
                tcpBufferSizes.string());
        }
        tcpBufferSizes = DEFAULT_TCP_BUFFER_SIZES;
        values = NULL;
        StringUtils::Split(tcpBufferSizes, ",", (ArrayOf<String>**)&values);
    }

    if (tcpBufferSizes.Equals(mCurrentTcpBufferSizes)) return;


    if (DBG) Slogger::D(TAG, "Setting tx/rx TCP buffers to %s", tcpBufferSizes.string());

    String defaultRwndKey("net.tcp.default_init_rwnd");
    String sysctlKey("sys.sysctl.tcp_def_init_rwnd");
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 defaultRwndValue, rwndValue;

    String prefix("/sys/kernel/ipv4/tcp_");
    ECode ec = NOERROR;
    ec = FileUtils::StringToFile(prefix + "rmem_min", (*values)[0]); FAIL_GOTO(ec, _EXIT_);
    ec = FileUtils::StringToFile(prefix + "rmem_def", (*values)[1]); FAIL_GOTO(ec, _EXIT_);
    ec = FileUtils::StringToFile(prefix + "rmem_max", (*values)[2]); FAIL_GOTO(ec, _EXIT_);
    ec = FileUtils::StringToFile(prefix + "wmem_min", (*values)[3]); FAIL_GOTO(ec, _EXIT_);
    ec = FileUtils::StringToFile(prefix + "wmem_def", (*values)[4]); FAIL_GOTO(ec, _EXIT_);
    ec = FileUtils::StringToFile(prefix + "wmem_max", (*values)[5]); FAIL_GOTO(ec, _EXIT_);
    mCurrentTcpBufferSizes = tcpBufferSizes;

_EXIT_:
    if (FAILED(ec)) {
        Slogger::E(TAG, "Can't set TCP buffer sizes:%08x", ec);
    }

    sysProp->GetInt32(defaultRwndKey, 0, &defaultRwndValue);

    sg->GetInt32(cr, ISettingsGlobal::TCP_DEFAULT_INIT_RWND, defaultRwndValue, &rwndValue);
    if (rwndValue != 0) {
        sysProp->Set(sysctlKey, StringUtils::ToString(rwndValue));
    }
}

void CConnectivityService::FlushVmDnsCache()
{
    /*
     * Tell the VMs to toss their DNS caches
     */
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_CLEAR_DNS_CACHE, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    /*
     * Connectivity events can happen before boot has completed ...
     */
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    Int64 ident = Binder::ClearCallingIdentity();
    mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
    Binder::RestoreCallingIdentity(ident);
}

ECode CConnectivityService::GetRestoreDefaultNetworkDelay(
    /* [in] */ Int32 networkType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String restoreDefaultNetworkDelayStr;
    sysProp->Get(NETWORK_RESTORE_DELAY_PROP_NAME, &restoreDefaultNetworkDelayStr);
    if (!restoreDefaultNetworkDelayStr.IsNullOrEmpty()) {
        Int32 v;
        ECode ec = StringUtils::Parse(restoreDefaultNetworkDelayStr, &v);
        if (SUCCEEDED(ec)) return v;
    }
    // if the system property isn't set, use the value for the apn type
    Int32 ret = RESTORE_DEFAULT_NETWORK_DELAY;

    if ((networkType <= IConnectivityManager::MAX_NETWORK_TYPE) &&
            ((*mNetConfigs)[networkType] != NULL)) {
        (*mNetConfigs)[networkType]->GetRestoreTime(&ret);
    }
    return ret;
}

void CConnectivityService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ const ArrayOf<String>& args)
{
    assert(0);
    // IndentingPrintWriter pw = new IndentingPrintWriter(writer, "  ");
    // if (mContext->checkCallingOrSelfPermission(
    //         android.Manifest.permission.DUMP)
    //         != PackageManager.PERMISSION_GRANTED) {
    //     pw.println("Permission Denial: can't dump ConnectivityService " +
    //             "from from pid=" + Binder.GetCallingPid() + ", uid=" +
    //             Binder::GetCallingUid());
    //     return;
    // }

    // pw.println("NetworkFactories for:");
    // pw.increaseIndent();
    // for (NetworkFactoryInfo nfi : mNetworkFactoryInfos.values()) {
    //     pw.println(nfi.name);
    // }
    // pw.decreaseIndent();
    // pw.println();

    // NetworkAgentInfo defaultNai = mNetworkForRequestId.Get(mDefaultRequest.requestId);
    // pw.print("Active default network: ");
    // if (defaultNai == NULL) {
    //     pw.println("none");
    // } else {
    //     pw.println(defaultNai.network.netId);
    // }
    // pw.println();

    // pw.println("Current Networks:");
    // pw.increaseIndent();
    // for (NetworkAgentInfo nai : mNetworkAgentInfos.values()) {
    //     pw.println(nai->toString());
    //     pw.increaseIndent();
    //     pw.println("Requests:");
    //     pw.increaseIndent();
    //     for (Int32 i = 0; i < nai->mNetworkRequests.size(); i++) {
    //         pw.println(nai->mNetworkRequests.ValueAt(i).toString());
    //     }
    //     pw.decreaseIndent();
    //     pw.println("Lingered:");
    //     pw.increaseIndent();
    //     for (NetworkRequest nr : nai->mNetworkLingered) pw.println(nr.toString());
    //     pw.decreaseIndent();
    //     pw.decreaseIndent();
    // }
    // pw.decreaseIndent();
    // pw.println();

    // pw.println("Network Requests:");
    // pw.increaseIndent();
    // for (NetworkRequestInfo nri : mNetworkRequests.values()) {
    //     pw.println(nri.toString());
    // }
    // pw.println();
    // pw.decreaseIndent();

    // pw.print("mActiveDefaultNetwork: " + mActiveDefaultNetwork);
    // if (mActiveDefaultNetwork != IConnectivityManager::TYPE_NONE) {
    //     NetworkInfo activeNetworkInfo = getActiveNetworkInfo();
    //     if (activeNetworkInfo != NULL) {
    //         pw.print(" " + activeNetworkInfo.GetState() +
    //                  "/" + activeNetworkInfo.GetDetailedState());
    //     }
    // }
    // pw.println();

    // pw.println("mLegacyTypeTracker:");
    // pw.increaseIndent();
    // mLegacyTypeTracker->dump(pw);
    // pw.decreaseIndent();
    // pw.println();

    //ISynchronize* syncObj = this;
    // synchronized(syncObj) {
    //     pw.println("NetworkTransitionWakeLock is currently " +
    //             (mNetTransitionWakeLock.isHeld() ? "" : "not ") + "held.");
    //     pw.println("It was last requested for "+mNetTransitionWakeLockCausedBy);
    // }
    // pw.println();

    // mTethering.dump(fd, pw, args);

    // if (mInetLog != NULL) {
    //     pw.println();
    //     pw.println("Inet condition reports:");
    //     pw.increaseIndent();
    //     for(Int32 i = 0; i < mInetLog.size(); i++) {
    //         pw.println(mInetLog.Get(i));
    //     }
    //     pw.decreaseIndent();
    // }
}

Boolean CConnectivityService::IsLiveNetworkAgent(
    /* [in] */ INetworkAgentInfo* networkAgent,
    /* [in] */ const String& msg)
{
    NetworkAgentInfo* nai = (NetworkAgentInfo*)networkAgent;
    if (nai->mNetwork == NULL) return FALSE;
    AutoPtr<NetworkAgentInfo> officialNai;
    ISynchronize* syncObj = (ISynchronize*)mNetworkForNetId->Probe(EIID_ISynchronize);
    synchronized(syncObj) {
        Int32 netId;
        nai->mNetwork->GetNetId(&netId);
        AutoPtr<IInterface> obj;
        mNetworkForNetId->Get(netId, (IInterface**)&obj);
        officialNai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(obj);
    }
    if (officialNai != NULL && Object::Equals(officialNai, nai)) return TRUE;
    if (officialNai != NULL || VDBG) {
        Slogger::E(TAG, "%s - IsLiveNetworkAgent found mismatched netId: %s - %s",
            msg.string(), TO_CSTR(officialNai), TO_CSTR(nai));
    }
    return FALSE;
}

Boolean CConnectivityService::IsRequest(
    /* [in] */ INetworkRequest* request)
{
    AutoPtr<IInterface> obj;
    mNetworkRequests->Get(request, (IInterface**)&obj);
    NetworkRequestInfo* nri = (NetworkRequestInfo*)IObject::Probe(obj);
    return nri->mIsRequest;
}

ECode CConnectivityService::GetTetherConnectedSta(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    Boolean bSupported = FALSE;
    IsTetheringSupported(&bSupported);
    if (bSupported) {
        assert(0 && "TODO");
        // return mTethering->GetTetherConnectedSta(result);
        return NOERROR;
    }
    else {
        *result = NULL;
        return NOERROR;
    }
}

ECode CConnectivityService::Tether(
    /* [in] */ const String& iface,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);
    *status = 0;

    AutoPtr<IConnectivityManagerHelper> helper;
    CConnectivityManagerHelper::AcquireSingleton((IConnectivityManagerHelper**)&helper);
    FAIL_RETURN(helper->EnforceTetherChangePermission(mContext))

    Boolean supported;
    if (IsTetheringSupported(&supported), supported) {
        return mTethering->Tether(iface, status);
    }
    else {
        *status = IConnectivityManager::TETHER_ERROR_UNSUPPORTED;
        return NOERROR;
    }
}

ECode CConnectivityService::Untether(
    /* [in] */ const String& iface,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);
    *status = 0;

    AutoPtr<IConnectivityManagerHelper> helper;
    CConnectivityManagerHelper::AcquireSingleton((IConnectivityManagerHelper**)&helper);
    FAIL_RETURN(helper->EnforceTetherChangePermission(mContext))

    Boolean supported;
    if (IsTetheringSupported(&supported), supported) {
        return mTethering->Untether(iface, status);
    }
    else {
        *status = IConnectivityManager::TETHER_ERROR_UNSUPPORTED;
        return NOERROR;
    }
}

ECode CConnectivityService::GetLastTetherError(
    /* [in] */ const String& iface,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status)
    *status = 0;
    FAIL_RETURN(EnforceTetherAccessPermission());

    Boolean supported;
    if (IsTetheringSupported(&supported), supported) {
        return mTethering->GetLastTetherError(iface, status);
    }
    else {
        *status = IConnectivityManager::TETHER_ERROR_UNSUPPORTED;
        return NOERROR;
    }
}

ECode CConnectivityService::GetTetherableUsbRegexs(
    /* [out, callee] */ ArrayOf<String>** regexs)
{
    VALIDATE_NOT_NULL(regexs)
    *regexs = NULL;
    FAIL_RETURN(EnforceTetherAccessPermission());
    Boolean supported;
    if (IsTetheringSupported(&supported), supported) {
        return mTethering->GetTetherableUsbRegexs(regexs);
    }
    else {
        AutoPtr< ArrayOf<String> > strArray = ArrayOf<String>::Alloc(0);
        *regexs = strArray;
        REFCOUNT_ADD(*regexs);
        return NOERROR;
    }
}

ECode CConnectivityService::GetTetherableWifiRegexs(
    /* [out, callee] */ ArrayOf<String>** regexs)
{
    VALIDATE_NOT_NULL(regexs)
    *regexs = NULL;
    FAIL_RETURN(EnforceTetherAccessPermission());
    Boolean supported;
    if (IsTetheringSupported(&supported), supported) {
        return mTethering->GetTetherableWifiRegexs(regexs);
    }
    else {
        AutoPtr< ArrayOf<String> > strArray = ArrayOf<String>::Alloc(0);
        *regexs = strArray;
        REFCOUNT_ADD(*regexs);
        return NOERROR;
    }
}

ECode CConnectivityService::GetTetherableBluetoothRegexs(
    /* [out, callee] */ ArrayOf<String>** regexs)
{
    VALIDATE_NOT_NULL(regexs)
    *regexs = NULL;

    FAIL_RETURN(EnforceTetherAccessPermission());
    Boolean supported;
    if (IsTetheringSupported(&supported), supported) {
        return mTethering->GetTetherableBluetoothRegexs(regexs);
    }
    else {
        AutoPtr< ArrayOf<String> > strArray = ArrayOf<String>::Alloc(0);
        *regexs = strArray;
        REFCOUNT_ADD(*regexs);
        return NOERROR;
    }
}

ECode CConnectivityService::SetUsbTethering(
    /* [in] */ Boolean enable,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);
    *status = 0;

    AutoPtr<IConnectivityManagerHelper> helper;
    CConnectivityManagerHelper::AcquireSingleton((IConnectivityManagerHelper**)&helper);
    FAIL_RETURN(helper->EnforceTetherChangePermission(mContext))
    Boolean supported;
    if (IsTetheringSupported(&supported), supported) {
        return mTethering->SetUsbTethering(enable, status);
    }
    else {
        *status = IConnectivityManager::TETHER_ERROR_UNSUPPORTED;
        return NOERROR;
    }
}

// TODO - move iface listing, queries, etc to new module
ECode CConnectivityService::GetTetherableIfaces(
    /* [out, callee] */ ArrayOf<String>** ifaces)
{
    VALIDATE_NOT_NULL(ifaces)
    *ifaces = NULL;
    FAIL_RETURN(EnforceTetherAccessPermission());
    return mTethering->GetTetherableIfaces(ifaces);
}

ECode CConnectivityService::GetTetheredIfaces(
    /* [out, callee] */ ArrayOf<String>** ifaces)
{
    VALIDATE_NOT_NULL(ifaces)
    *ifaces = NULL;
    FAIL_RETURN(EnforceTetherAccessPermission());
    return mTethering->GetTetheredIfaces(ifaces);
}

ECode CConnectivityService::GetTetheredDhcpRanges(
    /* [out, callee] */ ArrayOf<String>** ifaces)
{
    VALIDATE_NOT_NULL(ifaces)
    *ifaces = NULL;
    FAIL_RETURN(EnforceTetherAccessPermission());
    return mTethering->GetTetheredDhcpRanges(ifaces);
}

ECode CConnectivityService::GetTetheringErroredIfaces(
    /* [out, callee] */ ArrayOf<String>** ifaces)
{
    VALIDATE_NOT_NULL(ifaces)
    *ifaces = NULL;
    FAIL_RETURN(EnforceTetherAccessPermission());
    AutoPtr< ArrayOf<String> > i = mTethering->GetErroredIfaces();
    *ifaces = i.Get();
    REFCOUNT_ADD(*ifaces);
    return NOERROR;
}

ECode CConnectivityService::IsTetheringSupported(
    /* [out] */ Boolean* status)
{
    VALIDATE_NOT_NULL(status)
    *status = FALSE;
    FAIL_RETURN(EnforceTetherAccessPermission());

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String denied;
    sysProp->Get(String("ro.tether.denied"), &denied);
    Int32 defaultVal =  denied.Equals("TRUE") ? 0 : 1;

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    Boolean bval;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetInt32(resolver, ISettingsGlobal::TETHER_SUPPORTED, defaultVal, &value);
    Boolean tetherEnabledInSettings = (value != 0)
        && !(mUserManager->HasUserRestriction(IUserManager::DISALLOW_CONFIG_TETHERING, &bval), bval);

    if (tetherEnabledInSettings) {
        Boolean sc = FALSE;
        AutoPtr<ArrayOf<String> > usb, wifi, bt, types;
        mTethering->GetTetherableUsbRegexs((ArrayOf<String>**)&usb);
        if (usb->GetLength() > 0) sc = TRUE;
        if (!sc) {
            mTethering->GetTetherableWifiRegexs((ArrayOf<String>**)&wifi);
            if (usb->GetLength() > 0) sc = TRUE;
        }
        if (!sc) {
            mTethering->GetTetherableBluetoothRegexs((ArrayOf<String>**)&bt);
            if (usb->GetLength() > 0) sc = TRUE;
        }

        if (sc) {
            *status = mTethering->GetUpstreamIfaceTypes()->GetLength() != 0;
        }
    }
    return NOERROR;
}

ECode CConnectivityService::RequestNetworkTransitionWakelock(
    /* [in] */ const String& forWhom)
{
    Int32 serialNum = 0;
    {
        ISynchronize* syncObj = (ISynchronize*)Probe(EIID_ISynchronize);
        AutoLock lock(syncObj);

        Boolean isHeld = FALSE;
        mNetTransitionWakeLock->IsHeld(&isHeld);
        if (isHeld) {
            return NOERROR;
        }
        serialNum = ++mNetTransitionWakeLockSerialNumber;
        mNetTransitionWakeLock->AcquireLock();
        mNetTransitionWakeLockCausedBy = forWhom;
    }

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_EXPIRE_NET_TRANSITION_WAKELOCK,
        serialNum, 0, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessageDelayed(msg, mNetTransitionWakeLockTimeout, &result);
}

ECode CConnectivityService::ReportInetCondition(
    /* [in] */ Int32 networkType,
    /* [in] */ Int32 percentage)
{
    if (percentage > 50) return NOERROR;  // don't handle good network reports
    AutoPtr<INetworkAgentInfo> nai = mLegacyTypeTracker->GetNetworkForType(networkType);
    if (nai != NULL) return ReportBadNetwork(((NetworkAgentInfo*)(nai.Get()))->mNetwork);
    return NOERROR;
}

ECode CConnectivityService::ReportBadNetwork(
    /* [in] */ INetwork* network)
{
    FAIL_RETURN(EnforceAccessPermission())
    FAIL_RETURN(EnforceInternetPermission())

    if (network == NULL) return NOERROR;

    Int32 uid = Binder::GetCallingUid();
    AutoPtr<NetworkAgentInfo> nai;
    ISynchronize* syncObj = (ISynchronize*)mNetworkForNetId->Probe(EIID_ISynchronize);
    synchronized(syncObj) {
        Int32 netId;
        network->GetNetId(&netId);
        AutoPtr<IInterface> obj;
        mNetworkForNetId->Get(netId, (IInterface**)&obj);
        nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(obj);
    }
    if (nai == NULL) return NOERROR;
    if (DBG) Slogger::D(TAG, "reportBadNetwork(%s) by %d", nai->Name().string(), uid);
    synchronized(nai) {
        // Validating an uncreated network could result in a call to RematchNetworkAndRequests()
        // which isn't meant to work on uncreated networks.
        if (!nai->mCreated) return NOERROR;

        if (IsNetworkBlocked((INetworkAgentInfo*)nai.Get(), uid)) return NOERROR;

        nai->mNetworkMonitor->SendMessage(NetworkMonitor::CMD_FORCE_REEVALUATION, uid);
    }
    return NOERROR;
}

ECode CConnectivityService::GetProxy(
    /* [out] */ IProxyInfo** proxyInfo)
{
    VALIDATE_NOT_NULL(proxyInfo);
    *proxyInfo = NULL;

    // this information is already available as a world read/writable jvm property
    // so this API change wouldn't have a benifit.  It also breaks the passing
    // of proxy info to all the JVMs.
    // FAIL_RETURN(EnforceAccessPermission())
    synchronized(mProxyLock) {
        AutoPtr<IProxyInfo> ret = mGlobalProxy;
        if ((ret == NULL) && !mDefaultProxyDisabled) ret = mDefaultProxy;
        *proxyInfo = ret;
        REFCOUNT_ADD(*proxyInfo)
    }
    return NOERROR;
}

ECode CConnectivityService::SetGlobalProxy(
    /* [in] */ IProxyInfo* proxyProperties)
{
    FAIL_RETURN(EnforceConnectivityInternalPermission())

    synchronized(mProxyLock) {
        if (proxyProperties == mGlobalProxy) return NOERROR;
        if (proxyProperties != NULL && Object::Equals(proxyProperties, mGlobalProxy)) return NOERROR;
        if (mGlobalProxy != NULL && Object::Equals(mGlobalProxy, proxyProperties)) return NOERROR;

        String host("");
        Int32 port = 0;
        String exclList("");
        String pacFileUrl("");
        mGlobalProxy = NULL;
        Boolean bval;

        if (proxyProperties != NULL) {
            String hozt;
            proxyProperties->GetHost(&hozt);
            AutoPtr<IUriHelper> uriHelper;
            CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
            AutoPtr<IUri> empty, pfUrl;
            uriHelper->GetEMPTY((IUri**)&empty);
            proxyProperties->GetPacFileUrl((IUri**)&pfUrl);
            if (!TextUtils::IsEmpty(hozt) || !Object::Equals(empty, pfUrl)) {
                proxyProperties->IsValid(&bval);
                if (!bval) {
                    if (DBG)
                        Slogger::D(TAG, "Invalid proxy properties, ignoring: %s",
                            TO_CSTR(proxyProperties));
                    return NOERROR;
                }

                mGlobalProxy = NULL;
                CProxyInfo::New(proxyProperties, (IProxyInfo**)&mGlobalProxy);
                mGlobalProxy->GetHost(&host);
                mGlobalProxy->GetPort(&port);
                mGlobalProxy->GetExclusionListAsString(&exclList);
                if (!Object::Equals(empty, pfUrl)) {
                    pacFileUrl = Object::ToString(pfUrl);
                }
            }
        }

        AutoPtr<IContentResolver> res;
        mContext->GetContentResolver((IContentResolver**)&res);
        Int64 token = Binder::ClearCallingIdentity();
        AutoPtr<ISettingsGlobal> settingsGlobal;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
        settingsGlobal->PutString(res, ISettingsGlobal::GLOBAL_HTTP_PROXY_HOST, host, &bval);
        settingsGlobal->PutInt32(res, ISettingsGlobal::GLOBAL_HTTP_PROXY_PORT, port, &bval);
        settingsGlobal->PutString(res, ISettingsGlobal::GLOBAL_HTTP_PROXY_EXCLUSION_LIST, exclList, &bval);
        settingsGlobal->PutString(res, ISettingsGlobal::GLOBAL_HTTP_PROXY_PAC, pacFileUrl, &bval);
        Binder::RestoreCallingIdentity(token);

        if (mGlobalProxy == NULL) {
            proxyProperties = mDefaultProxy;
        }
        SendProxyBroadcast(proxyProperties);
    }
    return NOERROR;
}

void CConnectivityService::LoadGlobalProxy()
{
    AutoPtr<IContentResolver> res;
    mContext->GetContentResolver((IContentResolver**)&res);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);

    String host, exclList, pacFileUrl;
    settingsGlobal->GetString(res, ISettingsGlobal::GLOBAL_HTTP_PROXY_HOST, &host);
    Int32 port;
    settingsGlobal->GetInt32(res, ISettingsGlobal::GLOBAL_HTTP_PROXY_PORT, 0, &port);
    settingsGlobal->GetString(res, ISettingsGlobal::GLOBAL_HTTP_PROXY_EXCLUSION_LIST, &exclList);
    settingsGlobal->GetString(res, ISettingsGlobal::GLOBAL_HTTP_PROXY_PAC, &pacFileUrl);
    if (!TextUtils::IsEmpty(host) || !TextUtils::IsEmpty(pacFileUrl)) {
        AutoPtr<IProxyInfo> proxyProperties;
        if (!TextUtils::IsEmpty(pacFileUrl)) {
            CProxyInfo::New(pacFileUrl, (IProxyInfo**)&proxyProperties);
        }
        else {
            CProxyInfo::New(host, port, exclList, (IProxyInfo**)&proxyProperties);
        }

        Boolean bval;
        proxyProperties->IsValid(&bval);
        if (!bval) {
            if (DBG) Slogger::D(TAG, "Invalid proxy properties, ignoring: %s",
                TO_CSTR(proxyProperties));
            return;
        }

        synchronized(mProxyLock) {
            mGlobalProxy = proxyProperties;
        }
    }
}

ECode CConnectivityService::GetGlobalProxy(
    /* [out] */ IProxyInfo** proxyProperties)
{
    VALIDATE_NOT_NULL(proxyProperties)
    *proxyProperties = NULL;
    // this information is already available as a world read/writable jvm property
    // so this API change wouldn't have a benifit.  It also breaks the passing
    // of proxy info to all the JVMs.
    // FAIL_RETURN(EnforceAccessPermission())
    synchronized(mProxyLock) {
        *proxyProperties = mGlobalProxy;
        REFCOUNT_ADD(*proxyProperties)
    }
    return NOERROR;
}

void CConnectivityService::HandleApplyDefaultProxy(
    /* [in] */ IProxyInfo* pi)
{
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> empty, pfUrl;
    uriHelper->GetEMPTY((IUri**)&empty);

    AutoPtr<IProxyInfo> proxy = pi;
    if (proxy != NULL) {
        String host;
        proxy->GetHost(&host);
        proxy->GetPacFileUrl((IUri**)&pfUrl);
        if (TextUtils::IsEmpty(host) && Object::Equals(empty, pfUrl)) {
            proxy = NULL;
        }
    }

    Boolean bval;
    synchronized(mProxyLock) {
        if (mDefaultProxy != NULL && Object::Equals(mDefaultProxy, proxy)) return;
        if (mDefaultProxy == proxy) return; // catches repeated nulls
        if (proxy != NULL && (proxy->IsValid(&bval), !bval)) {
            if (DBG) Slogger::D(TAG, "Invalid proxy properties, ignoring: %s",
                TO_CSTR(proxy));
            return;
        }

        // This call could be coming from the PacManager, containing the port of the local
        // proxy.  If this new proxy matches the global proxy then copy this proxy to the
        // global (to get the correct local port), and send a broadcast.
        // TODO: Switch PacManager to have its own message to send back rather than
        // reusing EVENT_HAS_CHANGED_PROXY and this call to HandleApplyDefaultProxy.
        pfUrl = NULL;
        proxy->GetPacFileUrl((IUri**)&pfUrl);
        AutoPtr<IUri> gpfUrl;
        if (mGlobalProxy != NULL) mGlobalProxy->GetPacFileUrl((IUri**)&gpfUrl);

        if ((mGlobalProxy != NULL) && (proxy != NULL)
                && !Object::Equals(empty, pfUrl)
                && Object::Equals(pfUrl, gpfUrl)) {
            mGlobalProxy = proxy;
            SendProxyBroadcast(mGlobalProxy);
            return;
        }
        mDefaultProxy = proxy;

        if (mGlobalProxy != NULL) return;
        if (!mDefaultProxyDisabled) {
            SendProxyBroadcast(proxy);
        }
    }
}

void CConnectivityService::HandleDeprecatedGlobalHttpProxy()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    String proxy;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetString(resolver, ISettingsGlobal::HTTP_PROXY, &proxy);
    if (!TextUtils::IsEmpty(proxy)) {
        AutoPtr< ArrayOf<String> > data;
        StringUtils::Split(proxy, String(":"), (ArrayOf<String>**)&data);
        if (data == NULL || data->GetLength() == 0) {
            return;
        }

        String proxyHost = (*data)[0];
        Int32 proxyPort = 8080;
        if (data->GetLength() > 1) {
            ECode ec = StringUtils::Parse((*data)[1], &proxyPort);
            if (FAILED(ec)) return;
        }

        AutoPtr<IProxyInfo> p;
        CProxyInfo::New((*data)[0], proxyPort, String(""), (IProxyInfo**)&p);
        SetGlobalProxy(p);
    }
}

void CConnectivityService::SendProxyBroadcast(
    /* [in] */ IProxyInfo* proxy)
{
    AutoPtr<IProxyInfo> proxyProp = proxy;
    if (proxyProp == NULL) {
        CProxyInfo::New(String(""), 0, String(""), (IProxyInfo**)&proxyProp);
    }
    if (DBG) Slogger::D(TAG, "sending Proxy Broadcast for %p", proxy);
    AutoPtr<IIntent> intent;
    CIntent::New(Elastos::Droid::Net::IProxy::PROXY_CHANGE_ACTION, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING |
        IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(Elastos::Droid::Net::IProxy::EXTRA_PROXY_INFO, IParcelable::Probe(proxyProp));
    Int64 ident = Binder::ClearCallingIdentity();
    mContext->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
    Binder::RestoreCallingIdentity(ident);
}

Int32 CConnectivityService::ConvertFeatureToNetworkType(
    /* [in] */ Int32 networkType,
    /* [in] */ const String& feature)
{
    Int32 usedNetworkType = networkType;

    if (networkType == IConnectivityManager::TYPE_MOBILE) {
        // // "Features" accessible through the connectivity manager
        // static String FEATURE_ENABLE_MMS = "enableMMS";
        // static String FEATURE_ENABLE_SUPL = "enableSUPL";
        // static String FEATURE_ENABLE_DUN = "enableDUN";
        // static String FEATURE_ENABLE_HIPRI = "enableHIPRI";
        // static String FEATURE_ENABLE_DUN_ALWAYS = "enableDUNAlways";
        // static String FEATURE_ENABLE_FOTA = "enableFOTA";
        // static String FEATURE_ENABLE_IMS = "enableIMS";
        // static String FEATURE_ENABLE_CBS = "enableCBS";
        // static String FEATURE_ENABLE_EMERGENCY = "enableEmergency";

        if (TextUtils::Equals(feature, String("enableMMS")/*IPhone::FEATURE_ENABLE_MMS*/)) {
            usedNetworkType = IConnectivityManager::TYPE_MOBILE_MMS;
        }
        else if (TextUtils::Equals(feature, String("enableSUPL")/*IPhone::FEATURE_ENABLE_SUPL*/)) {
            usedNetworkType = IConnectivityManager::TYPE_MOBILE_SUPL;
        }
        else if (TextUtils::Equals(feature, String("enableDUN")/*IPhone::FEATURE_ENABLE_DUN*/) ||
            TextUtils::Equals(feature, String("enableDUNAlways")/*IPhone::FEATURE_ENABLE_DUN_ALWAYS*/)) {
            usedNetworkType = IConnectivityManager::TYPE_MOBILE_DUN;
        }
        else if (TextUtils::Equals(feature, String("enableHIPRI")/*IPhone::FEATURE_ENABLE_HIPRI*/)) {
            usedNetworkType = IConnectivityManager::TYPE_MOBILE_HIPRI;
        }
        else if (TextUtils::Equals(feature, String("enableFOTA")/*IPhone::FEATURE_ENABLE_FOTA*/)) {
            usedNetworkType = IConnectivityManager::TYPE_MOBILE_FOTA;
        }
        else if (TextUtils::Equals(feature, String("enableIMS")/*IPhone::FEATURE_ENABLE_IMS*/)) {
            usedNetworkType = IConnectivityManager::TYPE_MOBILE_IMS;
        }
        else if (TextUtils::Equals(feature, String("enableCBS")/*IPhone::FEATURE_ENABLE_CBS*/)) {
            usedNetworkType = IConnectivityManager::TYPE_MOBILE_CBS;
        }
        else if (TextUtils::Equals(feature, String("enableEmergency")/*IPhone::FEATURE_ENABLE_EMERGENCY*/)) {
            usedNetworkType = IConnectivityManager::TYPE_MOBILE_EMERGENCY;
        }
        else {
            Slogger::E(TAG, "Can't match any mobile netTracker!");
        }
    }
    else if (networkType == IConnectivityManager::TYPE_WIFI) {
        if (TextUtils::Equals(feature, String("p2p"))) {
            usedNetworkType = IConnectivityManager::TYPE_WIFI_P2P;
        }
        else {
            Slogger::E(TAG, "Can't match any wifi netTracker!");
        }
    }
    else {
        Slogger::E(TAG, "Unexpected network type");
    }
    return usedNetworkType;
}

ECode CConnectivityService::PrepareVpn(
    /* [in] */ const String& oldPackage,
    /* [in] */ const String& newPackage,
    /* [out] */ Boolean* status)
{
    VALIDATE_NOT_NULL(status);
    *status = FALSE;

    FAIL_RETURN(ThrowIfLockdownEnabled())
    Int32 user = UserHandle::GetUserId(Binder::GetCallingUid());
    synchronized(mVpnsLock) {
        HashMap<Int32, AutoPtr<Vpn> >::Iterator it = mVpns.Find(user);
        if (it != mVpns.End()) {
            AutoPtr<Vpn> vpn = it->mSecond;
            return vpn->Prepare(oldPackage, newPackage, status);
        }
    }
    return NOERROR;
}

ECode CConnectivityService::SetVpnPackageAuthorization(
    /* [in] */ Boolean authorized)
{
    Int32 user = UserHandle::GetUserId(Binder::GetCallingUid());
    synchronized(mVpnsLock) {
        HashMap<Int32, AutoPtr<Vpn> >::Iterator it = mVpns.Find(user);
        if (it != mVpns.End()) {
            AutoPtr<Vpn> vpn = it->mSecond;
            return vpn->SetPackageAuthorization(authorized);
        }
    }
    return NOERROR;
}

ECode CConnectivityService::EstablishVpn(
    /* [in] */ IVpnConfig* config,
    /* [out] */ IParcelFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);
    *fd = NULL;

    Int32 user = UserHandle::GetUserId(Binder::GetCallingUid());
    synchronized(mVpnsLock) {
        HashMap<Int32, AutoPtr<Vpn> >::Iterator it = mVpns.Find(user);
        if (it != mVpns.End()) {
            AutoPtr<Vpn> vpn = it->mSecond;
            return vpn->Establish(config, fd);
        }
    }
    return NOERROR;
}

ECode CConnectivityService::StartLegacyVpn(
    /* [in] */ IVpnProfile* profile)
{
    FAIL_RETURN(ThrowIfLockdownEnabled());
    AutoPtr<ILinkProperties> egress;
    GetActiveLinkProperties((ILinkProperties**)&egress);
    if (egress == NULL) {
        //throw new IllegalStateException("Missing active network connection");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int32 user = UserHandle::GetUserId(Binder::GetCallingUid());
    synchronized(mVpnsLock) {
        HashMap<Int32, AutoPtr<Vpn> >::Iterator it = mVpns.Find(user);
        if (it != mVpns.End()) {
            AutoPtr<Vpn> vpn = it->mSecond;
            return vpn->StartLegacyVpn(profile, mKeyStore, egress);
        }
    }
    return NOERROR;
}

ECode CConnectivityService::GetLegacyVpnInfo(
    /* [out] */ ILegacyVpnInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    FAIL_RETURN(ThrowIfLockdownEnabled());
    Int32 user = UserHandle::GetUserId(Binder::GetCallingUid());
    synchronized(mVpnsLock) {
        HashMap<Int32, AutoPtr<Vpn> >::Iterator it = mVpns.Find(user);
        if (it != mVpns.End()) {
            AutoPtr<Vpn> vpn = it->mSecond;
            return vpn->GetLegacyVpnInfo(info);
        }
    }
    return NOERROR;
}

ECode CConnectivityService::GetVpnConfig(
    /* [out] */ IVpnConfig** config)
{
    VALIDATE_NOT_NULL(config)
    *config = NULL;

    Int32 user = UserHandle::GetUserId(Binder::GetCallingUid());
    synchronized(mVpnsLock) {
        HashMap<Int32, AutoPtr<Vpn> >::Iterator it = mVpns.Find(user);
        if (it != mVpns.End()) {
            AutoPtr<Vpn> vpn = it->mSecond;
            AutoPtr<IVpnConfig> vc = vpn->GetVpnConfig();
            *config = vc;
            REFCOUNT_ADD(*config)
        }
    }
    return NOERROR;
}

ECode CConnectivityService::UpdateLockdownVpn(
    /* [out] */ Boolean* status)
{
    VALIDATE_NOT_NULL(status);
    *status = FALSE;

    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        Slogger::W(TAG, "Lockdown VPN only available to AID_SYSTEM");
        return NOERROR;
    }

    // Tear down existing lockdown if profile was removed
    mLockdownEnabled = LockdownVpnTracker::IsEnabled();
    if (mLockdownEnabled) {
        Boolean bval;
        mKeyStore->IsUnlocked(&bval);
        if (!bval) {
            Slogger::W(TAG, "KeyStore locked; unable to create LockdownTracker");
            return NOERROR;
        }

        AutoPtr<ArrayOf<Byte> > value, pn;
        mKeyStore->Get(ICredentials::LOCKDOWN_VPN, (ArrayOf<Byte>**)&pn);
        String profileName(*pn);
        StringBuilder sb(ICredentials::VPN);
        sb += profileName;
        mKeyStore->Get(sb.ToString(), (ArrayOf<Byte>**)&value);
        AutoPtr<IVpnProfileHelper> helper;
        CVpnProfileHelper::AcquireSingleton((IVpnProfileHelper**)&helper);
        AutoPtr<IVpnProfile> profile;
        helper->Decode(profileName, value, (IVpnProfile**)&profile);

        Int32 user = UserHandle::GetUserId(Binder::GetCallingUid());
        synchronized(mVpnsLock) {
            AutoPtr<Vpn> vpn;
            HashMap<Int32, AutoPtr<Vpn> >::Iterator it = mVpns.Find(user);
            if (it != mVpns.End()) {
                vpn = it->mSecond;
            }

            AutoPtr<LockdownVpnTracker> lvt = new LockdownVpnTracker();
            lvt->constructor(mContext, mNetd, this, vpn, profile);
            SetLockdownTracker(lvt);
        }
    }
    else {
        SetLockdownTracker(NULL);
    }

    *status = TRUE;
    return NOERROR;
}

void CConnectivityService::SetLockdownTracker(
    /* [in] */ LockdownVpnTracker* tracker)
{
    // Shutdown any existing tracker
    AutoPtr<LockdownVpnTracker> existing = mLockdownTracker;
    mLockdownTracker = NULL;
    if (existing != NULL) {
        existing->Shutdown();
    }

    //try {
    if (tracker != NULL) {
        mNetd->SetFirewallEnabled(TRUE);
        mNetd->SetFirewallInterfaceRule(String("lo"), TRUE);
        mLockdownTracker = tracker;
        mLockdownTracker->Init();
    }
    else {
        mNetd->SetFirewallEnabled(FALSE);
    }
    //} catch (RemoteException e) {
        // ignored; NMS lives inside system_server
    //}
}

ECode CConnectivityService::ThrowIfLockdownEnabled()
{
    if (mLockdownEnabled) {
        //throw new IllegalStateException("Unavailable in lockdown mode");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

//===========================================================
//+++++++++++++++++
//===========================================================

ECode CConnectivityService::SupplyMessenger(
    /* [in] */ Int32 networkType,
    /* [in] */ IMessenger* messenger)
{
    FAIL_RETURN(EnforceConnectivityInternalPermission())

    Boolean bval;
    IsNetworkTypeValid(networkType, &bval);
    if (bval && (*mNetTrackers)[networkType] != NULL) {
        (*mNetTrackers)[networkType]->SupplyMessenger(messenger);
    }
    return NOERROR;
}

ECode CConnectivityService::FindConnectionTypeForIface(
    /* [in] */ const String& iface,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IConnectivityManager::TYPE_NONE;

    FAIL_RETURN(EnforceConnectivityInternalPermission())

    if (TextUtils::IsEmpty(iface)) return NOERROR;

    ISynchronize* syncObj = (ISynchronize*)mNetworkForNetId->Probe(EIID_ISynchronize);
    synchronized(syncObj) {
        Int32 size;
        mNetworkForNetId->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mNetworkForNetId->ValueAt(i, (IInterface**)&obj);
            NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(obj);
            ILinkProperties* lp = nai->mLinkProperties;
            String in;
            lp->GetInterfaceName(&in);
            if (lp != NULL && iface.Equals(in) && nai->mNetworkInfo != NULL) {
                return nai->mNetworkInfo->GetType(result);
            }
        }
    }
    return NOERROR;
}

void CConnectivityService::SetEnableFailFastMobileData(
    /* [in] */ Int32 enabled)
{
    Int32 tag;

    if (enabled == IDctConstants::ENABLED) {
        mEnableFailFastMobileDataTag->IncrementAndGet(&tag);
    }
    else {
        mEnableFailFastMobileDataTag->Get(&tag);
    }
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_ENABLE_FAIL_FAST_MOBILE_DATA, tag, enabled, (IMessage**)&msg);
    Boolean bval;
    mHandler->SendMessage(msg, &bval);
}

ECode CConnectivityService::CheckMobileProvisioning(
    /* [in] */ Int32 suggestedTimeOutMs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // TODO: Remove?  Any reason to trigger a provisioning check?
    *result = -1;
    return NOERROR;
}

void CConnectivityService::SetProvNotificationVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 networkType,
    /* [in] */ const String& action)
{
    if (DBG) {
        Slogger::D(TAG, "SetProvNotificationVisible: E visible=%d networkType=%d action=%s",
            visible, networkType, action.string());
    }

    AutoPtr<IIntent> intent;
    CIntent::New(action, (IIntent**)&intent);
    AutoPtr<IPendingIntentHelper> piHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);

    AutoPtr<IPendingIntent> pendingIntent;
    piHelper->GetBroadcast(mContext, 0, intent, 0, (IPendingIntent**)&pendingIntent);
    // Concatenate the range of types onto the range of NetIDs.
    Int32 id = MAX_NET_ID + 1 + (networkType - IConnectivityManager::TYPE_NONE);
    SetProvNotificationVisibleIntent(visible, id, networkType, String(NULL), pendingIntent);
}

void CConnectivityService::SetProvNotificationVisibleIntent(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 id,
    /* [in] */ Int32 networkType,
    /* [in] */ const String& extraInfo,
    /* [in] */ IPendingIntent* intent)
{
    if (DBG) {
        Slogger::D(TAG, "SetProvNotificationVisibleIntent: E visible=%d networkType=%d extraInfo=%s",
            visible, networkType, extraInfo.string());
    }

    AutoPtr<IResourcesHelper> resHelper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&resHelper);
    AutoPtr<IResources> r;
    resHelper->GetSystem((IResources**)&r);
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::NOTIFICATION_SERVICE, (IInterface**)&service);
    AutoPtr<INotificationManager> notificationManager = INotificationManager::Probe(service);

    if (visible) {
        String title, details;
        Int32 icon;
        AutoPtr<INotification> notification;
        CNotification::New((INotification**)&notification);
        AutoPtr<ArrayOf<IInterface*> > signInParams = ArrayOf<IInterface*>::Alloc(1);
        signInParams->Set(0, CoreUtils::Convert(0).Get());
        AutoPtr<ArrayOf<IInterface*> > detailedParams = ArrayOf<IInterface*>::Alloc(1);
        detailedParams->Set(0, CoreUtils::Convert(extraInfo).Get());
        switch (networkType) {
            case IConnectivityManager::TYPE_WIFI: {

                    r->GetString(R::string::wifi_available_sign_in, signInParams, &title);
                    r->GetString(R::string::network_available_sign_in_detailed, detailedParams, &details);
                    icon = R::drawable::stat_notify_wifi_in_range;
                }
                break;
            case IConnectivityManager::TYPE_MOBILE:
            case IConnectivityManager::TYPE_MOBILE_HIPRI:
                r->GetString(R::string::network_available_sign_in, signInParams, &title);
                // TODO: Change this to pull from NetworkInfo once a printable
                // name has been added to it
                mTelephonyManager->GetNetworkOperatorName(&details);
                icon = R::drawable::stat_notify_rssi_in_range;
                break;
            default:
                r->GetString(R::string::network_available_sign_in, signInParams, &title);
                r->GetString(R::string::network_available_sign_in_detailed, detailedParams, &details);
                icon = R::drawable::stat_notify_rssi_in_range;
                break;
        }

        AutoPtr<ICharSequence> titleSeq = CoreUtils::Convert(title);
        AutoPtr<ICharSequence> detailsSeq = CoreUtils::Convert(details);
        notification->SetWhen(0);
        notification->SetIcon(icon);
        notification->SetFlags(INotification::FLAG_AUTO_CANCEL);
        notification->SetTickerText(titleSeq);

        AutoPtr<IResources> ctxRes;
        mContext->GetResources((IResources**)&ctxRes);
        Int32 realColor;
        ctxRes->GetColor(R::color::system_notification_accent_color, &realColor);
        notification->SetColor(realColor);
        AutoPtr<IPendingIntent> prevIntent;
        notification->GetContentIntent((IPendingIntent**)&prevIntent);
        notification->SetLatestEventInfo(mContext, titleSeq, detailsSeq, prevIntent);
        notification->SetContentIntent(intent);

        ECode ec = notificationManager->Notify(NOTIFICATION_ID, id, notification);
        if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
            Slogger::E(TAG, "setNotificaitionVisible: visible notificationManager npe=");
        }
    }
    else {
        ECode ec = notificationManager->Cancel(NOTIFICATION_ID, id);
        if (ec == (ECode)E_NULL_POINTER_EXCEPTION) {
            Slogger::E(TAG, "setNotificaitionVisible: cancel notificationManager npe=");
        }
    }
    mIsNotificationVisible = visible;
}

ECode CConnectivityService::GetProvisioningUrlBaseFromFile(
    /* [in] */ Int32 type,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    AutoPtr<IFileReader> fileReader;
    AutoPtr<IXmlPullParser> parser;
    AutoPtr<IConfiguration> config;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetConfiguration((IConfiguration**)&config);
    String tagType;

    switch (type) {
        case PROVISIONING:
            tagType = TAG_PROVISIONING_URL;
            break;
        case REDIRECTED_PROVISIONING:
            tagType = TAG_REDIRECTED_URL;
            break;
        default:
            Slogger::E(TAG, "GetProvisioningUrlBaseFromFile: Unexpected parameter %d", type);
            return E_RUNTIME_EXCEPTION;
    }

    ECode ec = NOERROR;
    String element, mcc, mnc, nullStr;
    Int32 ival, eventType;
    Int32 cfgMcc, cfgMnc;
    config->GetMcc(&cfgMcc);
    config->GetMnc(&cfgMnc);

    ec = CFileReader::New(mProvisioningUrlFile, (IFileReader**)&fileReader);
    FAIL_GOTO(ec, _EXIT_)

    Xml::NewPullParser((IXmlPullParser**)&parser);
    ec = parser->SetInput(IReader::Probe(fileReader));
    FAIL_GOTO(ec, _EXIT_)
    ec = XmlUtils::BeginDocument(parser, TAG_PROVISIONING_URLS);
    FAIL_GOTO(ec, _EXIT_)

    while (TRUE) {
        ec = XmlUtils::NextElement(parser);
        FAIL_GOTO(ec, _EXIT_)

        ec = parser->GetName(&element);
        FAIL_GOTO(ec, _EXIT_)
        if (element == NULL) break;

        if (element.Equals(tagType)) {
            parser->GetAttributeValue(nullStr, ATTR_MCC, &mcc);
            if (mcc != NULL) {
                ec = StringUtils::Parse(mcc, &ival);
                if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
                    Slogger::E(TAG, "NumberFormatException in GetProvisioningUrlBaseFromFile: mcc %s", mcc.string());
                }
                if (ival == cfgMcc) {
                    parser->GetAttributeValue(nullStr, ATTR_MNC, &mnc);
                    if (mcc != NULL) {
                        ec = StringUtils::Parse(mnc, &ival);
                        if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
                            Slogger::E(TAG, "NumberFormatException in GetProvisioningUrlBaseFromFile: mnc %s", mnc.string());
                        }
                        if (ival == cfgMnc) {
                            ec = parser->Next(&ival);
                            FAIL_GOTO(ec, _EXIT_)
                            parser->GetEventType(&eventType);
                            if (eventType == IXmlPullParser::TEXT) {
                                ec = parser->GetText(result);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

_EXIT_:
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        Slogger::E(TAG, "Carrier Provisioning Urls file not found");
    }
    else if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        Slogger::E(TAG, "Xml parser exception reading Carrier Provisioning Urls file: ");
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "I/O exception reading Carrier Provisioning Urls file: ");
    }

    if (fileReader != NULL) {
        ICloseable::Probe(fileReader)->Close();
    }

    return ec;
}

ECode CConnectivityService::GetMobileRedirectedProvisioningUrl(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    FAIL_RETURN(EnforceConnectivityInternalPermission())
    String url;
    FAIL_RETURN(GetProvisioningUrlBaseFromFile(REDIRECTED_PROVISIONING, &url))
    if (TextUtils::IsEmpty(url)) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        res->GetString(R::string::mobile_redirected_provisioning_url, &url);
    }
    *result = url;
    return NOERROR;
}

ECode CConnectivityService::GetMobileProvisioningUrl(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    FAIL_RETURN(EnforceConnectivityInternalPermission())
    String url;
    FAIL_RETURN(GetProvisioningUrlBaseFromFile(PROVISIONING, &url))

    if (TextUtils::IsEmpty(url)) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        res->GetString(R::string::mobile_provisioning_url, &url);
        Slogger::D(TAG, "GetMobileProvisioningUrl: mobile_provisioining_url from resource =%s", url.string());
    }
    else {
        Slogger::D(TAG, "GetMobileProvisioningUrl: mobile_provisioning_url from File =%s", url.string());
    }

    // populate the iccid, imei and phone number in the provisioning url.
    if (!TextUtils::IsEmpty(url)) {
        String phoneNumber;
        mTelephonyManager->GetLine1Number(&phoneNumber);
        if (TextUtils::IsEmpty(phoneNumber)) {
            phoneNumber = String("0000000000");
        }

        String str, ssn, id;
        mTelephonyManager->GetSimSerialNumber(&ssn);
        mTelephonyManager->GetDeviceId(&id);
        str.AppendFormat(url.string(), ssn.string(),
            id.string() /* IMEI */,
            phoneNumber.string() /* Phone numer */);
        url = str;
    }

    *result = url;
    return NOERROR;
}

ECode CConnectivityService::SetProvisioningNotificationVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 networkType,
    /* [in] */ const String& action)
{
    FAIL_RETURN(EnforceConnectivityInternalPermission())
    Int64 ident = Binder::ClearCallingIdentity();
    SetProvNotificationVisible(visible, networkType, action);
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CConnectivityService::SetAirplaneMode(
    /* [in] */ Boolean enable)
{
    FAIL_RETURN(EnforceConnectivityInternalPermission())
    Int64 ident = Binder::ClearCallingIdentity();

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    Boolean bval;
    settingsGlobal->PutInt32(cr, ISettingsGlobal::AIRPLANE_MODE_ON, enable ? 1 : 0, &bval);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_AIRPLANE_MODE_CHANGED, (IIntent**)&intent);
    intent->PutExtra(String("state"), enable);
    mContext->SendBroadcast(intent);

    Binder::RestoreCallingIdentity(ident);

    return NOERROR;
}

void CConnectivityService::OnUserStart(
    /* [in] */ Int32 userId)
{
    synchronized(mVpnsLock) {
        AutoPtr<Vpn> userVpn;
        HashMap<Int32, AutoPtr<Vpn> >::Iterator it = mVpns.Find(userId);
        if (it != mVpns.End()) {
            Slogger::E(TAG, "Starting user already has a VPN");
            return;
        }

        AutoPtr<ILooper> looper;
        mHandler->GetLooper((ILooper**)&looper);
        userVpn = new Vpn(looper, mContext, mNetd, this, userId);
        mVpns[userId] = userVpn;
    }
}

void CConnectivityService::OnUserStop(
    /* [in] */ Int32 userId)
{
    synchronized(mVpnsLock) {
        AutoPtr<Vpn> userVpn;
        HashMap<Int32, AutoPtr<Vpn> >::Iterator it = mVpns.Find(userId);
        if (it == mVpns.End()) {
            Slogger::E(TAG, "Stopping user has no VPN");
            return;
        }

        mVpns.Erase(it);
    }
}

ECode CConnectivityService::GetLinkQualityInfo(
    /* [in] */ Int32 networkType,
    /* [out] */ ILinkQualityInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = NULL;

    FAIL_RETURN(EnforceAccessPermission())
    Boolean bval;
    IsNetworkTypeValid(networkType, &bval);
    if (bval && (*mNetTrackers)[networkType] != NULL) {
        return (*mNetTrackers)[networkType]->GetLinkQualityInfo(info);
    }
    return NOERROR;
}

ECode CConnectivityService::GetActiveLinkQualityInfo(
    /* [out] */ ILinkQualityInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = NULL;

    FAIL_RETURN(EnforceAccessPermission())
    Boolean bval;
    IsNetworkTypeValid(mActiveDefaultNetwork, &bval);
    if (bval && (*mNetTrackers)[mActiveDefaultNetwork] != NULL) {
        return (*mNetTrackers)[mActiveDefaultNetwork]->GetLinkQualityInfo(info);
    }
    return NOERROR;
}

ECode CConnectivityService::GetAllLinkQualityInfo(
    /* [out, callee] */ ArrayOf<ILinkQualityInfo*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    FAIL_RETURN(EnforceAccessPermission())
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);

    for (Int32 i = 0; i < mNetTrackers->GetLength(); ++i) {
        INetworkStateTracker* tracker = (*mNetTrackers)[i];
        if (tracker != NULL) {
            AutoPtr<ILinkQualityInfo> li;
            tracker->GetLinkQualityInfo((ILinkQualityInfo**)&li);
            if (li != NULL) {
                list->Add(li);
            }
        }
    }

    return Arrays::FromList(IList::Probe(list), result);
}

/* Infrastructure for network sampling */

void CConnectivityService::HandleNetworkSamplingTimeout()
{
    if (SAMPLE_DBG) Slogger::D(TAG, "Sampling interval elapsed, updating statistics ..");

    // initialize list of interfaces ..
    AutoPtr<IMap> mapIfaceToSample;
    CHashMap::New((IMap**)&mapIfaceToSample);//<String, SamplingDataTracker.SamplingSnapshot>();
    for (Int32 i = 0; i < mNetTrackers->GetLength(); ++i) {
        INetworkStateTracker* tracker = (*mNetTrackers)[i];
        if (tracker != NULL) {
            String ifaceName;
            tracker->GetNetworkInterfaceName(&ifaceName);
            if (ifaceName != NULL) {
                AutoPtr<ICharSequence> csq = CoreUtils::Convert(ifaceName);
                mapIfaceToSample->Put(csq.Get(), NULL);
            }
        }
    }

    // Read samples for all interfaces
    AutoPtr<ISamplingDataTrackerHelper> helper;
    CSamplingDataTrackerHelper::AcquireSingleton((ISamplingDataTrackerHelper**)&helper);
    helper->GetSamplingSnapshots(mapIfaceToSample);

    // process samples for all networks
    for (Int32 i = 0; i < mNetTrackers->GetLength(); ++i) {
        INetworkStateTracker* tracker = (*mNetTrackers)[i];
        if (tracker != NULL) {
            String ifaceName;
            tracker->GetNetworkInterfaceName(&ifaceName);
            AutoPtr<ICharSequence> csq = CoreUtils::Convert(ifaceName);
            AutoPtr<IInterface> obj;
            mapIfaceToSample->Get(csq.Get(), (IInterface**)&obj);
            if (obj != NULL) {
                ISamplingDataTrackerSamplingSnapshot* ss = ISamplingDataTrackerSamplingSnapshot::Probe(obj);
                if (ss != NULL) {
                    // end the previous sampling cycle
                    tracker->StopSampling(ss);
                    // start a new sampling cycle ..
                    tracker->StartSampling(ss);
                }
            }

        }
    }

    if (SAMPLE_DBG) Slogger::D(TAG, "Done.");

    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 samplingIntervalInSeconds;
    settingsGlobal->GetInt32(cr, ISettingsGlobal::CONNECTIVITY_SAMPLING_INTERVAL_IN_SECONDS,
            DEFAULT_SAMPLING_INTERVAL_IN_SECONDS, &samplingIntervalInSeconds);

    if (SAMPLE_DBG) {
        Slogger::D(TAG, "Setting timer for %d seconds", samplingIntervalInSeconds);
    }

    SetAlarm(samplingIntervalInSeconds * 1000, mSampleIntervalElapsedIntent);
}

void CConnectivityService::SetAlarm(
    /* [in] */ Int32 timeoutInMilliseconds,
    /* [in] */ IPendingIntent* intent)
{
    AutoPtr<IResourcesHelper> resHelper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&resHelper);
    AutoPtr<IResources> res;
    resHelper->GetSystem((IResources**)&res);

    Int64 wakeupTime = SystemClock::GetElapsedRealtime() + timeoutInMilliseconds;
    Int32 alarmType;
    Boolean bval;
    res->GetBoolean(R::bool_::config_networkSamplingWakesDevice, &bval);
    if (bval) {
        alarmType = IAlarmManager::ELAPSED_REALTIME_WAKEUP;
    } else {
        alarmType = IAlarmManager::ELAPSED_REALTIME;
    }
    mAlarmManager->Set(alarmType, wakeupTime, intent);
}

ECode CConnectivityService::RequestNetwork(
    /* [in] */ INetworkCapabilities* inNetworkCapabilities,
    /* [in] */ IMessenger* messenger,
    /* [in] */ Int32 timeoutMs,
    /* [in] */ IBinder* binder,
    /* [in] */ Int32 legacyType,
    /* [out] */ INetworkRequest** request)
{
    VALIDATE_NOT_NULL(request)
    *request = NULL;

    Boolean bval;
    inNetworkCapabilities->HasCapability(INetworkCapabilities::NET_CAPABILITY_NOT_RESTRICTED, &bval);
    if (bval == FALSE) {
        FAIL_RETURN(EnforceConnectivityInternalPermission())
    }
    else {
        FAIL_RETURN(EnforceChangePermission())
    }

    AutoPtr<INetworkCapabilities> networkCapabilities;
    CNetworkCapabilities::New(inNetworkCapabilities, (INetworkCapabilities**)&networkCapabilities);

    // if UID is restricted, don't allow them to bring up metered APNs
    networkCapabilities->HasCapability(INetworkCapabilities::NET_CAPABILITY_NOT_METERED, &bval);
    if (bval == FALSE) {
        Int32 uidRules = INetworkPolicyManager::RULE_ALLOW_ALL;
        Int32 uid = Binder::GetCallingUid();
        synchronized(mRulesLock) {
            HashMap<Int32, Int32>::Iterator it = mUidRules.Find(uid);
            if (it != mUidRules.End()) {
                uidRules = it->mSecond;
            }
        }
        if ((uidRules & INetworkPolicyManager::RULE_REJECT_METERED) != 0) {
            // we could silently fail or we can filter the available nets to only give
            // them those they have access to.  Chose the more useful
            networkCapabilities->AddCapability(INetworkCapabilities::NET_CAPABILITY_NOT_METERED);
        }
    }

    if (timeoutMs < 0 || timeoutMs > IConnectivityManager::MAX_NETWORK_REQUEST_TIMEOUT_MS) {
        Slogger::E(TAG, "Bad timeout specified");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<INetworkRequest> networkRequest;
    CNetworkRequest::New(networkCapabilities, legacyType,
        NextNetworkRequestId(), (INetworkRequest**)&networkRequest);
    if (DBG) Slogger::D(TAG, "RequestNetwork for %s", TO_CSTR(networkRequest));
    AutoPtr<NetworkRequestInfo> nri = new NetworkRequestInfo(messenger, networkRequest, binder,
        NetworkRequestInfo::REQUEST, this);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_REGISTER_NETWORK_REQUEST, TO_IINTERFACE(nri), (IMessage**)&msg);
    mHandler->SendMessage(msg, &bval);
    if (timeoutMs > 0) {
        msg = NULL;
        mHandler->ObtainMessage(EVENT_TIMEOUT_NETWORK_REQUEST, TO_IINTERFACE(nri), (IMessage**)&msg);
        mHandler->SendMessageDelayed(msg, timeoutMs, &bval);
    }

    *request = networkRequest;
    REFCOUNT_ADD(*request)
    return NOERROR;
}

ECode CConnectivityService::PendingRequestForNetwork(
    /* [in] */ INetworkCapabilities* networkCapabilities,
    /* [in] */ IPendingIntent* operation,
    /* [out] */ INetworkRequest** request)
{
    VALIDATE_NOT_NULL(request)
    // TODO
    *request = NULL;
    return NOERROR;
}

ECode CConnectivityService::ListenForNetwork(
    /* [in] */ INetworkCapabilities* inNetworkCapabilities,
    /* [in] */ IMessenger* messenger,
    /* [in] */ IBinder* binder,
    /* [out] */ INetworkRequest** request)
{
    VALIDATE_NOT_NULL(request)
    *request = NULL;
    FAIL_RETURN(EnforceAccessPermission())

    AutoPtr<INetworkCapabilities> networkCapabilities;
    CNetworkCapabilities::New(inNetworkCapabilities, (INetworkCapabilities**)&networkCapabilities);

    AutoPtr<INetworkRequest> networkRequest;
    CNetworkRequest::New(networkCapabilities, IConnectivityManager::TYPE_NONE,
        NextNetworkRequestId(), (INetworkRequest**)&networkRequest);
    if (DBG) Slogger::D(TAG, "ListenForNetwork for %s", TO_CSTR(networkRequest));
    AutoPtr<NetworkRequestInfo> nri = new NetworkRequestInfo(messenger, networkRequest, binder,
        NetworkRequestInfo::LISTEN, this);

    Boolean bval;
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_REGISTER_NETWORK_LISTENER, TO_IINTERFACE(nri), (IMessage**)&msg);
    mHandler->SendMessage(msg, &bval);

    *request = networkRequest;
    REFCOUNT_ADD(*request)
    return NOERROR;
}

ECode CConnectivityService::PendingListenForNetwork(
    /* [in] */ INetworkCapabilities* networkCapabilities,
    /* [in] */ IPendingIntent* operation)
{
    return NOERROR;
}

ECode CConnectivityService::ReleaseNetworkRequest(
    /* [in] */ INetworkRequest* networkRequest)
{
    Boolean bval;
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_RELEASE_NETWORK_REQUEST, Binder::GetCallingUid(),
        0, networkRequest, (IMessage**)&msg);
    mHandler->SendMessage(msg, &bval);
    return NOERROR;
}

ECode CConnectivityService::RegisterNetworkFactory(
    /* [in] */ IMessenger* messenger,
    /* [in] */ const String& name)
{
    FAIL_RETURN(EnforceConnectivityInternalPermission())
    Boolean bval;
    AutoPtr<IAsyncChannel> channel;
    CAsyncChannel::New((IAsyncChannel**)&channel);
    AutoPtr<NetworkFactoryInfo> nfi = new NetworkFactoryInfo(name, messenger, channel);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_REGISTER_NETWORK_FACTORY, TO_IINTERFACE(nfi), (IMessage**)&msg);
    mHandler->SendMessage(msg, &bval);
    return NOERROR;
}

void CConnectivityService::HandleRegisterNetworkFactory(
    /* [in] */ NetworkFactoryInfo* nfi)
{
    String name = nfi->mName;
    if (DBG) Slogger::D(TAG, "Got NetworkFactory Messenger for %s", name.string());
    mNetworkFactoryInfos->Put(nfi->mMessenger.Get(), TO_IINTERFACE(nfi));
    nfi->mAsyncChannel->Connect(mContext, mTrackerHandler, nfi->mMessenger);
}

ECode CConnectivityService::UnregisterNetworkFactory(
    /* [in] */ IMessenger* messenger)
{
    FAIL_RETURN(EnforceConnectivityInternalPermission())
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_UNREGISTER_NETWORK_FACTORY, messenger, (IMessage**)&msg);
    Boolean bval;
    mHandler->SendMessage(msg, &bval);
    return NOERROR;
}

void CConnectivityService::HandleUnregisterNetworkFactory(
    /* [in] */ IMessenger* messenger)
{

    AutoPtr<IInterface> obj;
    mNetworkFactoryInfos->Remove(messenger, (IInterface**)&obj);
    AutoPtr<NetworkFactoryInfo> nfi = (NetworkFactoryInfo*)IObject::Probe(obj);
    if (nfi == NULL) {
        Slogger::E(TAG, "Failed to find Messenger in UnregisterNetworkFactory");
        return;
    }

    if (DBG) {
        Slogger::D(TAG, "UnregisterNetworkFactory for %s", nfi->mName.string());
    }
}

Boolean CConnectivityService::IsDefaultNetwork(
    /* [in] */ INetworkAgentInfo* nai)
{
    Int32 requestId;
    mDefaultRequest->GetRequestId(&requestId);
    AutoPtr<IInterface> obj;
    mNetworkForRequestId->Get(requestId, (IInterface**)&obj);

    return Object::Equals(obj, TO_IINTERFACE(nai));
}

ECode CConnectivityService::RegisterNetworkAgent(
    /* [in] */ IMessenger* messenger,
    /* [in] */ INetworkInfo* networkInfo,
    /* [in] */ ILinkProperties* linkProperties,
    /* [in] */ INetworkCapabilities* networkCapabilities,
    /* [in] */ Int32 currentScore,
    /* [in] */ INetworkMisc* networkMisc)
{
    FAIL_RETURN(EnforceConnectivityInternalPermission())

    AutoPtr<IAsyncChannel> channel;
    AutoPtr<INetworkInfo> ni;
    AutoPtr<ILinkProperties> lp;
    AutoPtr<INetworkCapabilities> nc;
    AutoPtr<INetworkMisc> nm;
    CAsyncChannel::New((IAsyncChannel**)&channel);
    CNetworkInfo::New(networkInfo, (INetworkInfo**)&ni);
    CLinkProperties::New(linkProperties, (ILinkProperties**)&lp);
    CNetworkCapabilities::New(networkCapabilities, (INetworkCapabilities**)&nc);
    CNetworkMisc::New(networkMisc, (INetworkMisc**)&nm);
    AutoPtr<NetworkAgentInfo> nai = new NetworkAgentInfo(messenger, channel,
        ni, lp, nc, currentScore, mContext, mTrackerHandler, nm);

    ISynchronize* syncObj = this;
    synchronized(syncObj) {
        nai->mNetworkMonitor->mSystemReady = mSystemReady;
    }
    if (DBG) Slogger::D(TAG, "RegisterNetworkAgent %s", TO_CSTR(nai));

    Boolean bval;
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(EVENT_REGISTER_NETWORK_AGENT, TO_IINTERFACE(nai), (IMessage**)&msg);
    mHandler->SendMessage(msg, &bval);
    return NOERROR;
}

void CConnectivityService::HandleRegisterNetworkAgent(
    /* [in] */ INetworkAgentInfo* na)
{
    if (VDBG) Slogger::D(TAG, "Got NetworkAgent Messenger");
    NetworkAgentInfo* nai = (NetworkAgentInfo*)na;
    mNetworkAgentInfos->Put(nai->mMessenger.Get(), na);
    AssignNextNetId(na);

    nai->mAsyncChannel->Connect(mContext, mTrackerHandler, nai->mMessenger);
    AutoPtr<INetworkInfo> networkInfo = nai->mNetworkInfo;
    nai->mNetworkInfo = NULL;
    UpdateNetworkInfo(na, networkInfo);
}

void CConnectivityService::UpdateLinkProperties(
    /* [in] */ INetworkAgentInfo* nai,
    /* [in] */ ILinkProperties* oldLp)
{
    NetworkAgentInfo* networkAgent = (NetworkAgentInfo*)nai;
    AutoPtr<ILinkProperties> newLp = networkAgent->mLinkProperties;
    Int32 netId;
    networkAgent->mNetwork->GetNetId(&netId);

    // The NetworkAgentInfo does not know whether clatd is running on its network or not. Before
    // we do anything else, make sure its LinkProperties are accurate.
    mClat->FixupLinkProperties(nai, oldLp);

    UpdateInterfaces(newLp, oldLp, netId);
    UpdateMtu(newLp, oldLp);
    // TODO - figure out what to do for clat
//        for (LinkProperties lp : newLp->GetStackedLinks()) {
//            UpdateMtu(lp, NULL);
//        }
    UpdateTcpBufferSizes(networkAgent);
    Boolean flushDns = UpdateRoutes(newLp, oldLp, netId);
    UpdateDnses(newLp, oldLp, netId, flushDns);
    UpdateClat(newLp, oldLp, networkAgent);

    if (IsDefaultNetwork(nai)) {
        AutoPtr<IProxyInfo> pi;
        newLp->GetHttpProxy((IProxyInfo**)&pi);
        HandleApplyDefaultProxy(pi);
    }
    // TODO - move this check to cover the whole function
    if (!Object::Equals(newLp, oldLp)) {
        NotifyNetworkCallbacks(networkAgent, IConnectivityManager::CALLBACK_IP_CHANGED);
    }
}

void CConnectivityService::UpdateClat(
    /* [in] */ ILinkProperties* newLp,
    /* [in] */ ILinkProperties* oldLp,
    /* [in] */ INetworkAgentInfo* na)
{
    Boolean wasRunningClat;
    mClat->IsRunningClat(na, &wasRunningClat);
    Boolean shouldRunClat = Nat464Xlat::RequiresClat(na);

    if (!wasRunningClat && shouldRunClat) {
        // Start clatd. If it's already been started but is not running yet, this is a no-op.
        mClat->StartClat(na);
    }
    else if (wasRunningClat && !shouldRunClat) {
        mClat->StopClat();
    }
}

void CConnectivityService::UpdateInterfaces(
    /* [in] */ ILinkProperties* newLp,
    /* [in] */ ILinkProperties* oldLp,
    /* [in] */ Int32 netId)
{
    AutoPtr<ILinkPropertiesCompareResult> interfaceDiff;
    CLinkPropertiesCompareResult::New((ILinkPropertiesCompareResult**)&interfaceDiff);

    if (oldLp != NULL) {
        oldLp->CompareAllInterfaceNames(newLp, (ILinkPropertiesCompareResult**)&interfaceDiff);
    }
    else if (newLp != NULL) {
        AutoPtr<IList> list;
        newLp->GetAllInterfaceNames((IList**)&list);
        interfaceDiff->SetAdded(list);
    }

    AutoPtr<IList> list;
    AutoPtr<IIterator> it;
    Boolean hasNext;
    ECode ec = NOERROR;

    interfaceDiff->GetAdded((IList**)&list);
    if (list != NULL) {
        list->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            String iface = Object::ToString(obj);

            if (DBG) Slogger::D(TAG, "Adding iface %s to network %d", iface.string(), netId);
            ec = mNetd->AddInterfaceToNetwork(iface, netId);
            if (FAILED(ec)) {
                Slogger::E(TAG, "Exception adding interface: %s, ec=%08x", iface.string(), ec);
            }
        }
    }

    list = NULL;
    it = NULL;
    interfaceDiff->GetRemoved((IList**)&list);
    if (list != NULL) {
        list->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            String iface = Object::ToString(obj);

            if (DBG) Slogger::D(TAG, "Removing iface %s from network %d", iface.string(), netId);
            ec = mNetd->RemoveInterfaceFromNetwork(iface, netId);
            if (FAILED(ec)) {
                Slogger::E(TAG, "Exception removing interface: %s, ec=%08x", iface.string(), ec);
            }
        }
    }
}

Boolean CConnectivityService::UpdateRoutes(
    /* [in] */ ILinkProperties* newLp,
    /* [in] */ ILinkProperties* oldLp,
    /* [in] */ Int32 netId)
{
    AutoPtr<ILinkPropertiesCompareResult> routeDiff;
    CLinkPropertiesCompareResult::New((ILinkPropertiesCompareResult**)&routeDiff);

    if (oldLp != NULL) {
        oldLp->CompareAllRoutes(newLp, (ILinkPropertiesCompareResult**)&routeDiff);
    }
    else if (newLp != NULL) {
        AutoPtr<IList> list;
        newLp->GetAllRoutes((IList**)&list);
        routeDiff->SetAdded(list);
    }

    AutoPtr<IList> added, removed;
    AutoPtr<IIterator> it;
    Boolean hasNext, bval;
    ECode ec = NOERROR;
    IRouteInfo* route;

    // add routes before removing old in case it helps with continuous connectivity

    // do this twice, adding non-nexthop routes first, then routes they are dependent on
    Int32 i = 0;
    while (i < 2) {
        ++i;
        added = NULL;
        it = NULL;
        routeDiff->GetAdded((IList**)&added);
        if (added != NULL) {
            added->GetIterator((IIterator**)&it);
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                route = IRouteInfo::Probe(obj);
                route->HasGateway(&bval);
                if (bval) continue;
                if (DBG) Slogger::D(TAG, "Adding Route [%s] to network %d",
                    TO_CSTR(route), netId);
                ec = mNetd->AddRoute(netId, route);
                if (FAILED(ec)) {
                    Boolean print = VDBG;
                    if (!print) {
                        AutoPtr<IIpPrefix> ip;
                        route->GetDestination((IIpPrefix**)&ip);
                        AutoPtr<IInetAddress> addr;
                        ip->GetAddress((IInetAddress**)&addr);
                        print = IInet4Address::Probe(addr) != NULL;
                    }
                    if (print) {
                        Slogger::E(TAG, "Exception in addRoute for non-gateway: %08x", ec);
                    }
                }
            }
        }
    }

    it = NULL;
    routeDiff->GetRemoved((IList**)&removed);
    if (removed != NULL) {
        removed->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            route = IRouteInfo::Probe(obj);
            route->HasGateway(&bval);

            if (DBG) Slogger::D(TAG, "Removing Route [%s] to network %d",
                TO_CSTR(route), netId);

            ec = mNetd->RemoveRoute(netId, route);
            if (FAILED(ec)) {
                Slogger::E(TAG, "Exception in removeRoute: %08x", ec);
            }
        }
    }

    Boolean e1, e2;
    added->IsEmpty(&e1);
    removed->IsEmpty(&e2);

    return !e1 || !e2;
}

void CConnectivityService::UpdateDnses(
    /* [in] */ ILinkProperties* newLp,
    /* [in] */ ILinkProperties* oldLp,
    /* [in] */ Int32 netId,
    /* [in] */ Boolean flush)
{
    Boolean bval;
    if (oldLp == NULL || ((newLp->IsIdenticalDnses(oldLp, &bval), bval) == FALSE)) {
        AutoPtr<IList> dnses;
        newLp->GetDnsServers((IList**)&dnses);
        Int32 size;
        dnses->GetSize(&size);
        if (size == 0 && mDefaultDns != NULL) {
            dnses = NULL;
            CArrayList::New((IList**)&dnses);
            dnses->Add(mDefaultDns.Get());
            if (DBG) {
                Slogger::E(TAG, "no dns provided for netId %d, so using defaults", netId);
            }
        }
        if (DBG) Slogger::D(TAG, "Setting Dns servers for network %d to %s",
            netId, TO_CSTR(dnses));

        AutoPtr<ArrayOf<String> > strs;
        NetworkUtils::MakeStrings(ICollection::Probe(dnses), (ArrayOf<String>**)&strs);
        String domains;
        newLp->GetDomains(&domains);
        ECode ec = mNetd->SetDnsServersForNetwork(netId, strs, domains);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Exception in setDnsServersForNetwork: %08x", ec);
        }

        Int32 requestId;
        mDefaultRequest->GetRequestId(&requestId);

        AutoPtr<IInterface> obj;
        mNetworkForRequestId->Get(requestId, (IInterface**)&obj);
        AutoPtr<NetworkAgentInfo> defaultNai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(obj);
        Int32 dfNetId;
        if (defaultNai != NULL
            && (defaultNai->mNetwork->GetNetId(&dfNetId), dfNetId) == netId) {
            SetDefaultDnsSystemProperties(ICollection::Probe(dnses));
        }
        FlushVmDnsCache();
    }
    else if (flush) {
        ECode ec = mNetd->FlushNetworkDnsCache(netId);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Exception in flushNetworkDnsCache: %08x", ec);
        }
        FlushVmDnsCache();
    }
}

void CConnectivityService::SetDefaultDnsSystemProperties(
    /* [in] */ ICollection* dnses)
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);

    Int32 last = 0;
    AutoPtr<IIterator> it;
    dnses->GetIterator((IIterator**)&it);
    Boolean hasNext;
    IInetAddress* dns;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        dns = IInetAddress::Probe(obj);
        ++last;
        String key("net.dns");
        key += last;
        String value;
        dns->GetHostAddress(&value);
        sysProp->Set(key, value);
    }

    for (Int32 i = last + 1; i <= mNumDnsEntries; ++i) {
        String key("net.dns");
        key += i;
        sysProp->Set(key, String(""));
    }
    mNumDnsEntries = last;
}

void CConnectivityService::UpdateCapabilities(
    /* [in] */ INetworkAgentInfo* nai,
    /* [in] */ INetworkCapabilities* networkCapabilities)
{
    //  TODO - turn this on in MR1 when we have more dogfooding time.
    // RematchAllNetworksAndRequests();
    NetworkAgentInfo* networkAgent = (NetworkAgentInfo*)nai;
    if (!Object::Equals(networkAgent->mNetworkCapabilities, networkCapabilities)) {
        ISynchronize* syncObj = (ISynchronize*)networkAgent->Probe(EIID_ISynchronize);
        synchronized(syncObj) {
            networkAgent->mNetworkCapabilities = networkCapabilities;
        }
        NotifyNetworkCallbacks(networkAgent, IConnectivityManager::CALLBACK_CAP_CHANGED);
    }
}

void CConnectivityService::SendUpdatedScoreToFactories(
    /* [in] */ INetworkAgentInfo* networkAgent)
{
    NetworkAgentInfo* nai = (NetworkAgentInfo*)networkAgent;
    Int32 size, score;
    nai->mNetworkRequests->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        nai->mNetworkRequests->ValueAt(i, (IInterface**)&obj);
        INetworkRequest* nr = INetworkRequest::Probe(obj);
        // Don't send listening requests to factories. b/17393458
        if (!IsRequest(nr)) continue;
        nai->GetCurrentScore(&score);
        SendUpdatedScoreToFactories(nr, score);
    }
}

void CConnectivityService::SendUpdatedScoreToFactories(
    /* [in] */ INetworkRequest* networkRequest,
    /* [in] */ Int32 score)
{
    if (VDBG) Slogger::D(TAG, "sending new Min Network Score(%d): %s",
        score, TO_CSTR(networkRequest));

    AutoPtr<ICollection> values;
    mNetworkFactoryInfos->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        NetworkFactoryInfo* nfi = (NetworkFactoryInfo*)IObject::Probe(obj);
        nfi->mAsyncChannel->SendMessage(INetworkFactory::CMD_REQUEST_NETWORK, score, 0, networkRequest);
    }
}

void CConnectivityService::CallCallbackForRequest(
    /* [in] */ NetworkRequestInfo* nri,
    /* [in] */ INetworkAgentInfo* nai,
    /* [in] */ Int32 notificationType)
{
    if (nri->mMessenger == NULL) return;  // Default request has no msgr

    NetworkAgentInfo* networkAgent = (NetworkAgentInfo*)nai;
    AutoPtr<INetworkRequest> request;
    CNetworkRequest::New(nri->mRequest, (INetworkRequest**)&request);
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutParcelable(String("CNetworkRequest"), IParcelable::Probe(request));
    AutoPtr<IMessage> msg;
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    helper->Obtain((IMessage**)&msg);
    if (notificationType != IConnectivityManager::CALLBACK_UNAVAIL
        && notificationType != IConnectivityManager::CALLBACK_RELEASED) {
        bundle->PutParcelable(String("CNetwork"), IParcelable::Probe(networkAgent->mNetwork));
    }
    switch (notificationType) {
        case IConnectivityManager::CALLBACK_LOSING: {
            msg->SetArg1(30 * 1000); // TODO - read this from NetworkMonitor
            break;
        }
        case IConnectivityManager::CALLBACK_CAP_CHANGED: {
            AutoPtr<INetworkCapabilities> nc;
            CNetworkCapabilities::New(networkAgent->mNetworkCapabilities, (INetworkCapabilities**)&nc);
            bundle->PutParcelable(String("CNetworkCapabilities"), IParcelable::Probe(nc));
            break;
        }
        case IConnectivityManager::CALLBACK_IP_CHANGED: {
            AutoPtr<ILinkProperties> lp;
            CLinkProperties::New(networkAgent->mLinkProperties, (ILinkProperties**)&lp);
            bundle->PutParcelable(String("CLinkProperties"), IParcelable::Probe(lp));
            break;
        }
    }
    msg->SetWhat(notificationType);
    msg->SetData(bundle);
    if (VDBG) {
        Slogger::D(TAG, "sending notification %s for %s",
            NotifyTypeToName(notificationType).string(),
            TO_CSTR(nri->mRequest));
    }

    ECode ec = nri->mMessenger->Send(msg);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        // may occur naturally in the race of binder death.
        Slogger::E(TAG, "RemoteException caught trying to send a callback msg for %s",
            TO_CSTR(nri->mRequest));
    }
}

void CConnectivityService::TeardownUnneededNetwork(
    /* [in] */ INetworkAgentInfo* networkAgent)
{
    NetworkAgentInfo* nai = (NetworkAgentInfo*)networkAgent;
    Int32 size;
    nai->mNetworkRequests->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        nai->mNetworkRequests->ValueAt(i, (IInterface**)&obj);
        INetworkRequest* nr = INetworkRequest::Probe(obj);
        // Ignore listening requests.
        if (!IsRequest(nr)) continue;
        Slogger::E(TAG, "Dead network still had at least %s",
            TO_CSTR(nr));
        break;
    }
    nai->mAsyncChannel->Disconnect();
}

void CConnectivityService::HandleLingerComplete(
    /* [in] */ INetworkAgentInfo* networkAgent)
{
    NetworkAgentInfo* oldNetwork = (NetworkAgentInfo*)networkAgent;
    if (oldNetwork == NULL) {
        Slogger::E(TAG, "Unknown NetworkAgentInfo in HandleLingerComplete");
        return;
    }
    if (DBG) {
        Slogger::D(TAG, "HandleLingerComplete for %s", oldNetwork->Name().string());
    }
    TeardownUnneededNetwork(networkAgent);
}

void CConnectivityService::HandleDefaultNetworkSwitch()
{
    AutoPtr<NetworkAgentInfo> currentDefaultNetwork;
    AutoPtr<ICollection> values;
    mNetworkAgentInfos->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> tmp;
        it->GetNext((IInterface**)&tmp);
        NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(tmp);
        Int32 id = 0;
        mDefaultRequest->GetRequestId(&id);
        AutoPtr<IInterface> p;
        nai->mNetworkRequests->Get(id, (IInterface**)&p);
        if(p != NULL) {
            currentDefaultNetwork = nai;
            break;
        }
    }
    if(currentDefaultNetwork == NULL) return;
    Slogger::D(TAG, "currentDefaultNetwork: %p", currentDefaultNetwork.Get());
    AutoPtr<NetworkAgentInfo> networkSwitchTo = currentDefaultNetwork;
    it = NULL;
    values->GetIterator((IIterator**)&it);
    hasNext = FALSE;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> tmp;
        it->GetNext((IInterface**)&tmp);
        NetworkAgentInfo* nai = (NetworkAgentInfo*)INetworkAgentInfo::Probe(tmp);
        assert(0 && "TODO");
        // if(mDefaultRequest->mNetworkCapabilities->SatisfiedByNetworkCapabilities(
        //             nai->mNetworkCapabilities)) {
        //     if(nai->GetCurrentScore() > networkSwitchTo->GetCurrentScore()) {
        //         networkSwitchTo = nai;
        //     }
        // }
    }
    Slogger::D(TAG, "network switch to: %p", networkSwitchTo.Get());
    if(networkSwitchTo != currentDefaultNetwork) {
        networkSwitchTo->mNetworkMonitor->SendMessage(NetworkMonitor::CMD_NETWORK_CONNECTED);
    }
}

void CConnectivityService::MakeDefault(
    /* [in] */ INetworkAgentInfo* networkAgent)
{
    NetworkAgentInfo* newNetwork = (NetworkAgentInfo*)networkAgent;
    if (DBG) Slogger::D(TAG, "Switching to new default network: %s", newNetwork->Name().string());
    newNetwork->mNetworkInfo->GetType(&mActiveDefaultNetwork);
    SetupDataActivityTracking(newNetwork);
    Int32 id;
    newNetwork->mNetwork->GetNetId(&id);
    ECode ec = mNetd->SetDefaultNetId(id);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Exception setting default network :%08x", ec);
    }
    NotifyLockdownVpn(newNetwork);
    AutoPtr<IProxyInfo> pi;
    newNetwork->mLinkProperties->GetHttpProxy((IProxyInfo**)&pi);
    HandleApplyDefaultProxy(pi);
    UpdateTcpBufferSizes(networkAgent);
}

void CConnectivityService::RematchNetworkAndRequests(
    /* [in] */ INetworkAgentInfo* networkAgent,
    /* [in] */ Boolean nascent)
{
    NetworkAgentInfo* newNetwork = (NetworkAgentInfo*)networkAgent;
    if (!newNetwork->mCreated) Slogger::E(TAG, "ERROR: uncreated network being rematched.");
    if (nascent && !newNetwork->mValidated) Slogger::E(TAG, "ERROR: nascent network not validated.");
    Boolean keep;
    newNetwork->IsVPN(&keep);
    Boolean isNewDefault = FALSE;
    if (DBG) Slogger::D(TAG, "rematching %s", newNetwork->Name().string());
    // Find and migrate to this Network any NetworkRequests for
    // which this network is now the best.
    if (VDBG) Slogger::D(TAG, " network has: %s", TO_CSTR(newNetwork->mNetworkCapabilities));


    List<AutoPtr<NetworkAgentInfo> > affectedNetworks;
    AutoPtr<ICollection> values;
    mNetworkRequests->GetValues((ICollection**)&values);
    AutoPtr<IIterator> it;
    values->GetIterator((IIterator**)&it);
    Boolean hasNext, bval;
    Int32 defRequestId, requestId, score, curScore, legacyType, netType;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> tmp;
        it->GetNext((IInterface**)&tmp);
        AutoPtr<NetworkRequestInfo> nri = (NetworkRequestInfo*)IProxyDeathRecipient::Probe(tmp);
        nri->mRequest->GetRequestId(&requestId);
        tmp = NULL;
        mNetworkForRequestId->Get(requestId, (IInterface**)&tmp);
        AutoPtr<NetworkAgentInfo> currentNetwork = (NetworkAgentInfo*)INetworkAgentInfo::Probe(tmp);
        if (newNetwork == currentNetwork.Get()) {
            if (DBG) {
                Slogger::D(TAG, "Network %s was already satisfying request %d. No change.",
                    newNetwork->Name().string(), requestId);
            }
            keep = TRUE;
            continue;
        }

        // check if it satisfies the NetworkCapabilities
        if (VDBG) Slogger::D(TAG, "  checking if request is satisfied: %s",
            TO_CSTR(nri->mRequest));
        AutoPtr<INetworkCapabilities> netCap;
        nri->mRequest->GetNetworkCapabilities((INetworkCapabilities**)&netCap);
        netCap->SatisfiedByNetworkCapabilities(newNetwork->mNetworkCapabilities, &bval);
        if (bval) {
            if (!nri->mIsRequest) {
                // This is not a request, it's a callback listener.
                // Add it to newNetwork regardless of score.
                newNetwork->AddRequest(nri->mRequest);
                continue;
            }

            // next check if it's better than any current network we're using for
            // this request
            curScore = 0;
            if (currentNetwork != NULL) {
                currentNetwork->GetCurrentScore(&curScore);
            }
            newNetwork->GetCurrentScore(&score);

            if (VDBG) {
                Slogger::D(TAG, "currentScore = %d, newScore= %d", curScore, score);
            }
            if (currentNetwork == NULL || curScore < score) {
                if (currentNetwork != NULL) {
                    if (DBG) Slogger::D(TAG, "   accepting network in place of %s",
                        currentNetwork->Name().string());
                    currentNetwork->mNetworkRequests->Remove(requestId);
                    currentNetwork->mNetworkLingered->Add(nri->mRequest);
                    affectedNetworks.PushBack(currentNetwork);
                }
                else {
                    if (DBG) Slogger::D(TAG, "   accepting network in place of NULL");
                }
                mNetworkForRequestId->Put(requestId, TO_IINTERFACE(newNetwork));
                newNetwork->AddRequest(nri->mRequest);

                legacyType = IConnectivityManager::TYPE_NONE;
                if (nri->mIsRequest) {
                    nri->mRequest->GetLegacyType(&legacyType);
                }
                if (legacyType!= IConnectivityManager::TYPE_NONE) {
                    mLegacyTypeTracker->Add(legacyType, newNetwork);
                }
                keep = TRUE;
                // Tell NetworkFactories about the new score, so they can stop
                // trying to connect if they know they cannot match it.
                // TODO - this could get expensive if we have alot of requests for this
                // network.  Think about if there is a way to reduce this.  Push
                // netid->request mapping to each factory?
                SendUpdatedScoreToFactories(nri->mRequest, score);
                mDefaultRequest->GetRequestId(&defRequestId);
                if (defRequestId == requestId) {
                    isNewDefault = TRUE;
                    // TODO: Remove following line.  It's redundant with MakeDefault call.
                    newNetwork->mNetworkInfo->GetType(&mActiveDefaultNetwork);
                    if (newNetwork->mLinkProperties != NULL) {
                        UpdateTcpBufferSizes(newNetwork);
                        AutoPtr<IList> srvs;
                        newNetwork->mLinkProperties->GetDnsServers((IList**)&srvs);
                        SetDefaultDnsSystemProperties(ICollection::Probe(srvs));
                    }
                    else {
                        AutoPtr<ICollection> srvs;
                        CArrayList::New((ICollection**)&srvs);
                        SetDefaultDnsSystemProperties(srvs);
                    }
                    // Maintain the illusion: since the legacy API only
                    // understands one network at a time, we must pretend
                    // that the current default network disconnected before
                    // the new one connected.
                    if (currentNetwork != NULL) {
                        currentNetwork->mNetworkInfo->GetType(&netType);
                        mLegacyTypeTracker->Remove(netType, currentNetwork);
                    }
                    mDefaultInetConditionPublished = newNetwork->mValidated ? 100 : 0;
                    newNetwork->mNetworkInfo->GetType(&netType);
                    mLegacyTypeTracker->Add(netType, newNetwork);
                    NotifyLockdownVpn(newNetwork);
                }
            }
        }
    }

    // Linger any networks that are no longer needed.
    NetworkAgentInfo* nai;
    List<AutoPtr<NetworkAgentInfo> >::Iterator lit;
    Int32 size;
    Boolean teardown;
    for (lit = affectedNetworks.Begin(); lit != affectedNetworks.End(); ++lit) {
        nai = *lit;
        nai->IsVPN(&teardown);
        teardown = !teardown && nai->mValidated;
        nai->mNetworkRequests->GetSize(&size);
        for (Int32 i = 0; i < size && teardown; i++) {
            AutoPtr<IInterface> obj;
            nai->mNetworkRequests->ValueAt(i, (IInterface**)&obj);
            INetworkRequest* nr = INetworkRequest::Probe(obj);
            if (nr == NULL) {
                Slogger::E(TAG, "Request %s not found in mNetworkRequests.", TO_CSTR(nr));
                Slogger::E(TAG, "  it came from request list  of %s", nai->Name().string());
            }
            else {
                bval = IsRequest(nr);
                if (bval) {
                    teardown = FALSE;
                }
            }
        }
        if (teardown) {
            nai->mNetworkMonitor->SendMessage(NetworkMonitor::CMD_NETWORK_LINGER);
            NotifyNetworkCallbacks((INetworkAgentInfo*)nai, IConnectivityManager::CALLBACK_LOSING);
        }
        else {
            // not going to linger, so kill the list of linger networks..  only
            // notify them of linger if it happens as the result of gaining another,
            // but if they transition and old network stays up, don't tell them of linger
            // or very delayed loss
            nai->mNetworkLingered->Clear();
            if (VDBG) Slogger::D(TAG, "Lingered for %s cleared", nai->Name().string());
        }
    }

    if (keep) {
        if (isNewDefault) {
            // Notify system services that this network is up.
            MakeDefault(newNetwork);
            ISynchronize* syncObj = this;
            synchronized(syncObj) {
                // have a new default network, release the transition wakelock in
                // a second if it's held.  The second pause is to allow apps
                // to reconnect over the new network
                mNetTransitionWakeLock->IsHeld(&bval);
                if (bval) {
                    AutoPtr<IMessage> msg;
                    mHandler->ObtainMessage(
                        EVENT_CLEAR_NET_TRANSITION_WAKELOCK,
                        mNetTransitionWakeLockSerialNumber, 0, (IMessage**)&msg);
                    mHandler->SendMessageDelayed(msg, 1000, &bval);
                }
            }
        }

        // Notify battery stats service about this network, both the normal
        // interface and any stacked links.
        // TODO: Avoid redoing this; this must only be done once when a network comes online.

        Int32 type;
        newNetwork->mNetworkInfo->GetType(&type);
        String baseIface;
        newNetwork->mLinkProperties->GetInterfaceName(&baseIface);

        AutoPtr<INetworkStatsFactoryHelper> nsfHelper;
        CNetworkStatsFactoryHelper::AcquireSingleton((INetworkStatsFactoryHelper**)&nsfHelper);
        AutoPtr<IIBatteryStats> bs;
        assert(0 && "TODO");
        //bs = CBatteryStatsService::GetService();
        bs->NoteNetworkInterfaceType(baseIface, type);
        AutoPtr<IList> links;
        newNetwork->mLinkProperties->GetStackedLinks((IList**)&links);
        it = NULL;
        links->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            ILinkProperties* stacked = ILinkProperties::Probe(obj);
            String stackedIface;
            stacked->GetInterfaceName(&stackedIface);
            bs->NoteNetworkInterfaceType(stackedIface, type);
            nsfHelper->NoteStackedIface(stackedIface, baseIface);
        }

        NotifyNetworkCallbacks(networkAgent, IConnectivityManager::CALLBACK_AVAILABLE);
    }
    else if (nascent) {
        // Only tear down newly validated networks here.  Leave unvalidated to either become
        // validated (and get evaluated against peers, one losing here) or
        // NetworkMonitor reports a bad network and we tear it down then.
        // Networks that have been up for a while and are validated should be torn down via
        // the lingering process so communication on that network is given time to wrap up.
        // TODO: Could teardown unvalidated networks when their NetworkCapabilities
        // satisfy no NetworkRequests.
        if (DBG) Slogger::D(TAG, "Validated network turns out to be unwanted.  Tear it down.");
        TeardownUnneededNetwork(networkAgent);
    }
}

void CConnectivityService::RematchAllNetworksAndRequests(
    /* [in] */ INetworkAgentInfo* changed,
    /* [in] */ Int32 oldScore)
{
    // TODO: This may get slow.  The "changed" parameter is provided for future optimization
    // to avoid the slowness.  It is not simply enough to process just "changed", for
    // example in the case where "changed"'s score decreases and another network should begin
    // satifying a NetworkRequest that "changed" currently satisfies.

    // Optimization: Only reprocess "changed" if its score improved.  This is safe because it
    // can only add more NetworkRequests satisfied by "changed", and this is exactly what
    // RematchNetworkAndRequests() handles.
    Int32 score = 0;
    if (changed != NULL) {
        changed->GetCurrentScore(&score);
    }

    if (changed != NULL && oldScore < score) {
        RematchNetworkAndRequests(changed, FALSE);
    }
    else {
        AutoPtr<ICollection> values;
        mNetworkAgentInfos->GetValues((ICollection**)&values);
        AutoPtr<IIterator> it;
        values->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            RematchNetworkAndRequests(INetworkAgentInfo::Probe(obj), FALSE);
        }
    }
}

void CConnectivityService::UpdateInetCondition(
    /* [in] */ INetworkAgentInfo* networkAgent,
    /* [in] */ Boolean valid)
{
    NetworkAgentInfo* nai = (NetworkAgentInfo*)networkAgent;
    // Don't bother updating until we've graduated to validated at least once.
    if (!nai->mValidated) return;
    // For now only update icons for default connection.
    // TODO: Update WiFi and cellular icons separately. b/17237507
    if (!IsDefaultNetwork(networkAgent)) return;

    Int32 newInetCondition = valid ? 100 : 0;
    // Don't repeat publish.
    if (newInetCondition == mDefaultInetConditionPublished) return;

    mDefaultInetConditionPublished = newInetCondition;
    SendInetConditionBroadcast(nai->mNetworkInfo);
}

void CConnectivityService::NotifyLockdownVpn(
    /* [in] */ INetworkAgentInfo* networkAgent)
{
    NetworkAgentInfo* nai = (NetworkAgentInfo*)networkAgent;
    if (mLockdownTracker != NULL) {
        Boolean bval;
        if (nai != NULL && nai->IsVPN(&bval)) {
            mLockdownTracker->OnVpnStateChanged(nai->mNetworkInfo);
        }
        else {
            mLockdownTracker->OnNetworkInfoChanged();
        }
    }
}

void CConnectivityService::SendNetworkInfoUpdateBroadcast(
    /* [in] */ Int32 type,
    /* [in] */ Int32 netid)
{
    AutoPtr<IIntent> intent;
    CIntent::New(NETID_UPDATE, (IIntent**)&intent);
    intent->PutExtra(EXTRA_NETWORK_TYPE, type);
    intent->PutExtra(EXTRA_NETID, netid);
    Slogger::D(TAG, "sendNetworkInfoUpdateBroadcast type = %d netid = %d", type, netid);

    mContext->SendBroadcast(intent);
}

void CConnectivityService::UpdateNetworkInfo(
    /* [in] */ INetworkAgentInfo* nai,
    /* [in] */ INetworkInfo* newInfo)
{
    NetworkAgentInfo* networkAgent = (NetworkAgentInfo*)nai;
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);

    NetworkInfoState state;
    newInfo->GetState(&state);
    AutoPtr<INetworkInfo> oldInfo;
    ISynchronize* syncObj = ISynchronize::Probe(nai);
    synchronized(syncObj) {
        oldInfo = networkAgent->mNetworkInfo;
        networkAgent->mNetworkInfo = newInfo;
    }
    NotifyLockdownVpn(nai);

    NetworkInfoState oldState;
    if (oldInfo != NULL && (oldInfo->GetState(&oldState)) == state) {
        if (VDBG) Slogger::D(TAG, "ignoring duplicate network state non-change");
        return;
    }

    if (DBG) {
        Slogger::D(TAG, "%s EVENT_NETWORK_INFO_CHANGED, going from %d to %d",
            (oldInfo == NULL ? 0 : (Int32)oldState), (Int32)state,
            networkAgent->Name().string());
    }

    if (state == NetworkInfoState_CONNECTED && !networkAgent->mCreated) {
        ECode ec = NOERROR;
        Boolean bval;
        Int32 netId;
        networkAgent->mNetwork->GetNetId(&netId);

        // This should never fail.  Specifying an already in use NetID will cause failure.
        if (networkAgent->IsVPN(&bval), bval) {
            Boolean isEmpty, allow = TRUE;
            AutoPtr<IList> srvs;
            networkAgent->mLinkProperties->GetDnsServers((IList**)&srvs);
            srvs->IsEmpty(&isEmpty);
            if (networkAgent->mNetworkMisc != NULL) {
                networkAgent->mNetworkMisc->GetAllowBypass(&bval);
                allow = !bval;
            }

            mNetd->CreateVirtualNetwork(netId, !isEmpty, allow);
        }
        else {
            mNetd->CreatePhysicalNetwork(netId);
        }
        if (FAILED(ec)) {
            Slogger::E(TAG, "Error creating network %d: %08x", netId, ec);
            return;
        }

        networkAgent->mCreated = TRUE;
        UpdateLinkProperties(nai, NULL);
        NotifyNetworkCallbacks(nai, IConnectivityManager::CALLBACK_PRECHECK);
        networkAgent->mNetworkMonitor->SendMessage(NetworkMonitor::CMD_NETWORK_CONNECTED);
        if (networkAgent->IsVPN(&bval), bval) {
            // Temporarily disable the default proxy (not global).
            synchronized(mProxyLock) {
                if (!mDefaultProxyDisabled) {
                    mDefaultProxyDisabled = TRUE;
                    if (mGlobalProxy == NULL && mDefaultProxy != NULL) {
                        SendProxyBroadcast(NULL);
                    }
                }
            }
            // TODO: support proxy per network.
        }
        // Consider network even though it is not yet validated.
        RematchNetworkAndRequests(nai, FALSE);
        Int32 val = 0;
        sysProp->GetInt32(String("persist.cne.feature"), 0, &val);
        Boolean isPropFeatureEnabled = (val == 3) ? TRUE : FALSE;
        if (isPropFeatureEnabled) {
            Int32 type = 0;
            newInfo->GetType(&type);
            if ((type == IConnectivityManager::TYPE_WIFI) ||
                (type == IConnectivityManager::TYPE_MOBILE)) {
                if (DBG) {
                    Slogger::D(TAG, "sending network info update for type = %d", type);
                }
                Int32 netId = 0;
                networkAgent->mNetwork->GetNetId(&netId);
                SendNetworkInfoUpdateBroadcast(type, netId);
            }
        }
    }
    else if (state == NetworkInfoState_DISCONNECTED ||
            state == NetworkInfoState_SUSPENDED) {
        Boolean bval;
        networkAgent->mAsyncChannel->Disconnect();
        if (networkAgent->IsVPN(&bval), bval) {
            synchronized(mProxyLock) {
                if (mDefaultProxyDisabled) {
                    mDefaultProxyDisabled = FALSE;
                    if (mGlobalProxy == NULL && mDefaultProxy != NULL) {
                        SendProxyBroadcast(mDefaultProxy);
                    }
                }
            }
        }
    }
}

void CConnectivityService::UpdateNetworkScore(
    /* [in] */ INetworkAgentInfo* networkAgentInfo,
    /* [in] */ Int32 score)
{
    NetworkAgentInfo* nai = (NetworkAgentInfo*)networkAgentInfo;
    if (DBG) Slogger::D(TAG, "UpdateNetworkScore for %s to %d", nai->Name().string(), score);
    if (score < 0) {
        Slogger::E(TAG, "UpdateNetworkScore for %s got a negative score (%d).  Bumping score to min of 0",
            nai->Name().string(), score);
        score = 0;
    }

    Int32 oldScore;
    nai->GetCurrentScore(&oldScore);
    nai->SetCurrentScore(score);

    if (nai->mCreated) RematchAllNetworksAndRequests(networkAgentInfo, oldScore);

    SendUpdatedScoreToFactories(networkAgentInfo);
}

// notify only this one new request of the current state
void CConnectivityService::NotifyNetworkCallback(
    /* [in] */ INetworkAgentInfo* nai,
    /* [in] */ NetworkRequestInfo* nri)
{
    Int32 notifyType = IConnectivityManager::CALLBACK_AVAILABLE;
    // TODO - read state from monitor to decide what to send.
//        if (nai->networkMonitor.isLingering()) {
//            notifyType = NetworkCallbacks.LOSING;
//        } else if (nai->networkMonitor.isEvaluating()) {
//            notifyType = NetworkCallbacks.CallCallbackForRequest(request, nai, notifyType);
//        }
    CallCallbackForRequest(nri, nai, notifyType);
}

void CConnectivityService::SendLegacyNetworkBroadcast(
    /* [in] */ INetworkAgentInfo* networkAgentInfo,
    /* [in] */ Boolean connected,
    /* [in] */ Int32 type)
{
    NetworkAgentInfo* nai = (NetworkAgentInfo*)networkAgentInfo;

    // The NetworkInfo we actually send out has no bearing on the real
    // state of affairs. For example, if the default connection is mobile,
    // and a request for HIPRI has just gone away, we need to pretend that
    // HIPRI has just disconnected. So we need to set the type to HIPRI and
    // the state to DISCONNECTED, even though the network is of type MOBILE
    // and is still connected.
    AutoPtr<INetworkInfo> info;
    CNetworkInfo::New(nai->mNetworkInfo, (INetworkInfo**)&info);
    String extraInfo, nullStr;
    info->GetExtraInfo(&extraInfo);
    info->SetType(type);
    if (connected) {
        info->SetDetailedState(NetworkInfoDetailedState_CONNECTED, nullStr, extraInfo);
        SendConnectedBroadcastDelayed(info, GetConnectivityChangeDelay());
    }
    else {
        Boolean bval;
        Int32 niType;
        info->GetType(&niType);
        info->SetDetailedState(NetworkInfoDetailedState_DISCONNECTED, nullStr, extraInfo);
        AutoPtr<IIntent> intent;
        CIntent::New(IConnectivityManager::CONNECTIVITY_ACTION, (IIntent**)&intent);
        intent->PutExtra(IConnectivityManager::EXTRA_NETWORK_INFO, IParcelable::Probe(info));
        intent->PutExtra(IConnectivityManager::EXTRA_NETWORK_TYPE, niType);
        if (info->IsFailover(&bval), bval) {
            intent->PutExtra(IConnectivityManager::EXTRA_IS_FAILOVER, TRUE);
            nai->mNetworkInfo->SetFailover(FALSE);
        }

        String reason;
        info->GetReason(&reason);
        if (reason != NULL) {
            intent->PutExtra(IConnectivityManager::EXTRA_REASON, reason);
        }
        info->GetExtraInfo(&extraInfo);
        if (extraInfo != NULL) {
            intent->PutExtra(IConnectivityManager::EXTRA_EXTRA_INFO, extraInfo);
        }

        AutoPtr<NetworkAgentInfo> newDefaultAgent;
        Int32 defRequestId;
        mDefaultRequest->GetRequestId(&defRequestId);
        AutoPtr<IInterface> obj;
        nai->mNetworkRequests->Get(defRequestId, (IInterface**)&obj);
        if (obj != NULL) {
            obj = NULL;
            mNetworkForRequestId->Get(defRequestId, (IInterface**)&obj);
            newDefaultAgent = (NetworkAgentInfo*)INetworkAgentInfo::Probe(obj);
            if (newDefaultAgent != NULL) {
                intent->PutExtra(IConnectivityManager::EXTRA_OTHER_NETWORK_INFO,
                    IParcelable::Probe(newDefaultAgent->mNetworkInfo));
            }
            else {
                intent->PutExtra(IConnectivityManager::EXTRA_NO_CONNECTIVITY, TRUE);
            }
        }
        intent->PutExtra(IConnectivityManager::EXTRA_INET_CONDITION, mDefaultInetConditionPublished);
        AutoPtr<IIntent> immediateIntent;
        CIntent::New(intent, (IIntent**)&immediateIntent);
        immediateIntent->SetAction(IConnectivityManager::CONNECTIVITY_ACTION_IMMEDIATE);
        SendStickyBroadcast(immediateIntent);
        SendStickyBroadcastDelayed(intent, GetConnectivityChangeDelay());
        if (newDefaultAgent != NULL) {
            SendConnectedBroadcastDelayed(newDefaultAgent->mNetworkInfo,
                GetConnectivityChangeDelay());
        }
    }
}

void CConnectivityService::NotifyNetworkCallbacks(
    /* [in] */ INetworkAgentInfo* nai,
    /* [in] */ Int32 notifyType)
{
    NetworkAgentInfo* networkAgent = (NetworkAgentInfo*)nai;
    if (DBG) Slogger::D(TAG, "notifyType %s for %s",
        NotifyTypeToName(notifyType).string(), networkAgent->Name().string());

    Int32 size;
    networkAgent->mNetworkRequests->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        networkAgent->mNetworkRequests->ValueAt(i, (IInterface**)&obj);
        AutoPtr<INetworkRequest> nr = INetworkRequest::Probe(obj);

        obj = NULL;
        mNetworkRequests->Get(nr.Get(), (IInterface**)&obj);
        AutoPtr<NetworkRequestInfo> nri = (NetworkRequestInfo*)IProxyDeathRecipient::Probe(obj);
        if (VDBG) Slogger::D(TAG, " sending notification for %s", TO_CSTR(nr));
        CallCallbackForRequest(nri, networkAgent, notifyType);
    }
}

String CConnectivityService::NotifyTypeToName(
    /* [in] */ Int32 notifyType)
{
    switch (notifyType) {
        case IConnectivityManager::CALLBACK_PRECHECK:    return String("PRECHECK");
        case IConnectivityManager::CALLBACK_AVAILABLE:   return String("AVAILABLE");
        case IConnectivityManager::CALLBACK_LOSING:      return String("LOSING");
        case IConnectivityManager::CALLBACK_LOST:        return String("LOST");
        case IConnectivityManager::CALLBACK_UNAVAIL:     return String("UNAVAILABLE");
        case IConnectivityManager::CALLBACK_CAP_CHANGED: return String("CAP_CHANGED");
        case IConnectivityManager::CALLBACK_IP_CHANGED:  return String("IP_CHANGED");
        case IConnectivityManager::CALLBACK_RELEASED:    return String("RELEASED");
    }
    return String("UNKNOWN");
}

AutoPtr<ILinkProperties> CConnectivityService::GetLinkPropertiesForTypeInternal(
    /* [in] */ Int32 networkType)
{
    AutoPtr<ILinkProperties> result;
    AutoPtr<NetworkAgentInfo> nai = (NetworkAgentInfo*)mLegacyTypeTracker->GetNetworkForType(networkType).Get();
    if (nai != NULL) {
        ISynchronize* syncObj = (ISynchronize*)nai->Probe(EIID_ISynchronize);
        synchronized(syncObj) {
            CLinkProperties::New(nai->mLinkProperties, (ILinkProperties**)&result);
            return result;
        }
    }

    CLinkProperties::New((ILinkProperties**)&result);
    return result;
}

AutoPtr<INetworkInfo> CConnectivityService::GetNetworkInfoForType(
    /* [in] */ Int32 networkType)
{
    if (!mLegacyTypeTracker->IsTypeSupported(networkType))
        return NULL;

    AutoPtr<NetworkAgentInfo> nai = (NetworkAgentInfo*)mLegacyTypeTracker->GetNetworkForType(networkType).Get();
    if (nai != NULL) {
        AutoPtr<INetworkInfo> result;
        CNetworkInfo::New(nai->mNetworkInfo, (INetworkInfo**)&result);
        result->SetType(networkType);
        return result;
    }

    AutoPtr<IConnectivityManagerHelper> helper;
    CConnectivityManagerHelper::AcquireSingleton((IConnectivityManagerHelper**)&helper);
    String ntn;
    helper->GetNetworkTypeName(networkType, &ntn);
    AutoPtr<INetworkInfo> result;
    CNetworkInfo::New(networkType, 0, ntn, String(""), (INetworkInfo**)&result);
    String nullStr;
    result->SetDetailedState(NetworkInfoDetailedState_DISCONNECTED, nullStr, nullStr);
    result->SetIsAvailable(TRUE);
    return result;
}

AutoPtr<INetworkCapabilities> CConnectivityService::GetNetworkCapabilitiesForType(
    /* [in] */ Int32 networkType)
{
    AutoPtr<NetworkAgentInfo> nai = (NetworkAgentInfo*)mLegacyTypeTracker->GetNetworkForType(networkType).Get();
    AutoPtr<INetworkCapabilities> netCap;
    if (nai != NULL) {
        ISynchronize* syncObj = (ISynchronize*)nai->Probe(EIID_ISynchronize);
        synchronized(syncObj) {
            CNetworkCapabilities::New(nai->mNetworkCapabilities, (INetworkCapabilities**)&netCap);
            return netCap;
        }
    }

    CNetworkCapabilities::New((INetworkCapabilities**)&netCap);
    return netCap;
}

ECode CConnectivityService::AddVpnAddress(
    /* [in] */ const String& address,
    /* [in] */Int32 prefixLength,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    FAIL_RETURN(ThrowIfLockdownEnabled())
    Int32 user = UserHandle::GetUserId(Binder::GetCallingUid());
    synchronized(mVpnsLock) {
        HashMap<Int32, AutoPtr<Vpn> >::Iterator it = mVpns.Find(user);
        if (it != mVpns.End()) {
            AutoPtr<Vpn> vpn = it->mSecond;
            return vpn->AddAddress(address, prefixLength);
        }
    }
    return NOERROR;
}

ECode CConnectivityService::RemoveVpnAddress(
    /* [in] */ const String& address,
    /* [in] */ Int32 prefixLength,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    FAIL_RETURN(ThrowIfLockdownEnabled())
    Int32 user = UserHandle::GetUserId(Binder::GetCallingUid());
    synchronized(mVpnsLock) {
        HashMap<Int32, AutoPtr<Vpn> >::Iterator it = mVpns.Find(user);
        if (it != mVpns.End()) {
            AutoPtr<Vpn> vpn = it->mSecond;
            return vpn->RemoveAddress(address, prefixLength);
        }
    }
    return NOERROR;
}

ECode CConnectivityService::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} //namespace Server
} //namespace Droid
} //namespace Elastos

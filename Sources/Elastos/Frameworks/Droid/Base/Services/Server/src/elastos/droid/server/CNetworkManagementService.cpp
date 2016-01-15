
#include "CNetworkManagementService.h"
#include "NetworkManagementSocketTagger.h"
#include "elastos/droid/net/LockdownVpnTracker.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuffer.h>
#include <Elastos.CoreLibrary.h>
#include <cutils/log.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Internal::Net::CNetworkStatsFactory;
using Elastos::Droid::Server::Net::LockdownVpnTracker;

namespace Elastos {
namespace Droid {
namespace Server {

CAR_INTERFACE_IMPL(CNetworkManagementService::NetdCallbackReceiver, INativeDaemonConnectorCallbacks)

//
// Netd Callback handling
//
ECode CNetworkManagementService::NetdCallbackReceiver::OnDaemonConnected()
{
    // event is dispatched from internal NDC thread, so we prepare the
    // daemon back on main thread.
    if (mOwner->mConnectedSignal != NULL) {
        mOwner->mConnectedSignal->CountDown();
        mOwner->mConnectedSignal = NULL;
    }
    else {
        AutoPtr<IRunnable> task = new ReceiverRunnable(mOwner);
        Boolean result;
        mOwner->mMainHandler->Post(task, &result);
    }
    return NOERROR;
}

ECode CNetworkManagementService::NetdCallbackReceiver::OnEvent(
    /* [in] */ Int32 code,
    /* [in] */ const String& raw,
    /* [in] */ const ArrayOf<String>& cooked,
    /* [out] */ Boolean* result)
{
    switch (code) {
    case NetdResponseCode::InterfaceChange:
        /*
         * a network interface change occured
         * Format: "NNN Iface added <name>"
         *         "NNN Iface removed <name>"
         *         "NNN Iface changed <name> <up/down>"
         *         "NNN Iface linkstatus <name> <up/down>"
         */
        if (cooked.GetLength() < 4 || !cooked[1].Equals("Iface")) {
            //throw new IllegalStateException(String.format("Invalid event from daemon (%s)", raw));
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        if (cooked[2].Equals("added")) {
            mOwner->NotifyInterfaceAdded(cooked[3]);
            *result = TRUE;
            return NOERROR;
        }
        else if (cooked[2].Equals("removed")) {
            mOwner->NotifyInterfaceRemoved(cooked[3]);
            *result = TRUE;
            return NOERROR;
        }
        else if (cooked[2].Equals("changed") && cooked.GetLength() == 5) {
            mOwner->NotifyInterfaceStatusChanged(cooked[3], cooked[4].Equals("up"));
            *result = TRUE;
            return NOERROR;
        }
        else if (cooked[2].Equals("linkstate") && cooked.GetLength() == 5) {
            mOwner->NotifyInterfaceLinkStateChanged(cooked[3], cooked[4].Equals("up"));
            *result = TRUE;
            return NOERROR;
        }
        //throw new IllegalStateException(String.format("Invalid event from daemon (%s)", raw));
        return E_ILLEGAL_STATE_EXCEPTION;
        // break;
    case NetdResponseCode::BandwidthControl:
        /*
         * Bandwidth control needs some attention
         * Format: "NNN limit alert <alertName> <ifaceName>"
         */
        if (cooked.GetLength() < 5 || !cooked[1].Equals("limit")) {
            //throw new IllegalStateException(String.format("Invalid event from daemon (%s)", raw));
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        if (cooked[2].Equals("alert")) {
            mOwner->NotifyLimitReached(cooked[3], cooked[4]);
            *result = TRUE;
            return NOERROR;
        }
        //throw new IllegalStateException(String.format("Invalid event from daemon (%s)", raw));
        return E_ILLEGAL_STATE_EXCEPTION;
        // break;
    case NetdResponseCode::InterfaceClassActivity:
    {
        /*
         * An network interface class state changed (active/idle)
         * Format: "NNN IfaceClass <active/idle> <label>"
         */
        if (cooked.GetLength() < 4 || !cooked[1].Equals("IfaceClass")) {
            //throw new IllegalStateException(String.format("Invalid event from daemon (%s)", raw));
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        Boolean isActive = cooked[2].Equals("active");
        mOwner->NotifyInterfaceClassActivity(cooked[3], isActive);
        *result = TRUE;
        return NOERROR;
        // break;
    }
    default: break;
    }
    *result = FALSE;
    return NOERROR;
}

const String CNetworkManagementService::LIMIT_GLOBAL_ALERT("globalAlert");
const String CNetworkManagementService::TAG("NetworkManagementService");
const Boolean CNetworkManagementService::DBG = FALSE;
const String CNetworkManagementService::NETD_TAG("NetdConnector");
const String CNetworkManagementService::ADD("add");
const String CNetworkManagementService::REMOVE("remove");
const String CNetworkManagementService::ALLOW("allow");
const String CNetworkManagementService::DENY("deny");
const String CNetworkManagementService::DEFAULT("default");
const String CNetworkManagementService::SECONDARY("secondary");

CNetworkManagementService::CNetworkManagementService()
    : mActiveQuotas(50)
    , mActiveAlerts(50)
    , mUidRejectOnQuota(20)
    , mActiveIdleTimers(20)
    , mBandwidthControlEnabled(FALSE)
    , mFirewallEnabled(FALSE)
{
    CHandler::New((IHandler**)&mMainHandler);
    CCountDownLatch::New(1, (ICountDownLatch**)&mConnectedSignal);
    CRemoteCallbackList::New((IRemoteCallbackList**)&mObservers);
    CNetworkStatsFactory::New((INetworkStatsFactory**)&mStatsFactory);
}

/**
 * Constructs a new NetworkManagementService instance
 *
 * @param context  Binder context for this service
 */
ECode CNetworkManagementService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String value;
    if (sysProp->Get(String("ro.product.device"), &value), String("simulator").Equals(value)) {
        return NOERROR;
    }

    mConnector = new NativeDaemonConnector(
            new NetdCallbackReceiver(this), String("netd"), 10, NETD_TAG, 160);
    CThread::New(mConnector, NETD_TAG, (IThread**)&mThread);

//     // Add ourself to the Watchdog monitors.
// //    Watchdog.getInstance().addMonitor(this);
    return NOERROR;
}

AutoPtr<CNetworkManagementService> CNetworkManagementService::Create(
    /* [in] */ IContext* context)
{
    AutoPtr<CNetworkManagementService> service;
    CNetworkManagementService::NewByFriend(context, (CNetworkManagementService**)&service);
    AutoPtr<ICountDownLatch> connectedSignal = service->mConnectedSignal;
    if (DBG) Slogger::D(TAG, "Creating NetworkManagementService");
    service->mThread->Start();
    if (DBG) Slogger::D(TAG, "Awaiting socket connection");
    connectedSignal->Await();
    if (DBG) Slogger::D(TAG, "Connected");
    return service;
}

void CNetworkManagementService::SystemReady()
{
    PrepareNativeDaemon();
    if (DBG) Slogger::D(TAG, "Prepared");
}

ECode CNetworkManagementService::RegisterObserver(
    /* [in] */ INetworkManagementEventObserver* observer)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    Boolean result;
    return mObservers->Register(observer, &result);
}

ECode CNetworkManagementService::UnregisterObserver(
    /* [in] */ INetworkManagementEventObserver* observer)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    Boolean result;
    return mObservers->Unregister(observer, &result);
}

/**
 * Notify our observers of an interface status change
 */
void CNetworkManagementService::NotifyInterfaceStatusChanged(
    /* [in] */ const String& iface,
    /* [in] */ Boolean up)
{
    Int32 length;
    mObservers->BeginBroadcast(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<INetworkManagementEventObserver> observer;
        mObservers->GetBroadcastItem(i, (IInterface**)&observer);
        observer->InterfaceStatusChanged(iface, up);
    }
    mObservers->FinishBroadcast();
}

/**
 * Notify our observers of an interface link state change
 * (typically, an Ethernet cable has been plugged-in or unplugged).
 */
void CNetworkManagementService::NotifyInterfaceLinkStateChanged(
    /* [in] */ const String& iface,
    /* [in] */ Boolean up)
{
    Int32 length;
    mObservers->BeginBroadcast(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<INetworkManagementEventObserver> observer;
        mObservers->GetBroadcastItem(i, (IInterface**)&observer);
        observer->InterfaceLinkStateChanged(iface, up);
    }
    mObservers->FinishBroadcast();
}

/**
 * Notify our observers of an interface addition.
 */
void CNetworkManagementService::NotifyInterfaceAdded(
    /* [in] */ const String& iface)
{
    Int32 length;
    mObservers->BeginBroadcast(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<INetworkManagementEventObserver> observer;
        mObservers->GetBroadcastItem(i, (IInterface**)&observer);
        observer->InterfaceAdded(iface);
    }
    mObservers->FinishBroadcast();
}

/**
 * Notify our observers of an interface removal.
 */
void CNetworkManagementService::NotifyInterfaceRemoved(
    /* [in] */ const String& iface)
{
    // netd already clears out quota and alerts for removed ifaces; update
    // our sanity-checking state.
    mActiveAlerts.Erase(iface);
    mActiveQuotas.Erase(iface);

    Int32 length;
    mObservers->BeginBroadcast(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<INetworkManagementEventObserver> observer;
        mObservers->GetBroadcastItem(i, (IInterface**)&observer);
        observer->InterfaceRemoved(iface);
    }
    mObservers->FinishBroadcast();
}

/**
 * Notify our observers of a limit reached.
 */
void CNetworkManagementService::NotifyLimitReached(
    /* [in] */ const String& limitName,
    /* [in] */ const String& iface)
{
    Int32 length;
    mObservers->BeginBroadcast(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<INetworkManagementEventObserver> observer;
        mObservers->GetBroadcastItem(i, (IInterface**)&observer);
        observer->LimitReached(limitName, iface);
    }
    mObservers->FinishBroadcast();
}

/**
 * Notify our observers of a change in the data activity state of the interface
 */
void CNetworkManagementService::NotifyInterfaceClassActivity(
    /* [in] */ const String& label,
    /* [in] */ Boolean active)
{
    Int32 length;
    mObservers->BeginBroadcast(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<INetworkManagementEventObserver> observer;
        mObservers->GetBroadcastItem(i, (IInterface**)&observer);
        observer->InterfaceClassDataActivityChanged(label, active);
    }
    mObservers->FinishBroadcast();
}

/**
 * Prepare native daemon once connected, enabling modules and pushing any
 * existing in-memory rules.
 */
void CNetworkManagementService::PrepareNativeDaemon()
{
    mBandwidthControlEnabled = FALSE;

    // only enable bandwidth control when support exists
    Boolean hasKernelSupport;
    AutoPtr<IFile> files;
    CFile::New(String("/proc/net/xt_qtaguid/ctrl"), (IFile**)&files);
    files->Exists(&hasKernelSupport);
    if (hasKernelSupport) {
        Slogger::D(TAG, "enabling bandwidth control");
        // try {
        AutoPtr<ICharSequence> cseq;
        CString::New(String("enable"), (ICharSequence**)&cseq);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, cseq.Get());
        AutoPtr<NativeDaemonEvent> event;
        mConnector->Execute(String("bandwidth"), args, (NativeDaemonEvent**)&event);
        mBandwidthControlEnabled = TRUE;
        // } catch (NativeDaemonConnectorException e) {
        //     Log.wtf(TAG, "problem enabling bandwidth controls", e);
        // }
    }
    else {
        Slogger::D(TAG, "not enabling bandwidth control");
    }

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    sysProp->Set(NetworkManagementSocketTagger::PROP_QTAGUID_ENABLED,
            mBandwidthControlEnabled ? String("1") : String("0"));

    // push any existing quota or UID rules
    {
        AutoLock lock(mQuotaLock);

        if (mActiveQuotas.Begin() != mActiveQuotas.End()) {
            Slogger::D(TAG, "pushing %d active quota rules", mActiveQuotas.GetSize());
            HashMap<String, Int64>::Iterator iter;
            for(iter = mActiveQuotas.Begin(); iter != mActiveQuotas.End(); ++iter) {
                SetInterfaceQuota(iter->mFirst, iter->mSecond);
            }
            mActiveQuotas.Clear();
        }

        if (mActiveAlerts.Begin() != mActiveAlerts.End()) {
            Slogger::D(TAG, "pushing %d active alert rules", mActiveAlerts.GetSize());
            HashMap<String, Int64>::Iterator iter;
            for(iter = mActiveAlerts.Begin(); iter != mActiveAlerts.End(); ++iter) {
                SetInterfaceAlert(iter->mFirst, iter->mSecond);
            }
            mActiveAlerts.Clear();
        }

        if (mUidRejectOnQuota.Begin() != mUidRejectOnQuota.End()) {
            Slogger::D(TAG, "pushing %d active uid rules", mUidRejectOnQuota.GetSize());
            HashMap<Int32, Boolean>::Iterator iter;
            for(iter = mUidRejectOnQuota.Begin(); iter != mUidRejectOnQuota.End(); ++iter) {
                SetUidNetworkRules(iter->mFirst, iter->mSecond);
            }
            mUidRejectOnQuota.Clear();
        }
    }

    // TODO: Push any existing firewall state
    SetFirewallEnabled(mFirewallEnabled || LockdownVpnTracker::IsEnabled());
}

// //
// // IINetworkManagementService members
// //

ECode CNetworkManagementService::ListInterfaces(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
     // try {
    AutoPtr<ICharSequence> cseq;
    CString::New(String("list"), (ICharSequence**)&cseq);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, cseq.Get());
    AutoPtr< ArrayOf<NativeDaemonEvent*> > events;
    mConnector->ExecuteForList(String("interface"), args, (ArrayOf<NativeDaemonEvent*>**)&events);
    AutoPtr< ArrayOf<String> > resArray = NativeDaemonEvent::FilterMessageList(
            *events, NetdResponseCode::InterfaceListResult);
    *result = resArray;
    REFCOUNT_ADD(*result);
     // } catch (NativeDaemonConnectorException e) {
     //     throw e.rethrowAsParcelableException();
     // }
     return NOERROR;
}

ECode CNetworkManagementService::GetInterfaceConfig(
    /* [in] */ const String& iface,
    /* [out] */ IInterfaceConfiguration** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    AutoPtr<NativeDaemonEvent> event;
     // try {
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("getcfg"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(iface, (ICharSequence**)&cseq1);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    FAIL_RETURN(mConnector->Execute(String("interface"), args, (NativeDaemonEvent**)&event));
     // } catch (NativeDaemonConnectorException e) {
     //     throw e.rethrowAsParcelableException();
     // }

    FAIL_RETURN(event->CheckCode(NetdResponseCode::InterfaceGetCfgResult));
     // Rsp: 213 xx:xx:xx:xx:xx:xx yyy.yyy.yyy.yyy zzz flag1 flag2 flag3

    AutoPtr<StringTokenizer> st = new StringTokenizer(event->GetMessage());

    AutoPtr<IInterfaceConfiguration> cfg;
    // try {
    CInterfaceConfiguration::New((IInterfaceConfiguration**)&cfg);
    String delim(" ");
    cfg->SetHardwareAddress(String(st->NextToken(delim)));
    AutoPtr<IInetAddress> addr;
    Int32 prefixLength = 0;
    // try {
    NetworkUtils::NumericToInetAddress(String(st->NextToken()), (IInetAddress**)&addr);
    // } catch (IllegalArgumentException iae) {
    //     Slog.e(TAG, "Failed to parse ipaddr", iae);
    // }

    // try {
    prefixLength = atoi(st->NextToken());
    // } catch (NumberFormatException nfe) {
    //     Slog.e(TAG, "Failed to parse prefixLength", nfe);
    // }

    AutoPtr<ILinkAddress> linkAddr;
    CLinkAddress::New(addr, prefixLength, (ILinkAddress**)&linkAddr);
    cfg->SetLinkAddress(linkAddr);
    while (st->HasMoreTokens()) {
        cfg->SetFlag(String(st->NextToken()));
    }
    // } catch (NoSuchElementException nsee) {
    //     throw new IllegalStateException("Invalid response from daemon: " + event);
    // }
    *result = cfg;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CNetworkManagementService::SetInterfaceConfig(
    /* [in] */ const String& iface,
    /* [in] */ IInterfaceConfiguration* cfg)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ILinkAddress> linkAddr;
    cfg->GetLinkAddress((ILinkAddress**)&linkAddr);
    AutoPtr<IInetAddress> inetAddr;
    if (linkAddr == NULL || (linkAddr->GetAddress((IInetAddress**)&inetAddr), inetAddr == NULL)) {
        //throw new IllegalStateException("NULL LinkAddress given");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    String hostAddr;
    inetAddr->GetHostAddress(&hostAddr);
    Int32 prefixLength;
    linkAddr->GetNetworkPrefixLength(&prefixLength);

    AutoPtr<ICharSequence> cseq0;
    CString::New(String("getcfg"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(iface, (ICharSequence**)&cseq1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(hostAddr, (ICharSequence**)&cseq2);
    AutoPtr<IInteger32> iint3;
    CInteger32::New(prefixLength, (IInteger32**)&iint3);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(4);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, cseq2.Get());
    args->Set(3, iint3.Get());
    AutoPtr<NativeDaemonConnector::Command> cmd = new NativeDaemonConnector::Command(
        String("interface"), args);

    AutoPtr< ArrayOf<String> > flags;
    cfg->GetFlags((ArrayOf<String>**)&flags);
    for (Int32 i = 0; i < flags->GetLength(); i++) {
        AutoPtr<ICharSequence> flag;
        CString::New((*flags)[i], (ICharSequence**)&flag);
        cmd->AppendArg(flag);
    }

    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(cmd, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::SetInterfaceDown(
    /* [in] */ const String& iface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<IInterfaceConfiguration> ifcg;
    FAIL_RETURN(GetInterfaceConfig(iface, (IInterfaceConfiguration**)&ifcg));
    ifcg->SetInterfaceDown();
    SetInterfaceConfig(iface, ifcg);
    return NOERROR;
}

ECode CNetworkManagementService::SetInterfaceUp(
    /* [in] */ const String& iface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<IInterfaceConfiguration> ifcg;
    FAIL_RETURN(GetInterfaceConfig(iface, (IInterfaceConfiguration**)&ifcg));
    ifcg->SetInterfaceUp();
    SetInterfaceConfig(iface, ifcg);
    return NOERROR;
}

ECode CNetworkManagementService::SetInterfaceIpv6PrivacyExtensions(
    /* [in] */ const String& iface,
    /* [in] */ Boolean enable)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("ipv6privacyextensions"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(iface, (ICharSequence**)&cseq1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(enable ? String("enable") : String("disable"), (ICharSequence**)&cseq2);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, cseq2.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("interface"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

// /* TODO: This is right now a IPv4 only function. Works for wifi which loses its
//    IPv6 addresses on interface down, but we need to do full clean up here */
ECode CNetworkManagementService::ClearInterfaceAddresses(
    /* [in] */ const String& iface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("clearaddrs"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(iface, (ICharSequence**)&cseq1);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("interface"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::EnableIpv6(
    /* [in] */ const String& iface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("ipv6"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(iface, (ICharSequence**)&cseq1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(String("enable"), (ICharSequence**)&cseq2);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, cseq2.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("interface"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::DisableIpv6(
    /* [in] */ const String& iface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("ipv6"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(iface, (ICharSequence**)&cseq1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(String("disable"), (ICharSequence**)&cseq2);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, cseq2.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("interface"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::AddRoute(
    /* [in] */ const String& interfaceName,
    /* [in] */ IRouteInfo* route)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    return ModifyRoute(interfaceName, ADD, route, DEFAULT);
}

ECode CNetworkManagementService::RemoveRoute(
    /* [in] */ const String& interfaceName,
    /* [in] */ IRouteInfo* route)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    return ModifyRoute(interfaceName, REMOVE, route, DEFAULT);
}

ECode CNetworkManagementService::AddSecondaryRoute(
    /* [in] */ const String& interfaceName,
    /* [in] */ IRouteInfo* route)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    return ModifyRoute(interfaceName, ADD, route, SECONDARY);
}

ECode CNetworkManagementService::RemoveSecondaryRoute(
    /* [in] */ const String& interfaceName,
    /* [in] */ IRouteInfo* route)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    return ModifyRoute(interfaceName, REMOVE, route, SECONDARY);
}

ECode CNetworkManagementService::ModifyRoute(
    /* [in] */ const String& interfaceName,
    /* [in] */ const String& action,
    /* [in] */ IRouteInfo* route,
    /* [in] */ const String& type)
{
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("route"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(action, (ICharSequence**)&cseq1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(interfaceName, (ICharSequence**)&cseq2);
    AutoPtr<ICharSequence> cseq3;
    CString::New(type, (ICharSequence**)&cseq3);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(4);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, cseq2.Get());
    args->Set(3, cseq3.Get());
    AutoPtr<NativeDaemonConnector::Command> cmd = new NativeDaemonConnector::Command(
        String("interface"), args);

    // create triplet: dest-ip-addr prefixlength gateway-ip-addr
    AutoPtr<ILinkAddress> la;
    route->GetDestination((ILinkAddress**)&la);
    AutoPtr<IInetAddress> netAddr;
    la->GetAddress((IInetAddress**)&netAddr);

    String str;
    netAddr->GetHostAddress(&str);
    AutoPtr<ICharSequence> istr;
    CString::New(str, (ICharSequence**)&istr);
    cmd->AppendArg(istr);

    Int32 num;
    la->GetNetworkPrefixLength(&num);
    AutoPtr<IInteger32> inum;
    CInteger32::New(num, (IInteger32**)&inum);
    cmd->AppendArg(inum);

    AutoPtr<IInetAddress> gateway;
    route->GetGateway((IInetAddress**)&gateway);
    if (gateway == NULL) {
        if (IInet4Address::Probe(netAddr) != NULL) {
            AutoPtr<ICharSequence> istr0;
            CString::New(String("0.0.0.0"), (ICharSequence**)&istr0);
            cmd->AppendArg(istr0);
        }
        else {
            AutoPtr<ICharSequence> istr0;
            CString::New(String("::0"), (ICharSequence**)&istr0);
            cmd->AppendArg(istr0);
        }
    }
    else {
        gateway->GetHostAddress(&str);
        AutoPtr<ICharSequence> istr0;
        CString::New(str, (ICharSequence**)&istr0);
        cmd->AppendArg(istr0);
    }
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(cmd, (NativeDaemonEvent**)&event);
    return NOERROR;
}

ECode CNetworkManagementService::ReadRouteList(
    /* [in] */ const String& filename,
    /* [out, callee] */ List<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IFileInputStream> fstream;
    CFileInputStream::New(filename, (IFileInputStream**)&fstream);

    AutoPtr< List<String> > strList = new List<String>();
    AutoPtr<IDataInputStream> in;
    CDataInputStream::New(fstream, (IDataInputStream**)&in);
    AutoPtr<IBufferedReader> br;
    AutoPtr<IInputStreamReader> isReader;
    CInputStreamReader::New(in, (IInputStreamReader**)&isReader);
    CBufferedReader::New(isReader, (IBufferedReader**)&br);
    String s;

    // throw away the title line
    br->ReadLine(&s);
    while (!s.IsNullOrEmpty()) {
        strList->PushBack(s);
        br->ReadLine(&s);
    }
    *result = strList;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CNetworkManagementService::GetRoutes(
    /* [in] */ const String& interfaceName,
    /* [out, callee] */ ArrayOf<IRouteInfo*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    List<AutoPtr<IRouteInfo> > routes;

    // v4 routes listed as:
    // iface dest-addr gateway-addr flags refcnt use metric netmask mtu window IRTT
    AutoPtr< List<String> > strList;
    ReadRouteList(String("/proc/net/route"), (List<String>**)&strList);
    List<String>::Iterator iter;
    for (iter = strList->Begin(); iter != strList->End(); ++iter) {
        AutoPtr<ArrayOf<String> > fields;
        StringUtils::Split(*iter, String("\t"), (ArrayOf<String>**)&fields);

        if (fields->GetLength() > 7) {
            String iface = (*fields)[0];

            if (interfaceName.Equals(iface)) {
                String dest = (*fields)[1];
                String gate = (*fields)[2];
                String flags = (*fields)[3]; // future use?
                String mask = (*fields)[7];
//                try {
                    // address stored as a hex string, ex: 0014A8C0
                AutoPtr<IInetAddress> destAddr;
                NetworkUtils::Int32ToInetAddress((Int32)StringUtils::ParseInt64(dest, 16), (IInetAddress**)&destAddr);
                Int32 prefixLength = NetworkUtils::NetmaskIntToPrefixLength((Int32)StringUtils::ParseInt64(mask, 16));
                AutoPtr<ILinkAddress> linkAddress;
                CLinkAddress::New(destAddr, prefixLength, (ILinkAddress**)&linkAddress);

                // address stored as a hex string, ex 0014A8C0
                AutoPtr<IInetAddress> gatewayAddr;
                NetworkUtils::Int32ToInetAddress((Int32)StringUtils::ParseInt64(gate, 16), (IInetAddress**)&gatewayAddr);

                AutoPtr<IRouteInfo> route;
                CRouteInfo::New(linkAddress, gatewayAddr, (IRouteInfo**)&route);
                routes.PushBack(route);
//                } catch (Exception e) {
//                    Logger::E(TAG, "Error parsing route " + s + " : " + e);
//                    continue;
//                }
            }
        }
    }

    strList = NULL;

    // v6 routes listed as:
    // dest-addr prefixlength ?? ?? gateway-addr ?? ?? ?? ?? iface
    ReadRouteList(String("/proc/net/ipv6_route"), (List<String>**)&strList);
    for (iter = strList->Begin(); iter != strList->End(); ++iter) {
        AutoPtr<ArrayOf<String> > fields;
        StringUtils::Split(*iter, String("\\s+"), (ArrayOf<String>**)&fields);

        if (fields->GetLength() > 9) {
            String iface = (*fields)[9].Trim();
            if (interfaceName.Equals(iface)) {
                String dest = (*fields)[0];
                String prefix = (*fields)[1];
                String gate = (*fields)[4];

//                try {
                    // prefix length stored as a hex string, ex 40
                Int32 prefixLength = StringUtils::ParseInt32(prefix, 16);

                // address stored as a 32 char hex string
                // ex fe800000000000000000000000000000

                AutoPtr<IInetAddress> destAddr;
                NetworkUtils::HexToInet6Address(dest, (IInetAddress**)&destAddr);

                AutoPtr<ILinkAddress> linkAddress;
                CLinkAddress::New(destAddr, prefixLength, (ILinkAddress**)&linkAddress);

                AutoPtr<IInetAddress> gateAddr;
                NetworkUtils::HexToInet6Address(gate, (IInetAddress**)&gateAddr);

                AutoPtr<IRouteInfo> route;
                CRouteInfo::New(linkAddress, gateAddr, (IRouteInfo**)&route);
                routes.PushBack(route);
//                } catch (Exception e) {
//                    Logger::E(TAG, "Error parsing route " + s + " : " + e);
//                    continue;
//                }
            }
        }
    }

    List<AutoPtr<IRouteInfo> >::Iterator iter2;
    AutoPtr< ArrayOf<IRouteInfo*> > resArray = ArrayOf<IRouteInfo*>::Alloc(routes.GetSize());
    iter2 = routes.Begin();
    for (Int32 i = 0; iter2 != routes.End(); ++iter2) {
        resArray->Set(i++, *iter2);
    }
    *result = resArray;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CNetworkManagementService::Shutdown()
{
    // TODO: remove from aidl if nobody calls externally
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::SHUTDOWN/*SHUTDOWN*/, TAG));
     Slogger::D(TAG, "Shutting down");
     return NOERROR;
}

ECode CNetworkManagementService::GetIpForwardingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    AutoPtr<ICharSequence> cseq0;
    CString::New(String("status"), (ICharSequence**)&cseq0);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, cseq0.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("ipfwd"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }

     // 211 Forwarding enabled
     FAIL_RETURN(event->CheckCode(NetdResponseCode::IpFwdStatusResult));
     *result = event->GetMessage().EndWith("enabled");
     return NOERROR;
}

ECode CNetworkManagementService::SetIpForwardingEnabled(
    /* [in] */ Boolean enable)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(enable ? String("enable") : String("disable"), (ICharSequence**)&cseq0);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, cseq0.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("ipfwd"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::StartTethering(
    /* [in] */ ArrayOf<String>* dhcpRange)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
     // cmd is "tether start first_start first_stop second_start second_stop ..."
     // an odd number of addrs will fail

    AutoPtr<ICharSequence> cseq0;
    CString::New(String("start"), (ICharSequence**)&cseq0);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, cseq0.Get());
    AutoPtr<NativeDaemonConnector::Command> cmd = new NativeDaemonConnector::Command(
        String("tether"), args);
    for (Int32 i = 0; i < dhcpRange->GetLength(); i++) {
        AutoPtr<ICharSequence> str;
        CString::New((*dhcpRange)[i], (ICharSequence**)&str);
        cmd->AppendArg(str);
    }

    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(cmd, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::StopTethering()
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("stop"), (ICharSequence**)&cseq0);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, cseq0.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("tether"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::IsTetheringStarted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    AutoPtr<ICharSequence> cseq0;
    CString::New(String("status"), (ICharSequence**)&cseq0);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, cseq0.Get());

    AutoPtr<NativeDaemonEvent> event;
    // try {
    mConnector->Execute(String("tether"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }

    // 210 Tethering services started
    FAIL_RETURN(event->CheckCode(NetdResponseCode::TetherStatusResult));
    *result = event->GetMessage().EndWith("started");
    return NOERROR;
}

// TODO(BT) Remove
ECode CNetworkManagementService::StartReverseTethering(
    /* [in] */ const String& iface)
{
    if (DBG) Slogger::D(TAG, "startReverseTethering in");
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    // cmd is "tether start first_start first_stop second_start second_stop ..."
    // an odd number of addrs will fail
    StringBuffer cmd("tether start-reverse");
    cmd += String(" ");
    cmd += iface;
//    if (DBG) Slogger::D(TAG, "startReverseTethering cmd: " + cmd);
//    try {
    String strcmd = cmd.ToString();
    List<String> responses;
    FAIL_RETURN(mConnector->DoCommand(strcmd, responses));
//    } catch (NativeDaemonConnectorException e) {
//        throw new IllegalStateException("Unable to communicate to native daemon");
//    }
//     BluetoothTetheringDataTracker.getInstance().startReverseTether(iface);
//     return NOERROR;
    assert(0);
    return E_NOT_IMPLEMENTED;
}

// TODO(BT) Remove
ECode CNetworkManagementService::StopReverseTethering()
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
//    try {
    String cmd("tether stop-reverse");
    List<String> responses;
    FAIL_RETURN(mConnector->DoCommand(cmd, responses));
//    } catch (NativeDaemonConnectorException e) {
//        throw new IllegalStateException("Unable to communicate to native daemon to stop tether");
//    }
//     BluetoothTetheringDataTracker.getInstance().stopReverseTether();
//     return NOERROR;
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CNetworkManagementService::TetherInterface(
    /* [in] */ const String& iface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("interface"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(String("add"), (ICharSequence**)&cseq1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(iface, (ICharSequence**)&cseq2);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, cseq2.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("tether"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::UntetherInterface(
    /* [in] */ const String& iface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("interface"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(String("remove"), (ICharSequence**)&cseq1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(iface, (ICharSequence**)&cseq2);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, cseq2.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("tether"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::ListTetheredInterfaces(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("interface"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(String("list"), (ICharSequence**)&cseq1);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    // try {
    AutoPtr<ArrayOf<NativeDaemonEvent*> > events;
    mConnector->ExecuteForList(String("tether"), args, (ArrayOf<NativeDaemonEvent*>**)&events);
    AutoPtr< ArrayOf<String> > resArray = NativeDaemonEvent::FilterMessageList(*events, NetdResponseCode::TetherInterfaceListResult);
    *result = resArray;
    REFCOUNT_ADD(*result);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::SetDnsForwarders(
    /* [in] */ ArrayOf<String>* dns)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    AutoPtr<ICharSequence> cseq0;
    CString::New(String("dns"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(String("set"), (ICharSequence**)&cseq1);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    AutoPtr<NativeDaemonConnector::Command> cmd = new NativeDaemonConnector::Command(
            String("tether"), args);
    for (Int32 i = 0; i < dns->GetLength(); i++) {
        AutoPtr<IInetAddress> inetAddr;
        NetworkUtils::NumericToInetAddress((*dns)[i], (IInetAddress**)&inetAddr);
        String str;
        inetAddr->GetHostAddress(&str);
        AutoPtr<ICharSequence> cseq;
        CString::New(str, (ICharSequence**)&cseq);
        cmd->AppendArg(cseq);
    }
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(cmd, (NativeDaemonEvent**)&event);
    return NOERROR;
}

ECode CNetworkManagementService::GetDnsForwarders(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("dns"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(String("list"), (ICharSequence**)&cseq1);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    // try {
    AutoPtr<ArrayOf<NativeDaemonEvent*> > events;
    mConnector->ExecuteForList(String("tether"), args, (ArrayOf<NativeDaemonEvent*>**)&events);
    AutoPtr< ArrayOf<String> > resArray = NativeDaemonEvent::FilterMessageList(*events, NetdResponseCode::TetherDnsFwdTgtListResult);
    *result = resArray;
    REFCOUNT_ADD(*result);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::ModifyNat(
    /* [in] */ const String& action,
    /* [in] */ const String& internalInterface,
    /* [in] */ const String& externalInterface)
{
    AutoPtr<ICharSequence> cseq0;
    CString::New(action, (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(internalInterface, (ICharSequence**)&cseq1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(externalInterface, (ICharSequence**)&cseq2);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, cseq2.Get());
    AutoPtr<NativeDaemonConnector::Command> cmd = new NativeDaemonConnector::Command(
        String("nat"), args);

     AutoPtr<INetworkInterface> internalNetworkInterface;
     AutoPtr<INetworkInterfaceHelper> helper;
     CNetworkInterfaceHelper::AcquireSingleton((INetworkInterfaceHelper**)&helper);
     helper->GetByName(internalInterface, (INetworkInterface**)&internalNetworkInterface);

     if (internalNetworkInterface == NULL) {
        AutoPtr<ICharSequence> cseq;
        CString::New(String("0"), (ICharSequence**)&cseq);
        cmd->AppendArg(cseq);
     } else {
        AutoPtr<IList> interfaceAddresses;
        internalNetworkInterface->GetInterfaceAddresses((IList**)&interfaceAddresses);
        AutoPtr<IIterator> emu;
        FAIL_RETURN(interfaceAddresses->GetIterator((IIterator**)&emu));
        Boolean hasNext;
        while (emu->HasNext(&hasNext), hasNext ) {
            AutoPtr<IInterfaceAddress> ia;
            emu->Next((IInterface**)&ia);
            AutoPtr<IInetAddress> addrIn;
            AutoPtr<IInetAddress> addrOut;
            Int16 prefixLen;
            ia->GetAddress((IInetAddress**)&addrIn);
            ia->GetNetworkPrefixLength(&prefixLen);
            NetworkUtils::GetNetworkPart(addrIn, (Int32)prefixLen, (IInetAddress**)&addrOut);
            String str;
            addrOut->GetHostAddress(&str);
            AutoPtr<ICharSequence> c0;
            CString::New(str, (ICharSequence**)&c0);
            AutoPtr<ICharSequence> c1;
            CString::New(String("/"), (ICharSequence**)&c1);
            AutoPtr<IInteger16> i2;
            CInteger16::New(prefixLen, (IInteger16**)&i2);
            cmd->AppendArg(c0);
            cmd->AppendArg(c1);
            cmd->AppendArg(i2);
        }
    }
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(cmd, (NativeDaemonEvent**)&event);
    return NOERROR;
}

ECode CNetworkManagementService::EnableNat(
    /* [in] */ const String& internalInterface,
    /* [in] */ const String& externalInterface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    return ModifyNat(String("enable"), internalInterface, externalInterface);
}

ECode CNetworkManagementService::DisableNat(
    /* [in] */ const String& internalInterface,
    /* [in] */ const String& externalInterface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    return ModifyNat(String("disable"), internalInterface, externalInterface);
}

ECode CNetworkManagementService::ListTtys(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    AutoPtr<NativeDaemonConnector::Command> cmd = new NativeDaemonConnector::Command(
        String("list_ttys"), NULL);
    AutoPtr<ArrayOf<NativeDaemonEvent*> > events;
    mConnector->ExecuteForList(cmd, (ArrayOf<NativeDaemonEvent*>**)&events);
    AutoPtr< ArrayOf<String> > resArray = NativeDaemonEvent::FilterMessageList(*events, NetdResponseCode::TtyListResult);
    *result = resArray;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CNetworkManagementService::AttachPppd(
    /* [in] */ const String& tty,
    /* [in] */ const String& localAddr,
    /* [in] */ const String& remoteAddr,
    /* [in] */ const String& dns1Addr,
    /* [in] */ const String& dns2Addr)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    AutoPtr<IInetAddress> inetAddr;
    String str1, str2, str3, str4;
    NetworkUtils::NumericToInetAddress(localAddr, (IInetAddress**)&inetAddr);
    inetAddr->GetHostAddress(&str1);
    inetAddr = NULL;
    NetworkUtils::NumericToInetAddress(remoteAddr, (IInetAddress**)&inetAddr);
    inetAddr->GetHostAddress(&str2);
    inetAddr = NULL;
    NetworkUtils::NumericToInetAddress(dns1Addr, (IInetAddress**)&inetAddr);
    inetAddr->GetHostAddress(&str3);
    inetAddr = NULL;
    NetworkUtils::NumericToInetAddress(dns2Addr, (IInetAddress**)&inetAddr);
    inetAddr->GetHostAddress(&str4);

    AutoPtr<ICharSequence> cseq0;
    CString::New(String("attach"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(tty, (ICharSequence**)&cseq1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(str1, (ICharSequence**)&cseq2);
    AutoPtr<ICharSequence> cseq3;
    CString::New(str2, (ICharSequence**)&cseq3);
    AutoPtr<ICharSequence> cseq4;
    CString::New(str3, (ICharSequence**)&cseq4);
    AutoPtr<ICharSequence> cseq5;
    CString::New(str4, (ICharSequence**)&cseq5);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(6);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, cseq2.Get());
    args->Set(3, cseq3.Get());
    args->Set(4, cseq4.Get());
    args->Set(5, cseq5.Get());
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("pppd"), args, (NativeDaemonEvent**)&event);
    return NOERROR;
}

ECode CNetworkManagementService::DetachPppd(
    /* [in] */ const String& tty)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("detach"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(tty, (ICharSequence**)&cseq1);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("pppd"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::StartAccessPoint(
    /* [in] */ IWifiConfiguration* wifiConfig,
    /* [in] */ const String& wlanIface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    // try {
    WifiFirmwareReload(wlanIface, String("AP"));
    StringBuffer cmd("softap start ");
    cmd += wlanIface;
    String strcmd = cmd.ToString();
    List<String> responses;
    FAIL_RETURN(mConnector->DoCommand(strcmd, responses));
    if (wifiConfig == NULL) {
        AutoPtr<ICharSequence> cseq0;
        CString::New(String("set"), (ICharSequence**)&cseq0);
        AutoPtr<ICharSequence> cseq1;
        CString::New(wlanIface, (ICharSequence**)&cseq1);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
        args->Set(0, cseq0.Get());
        args->Set(1, cseq1.Get());
        AutoPtr<NativeDaemonEvent> event;
        mConnector->Execute(String("softap"), args, (NativeDaemonEvent**)&event);
    } else {
        String strSSID;
        wifiConfig->GetSSID(&strSSID);
        String strPreShareKey;
        wifiConfig->GetPreSharedKey(&strPreShareKey);
        AutoPtr<ICharSequence> cseq0;
        CString::New(String("set"), (ICharSequence**)&cseq0);
        AutoPtr<ICharSequence> cseq1;
        CString::New(wlanIface, (ICharSequence**)&cseq1);
        AutoPtr<ICharSequence> cseq2;
        CString::New(strSSID, (ICharSequence**)&cseq2);
        AutoPtr<ICharSequence> cseq3;
        CString::New(GetSecurityType(wifiConfig), (ICharSequence**)&cseq3);
        AutoPtr<ICharSequence> cseq4;
        CString::New(strPreShareKey, (ICharSequence**)&cseq4);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(5);
        args->Set(0, cseq0.Get());
        args->Set(1, cseq1.Get());
        args->Set(2, cseq2.Get());
        args->Set(3, cseq3.Get());
        args->Set(4, cseq4.Get());
        AutoPtr<NativeDaemonEvent> event;
        mConnector->Execute(String("softap"), args, (NativeDaemonEvent**)&event);
    }
    AutoPtr<ICharSequence> cseq;
    CString::New(String("startap"), (ICharSequence**)&cseq);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, cseq.Get());
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("softap"), args, (NativeDaemonEvent**)&event);
    return NOERROR;
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
}

String CNetworkManagementService::GetSecurityType(
    /* [in] */ IWifiConfiguration* wifiConfig)
{
    Int32 num;
    wifiConfig->GetAuthType(&num);
    switch (num) {
        case IWifiConfigurationKeyMgmt::WPA_PSK:
        return String("wpa-psk");
        case IWifiConfigurationKeyMgmt::WPA2_PSK:
        return String("wpa2-psk");
        default:
        return String("open");
    }
}

// /* @param mode can be "AP", "STA" or "P2P" */
ECode CNetworkManagementService::WifiFirmwareReload(
    /* [in] */ const String& wlanIface,
    /* [in] */ const String& mode)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("fwreload"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(wlanIface, (ICharSequence**)&cseq1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(mode, (ICharSequence**)&cseq2);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, cseq2.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("softap"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::StopAccessPoint(
    /* [in] */ const String& wlanIface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("stopap"), (ICharSequence**)&cseq0);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, cseq0.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("softap"), args, (NativeDaemonEvent**)&event);
    WifiFirmwareReload(wlanIface, String("STA"));
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::SetAccessPoint(
    /* [in] */ IWifiConfiguration* wifiConfig,
    /* [in] */ const String& wlanIface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

     if (wifiConfig == NULL) {
        AutoPtr<ICharSequence> cseq0;
        CString::New(String("set"), (ICharSequence**)&cseq0);
        AutoPtr<ICharSequence> cseq1;
        CString::New(wlanIface, (ICharSequence**)&cseq1);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
        args->Set(0, cseq0.Get());
        args->Set(1, cseq1.Get());
        AutoPtr<NativeDaemonEvent> event;
        mConnector->Execute(String("softap"), args, (NativeDaemonEvent**)&event);
     } else {
        String ssid;
        wifiConfig->GetSSID(&ssid);
        String presharedKey;
        wifiConfig->GetPreSharedKey(&presharedKey);

        AutoPtr<ICharSequence> cseq0;
        CString::New(String("set"), (ICharSequence**)&cseq0);
        AutoPtr<ICharSequence> cseq1;
        CString::New(wlanIface, (ICharSequence**)&cseq1);
        AutoPtr<ICharSequence> cseq2;
        CString::New(ssid, (ICharSequence**)&cseq2);
        AutoPtr<ICharSequence> cseq3;
        CString::New(GetSecurityType(wifiConfig), (ICharSequence**)&cseq3);
        AutoPtr<ICharSequence> cseq4;
        CString::New(presharedKey, (ICharSequence**)&cseq4);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(5);
        args->Set(0, cseq0.Get());
        args->Set(1, cseq1.Get());
        args->Set(2, cseq2.Get());
        args->Set(3, cseq3.Get());
        args->Set(4, cseq4.Get());
        AutoPtr<NativeDaemonEvent> event;
        mConnector->Execute(String("softap"), args, (NativeDaemonEvent**)&event);
     }
    return NOERROR;
}

ECode CNetworkManagementService::AddIdleTimer(
    /* [in] */ const String& iface,
    /* [in] */ Int32 timeout,
    /* [in] */ const String& label)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    if (DBG) Slogger::D(TAG, "Adding idletimer");

    {
        AutoLock lock(mIdleTimerLock);
        AutoPtr<IdleTimerParams> params = mActiveIdleTimers[iface];
        if (params != NULL) {
            // the interface already has idletimer, update network count
            params->mNetworkCount++;
            return NOERROR;
        }
        AutoPtr<ICharSequence> cseq0;
        CString::New(String("add"), (ICharSequence**)&cseq0);
        AutoPtr<ICharSequence> cseq1;
        CString::New(iface, (ICharSequence**)&cseq1);
        AutoPtr<ICharSequence> cseq2;
        CString::New(StringUtils::Int32ToString(timeout), (ICharSequence**)&cseq2);
        AutoPtr<ICharSequence> cseq3;
        CString::New(label, (ICharSequence**)&cseq3);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(4);
        args->Set(0, cseq0.Get());
        args->Set(1, cseq1.Get());
        args->Set(2, cseq2.Get());
        args->Set(3, cseq3.Get());
        AutoPtr<NativeDaemonEvent> event;
        mConnector->Execute(String("idletimer"), args, (NativeDaemonEvent**)&event);
        AutoPtr<IdleTimerParams> timer = new IdleTimerParams(timeout, label);
        mActiveIdleTimers[iface] = timer;
    }

    return NOERROR;
}

ECode CNetworkManagementService::RemoveIdleTimer(
    /* [in] */ const String& iface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    if (DBG) Slogger::D(TAG, "Removing idletimer");

    {
        AutoLock lock(mIdleTimerLock);
        AutoPtr<IdleTimerParams> params = mActiveIdleTimers[iface];
        if (params == NULL || --(params->mNetworkCount) > 0) {
            return NOERROR;
        }
        AutoPtr<ICharSequence> cseq0;
        CString::New(String("remove"), (ICharSequence**)&cseq0);
        AutoPtr<ICharSequence> cseq1;
        CString::New(iface, (ICharSequence**)&cseq1);
        AutoPtr<ICharSequence> cseq2;
        CString::New(StringUtils::Int32ToString(params->mTimeout), (ICharSequence**)&cseq2);
        AutoPtr<ICharSequence> cseq3;
        CString::New(params->mLabel, (ICharSequence**)&cseq3);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(4);
        args->Set(0, cseq0.Get());
        args->Set(1, cseq1.Get());
        args->Set(2, cseq2.Get());
        args->Set(3, cseq3.Get());
        AutoPtr<NativeDaemonEvent> event;
        mConnector->Execute(String("idletimer"), args, (NativeDaemonEvent**)&event);
        mActiveIdleTimers.Erase(iface);
    }
    return NOERROR;
}

ECode CNetworkManagementService::GetNetworkStatsSummaryDev(
    /* [out] */ INetworkStats** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    return mStatsFactory->ReadNetworkStatsSummaryDev(result);
}

ECode CNetworkManagementService::GetNetworkStatsSummaryXt(
    /* [out] */ INetworkStats** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    return mStatsFactory->ReadNetworkStatsSummaryXt(result);
}

ECode CNetworkManagementService::GetNetworkStatsDetail(
    /* [out] */ INetworkStats** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    return mStatsFactory->ReadNetworkStatsDetail(INetworkStats::UID_ALL, result);
}

ECode CNetworkManagementService::SetInterfaceQuota(
    /* [in] */ const String& iface,
    /* [in] */ Int64 quotaBytes)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    // silently discard when control disabled
    // TODO: eventually migrate to be always enabled
    if (!mBandwidthControlEnabled)
        return NOERROR;

    {
        AutoLock lock(mQuotaLock);
        HashMap<String, Int64>::Iterator iter = mActiveQuotas.Find(iface);
        if (iter != mActiveQuotas.End()) {
            //throw new IllegalStateException("iface " + iface + " already has quota");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        // TODO: support quota shared across interfaces
        AutoPtr<ICharSequence> cseq0;
        CString::New(String("setiquota"), (ICharSequence**)&cseq0);
        AutoPtr<ICharSequence> cseq1;
        CString::New(iface, (ICharSequence**)&cseq1);
        AutoPtr<IInteger64> iint2;
        CInteger64::New(quotaBytes, (IInteger64**)&iint2);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
        args->Set(0, cseq0.Get());
        args->Set(1, cseq1.Get());
        args->Set(2, iint2.Get());
        AutoPtr<NativeDaemonEvent> event;
        mConnector->Execute(String("bandwidth"), args, (NativeDaemonEvent**)&event);
        mActiveQuotas[iface] = quotaBytes;
    }
    return NOERROR;
}

ECode CNetworkManagementService::RemoveInterfaceQuota(
    /* [in] */ const String& iface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    // silently discard when control disabled
    // TODO: eventually migrate to be always enabled
    if (!mBandwidthControlEnabled)
        return NOERROR;

    {
        AutoLock lock(mQuotaLock);
        HashMap<String, Int64>::Iterator iter = mActiveQuotas.Find(iface);
        if (iter != mActiveQuotas.End()) {
            // TODO: eventually consider throwing
            return NOERROR;
        }
        mActiveQuotas.Erase(iface);
        mActiveAlerts.Erase(iface);
        // TODO: support quota shared across interfaces
        AutoPtr<ICharSequence> cseq0;
        CString::New(String("removeiquota"), (ICharSequence**)&cseq0);
        AutoPtr<ICharSequence> cseq1;
        CString::New(iface, (ICharSequence**)&cseq1);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
        args->Set(0, cseq0.Get());
        args->Set(1, cseq1.Get());
        AutoPtr<NativeDaemonEvent> event;
        mConnector->Execute(String("bandwidth"), args, (NativeDaemonEvent**)&event);
    }
    return NOERROR;
}

ECode CNetworkManagementService::SetInterfaceAlert(
    /* [in] */ const String& iface,
    /* [in] */ Int64 alertBytes)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    // silently discard when control disabled
    // TODO: eventually migrate to be always enabled
    if (!mBandwidthControlEnabled)
        return NOERROR;

    // quick sanity check
    HashMap<String, Int64>::Iterator iter = mActiveQuotas.Find(iface);
    if (iter != mActiveQuotas.End()) {
        //throw new IllegalStateException("setting alert requires existing quota on iface");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    {
        AutoLock lock(mQuotaLock);
        HashMap<String, Int64>::Iterator iter2 = mActiveAlerts.Find(iface);
        if (iter2 != mActiveAlerts.End()) {
            //throw new IllegalStateException("iface " + iface + " already has alert");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
         // TODO: support alert shared across interfaces
        AutoPtr<ICharSequence> cseq0;
        CString::New(String("setinterfacealert"), (ICharSequence**)&cseq0);
        AutoPtr<ICharSequence> cseq1;
        CString::New(iface, (ICharSequence**)&cseq1);
        AutoPtr<IInteger64> iint2;
        CInteger64::New(alertBytes, (IInteger64**)&iint2);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
        args->Set(0, cseq0.Get());
        args->Set(1, cseq1.Get());
        args->Set(2, iint2.Get());
        AutoPtr<NativeDaemonEvent> event;
        mConnector->Execute(String("bandwidth"), args, (NativeDaemonEvent**)&event);
        mActiveAlerts[iface] = alertBytes;
    }
    return NOERROR;
}

ECode CNetworkManagementService::RemoveInterfaceAlert(
    /* [in] */ const String& iface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    // silently discard when control disabled
    // TODO: eventually migrate to be always enabled
    if (!mBandwidthControlEnabled)
        return NOERROR;

    {
        AutoLock lock(mQuotaLock);
        HashMap<String, Int64>::Iterator iter = mActiveAlerts.Find(iface);
        if (iter != mActiveAlerts.End()) {
            // TODO: eventually consider throwing
            return NOERROR;
        }

        // TODO: support alert shared across interfaces
        AutoPtr<ICharSequence> cseq0;
        CString::New(String("removeinterfacealert"), (ICharSequence**)&cseq0);
        AutoPtr<ICharSequence> cseq1;
        CString::New(iface, (ICharSequence**)&cseq1);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
        args->Set(0, cseq0.Get());
        args->Set(1, cseq1.Get());
        AutoPtr<NativeDaemonEvent> event;
        mConnector->Execute(String("bandwidth"), args, (NativeDaemonEvent**)&event);
        mActiveAlerts.Erase(iface);
    }
    return NOERROR;
}

ECode CNetworkManagementService::SetGlobalAlert(
    /* [in] */ Int64 alertBytes)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    // silently discard when control disabled
    // TODO: eventually migrate to be always enabled
    if (!mBandwidthControlEnabled)
        return NOERROR;

    AutoPtr<ICharSequence> cseq0;
    CString::New(String("setglobalalert"), (ICharSequence**)&cseq0);
    AutoPtr<IInteger64> iint1;
    CInteger64::New(alertBytes, (IInteger64**)&iint1);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, cseq0.Get());
    args->Set(1, iint1.Get());
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("bandwidth"), args, (NativeDaemonEvent**)&event);
    return NOERROR;
}

ECode CNetworkManagementService::SetUidNetworkRules(
    /* [in] */ Int32 uid,
    /* [in] */ Boolean rejectOnQuotaInterfaces)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    // silently discard when control disabled
    // TODO: eventually migrate to be always enabled
    if (!mBandwidthControlEnabled)
        return NOERROR;

    {
        AutoLock lock(mQuotaLock);
        Boolean oldRejectOnQuota = mUidRejectOnQuota[uid];
        if (oldRejectOnQuota == rejectOnQuotaInterfaces) {
            // TODO: eventually consider throwing
            return NOERROR;
        }

        AutoPtr<ICharSequence> cseq0;
        CString::New(String("setglobalalert"), (ICharSequence**)&cseq0);
        AutoPtr<ICharSequence> cseq1;
        CString::New(
                rejectOnQuotaInterfaces ? String("addnaughtyapps") : String("removenaughtyapps"), (ICharSequence**)&cseq1);
        AutoPtr<IInteger32> iint2;
        CInteger32::New(uid, (IInteger32**)&iint2);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
        args->Set(0, cseq0.Get());
        args->Set(1, cseq1.Get());
        args->Set(2, iint2.Get());
        AutoPtr<NativeDaemonEvent> event;
        mConnector->Execute(String("bandwidth"), args, (NativeDaemonEvent**)&event);

        if (rejectOnQuotaInterfaces) {
            mUidRejectOnQuota[uid] = TRUE;
        } else {
            mUidRejectOnQuota.Erase(uid);
        }
    }
    return NOERROR;
}

ECode CNetworkManagementService::IsBandwidthControlEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    *result = mBandwidthControlEnabled;
    return NOERROR;
}

ECode CNetworkManagementService::GetNetworkStatsUidDetail(
    /* [in] */ Int32 uid,
    /* [out] */ INetworkStats** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    return mStatsFactory->ReadNetworkStatsDetail(uid, result);
}

ECode CNetworkManagementService::GetNetworkStatsTethering(
    /* [in] */ ArrayOf<String>* ifacePairs,
    /* [out] */ INetworkStats** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    if (ifacePairs->GetLength() % 2 != 0) {
        //throw new IllegalArgumentException("unexpected ifacePairs; length=" + ifacePairs.length);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<INetworkStats> stats;
    CNetworkStats::New(SystemClock::GetElapsedRealtime(), 1, (INetworkStats**)&stats);
    for (Int32 i = 0; i < ifacePairs->GetLength(); i += 2) {
        String ifaceIn = (*ifacePairs)[i];
        String ifaceOut = (*ifacePairs)[i + 1];
        if (ifaceIn != NULL && ifaceOut != NULL) {
            AutoPtr<INetworkStatsEntry> entry;
            GetNetworkStatsTethering(ifaceIn, ifaceOut, (INetworkStatsEntry**)&entry);
            stats->CombineValues(entry);
        }
    }
    *result = stats;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CNetworkManagementService::GetNetworkStatsTethering(
    /* [in] */ const String& ifaceIn,
    /* [in] */ const String& ifaceOut,
    /* [out] */ INetworkStatsEntry** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ICharSequence> cseq0;
    CString::New(String("gettetherstats"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(ifaceIn, (ICharSequence**)&cseq1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(ifaceOut, (ICharSequence**)&cseq2);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, cseq2.Get());
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("bandwidth"), args, (NativeDaemonEvent**)&event);

    FAIL_RETURN(event->CheckCode(NetdResponseCode::TetheringStatsResult));

    // 221 ifaceIn ifaceOut rx_bytes rx_packets tx_bytes tx_packets
    AutoPtr<ArrayOf<String> > values;
    StringUtils::Split(event->GetMessage(), String(" \t\n\r\f"), (ArrayOf<String>**)&values);

    Int32 index = 0;
    index++;
    index++;

    AutoPtr<INetworkStatsEntry> entry;
    CNetworkStatsEntry::New((INetworkStatsEntry**)&entry);
    entry->SetIface(ifaceIn);
    entry->SetUid(ITrafficStats::UID_TETHERING);
    entry->SetSet(INetworkStats::SET_DEFAULT);
    entry->SetTag(INetworkStats::TAG_NONE);
    entry->SetRxBytes(StringUtils::ParseInt64((*values)[index++]));
    entry->SetRxPackets(StringUtils::ParseInt64((*values)[index++]));
    entry->SetTxBytes(StringUtils::ParseInt64((*values)[index++]));
    entry->SetTxPackets(StringUtils::ParseInt64((*values)[index++]));
    *result = entry;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CNetworkManagementService::SetInterfaceThrottle(
    /* [in] */ const String& iface,
    /* [in] */ Int32 rxKbps,
    /* [in] */ Int32 txKbps)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("setthrottle"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(iface, (ICharSequence**)&cseq1);
    AutoPtr<IInteger32> iint2;
    CInteger32::New(rxKbps, (IInteger32**)&iint2);
    AutoPtr<IInteger32> iint3;
    CInteger32::New(txKbps, (IInteger32**)&iint3);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(4);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, iint2.Get());
    args->Set(3, iint3.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("interface"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::GetInterfaceThrottle(
    /* [in] */ const String& iface,
    /* [in] */ Boolean rx,
    /* [out] */ Int32* throttle)
{
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("getthrottle"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(iface, (ICharSequence**)&cseq1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(rx ? String("rx") : String("tx"), (ICharSequence**)&cseq2);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, cseq2.Get());
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("interface"), args, (NativeDaemonEvent**)&event);

    if (rx) {
        FAIL_RETURN(event->CheckCode(NetdResponseCode::InterfaceRxThrottleResult));
    } else {
        FAIL_RETURN(event->CheckCode(NetdResponseCode::InterfaceTxThrottleResult));
    }

    const char* str = event->GetMessage();
    *throttle = atoi(str);
    return NOERROR;
}

ECode CNetworkManagementService::GetInterfaceRxThrottle(
    /* [in] */ const String& iface,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    return GetInterfaceThrottle(iface, TRUE, result);
}

ECode CNetworkManagementService::GetInterfaceTxThrottle(
    /* [in] */ const String& iface,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    return GetInterfaceThrottle(iface, FALSE, result);
}

ECode CNetworkManagementService::SetDefaultInterfaceForDns(
    /* [in] */ const String& iface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("setdefaultif"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(iface, (ICharSequence**)&cseq1);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("resolver"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::SetDnsServersForInterface(
    /* [in] */ const String& iface,
    /* [in] */ ArrayOf<String>* servers)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));

    AutoPtr<ICharSequence> cseq0;
    CString::New(String("setifdns"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(iface, (ICharSequence**)&cseq1);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    AutoPtr<NativeDaemonConnector::Command> cmd = new NativeDaemonConnector::Command(
        String("resolver"), args);

    for (Int32 i = 0; i < servers->GetLength(); i++) {
        AutoPtr<IInetAddress> addr;
        NetworkUtils::NumericToInetAddress((*servers)[i], (IInetAddress**)&addr);
        Boolean bol;
        addr->IsAnyLocalAddress(&bol);
        if (bol == FALSE) {
            String str;
            addr->GetHostAddress(&str);
            AutoPtr<ICharSequence> cseq;
            CString::New(str, (ICharSequence**)&cseq);
            cmd->AppendArg(cseq);
        }
    }

    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(cmd, (NativeDaemonEvent**)&event);
    return NOERROR;
}

ECode CNetworkManagementService::FlushDefaultDnsCache()
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("flushdefaultif"), (ICharSequence**)&cseq0);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, cseq0.Get());
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("resolver"), args, (NativeDaemonEvent**)&event);
    return NOERROR;
}

ECode CNetworkManagementService::FlushInterfaceDnsCache(
    /* [in] */ const String& iface)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL/*CONNECTIVITY_INTERNAL*/, TAG));
    AutoPtr<ICharSequence> cseq0;
    CString::New(String("flushif"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(iface, (ICharSequence**)&cseq1);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    // try {
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("resolver"), args, (NativeDaemonEvent**)&event);
    // } catch (NativeDaemonConnectorException e) {
    //     throw e.rethrowAsParcelableException();
    // }
    return NOERROR;
}

ECode CNetworkManagementService::SetFirewallEnabled(
    /* [in] */ Boolean enabled)
{
    FAIL_RETURN(EnforceSystemUid());

    AutoPtr<ICharSequence> cseq0;
    CString::New(enabled ? String("enable") : String("disable"), (ICharSequence**)&cseq0);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, cseq0.Get());
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("firewall"), args, (NativeDaemonEvent**)&event);
    mFirewallEnabled = enabled;
    return NOERROR;
}

ECode CNetworkManagementService::IsFirewallEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(EnforceSystemUid());
    *result = mFirewallEnabled;
    return NOERROR;
}

ECode CNetworkManagementService::SetFirewallInterfaceRule(
    /* [in] */ const String& iface,
    /* [in] */ Boolean allow)
{
    EnforceSystemUid();
//     Preconditions::CheckState(mFirewallEnabled);
    String rule = allow ? ALLOW : DENY;

    AutoPtr<ICharSequence> cseq0;
    CString::New(String("set_interface_rule"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(iface, (ICharSequence**)&cseq1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(rule, (ICharSequence**)&cseq2);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, cseq2.Get());
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("firewall"), args, (NativeDaemonEvent**)&event);
    return NOERROR;
}

ECode CNetworkManagementService::SetFirewallEgressSourceRule(
    /* [in] */ const String& addr,
    /* [in] */ Boolean allow)
{
    FAIL_RETURN(EnforceSystemUid());
//     Preconditions::CheckState(mFirewallEnabled);
    String rule = allow ? ALLOW : DENY;

    AutoPtr<ICharSequence> cseq0;
    CString::New(String("set_egress_source_rule"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(addr, (ICharSequence**)&cseq1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(rule, (ICharSequence**)&cseq2);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, cseq2.Get());
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("firewall"), args, (NativeDaemonEvent**)&event);
    return NOERROR;
}

ECode CNetworkManagementService::SetFirewallEgressDestRule(
    /* [in] */ const String& addr,
    /* [in] */ Int32 port,
    /* [in] */ Boolean allow)
{
    FAIL_RETURN(EnforceSystemUid());
//     Preconditions::CheckState(mFirewallEnabled);
    String rule = allow ? ALLOW : DENY;

    AutoPtr<ICharSequence> cseq0;
    CString::New(String("set_egress_dest_rule"), (ICharSequence**)&cseq0);
    AutoPtr<ICharSequence> cseq1;
    CString::New(addr, (ICharSequence**)&cseq1);
    AutoPtr<IInteger32> iint2;
    CInteger32::New(port, (IInteger32**)&iint2);
    AutoPtr<ICharSequence> cseq3;
    CString::New(rule, (ICharSequence**)&cseq3);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(4);
    args->Set(0, cseq0.Get());
    args->Set(1, cseq1.Get());
    args->Set(2, iint2.Get());
    args->Set(3, cseq3.Get());
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("firewall"), args, (NativeDaemonEvent**)&event);
    return NOERROR;
}

ECode CNetworkManagementService::SetFirewallUidRule(
    /* [in] */ Int32 uid,
    /* [in] */ Boolean allow)
{
    FAIL_RETURN(EnforceSystemUid());
    // Preconditions::CheckState(mFirewallEnabled);
    String rule = allow ? ALLOW : DENY;

    AutoPtr<ICharSequence> cseq0;
    CString::New(String("set_uid_rule"), (ICharSequence**)&cseq0);
    AutoPtr<IInteger32> iint1;
    CInteger32::New(uid, (IInteger32**)&iint1);
    AutoPtr<ICharSequence> cseq2;
    CString::New(rule, (ICharSequence**)&cseq2);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, cseq0.Get());
    args->Set(1, iint1.Get());
    args->Set(2, cseq2.Get());
    AutoPtr<NativeDaemonEvent> event;
    mConnector->Execute(String("firewall"), args, (NativeDaemonEvent**)&event);
    return NOERROR;
}

ECode CNetworkManagementService::EnforceSystemUid()
{
    Int32 uid = Binder::GetCallingUid();
    if (uid != IProcess::SYSTEM_UID) {
        //throw new SecurityException("Only available to AID_SYSTEM");
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

// //@Override
// ECode CNetworkManagementService::Monitor()
// {
//     if (mConnector != NULL) {
//         mConnector->Monitor();
//     }
//     return NOERROR;
// }

// //@Override
// void CNetworkManagementService::Dump(
//     /* [in] */ IFileDescriptor* fd,
//     /* [in] */ IPrintWriter* pw,
//     /* [in] */ ArrayOf<String>* args)
// {
//     FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, TAG));

//     pw->Println("NetworkManagementService NativeDaemonConnector Log:");
//     mConnector->Dump(fd, pw, args);
//     pw->Println();

//     pw->Print("Bandwidth control enabled: "); pw->Println(mBandwidthControlEnabled);

//     {
//         AutoLock lock(mQuotaLock);
//         pw->Print("Active quota ifaces: "); pw->Println(mActiveQuotas.ToString());
//         pw->Print("Active alert ifaces: "); pw->Println(mActiveAlerts.ToString());
//     }

//     {
//         AutoLock lock(mUidRejectOnQuota);
//         pw->Print("UID reject on quota ifaces: [");
//         const Int32 size = mUidRejectOnQuota.GetSize();
//         for (Int32 i = 0; i < size; i++) {
//             pw->Print(mUidRejectOnQuota.keyAt(i));
//             if (i < size - 1) pw->Print(",");
//         }
//         pw->Println("]");
//     }

//     pw->Print("Firewall enabled: "); pw->Println(mFirewallEnabled);
// }

} // namespace Server
} // namepsace Droid
} // namespace Elastos

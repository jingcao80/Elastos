
#include "CPppoeStateTracker.h"
#include "CPppoeNative.h"
#include "CPppoeDevInfo.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/net/CNetworkInfo.h"
#include "elastos/droid/net/CLinkCapabilities.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Pppoe {

const String CPppoeStateTracker::TAG("CPppoeStateTracker");
const String CPppoeStateTracker::PROP_PPP_ADDR("dhcp.ppp0.ipaddress");
const String CPppoeStateTracker::PROP_PPP_MASK("dhcp.ppp0.mask");
const String CPppoeStateTracker::PROP_PPP_DNS1("dhcp.ppp0.dns1");
const String CPppoeStateTracker::PROP_PPP_DNS2("dhcp.ppp0.dns2");
const String CPppoeStateTracker::PROP_PPP_GW("dhcp.ppp0.gateway");
const String CPppoeStateTracker::PROP_VAL_PPP_NOERR("0:0");
const String CPppoeStateTracker::PROP_NAME_PPP_ERRCODE("net.ppp.errcode");

CAR_INTERFACE_IMPL(CPppoeStateTracker::MyHandlerCallback, IHandlerCallback)

CPppoeStateTracker::MyHandlerCallback::MyHandlerCallback(CPppoeStateTracker* owner)
    :mOwner(owner)
{}

ECode CPppoeStateTracker::MyHandlerCallback::HandleMessage(
   /* [in] */ IMessage* msg,
   /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
}

ECode CPppoeStateTracker::StopInterface(
    /* [in] */ Boolean suspend,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mEM != NULL) {
        AutoPtr<IPppoeDevInfo> info;
        FAIL_RETURN(mEM->GetSavedPppoeConfig((IPppoeDevInfo**)&info));
        Boolean result;
        mEM->PppoeConfigured(&result);
        if (info != NULL && result) {
            mInterfaceStopped = TRUE;
            Slogger::I(TAG, "stop interface");
            String ifname;
            info->GetIfName(&ifname);
            //NetworkUtils::ResetConnections(ifname, INetworkUtils::RESET_ALL_ADDRESSES);
            // if (!suspend)
            //     NetworkUtils::DisableInterface(ifname);
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode CPppoeStateTracker::ResetInterface(
    /* [out] */ Boolean* result)
{
    /*
     * This will guide us to enabled the enabled device
     */

    Slogger::I(TAG, ">>>resetInterface");
    if (mEM != NULL) {
        Boolean result;
        mEM->PppoeConfigured(&result);
        Slogger::I(TAG, "pppoeConfigured: &d", result);
        AutoPtr<IPppoeDevInfo> info;
        FAIL_RETURN(mEM->GetSavedPppoeConfig((IPppoeDevInfo**)&info));

        String ifname, ip, netmask, dns;
        FAIL_RETURN(info->GetIfName(&ifname));
        FAIL_RETURN(info->GetIpAddress(&ip));
        FAIL_RETURN(info->GetNetMask(&netmask));
        FAIL_RETURN(info->GetDnsAddr(&dns));

        if (info != NULL && result) {
            Slogger::I(TAG, "IfName: %s", ifname.string());
            Slogger::I(TAG, "IP: %s", ip.string());
            Slogger::I(TAG, "Mask: %s", netmask.string());
            Slogger::I(TAG, "DNS: %s", dns.string());

            synchronized(mSyncLock)
            {
                if(mInterfaceName != NULL) {
                    Slogger::I(TAG, "reset device: %s", mInterfaceName.string());
                    //NetworkUtils::ResetConnections(mInterfaceName, INetworkUtils::RESET_ALL_ADDRESSES);
                }

                Slogger::I(TAG, "Force the connection disconnected before configuration");
                SetPppoeState(FALSE, EVENT_DISCONNECTED, IPppoeManager::PROP_VAL_PPP_NOERR);
                ConfigureInterface(info);
            }
        }
    }
    *result = TRUE;

    return NOERROR;
}

ECode CPppoeStateTracker::GetTcpBufferSizesPropName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = String("net.tcp.buffersize.default");
    return NOERROR;
}

ECode CPppoeStateTracker::StartPolling()
{
    Slogger::I(TAG, "start monitoring");
    mMonitor->StartMonitoring();
    return NOERROR;
}

ECode CPppoeStateTracker::IsAvailable(
    /* [out] */ Boolean* result)
{
    Int32 totalInterface, state;
    mEM->GetTotalInterface(&totalInterface);
    mEM->GetPppoeState(&state);
    *result = ((totalInterface != 0) && (state != IPppoeManager::PPPOE_STATE_DISABLED));
    return NOERROR;
}

ECode CPppoeStateTracker::Reconnect(
    /* [out] */ Boolean* result)
{
    Slogger::I(TAG, ">>>reconnect");
    //try
    Int32 state;
    ECode ec = mEM->GetPppoeState(&state);
    if (SUCCEEDED(ec)) {
        if (state != IPppoeManager::PPPOE_STATE_DISABLED ) {
            // maybe this is the first time we run, so set it to enabled
            if (SUCCEEDED(mEM->SetPppoeEnabled(TRUE))) {
                Boolean result;
                if (SUCCEEDED(mEM->PppoeConfigured(&result)) && !result) {
                    ec = mEM->PppoeSetDefaultConf();
                }
                if (SUCCEEDED(ec)) {
                    return ResetInterface(&result);
                }
            }
        }
    }

    /*} catch (UnknownHostException e) {
        // TODO Auto-generated catch block
        e.printStackTrace();
    }*/
    *result = FALSE;
    return NOERROR;
}

ECode CPppoeStateTracker::SetRadio(
    /* [in] */ Boolean turnOn,
    /* [out] */ Boolean* result)
{
    *result = FALSE;
    return E_NOT_IMPLEMENTED;
}

ECode CPppoeStateTracker::StartMonitoring(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* target)
{
    /*
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(target);
    Slogger::I(TAG, "start to monitor the pppoe devices");
    if (mServiceStarted) {
        mContext = context;
        AutoPtr<IInterface> obj;
        FAIL_RETURN(mContext->GetSystemService(IContext::PPPOE_SERVICE, (IPppoeManager**)&obj));
        mEM = IPppoeManager::Probe(obj);
        mTarget = target;
        AutoPtr<ILooper> looper;
        ASSERT_SUCCEEDED(mTarget->GetLooper((ILooper**)&looper));
        CHandler::New()
        mTrackerTarget = new Handler(target.getLooper(), mTrackerHandlerCallback);
        if (mEM == NULL) {
            Slogger::I(TAG,"failed to start startMonitoring");
            return NOERROR;
        }

        Int32 state = mEM.getPppoeState();
        if (state != mEM.PPPOE_STATE_DISABLED) {
            if (state == mEM.PPPOE_STATE_UNKNOWN){
                // maybe this is the first time we run, so set it to enabled
                mEM.setPppoeEnabled(TRUE);
            } else {
                try {
                    resetInterface();
                } catch (UnknownHostException e) {
                    Slogger::E(TAG, "Wrong pppoe configuration");
                }
            }
        }
    }
    */
    return NOERROR;
}

ECode CPppoeStateTracker::Teardown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    StopInterface(FALSE, result);
    *result = (mEM != NULL) ? *result : FALSE;
    return NOERROR;
}

ECode CPppoeStateTracker::CaptivePortalCheckComplete()
{
    return E_NOT_IMPLEMENTED;
}

ECode CPppoeStateTracker::GetDhcpInfo(
    /* [out] */ IDhcpInfo** dhcpInfo)
{
    VALIDATE_NOT_NULL(dhcpInfo);
    AutoPtr<IDhcpInfo> d = mDhcpInfoInternal->MakeDhcpInfo();
    *dhcpInfo = d;
    REFCOUNT_ADD(*dhcpInfo);
    return NOERROR;
}

ECode CPppoeStateTracker::NotifyPppConnected(
    /* [in] */ const String& ifname)
{
    Slogger::I(TAG, "report interface is up for %s", ifname.string());
    synchronized(mSyncLock)
    {
        //TODO: Need IHandler::sendEmptyMessage
        //mTrackerTarget->SendEmptyMessage(EVENT_CONNECTED);
    }
    return NOERROR;
}

ECode CPppoeStateTracker::NotifyStateChange(
    /* [in] */ const String& ifname,
    /* [in] */ NetworkInfoDetailedState state)
{
    Slogger::I(TAG, "report new state %d on dev %s", state, ifname.string());
    if (ifname.Equals(mInterfaceName)) {
        Slogger::I(TAG, "update network state tracker");

        synchronized(mSyncLock)
        {
            //TODO: Need IHandler::sendEmptyMessage
            // mTrackerTarget->SendEmptyMessage(state->Equals(DetailedState::CONNECTED)
            //     ? EVENT_CONNECTED : EVENT_DISCONNECTED);
        }
    }
    return NOERROR;
}

ECode CPppoeStateTracker::GetNetworkInfo(
    /* [out] */ INetworkInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return CNetworkInfo::New(mNetworkInfo, info);
}

ECode CPppoeStateTracker::GetLinkProperties(
    /* [out] */ ILinkProperties** linkProperties)
{
    VALIDATE_NOT_NULL(linkProperties);
    return CLinkProperties::New(mLinkProperties, linkProperties);
}

ECode CPppoeStateTracker::GetLinkCapabilities(
    /* [out] */ ILinkCapabilities** linkCapabilities)
{
    VALIDATE_NOT_NULL(linkCapabilities);
    return CLinkCapabilities::New(mLinkCapabilities, linkCapabilities);
}

ECode CPppoeStateTracker::SetUserDataEnable(
    /* [in] */ Boolean enabled)
{
    Slogger::D(TAG, "ignoring setUserDataEnable(%d)", enabled);
    return NOERROR;
}

ECode CPppoeStateTracker::SetPolicyDataEnable(
    /* [in] */ Boolean enabled)
{
    Slogger::D(TAG, "ignoring setPolicyDataEnable(%d)", enabled);
    return NOERROR;
}

ECode CPppoeStateTracker::IsPrivateDnsRouteSet(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Slogger::V(TAG, "isPrivateDnsRouteSet");
    return mPrivateDnsRouteSet->Get(result);
}

ECode CPppoeStateTracker::PrivateDnsRouteSet(
    /* [in] */ Boolean enabled)
{
    Slogger::V(TAG, "privateDnsRouteSet");
    return mPrivateDnsRouteSet->Set(enabled);
}

ECode CPppoeStateTracker::IsDefaultRouteSet(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Slogger::V(TAG, "isDefaultRouteSet");
    return mDefaultRouteSet->Get(result);
}

ECode CPppoeStateTracker::DefaultRouteSet(
    /* [in] */ Boolean enabled)
{
    Slogger::V(TAG, "defaultRouteSet");
    return mDefaultRouteSet->Set(enabled);
}

ECode CPppoeStateTracker::SetTeardownRequested(
    /* [in] */ Boolean isRequested)
{
    Slogger::V(TAG, "setTeardownRequested( %d )", isRequested);
    return mTeardownRequested->Set(isRequested);
}

ECode CPppoeStateTracker::IsTeardownRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mTeardownRequested->Get(result));
    Slogger::V(TAG, "isTeardownRequested: %d", result);
    return NOERROR;
}

ECode CPppoeStateTracker::SetDependencyMet(
    /* [in] */ Boolean met)
{
    // not supported on this network
    return E_NOT_IMPLEMENTED;
}

ECode CPppoeStateTracker::constructor(
    /* [in] */ Int32 netType,
    /* [in] */ const String& networkName)
{
    // initialize memeber variable
    mPppInterfaceAdded = FALSE;
    mInterfaceName = "ppp0";
    FAIL_RETURN(CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mTeardownRequested));
    FAIL_RETURN(CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mPrivateDnsRouteSet));
    FAIL_RETURN(CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mDefaultRouteSet));

    Slogger::I(TAG, "Starts ...");
    FAIL_RETURN(CNetworkInfo::New(netType, 0, networkName, String(""), (INetworkInfo**)&mNetworkInfo));
    mNetworkInfo->SetIsAvailable(FALSE);
    SetTeardownRequested(FALSE);
    FAIL_RETURN(CLinkProperties::New((ILinkProperties**)&mLinkProperties));
    FAIL_RETURN(CLinkCapabilities::New((ILinkCapabilities**)&mLinkCapabilities));
    AutoPtr<IPppoeNative> pppoeNative;
    FAIL_RETURN(CPppoeNative::AcquireSingleton((IPppoeNative**)&pppoeNative));
    Int32 val;
    pppoeNative->InitPppoeNative(&val);

    if (val != 0 ) {
        Slogger::E(TAG,"Can not init pppoe device layers");
        return E_UNEXPECTED;
     }

    Slogger::I(TAG,"Successed");
    mServiceStarted = TRUE;
    FAIL_RETURN(CPppoeMonitor::New(this, (IPppoeMonitor**)&mMonitor));
    mTrackerHandlerCallback = new MyHandlerCallback(this);
    return NOERROR;
}

ECode CPppoeStateTracker::ConfigureInterfaceStatic(
    /* [in] */ const String& ifname,
    /* [in] */ DhcpInfoInternal* dhcpInfoInternal,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
}

ECode CPppoeStateTracker::ConfigureInterface(
    /* [in] */ IPppoeDevInfo* info)
{
    // wait for IINetworkManagementService.Stub.asInterface
    /*
    IBinder b = ServiceManager::GetService(Context.NETWORKMANAGEMENT_SERVICE);
    IINetworkManagementService netd = IINetworkManagementService::Probe(b);
    InterfaceConfiguration ifcg = new InterfaceConfiguration();
    ifcg.setLinkAddress(dhcpInfoInternal.makeLinkAddress());
    Log.v(TAG, "configureInterfaceStatic: ifname:" + ifname);
    try {
        netd.setInterfaceConfig(ifname, ifcg);
        mLinkProperties = dhcpInfoInternal.makeLinkProperties();
        mLinkProperties.setInterfaceName(ifname);
        Log.v(TAG, "IP configuration succeeded");
        return TRUE;
    } catch (RemoteException re) {
        Log.v(TAG, "IP configuration failed: " + re);
        return FALSE;
    } catch (IllegalStateException e) {
        Log.v(TAG, "IP configuration failed: " + e);
        return FALSE;
    }
    */
    return E_NOT_IMPLEMENTED;
}

ECode CPppoeStateTracker::PostNotification(
    /* [in] */ Int32 event,
    /* [in] */ const String& errcode)
{
    AutoPtr<IIntent> intent;
    FAIL_RETURN(CIntent::New(IPppoeManager::PPPOE_STATE_CHANGED_ACTION, (IIntent**)&intent));
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    intent->PutExtra(IPppoeManager::EXTRA_PPPOE_STATE, event);
    intent->PutExtra(IPppoeManager::EXTRA_PPPOE_ERRCODE, errcode);
    mContext->SendBroadcast(intent);

    Slogger::D(TAG, "Send PPPOE_STATE_CHANGED_ACTION");
    return NOERROR;
}

ECode CPppoeStateTracker::SetPppoeState(
    /* [in] */ Boolean state,
    /* [in] */ Int32 event,
    /* [in] */ const String& errcode)
{
    Boolean result;
    mNetworkInfo->IsConnected(&result);
    Slogger::D(TAG, "PST.SetPppoeState() is connected: %d, state:%d", result, state);
    if (event == EVENT_CONNECT_FAILED || result != state) {
        mNetworkInfo->SetIsAvailable(state);
        PostNotification(event, errcode);
    }

    if (result != state) {
        if (state) {
            mNetworkInfo->SetDetailedState(NetworkInfoDetailedState_CONNECTED, String(NULL), String(NULL));
        } else {
            mNetworkInfo->SetDetailedState(NetworkInfoDetailedState_DISCONNECTED, String(NULL), String(NULL));
            if( EVENT_DISCONNECTED == event ) {
                Slogger::D(TAG, "EVENT_DISCONNECTED: StopInterface");
                StopInterface(TRUE, &result);
            }
        }
        //wait for further research
        /*
        AutoPtr<IMessage> msg = mTarget->ObtainMessage(EVENT_CONFIGURATION_CHANGED, mNetworkInfo);
        msg->SendToTarget();

        msg = mTarget->ObtainMessage(EVENT_STATE_CHANGED, mNetworkInfo);
        msg->SendToTarget();
        */
    }
    return NOERROR;
}

} // namespace Pppoe
} // namespace Net
} // namespace Droid
} // namespace Elastos



#include "BluetoothTetheringDataTracker.h"
#include "CBluetoothAdapter.h"
#include "elastos/droid/net/CLinkCapabilities.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/CNetworkInfo.h"
#include "elastos/droid/net/CLinkAddress.h"
#include "elastos/droid/net/CRouteInfo.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/os/CSystemProperties.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Thread.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::Thread;
using Elastos::Net::IInetAddress;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::CLinkCapabilities;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED;
using Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::CLinkAddress;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Net::CRouteInfo;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//====================================================
// BluetoothTetheringDataTracker::ProfileServiceListener
//====================================================

BluetoothTetheringDataTracker::ProfileServiceListener::ProfileServiceListener(
    /* [in] */ BluetoothTetheringDataTracker* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(BluetoothTetheringDataTracker::ProfileServiceListener, IBluetoothProfileServiceListener)

ECode BluetoothTetheringDataTracker::ProfileServiceListener::OnServiceConnected(
    /* [in] */ Int32 profile,
    /* [in] */ IBluetoothProfile* proxy)
{
    mHost->mBluetoothPan = (BluetoothPan*)proxy;
    return NOERROR;
}

ECode BluetoothTetheringDataTracker::ProfileServiceListener::OnServiceDisconnected(
    /* [in] */ Int32 profile)
{
    mHost->mBluetoothPan = NULL;
    return NOERROR;
}


//====================================================
// BluetoothTetheringDataTracker::DhcpRunnable
//====================================================

BluetoothTetheringDataTracker::DhcpRunnable::DhcpRunnable(
    /* [in] */ BluetoothTetheringDataTracker* host)
    : mHost(host)
{}

ECode BluetoothTetheringDataTracker::DhcpRunnable::Run()
{
    //TODO(): Add callbacks for failure and success case.
    //Currently this thread runs independently.
    if (DBG) Logger::D(TAG, "startReverseTether mCsHandler: %p", mHost->mCsHandler.Get());
    StringBuilder sb("dhcp.");
    sb += sIface;
    sb += ".result";
    String DhcpResultName = sb.ToString();
    String result("");
    if (VDBG) Logger::D(TAG, "waiting for change of sys prop dhcp result: %s", DhcpResultName.string());
    for (Int32 i = 0; i < 30 * 5; i++) {
        // try {
        ECode ec = Thread::Sleep(200);
        if (FAILED(ec)) {
            return NOERROR;
        }
        // } catch (InterruptedException ie) { return;}
        AutoPtr<ISystemProperties> sp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
        sp->Get(DhcpResultName, &result);
        if (VDBG) Logger::D(TAG, "read %s: %s", DhcpResultName.string(), result.string());
        if(result.Equals("failed")) {
            Logger::E(TAG, "startReverseTether, failed to start dhcp service");
            return NOERROR;
        }
        if(result.Equals("ok")) {
            if (VDBG) Logger::D(TAG, "startReverseTether, dhcp resut: %s", result.string());
            if(mHost->ReadLinkProperty(sIface)) {
                mHost->mNetworkInfo->SetIsAvailable(TRUE);
                mHost->mNetworkInfo->SetDetailedState(NetworkInfoDetailedState_CONNECTED, String(NULL), String(NULL));

                if (VDBG) Logger::D(TAG, "startReverseTether mCsHandler: %p", mHost->mCsHandler.Get());
                if(mHost->mCsHandler != NULL) {
                    AutoPtr<IMessage> msg;
                    mHost->mCsHandler->ObtainMessage(EVENT_CONFIGURATION_CHANGED, mHost->mNetworkInfo, (IMessage**)&msg);
                    msg->SendToTarget();

                    AutoPtr<IMessage> msg1;
                    mHost->mCsHandler->ObtainMessage(EVENT_STATE_CHANGED, mHost->mNetworkInfo, (IMessage**)&msg1);
                    msg1->SendToTarget();
                }
            }
            return NOERROR;
        }
    }
    Logger::E(TAG, "startReverseTether, dhcp failed, resut: ", result.string());
    return NOERROR;
}


//====================================================
// BluetoothTetheringDataTracker
//====================================================

const String BluetoothTetheringDataTracker::NETWORKTYPE("BLUETOOTH_TETHER");
const String BluetoothTetheringDataTracker::TAG("BluetoothTethering");
const Boolean BluetoothTetheringDataTracker::DBG = TRUE;
const Boolean BluetoothTetheringDataTracker::VDBG = FALSE;
AutoPtr<BluetoothTetheringDataTracker> BluetoothTetheringDataTracker::sInstance;
String BluetoothTetheringDataTracker::sIface;

BluetoothTetheringDataTracker::BluetoothTetheringDataTracker()
{
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mTeardownRequested);
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mPrivateDnsRouteSet);
    CAtomicInteger32::New(0, (IAtomicInteger32**)&mDefaultGatewayAddr);
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mDefaultRouteSet);

    CNetworkInfo::New(IConnectivityManager::TYPE_BLUETOOTH, 0, NETWORKTYPE, String(""), (INetworkInfo**)&mNetworkInfo);
    CLinkProperties::New((ILinkProperties**)&mLinkProperties);
    CLinkCapabilities::New((ILinkCapabilities**)&mLinkCapabilities);

    mNetworkInfo->SetIsAvailable(FALSE);
    SetTeardownRequested(FALSE);
}

PInterface BluetoothTetheringDataTracker::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(INetworkStateTracker*)this;
    }
    return NULL;
}

UInt32 BluetoothTetheringDataTracker::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 BluetoothTetheringDataTracker::Release()
{
    return ElRefBase::Release();
}

ECode BluetoothTetheringDataTracker::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return NOERROR;
}

AutoPtr<BluetoothTetheringDataTracker> BluetoothTetheringDataTracker::GetInstance()
{
    if (sInstance == NULL) sInstance = new BluetoothTetheringDataTracker();
    return sInstance;
}

ECode BluetoothTetheringDataTracker::SetTeardownRequested(
    /* [in] */ Boolean isRequested)
{
    return mTeardownRequested->Set(isRequested);
}

ECode BluetoothTetheringDataTracker::IsTeardownRequested(
    /* [out] */ Boolean* isRequested)
{
    VALIDATE_NOT_NULL(isRequested)
    return mTeardownRequested->Get(isRequested);
}

ECode BluetoothTetheringDataTracker::StartMonitoring(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* target)
{
    if (DBG) Logger::D(TAG, "startMonitoring: target: %p", target);
    mContext = context;
    mCsHandler = target;
    if (VDBG) Logger::D(TAG, "startMonitoring: mCsHandler: %p", mCsHandler.Get());
    AutoPtr<IBluetoothAdapter> adapter = CBluetoothAdapter::GetDefaultAdapter();
    if (adapter != NULL) {
        Boolean result;
        adapter->GetProfileProxy(mContext, mProfileServiceListener, IBluetoothProfile::PAN, &result);
    }
    return NOERROR;
}

ECode BluetoothTetheringDataTracker::Teardown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mTeardownRequested->Set(TRUE);
    if (mBluetoothPan != NULL) {
        AutoPtr< ArrayOf<IBluetoothDevice*> > devices;
        mBluetoothPan->GetConnectedDevices((ArrayOf<IBluetoothDevice*>**)&devices);
        for (Int32 i = 0; i < devices->GetLength(); i++) {
            Boolean result;
            mBluetoothPan->Disconnect((*devices)[i], &result);
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothTetheringDataTracker::CaptivePortalCheckComplete()
{
    return E_NOT_IMPLEMENTED;
}

ECode BluetoothTetheringDataTracker::Reconnect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mTeardownRequested->Set(FALSE);
    //Ignore
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothTetheringDataTracker::SetRadio(
    /* [in] */ Boolean turnOn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

/*synchronized*/
ECode BluetoothTetheringDataTracker::IsAvailable(
    /* [out] */ Boolean* isAvailable)
{
    VALIDATE_NOT_NULL(isAvailable)
    AutoLock lock(mLock);
    return mNetworkInfo->IsAvailable(isAvailable);
}

Int32 BluetoothTetheringDataTracker::StartUsingNetworkFeature(
    /* [in] */ const String& feature,
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid)
{
    return -1;
}

Int32 BluetoothTetheringDataTracker::StopUsingNetworkFeature(
    /* [in] */ const String& feature,
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid)
{
    return -1;
}

ECode BluetoothTetheringDataTracker::SetUserDataEnable(
    /* [in] */ Boolean enabled)
{
    Logger::W(TAG, "ignoring setUserDataEnable(%d)", enabled);
    return NOERROR;
}

ECode BluetoothTetheringDataTracker::SetPolicyDataEnable(
    /* [in] */ Boolean enabled)
{
    Logger::W(TAG, "ignoring setPolicyDataEnable(%d)", enabled);
    return NOERROR;
}

ECode BluetoothTetheringDataTracker::IsPrivateDnsRouteSet(
    /* [out] */ Boolean* isSet)
{
    VALIDATE_NOT_NULL(isSet)
    return mPrivateDnsRouteSet->Get(isSet);
}

ECode BluetoothTetheringDataTracker::SetPrivateDnsRoute(
    /* [in] */ Boolean enabled)
{
    return mPrivateDnsRouteSet->Set(enabled);
}

/*Synchronized*/
ECode BluetoothTetheringDataTracker::GetNetworkInfo(
    /* [out] */ INetworkInfo** info)
{
    VALIDATE_NOT_NULL(info)
    AutoLock lock(mLock);
    *info = mNetworkInfo;
    REFCOUNT_ADD(*info)
    return NOERROR;
}

/*Synchronized*/
ECode BluetoothTetheringDataTracker::GetLinkProperties(
    /* [out] */ ILinkProperties** linkProperties)
{
    VALIDATE_NOT_NULL(linkProperties)
    AutoLock lock(mLock);
    return CLinkProperties::New(mLinkProperties, linkProperties);
}

ECode BluetoothTetheringDataTracker::GetLinkCapabilities(
    /* [out] */ ILinkCapabilities** linkCapabilities)
{
    VALIDATE_NOT_NULL(linkCapabilities)
    return CLinkCapabilities::New(mLinkCapabilities, linkCapabilities);
}

Int32 BluetoothTetheringDataTracker::GetDefaultGatewayAddr()
{
    Int32 value;
    mDefaultGatewayAddr->Get(&value);
    return value;
}

ECode BluetoothTetheringDataTracker::IsDefaultRouteSet(
    /* [out] */ Boolean* isSet)
{
    VALIDATE_NOT_NULL(isSet)
    return mDefaultRouteSet->Get(isSet);
}

ECode BluetoothTetheringDataTracker::SetDefaultRoute(
    /* [in] */ Boolean enabled)
{
    return mDefaultRouteSet->Set(enabled);;
}

ECode BluetoothTetheringDataTracker::GetTcpBufferSizesPropName(
    /* [out] */ String* propName)
{
    VALIDATE_NOT_NULL(propName)
    *propName = String("net.tcp.buffersize.wifi");
    return NOERROR;
}

Int16 BluetoothTetheringDataTracker::CountPrefixLength(
    /* [in] */ ArrayOf<Byte>* mask)
{
    Int16 count = 0;
    for (Int32 i = 0; i < mask->GetLength(); ++i) {
        Byte b = (*mask)[i];
        for (Int32 j = 0; j < 8; ++j) {
            if ((b & (1 << j)) != 0) {
                ++count;
            }
        }
    }
    return count;
}

Boolean BluetoothTetheringDataTracker::ReadLinkProperty(
    /* [in] */ const String& iface)
{
    StringBuilder sb("dhcp.");
    sb += iface;
    sb += ".";
    String DhcpPrefix = sb.ToString();
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String ip, dns1, dns2, gateway, mask;
    sp->Get(DhcpPrefix + "ipaddress", &ip);
    sp->Get(DhcpPrefix + "dns1", &dns1);
    sp->Get(DhcpPrefix + "dns2", &dns2);
    sp->Get(DhcpPrefix + "gateway", &gateway);
    sp->Get(DhcpPrefix + "mask", &mask);
    if(ip.IsEmpty() || gateway.IsEmpty()) {
        Logger::E(TAG, "readLinkProperty, ip: %s, gateway: %s, can not be empty", ip.string(), gateway.string());
        return FALSE;
    }

    AutoPtr<IInetAddress> maskAddress;
    NetworkUtils::NumericToInetAddress(mask, (IInetAddress**)&maskAddress);
    AutoPtr< ArrayOf<Byte> > address;
    maskAddress->GetAddress((ArrayOf<Byte>**)&address);
    Int32 PrefixLen = CountPrefixLength(address);

    AutoPtr<IInetAddress> ipAddress;
    NetworkUtils::NumericToInetAddress(ip, (IInetAddress**)&ipAddress);
    AutoPtr<ILinkAddress> linkAddress;
    CLinkAddress::New(ipAddress, PrefixLen, (ILinkAddress**)&linkAddress);
    mLinkProperties->AddLinkAddress(linkAddress);

    AutoPtr<IInetAddress> gatewayAddress;
    NetworkUtils::NumericToInetAddress(gateway, (IInetAddress**)&gatewayAddress);
    AutoPtr<IRouteInfo> ri;
    CRouteInfo::New(gatewayAddress, (IRouteInfo**)&ri);
    mLinkProperties->AddRoute(ri);

    if(!dns1.IsEmpty()) {
        AutoPtr<IInetAddress> dns1Address;
        NetworkUtils::NumericToInetAddress(dns1, (IInetAddress**)&dns1Address);
        mLinkProperties->AddDns(dns1Address);
    }

    if(!dns2.IsEmpty()) {
        AutoPtr<IInetAddress> dns2Address;
        NetworkUtils::NumericToInetAddress(dns2, (IInetAddress**)&dns2Address);
        mLinkProperties->AddDns(dns2Address);
    }

    mLinkProperties->SetInterfaceName(iface);
    return TRUE;
}

// synchronized
void BluetoothTetheringDataTracker::StartReverseTether(
    /* [in] */ const String& iface)
{
    AutoLock lock(mLock);
    sIface = iface;
    if (DBG) Logger::D(TAG, "startReverseTether mCsHandler: %p", mCsHandler.Get());
    AutoPtr<IRunnable> runnable = new DhcpRunnable(this);
    CThread::New(runnable, (IThread**)&mDhcpThread);
    mDhcpThread->Start();
}

// synchronized
void BluetoothTetheringDataTracker::StopReverseTether()
{
    AutoLock lock(mLock);
    //NetworkUtils.stopDhcp(iface);
    Boolean isAlive;
    if(mDhcpThread != NULL && (mDhcpThread->IsAlive(&isAlive), isAlive)) {
        mDhcpThread->Interrupt();
        // try {
        ECode ec = mDhcpThread->Join();
        if (FAILED(ec)) {
            return;
        }
        // } catch (InterruptedException ie) { return; }
    }
    mLinkProperties->Clear();
    mNetworkInfo->SetIsAvailable(FALSE);
    mNetworkInfo->SetDetailedState(NetworkInfoDetailedState_DISCONNECTED, String(NULL), String(NULL));

    AutoPtr<IMessage> msg;
    mCsHandler->ObtainMessage(EVENT_CONFIGURATION_CHANGED, mNetworkInfo, (IMessage**)&msg);
    msg->SendToTarget();

    AutoPtr<IMessage> msg1;
    mCsHandler->ObtainMessage(EVENT_STATE_CHANGED, mNetworkInfo, (IMessage**)&msg1);
    msg1->SendToTarget();
}

ECode BluetoothTetheringDataTracker::SetDependencyMet(
    /* [in] */ Boolean met)
{
    // not supported on this network
    return NOERROR;
}

} // Bluetooth
} // Droid
} // Elastos


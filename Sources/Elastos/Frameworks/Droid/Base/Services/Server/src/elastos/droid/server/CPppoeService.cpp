
#include "CPppoeService.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::IO::CBufferedReader;
using Elastos::IO::CFileReader;
using Elastos::IO::EIID_ICloseable;
using Elastos::IO::IBufferedReader;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileReader;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::Pppoe::CPppoeDevInfo;
using Elastos::Droid::Net::Pppoe::CPppoeNative;
using Elastos::Droid::Net::Pppoe::IPppoeDevInfo;
using Elastos::Droid::Net::Pppoe::IPppoeManager;
using Elastos::Droid::Net::Pppoe::IPppoeNative;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;

namespace Elastos {
namespace Droid {
namespace Server {

String CPppoeService::TAG("CPppoeService");

CPppoeService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CPppoeService* owner)
    : mOwner(owner)
{}

ECode CPppoeService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Boolean result;
    ECode ec = NOERROR;
    if (action.Equals(IIntent::ACTION_SCREEN_ON)) {
        Slogger::D(CPppoeService::TAG, "ACTION_SCREEN_ON");
        //try {
        ec = mOwner->mTracker->ResetInterface(&result);
        //} catch (UnknownHostException e) {
        //    Slog.e(TAG, "Wrong pppoe configuration");
        //}
    }
    else if (action.Equals(IIntent::ACTION_SCREEN_OFF)) {
        Slogger::D(CPppoeService::TAG, "ACTION_SCREEN_OFF");
        ec = mOwner->mTracker->StopInterface(FALSE, &result);
    }
    return ec;
}

CPppoeService::MyStopInterfaceThread::MyStopInterfaceThread(
    /* [in] */ CPppoeService* owner)
    : mOwner(owner)
{
    Thread::constructor(String("CPppoeService::MyStopInterfaceThread"));
}

ECode CPppoeService::MyStopInterfaceThread::Run()
{
    Boolean result;
    return mOwner->mTracker->StopInterface(FALSE, &result);
}


CPppoeService::MyResetInterfaceThread::MyResetInterfaceThread(
    /* [in] */ CPppoeService* owner)
    : mOwner(owner)
{
    Thread::constructor(String("CPppoeService::MyResetInterfaceThread"));
}

ECode CPppoeService::MyResetInterfaceThread::Run()
{
    Boolean result;
    return mOwner->mTracker->ResetInterface(&result);
}


ECode CPppoeService::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IPppoeStateTracker* tracker)
{
    mPppoeState = IPppoeManager::PPPOE_STATE_UNKNOWN;
    mReceiver = new MyBroadcastReceiver(this);
    mTracker = tracker;
    mContext = context;

    isPppoeEnabled = GetPersistedState();
    Slogger::I(TAG,"Pppoe dev enabled %d", isPppoeEnabled );
    AutoPtr<ArrayOf<String> > tmpDevName;
    GetDeviceNameList((ArrayOf<String>**)&tmpDevName);
    SetPppoeState(isPppoeEnabled);
    RegisterForBroadcasts();
    Slogger::I(TAG, "Trigger the pppoe monitor");
    mTracker->StartPolling();
    return NOERROR;
}

ECode CPppoeService::GetDeviceNameList(
    /* [out, callee] */ ArrayOf<String>** list)
{
    VALIDATE_NOT_NULL(list);
    *list = (ScanPppoeDevice() > 0) ? DevName : NULL;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode CPppoeService::SetPppoeState(
    /* [in] */ Int32 state)
{
    AutoLock lock(this);

    Slogger::I(TAG, "setPppoeState from %d to %d", mPppoeState, state);

    if (mPppoeState != state){
        mPppoeState = state;
        if (state == IPppoeManager::PPPOE_STATE_DISABLED) {
            PersistPppoeEnabled(FALSE);
            //mTracker.stopInterface(FALSE);
            AutoPtr<MyStopInterfaceThread> stopInterfaceThread = new MyStopInterfaceThread(this);
            stopInterfaceThread->Start();
        } else {
            PersistPppoeEnabled(TRUE);
            Boolean isConfigured;
            IsPppoeConfigured(&isConfigured);
            if (!isConfigured) {
                // If user did not configure any interfaces yet, pick the first one
                // and enable it.
                SetPppoeMode(IPppoeDevInfo::PPPOE_CONN_MODE_DHCP);
            }
        //              try {
        //                  mTracker.resetInterface();
        //              } catch (UnknownHostException e) {
        //                  Slog.e(TAG, "Wrong pppoe configuration");
        //              }
            AutoPtr<MyResetInterfaceThread> resetInterfaceThread = new MyResetInterfaceThread(this);
            resetInterfaceThread->Start();
        }
    }

    return NOERROR;
}

ECode CPppoeService::GetPppoeState(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    *val = mPppoeState;
    return NOERROR;
}

ECode CPppoeService::UpdatePppoeDevInfo(
    /* [in] */ IPppoeDevInfo* info)
{
    AutoLock lock(this);
    const AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Boolean result;
    String ifName, ipAddress, dnsAddr, netMask, routeAddr;
    info->GetIfName(&ifName);
    info->GetIpAddress(&ipAddress);
    info->GetDnsAddr(&dnsAddr);
    info->GetRouteAddr(&routeAddr);
    info->GetNetMask(&netMask);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->PutInt32(cr, ISettingsSecure::PPPOE_CONF, 1, &result);
    settingsSecure->PutString(cr, ISettingsSecure::PPPOE_IFNAME, ifName, &result);
    settingsSecure->PutString(cr, ISettingsSecure::PPPOE_IP, ipAddress, &result);
    settingsSecure->PutString(cr, ISettingsSecure::PPPOE_DNS, dnsAddr, &result);
    settingsSecure->PutString(cr, ISettingsSecure::PPPOE_ROUTE, routeAddr, &result);
    settingsSecure->PutString(cr, ISettingsSecure::PPPOE_MASK, netMask, &result);
    return NOERROR;
}

ECode CPppoeService::IsPppoeConfigured(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    const AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 x;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetInt32(cr, ISettingsSecure::PPPOE_CONF, 0, &x);
    Boolean result2;
    if (x == 0) {
        settingsSecure->PutString(cr, ISettingsSecure::PPPOE_IFNAME, String("ppp0"), &result2);
        settingsSecure->PutString(cr, ISettingsSecure::PPPOE_IP, String("0.0.0.0"), &result2);
        settingsSecure->PutString(cr, ISettingsSecure::PPPOE_DNS, String("0.0.0.0"), &result2);
        settingsSecure->PutString(cr, ISettingsSecure::PPPOE_MASK, String("255.255.255.0"), &result2);
        settingsSecure->PutString(cr, ISettingsSecure::PPPOE_ROUTE, String("0.0.0.0"), &result2);
        Slogger::I(TAG, "@@@@@@NO CONFIG. set default");
        settingsSecure->PutInt32(cr, ISettingsSecure::PPPOE_CONF, 1, &result2);
    }

    settingsSecure->GetInt32(cr, ISettingsSecure::PPPOE_CONF, 0, &x);

    if (x == 1)
        *result = TRUE;
    else
        *result = FALSE;
    return NOERROR;
}

ECode CPppoeService::GetSavedPppoeConfig(
    /* [out] */ IPppoeDevInfo** info)
{
    VALIDATE_NOT_NULL(info);
    AutoLock lock(this);
    Boolean result;
    IsPppoeConfigured(&result);
    if (result) {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<IPppoeDevInfo> localInfo;
        FAIL_RETURN(CPppoeDevInfo::New((IPppoeDevInfo**)&localInfo));
        String ifName, ipAddress, dnsAddr, netMask, routeAddr;
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        settingsSecure->GetString(cr, ISettingsSecure::PPPOE_IFNAME, &ifName);
        settingsSecure->GetString(cr, ISettingsSecure::PPPOE_IP, &ipAddress);
        settingsSecure->GetString(cr, ISettingsSecure::PPPOE_DNS, &dnsAddr);
        settingsSecure->GetString(cr, ISettingsSecure::PPPOE_MASK, &netMask);
        settingsSecure->GetString(cr, ISettingsSecure::PPPOE_ROUTE, &routeAddr);
        localInfo->SetIfName(ifName);
        localInfo->SetIpAddress(ipAddress);
        localInfo->SetDnsAddr(dnsAddr);
        localInfo->SetNetMask(netMask);
        localInfo->SetRouteAddr(routeAddr);
        *info = localInfo;
        REFCOUNT_ADD(*info);
    }
    else {
        *info = NULL;
    }
    return NOERROR;
}

ECode CPppoeService::GetTotalInterface(
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    AutoPtr<IPppoeNative> pppoeNative;
    FAIL_RETURN(CPppoeNative::AcquireSingleton((IPppoeNative**)&pppoeNative));
    return pppoeNative->GetInterfaceCnt(val);
}

ECode CPppoeService::SetPppoeMode(
    /* [in] */ const String& mode)
{
    AutoLock lock(this);
    const AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 devCount = DevName->GetLength();
    for(Int32 i = 0; i < devCount; i++) {
        Slogger::I(TAG,"Set pppoe mode %s mode %s", (*DevName)[i].string(), mode.string());
    }
    Boolean result;
    if (DevName != NULL) {
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        settingsSecure->PutString(cr, ISettingsSecure::PPPOE_IFNAME, (*DevName)[0], &result);
        settingsSecure->PutInt32(cr, ISettingsSecure::PPPOE_CONF, 1, &result);
    }
    return NOERROR;
}

ECode CPppoeService::IsPppoeDeviceUp(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    ECode ec;
    AutoPtr<IFileReader> fr;
    String fileName = String("/sys/class/net/ppp0/operstate");
    ec = CFileReader::New(fileName, (IFileReader**)&fr);
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "get PppoeDevice status error: IOException when read: %s", fileName.string());
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IBufferedReader> br;
    ec =CBufferedReader::New(fr, 32, (IBufferedReader**)&br);
    AutoPtr<ICloseable> closeableFr = (ICloseable*)fr->Probe(EIID_ICloseable);
    AutoPtr<ICloseable> closeableBr = (ICloseable*)br->Probe(EIID_ICloseable);
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "get PppoeDevice status error:IOException when read: %s", fileName.string());
        *result = FALSE;
        closeableFr->Close();
        return NOERROR;
    }
    String status;
    ec = br->ReadLine(&status);
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "get PppoeDevice status error:IO Exception when read: ", fileName.string());
        closeableBr->Close();
        closeableFr->Close();
        *result = FALSE;
        return NOERROR;
    }
    if (status != NULL && status.Equals(String("up"))) {
        Slogger::D(TAG, "PppoeDevice status: %s", status.string());
        *result = TRUE;
    }
    else if (status != NULL && status.Equals(String("down"))) {
        Slogger::D(TAG, "PppoeDevice status: %s", status.string());
        *result = FALSE;
    }
    else {
        *result = FALSE;
    }
    closeableBr->Close();
    closeableFr->Close();
// } catch (IOException e) {
//     Slog.d(TAG, "get PppoeDevice status error");
//     return FALSE;
// }
    return NOERROR;

}

ECode CPppoeService::GetDhcpInfo(
    /* [out] */ IDhcpInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return mTracker->GetDhcpInfo(info);
}

ECode CPppoeService::RegisterForBroadcasts()
{
    AutoPtr<IIntentFilter> intentFilter;
    FAIL_RETURN(CIntentFilter::New((IIntentFilter**)&intentFilter));
    intentFilter->AddAction(IIntent::ACTION_SCREEN_ON);
    intentFilter->AddAction(IIntent::ACTION_SCREEN_OFF);
    AutoPtr<IIntent> intent;
    return mContext->RegisterReceiver(mReceiver, intentFilter, (IIntent**)&intent);
}

Int32 CPppoeService::ScanPppoeDevice()
{
    Int32 i = 0,j;
    AutoPtr<IPppoeNative> pppoeNative;
    FAIL_RETURN((CPppoeNative::AcquireSingleton((IPppoeNative**)&pppoeNative)));
    Int32 interfaceCnt;
    pppoeNative->GetInterfaceCnt(&interfaceCnt);
    if ((i = interfaceCnt) != 0) {
        Slogger::I(TAG, "total found %d net devices", i);
        DevName = ArrayOf<String>::Alloc(i);
    }
    else
        return i;

    for (j = 0; j < i; j++) {
        pppoeNative->GetInterfaceName(j, &((*DevName)[j]));
        if ((*DevName)[j] == NULL)
            break;
        Slogger::I(TAG,"device %d name %s", j, (*DevName)[j].string());
    }

    return i;
}

Int32 CPppoeService::GetPersistedState()
{
    const AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 val;
    //try {
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetInt32(cr, ISettingsSecure::PPPOE_ON, IPppoeManager::PPPOE_STATE_UNKNOWN, &val);
    //} catch (Settings::SettingNotFoundException e) {
    //    return PppoeManager.PPPOE_STATE_UNKNOWN;
    //}
    return val;
}

ECode CPppoeService::PersistPppoeEnabled(Boolean enabled)
{
    AutoLock lock(this);
    const AutoPtr<IContentResolver> cr;
    Boolean result;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    return settingsSecure->PutInt32(cr, ISettingsSecure::PPPOE_ON,
    enabled ? IPppoeManager::PPPOE_STATE_ENABLED : IPppoeManager::PPPOE_STATE_DISABLED, &result);
}

} // namespace Server
} // namespace Droid
} // namespace Elastos


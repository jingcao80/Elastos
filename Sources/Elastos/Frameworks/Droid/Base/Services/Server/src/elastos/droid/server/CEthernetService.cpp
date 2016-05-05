
#include "CEthernetService.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::Net::IInetAddress;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Net::IInterfaceConfiguration;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::Ethernet::CEthernetDevInfo;
using Elastos::Droid::Net::Ethernet::IEthernetManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CEthernetService::TAG("EthernetService");
const Int32 CEthernetService::ETHERNET_HAS_CONFIG = 1;
const Boolean CEthernetService::sLocalLOGV = TRUE;

CEthernetService::EthReceiver::EthReceiver(
    /* [in] */ CEthernetService* host)
    : mHost(host)
{}

ECode CEthernetService::EthReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->HandleReceive(context, intent);
    return NOERROR;
}

CEthernetService::CEthernetService()
    : mEthState(IEthernetManager::ETHERNET_STATE_DISABLED)
    , mIsEthEnabled(0)
    , SYS_NET("/sys/class/net/")
    , ETH_USED("")
{}

ECode CEthernetService::IsConfigured(
    /* [out] */ Boolean* isConfigured)
{
    VALIDATE_NOT_NULL(isConfigured);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 value;

    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetInt32(cr, ISettingsSecure::ETHERNET_CONF, 0, &value);

    *isConfigured = value == ETHERNET_HAS_CONFIG;

    return NOERROR;
}

ECode CEthernetService::GetSavedConfig(
    /* [out] */ IEthernetDevInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    AutoLock lock(this);

    Boolean isConfigured;
    if (IsConfigured(&isConfigured), isConfigured) {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<IEthernetDevInfo> infoTemp;
        FAIL_RETURN(CEthernetDevInfo::New((IEthernetDevInfo**)&infoTemp));

        String str;
        Int32 ival;

        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        settingsSecure->GetInt32(cr, ISettingsSecure::ETHERNET_MODE, &ival);
        infoTemp->SetConnectMode(ival);

        settingsSecure->GetString(cr, ISettingsSecure::ETHERNET_IFNAME, &str);
        infoTemp->SetIfName(str);

        settingsSecure->GetString(cr, ISettingsSecure::ETHERNET_IP, &str);
        infoTemp->SetIpAddress(str);

        settingsSecure->GetString(cr, ISettingsSecure::ETHERNET_DNS, &str);
        infoTemp->SetDnsAddr(str);

        settingsSecure->GetString(cr, ISettingsSecure::ETHERNET_MASK, &str);
        infoTemp->SetNetMask(str);

        settingsSecure->GetString(cr, ISettingsSecure::ETHERNET_ROUTE, &str);
        infoTemp->SetGateWay(str);

        *info = infoTemp;
        REFCOUNT_ADD(*info)
        return NOERROR;
    }

    return NOERROR;
}

ECode CEthernetService::SetMode(
    /* [in] */ Int32 mode)
{
    AutoLock lock(this);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    if (mDevName != NULL) {
        Boolean result;
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        settingsSecure->PutString(cr, ISettingsSecure::ETHERNET_IFNAME, (*mDevName)[0], &result);
        settingsSecure->PutInt32(cr, ISettingsSecure::ETHERNET_CONF, 1, &result);
        settingsSecure->PutInt32(cr, ISettingsSecure::ETHERNET_MODE, mode, &result);
    }
    return NOERROR;
}

ECode CEthernetService::UpdateDevInfo(
    /* [in] */ IEthernetDevInfo* info)
{
    AutoLock lock(this);
    assert(info);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Boolean result;
    String str, name;

    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->PutInt32(cr, ISettingsSecure::ETHERNET_CONF, 1, &result);

    info->GetIfName(&name);
    settingsSecure->PutString(cr, ISettingsSecure::ETHERNET_IFNAME, name, &result);

    info->GetIpAddress(&str);
    settingsSecure->PutString(cr, ISettingsSecure::ETHERNET_IP, str, &result);

    Int32 ival;
    info->GetConnectMode(&ival);
    settingsSecure->PutInt32(cr, ISettingsSecure::ETHERNET_MODE, ival, &result);

    info->GetDnsAddr(&str);
    settingsSecure->PutString(cr, ISettingsSecure::ETHERNET_DNS, str, &result);

    info->GetGateWay(&str);
    settingsSecure->PutString(cr, ISettingsSecure::ETHERNET_ROUTE, str, &result);

    info->GetNetMask(&str);
    settingsSecure->PutString(cr, ISettingsSecure::ETHERNET_MASK, str, &result);

    if (mEthState == IEthernetManager::ETHERNET_STATE_ENABLED) {
        Boolean result;
        mTracker->Teardown(&result);
        mTracker->Reconnect(&result);
        /*
        try {
            mNMService.setInterfaceUp(info.getIfName());
        } catch (RemoteException e) {
            Log.e(TAG, "setInterfaceUp is error: " + e);
        }
        */
    }
    if (mDeviceMap.Find(name) != mDeviceMap.End()){
        mDeviceMap[name] = info;
    }

    return NOERROR;
}

ECode CEthernetService::GetTotalInterface(
    /* [out] */ Int32* cnt)
{
    VALIDATE_NOT_NULL(cnt);
    {
        AutoLock lock(mDeviceMapLock);
        *cnt = mDeviceMap.GetSize();
    }
    return NOERROR;
}

void CEthernetService::ScanDevice()
{
    AutoPtr<ArrayOf<String> > devices;
    // try{
        mNMService->ListInterfaces((ArrayOf<String>**)&devices);
        Int32 length = devices != NULL ? devices->GetLength() : 0;
        for (Int32 i = 0; i < length; i++) {
            String iface = (*devices)[i];
            if (IsEth(iface)){
                AutoPtr<IEthernetDevInfo> value;
                CEthernetDevInfo::New((IEthernetDevInfo**)&value);
                AutoPtr<IInterfaceConfiguration> config;
                if (FAILED(mNMService->GetInterfaceConfig(iface, (IInterfaceConfiguration**)&config)))
                    return;
                value->SetIfName(iface);
                String hwAddr;
                config->GetHardwareAddress(&hwAddr);
                value->SetHwaddr(hwAddr);
                AutoPtr<ILinkAddress> la;
                config->GetLinkAddress((ILinkAddress**)&la);
                AutoPtr<IInetAddress> iaddr;
                la->GetAddress((IInetAddress**)&iaddr);
                String ipAddr;
                iaddr->GetHostAddress(&ipAddr);
                value->SetIpAddress(ipAddr);
                {
                    AutoLock lock(mDeviceMapLock);
                    mDeviceMap[iface] = value;
                }
            }
        }
    // } catch (Exception e) {
    //     Log.e(TAG, "Could not get Config of interfaces " + e);
    // }
}

Boolean CEthernetService::IsEth(
    /* [in] */ const String& ifname)
{
    if (ifname.StartWith("sit") || ifname.StartWith("lo") || ifname.StartWith("ppp")
        || ifname.StartWith("ippp") || ifname.StartWith("tun")
        || ifname.StartWith("gre") || ifname.StartWith("ip6"))
        return FALSE;

    AutoPtr<IFile> file;
    CFile::New(SYS_NET + ifname + "/phy80211", (IFile**)&file);
    Boolean exists;
    file->Exists(&exists);
    if (exists)
        return FALSE;

    file = NULL;
    CFile::New(SYS_NET + ifname + "/wireless", (IFile**)&file);
    file->Exists(&exists);
    if (exists)
        return FALSE;

    file = NULL;
    CFile::New(SYS_NET + ifname + "/wimax", (IFile**)&file);
    file->Exists(&exists);
    if (exists)
        return FALSE;

    file = NULL;
    CFile::New(SYS_NET + ifname + "/bridge", (IFile**)&file);
    file->Exists(&exists);
    if (exists)
        return FALSE;

    return TRUE;
}

void CEthernetService::HandleReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IEthernetManager::NETWORK_STATE_CHANGED_ACTION.Equals(action)) {
        AutoPtr<IParcelable> p;
        intent->GetParcelableExtra(
            IEthernetManager::EXTRA_LINK_PROPERTIES, (IParcelable**)&p);
        AutoPtr<ILinkProperties> linkProperties = ILinkProperties::Probe(p);
        Int32 event;
        intent->GetInt32Extra(
            IEthernetManager::EXTRA_ETHERNET_STATE,
            IEthernetManager::EVENT_CONFIGURATION_SUCCEEDED, &event);

        switch(event){
        case IEthernetManager::EVENT_CONFIGURATION_SUCCEEDED:
        case IEthernetManager::EVENT_CONFIGURATION_FAILED:
        case IEthernetManager::EVENT_DISCONNECTED:
            break;
        default:
            break;
        }
    }
}

ECode CEthernetService::GetDeviceNameList(
    /* [out] */ IObjectContainer** list)
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;
    {
        AutoLock lock(mDeviceMapLock);
        if (mDeviceMap.IsEmpty())
            return NOERROR;

        CParcelableObjectContainer::New(list);
        HashMap<String, AutoPtr<IEthernetDevInfo> >::Iterator iter;
        for (iter = mDeviceMap.Begin(); iter != mDeviceMap.End(); ++iter) {
            (*list)->Add(iter->mSecond.Get());
        }
    }

    return NOERROR;
}

void CEthernetService::PersistEnabled(
    /* [in] */ Boolean enabled)
{
    AutoLock lock(this);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Boolean result;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->PutInt32(cr, ISettingsSecure::ETHERNET_ON,
            enabled ? IEthernetManager::ETHERNET_STATE_ENABLED
            : IEthernetManager::ETHERNET_STATE_DISABLED, &result);
}

ECode CEthernetService::SetState(
    /* [in] */ Int32 state)
{
    AutoLock lock(this);

    if (mEthState != state) {
        mEthState = state;
        if (state == IEthernetManager::ETHERNET_STATE_DISABLED) {
            PersistEnabled(FALSE);
            Boolean result;
            mTracker->Teardown(&result);
        }
        else if (state == IEthernetManager::ETHERNET_STATE_ENABLED){
            PersistEnabled(TRUE);
            Boolean result;
            mTracker->Reconnect(&result);
        }
    }
    return NOERROR;
}

ECode CEthernetService::GetState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    Boolean isOn;
    IsOn(&isOn);
    mEthState = isOn ? IEthernetManager::ETHERNET_STATE_ENABLED
                : IEthernetManager::ETHERNET_STATE_DISABLED;
    *state = mEthState;
    return NOERROR;
}

void CEthernetService::SendChangedBroadcast(
    /* [in] */ IEthernetDevInfo* info,
    /* [in] */ Int32 event)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IEthernetManager::ETHERNET_STATE_CHANGED_ACTION, (IIntent**)&intent);
    intent->AddFlags(
        IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT
        | IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    intent->PutExtra(
        IEthernetManager::EXTRA_ETHERNET_INFO, IParcelable::Probe(info));
    intent->PutExtra(IEthernetManager::EXTRA_ETHERNET_STATE, event);
    mContext->SendStickyBroadcast(intent);
}

ECode CEthernetService::AddInterfaceToService(
    /* [in] */ const String& iface,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    if (!IsEth(iface))
        return NOERROR;
    if (ETH_USED.IsEmpty())
        ETH_USED = iface;

    AutoPtr<IFile> file;
    CFile::New(SYS_NET + iface + "/ifindex", (IFile**)&file);
    Boolean exists;
    file->Exists(&exists);
    if (exists)
        return NOERROR;


    AutoLock lock(mDeviceMapLock);
    //try{
    if (mDeviceMap.Find(iface) == mDeviceMap.End()) {
        AutoPtr<IEthernetDevInfo> value;
        CEthernetDevInfo::New((IEthernetDevInfo**)&value);
        AutoPtr<IInterfaceConfiguration> config;
        mNMService->GetInterfaceConfig(iface, (IInterfaceConfiguration**)&config);
        value->SetIfName(iface);
        String hwAddr;
        config->GetHardwareAddress(&hwAddr);
        value->SetHwaddr(hwAddr);
        AutoPtr<ILinkAddress> la;
        config->GetLinkAddress((ILinkAddress**)&la);
        AutoPtr<IInetAddress> iaddr;
        la->GetAddress((IInetAddress**)&iaddr);
        String ipAddr;
        iaddr->GetHostAddress(&ipAddr);
        value->SetIpAddress(ipAddr);
        mDeviceMap[iface] = value;
        SendChangedBroadcast(value, IEthernetManager::EVENT_NEWDEV);
    }
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Can't get the Interface Configure" + e);
    // }

    *res = TRUE;
    return NOERROR;
}

ECode CEthernetService::RemoveInterfaceFormService(
    /* [in] */ const String& name)
{
    if (!IsEth(name))
        return NOERROR;

    AutoLock lock(mDeviceMapLock);
    if (mDeviceMap.Find(name) != mDeviceMap.End()){
        SendChangedBroadcast(mDeviceMap[name], IEthernetManager::EVENT_DEVREM);
        mDeviceMap.Erase(name);
    }

    return NOERROR;
}

ECode CEthernetService::IsOn(
    /* [out] */ Boolean* isOn)
{
    VALIDATE_NOT_NULL(isOn);

    AutoLock lock(this);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    // try{
    Int32 ival;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    ECode ec = settingsSecure->GetInt32(cr, ISettingsSecure::ETHERNET_ON, &ival);
    if (FAILED(ec)) {
        *isOn = FALSE;
        return NOERROR;
    }
    // } catch (Settings.SettingNotFoundException e) {
    //     return FALSE;
    // }
    *isOn =  ival == 0 ? FALSE : TRUE;
    return NOERROR;
}

ECode CEthernetService::IsDhcp(
    /* [out] */ Boolean* isDhcp)
{
    VALIDATE_NOT_NULL(isDhcp);

    AutoLock lock(this);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    // try{
    Int32 ival;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    ECode ec = settingsSecure->GetInt32(cr, ISettingsSecure::ETHERNET_MODE, &ival);
    if (FAILED(ec)) {
        *isDhcp = FALSE;
        return NOERROR;
    }
    // } catch (Settings.SettingNotFoundException e) {
    //     return FALSE;
    // }
    *isDhcp = ival == 0 ? FALSE : TRUE;

    return NOERROR;
}

ECode CEthernetService::CheckLink(
    /* [in] */ const String& ifname,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = 0;
    AutoPtr<IFileInputStream> fstream;
    String s;
    do {
        AutoPtr<IFile> file;
        CFile::New(SYS_NET + ifname, (IFile**)&file);
        Boolean exists;
        file->Exists(&exists);
        if (!exists) {
            *ret = -1;
            return NOERROR;
        }

        if (FAILED(CFileInputStream::New(SYS_NET + ifname + "/carrier", (IFileInputStream**)&fstream)))
            break;
        AutoPtr<IDataInputStream> in;
        if (FAILED(CDataInputStream::New(fstream, (IDataInputStream**)&in)))
            break;
        AutoPtr<IInputStreamReader> isr;
        if (FAILED(CInputStreamReader::New(in, (IInputStreamReader**)&isr)))
            break;
        AutoPtr<IBufferedReader> br;
        if (FAILED(CBufferedReader::New(isr, (IBufferedReader**)&br)))
            break;

        br->ReadLine(&s);
    } while (0);

    if (fstream != NULL) {
        ICloseable::Probe(fstream)->Close();
    }

    if (!s.IsNull() && s.Equals("1"))
        *ret = 1;
    return NOERROR;
}

ECode CEthernetService::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result =  String("CEthernetService");
    return NOERROR;
}

ECode CEthernetService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    mNMService = IINetworkManagementService::Probe(
        ServiceManager::GetService(IContext::NETWORKMANAGEMENT_SERVICE);
    mTracker = EthernetDataTracker::GetInstance();
    mEthStateReceiver = new EthReceiver(this);
    CIntentFilter::New((IIntentFilter**)&mFilter);
    mFilter->AddAction(IEthernetManager::NETWORK_STATE_CHANGED_ACTION);
    ScanDevice();
    if (sLocalLOGV == TRUE)
        Slogger::I(TAG, "EthernetService Starting......\n");

    AutoPtr<IIntent> res;
    context->RegisterReceiver(mEthStateReceiver, mFilter, (IIntent**)&res);

    return NOERROR;
}

} // namespace Server
} // namepsace Droid
} // namespace Elastos

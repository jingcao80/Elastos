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

#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/net/VpnService.h"
#include "elastos/droid/internal/net/CVpnConfig.h"
#include "elastos/droid/net/CLinkAddress.h"
#include "elastos/droid/net/CRouteInfo.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/system/OsConstants.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IService;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::Net::CVpnConfig;
using Elastos::Droid::Internal::Net::IVpnConfig;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkUtils;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Os::Build;
// using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::System::OsConstants;

using Elastos::Core::CObject;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IDatagramSocket;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInet6Address;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetAddressHelper;
using Elastos::Net::ISocket;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

//=============================================================
// VpnService::Callback
//=============================================================
VpnService::Callback::Callback(
    /* [in] */ VpnService* host)
    : mHost(host)
{}
ECode VpnService::Callback::OnTransact(
    /* [in] */ Int32 code,
    /* [in] */ IParcel* data,
    /* [in] */ IParcel* reply,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (code == IBinder::LAST_CALL_TRANSACTION) {
        mHost->OnRevoke();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//=============================================================
// VpnService
//=============================================================
CAR_INTERFACE_IMPL(VpnService, Service, IVpnService)

ECode VpnService::constructor()
{
    return NOERROR;
}

AutoPtr<IIConnectivityManager> VpnService::GetService()
{
    AutoPtr<IServiceManager> mgr;
    // TODO: Waiting for CServiceManager
    assert(0);
    // CServiceManager::AcquireSingleton((IServiceManager**)&mgr);

    AutoPtr<IInterface> service;
    mgr->GetService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&service);

    AutoPtr<IIConnectivityManager> connService;
    connService = IIConnectivityManager::Probe(service);
    return connService;
}

ECode VpnService::Prepare(
    /* [in] */ IContext* context,
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result);

        // try {
    String packageName;
    context->GetPackageName(&packageName);
    Boolean prepareVpnOk;
    ECode ec = GetService()->PrepareVpn(packageName, String(NULL), &prepareVpnOk);
    if (FAILED(ec) && ec != (ECode)E_REMOTE_EXCEPTION) return ec;
    if (prepareVpnOk) {
        *result = NULL;
        return NOERROR;
    }
        // } catch (RemoteException e) {
            // ignore
        // }
    return CVpnConfig::GetIntentForConfirmation(result);
}

ECode VpnService::Protect(
    /* [in] */ Int32 socket,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return NetworkUtils::ProtectFromVpn(socket, result);
}

ECode VpnService::Protect(
    /* [in] */ ISocket* socket,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IFileDescriptor> descriptor;
    socket->GetFileDescriptor((IFileDescriptor**)&descriptor);

    Int32 fd;
    descriptor->GetDescriptor(&fd);

    return Protect(fd, result);
}

ECode VpnService::Protect(
    /* [in] */ IDatagramSocket* socket,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IFileDescriptor> descriptor;
    socket->GetFileDescriptor((IFileDescriptor**)&descriptor);

    Int32 fd;
    descriptor->GetDescriptor(&fd);

    return Protect(fd, result);
}

ECode VpnService::AddAddress(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 prefixLength,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Check(address, prefixLength);
        // try {
    String host;
    address->GetHostAddress(&host);
    ECode ec = GetService()->AddVpnAddress(host, prefixLength, result);
    if (!FAILED(ec)) return NOERROR;
        // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E("VpnService", "%d", ec);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return ec;
        // }
}

ECode VpnService::RemoveAddress(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 prefixLength,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Check(address, prefixLength);
        // try {
    String host;
    address->GetHostAddress(&host);
    ECode ec = GetService()->RemoveVpnAddress(host, prefixLength, result);
    if (!FAILED(ec)) return NOERROR;
        // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E("VpnService", "%d", ec);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
        // }
    return ec;
}

ECode VpnService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result);

    String action;
    intent->GetAction(&action);

    if (intent != NULL && IVpnService::SERVICE_INTERFACE.Equals(action)) {
        *result = IBinder::Probe(new Callback(this));
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    *result = NULL;
    return NOERROR;
}

ECode VpnService::OnRevoke()
{
    return StopSelf();
}

ECode VpnService::Check(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 prefixLength)
{
    Boolean loopbackAddr;
    if (address->IsLoopbackAddress(&loopbackAddr), loopbackAddr) {
        Logger::E("VpnService", "Bad address");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (IInet4Address::Probe(address) != NULL) {
        if (prefixLength < 0 || prefixLength > 32) {
            Logger::E("VpnService", "Bad prefixLength");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    } else if (IInet6Address::Probe(address) != NULL) {
        if (prefixLength < 0 || prefixLength > 128) {
            Logger::E("VpnService", "Bad prefixLength");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    } else {
        Logger::E("VpnService", "Unsupported family");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

//=====================================================
// VpnServiceBuilder
//=====================================================

CAR_INTERFACE_IMPL(VpnServiceBuilder, Object, IVpnServiceBuilder)

VpnServiceBuilder::VpnServiceBuilder()
    : mConfig(CreateConfig())
    , mAddresses(CreateList())
    , mRoutes(CreateList())
{}

AutoPtr<IVpnConfig> VpnServiceBuilder::CreateConfig()
{
    AutoPtr<IVpnConfig> rev;
    CVpnConfig::New((IVpnConfig**)&rev);
    return rev;

}

AutoPtr<IList> VpnServiceBuilder::CreateList()
{
    AutoPtr<IList> rev;
    CArrayList::New((IList**)&rev);
    return rev;
}

ECode VpnServiceBuilder::constructor(
    /* [in] */ IVpnService* host)
{
    mHost = (VpnService*) host;
    return mConfig->SetUser(String("VpnService")); // VpnService.this.getClass().getName();
}

ECode VpnServiceBuilder::SetSession(
    /* [in] */ const String& session)
{
    return mConfig->SetSession(session);
}

ECode VpnServiceBuilder::SetConfigureIntent(
    /* [in] */ IPendingIntent* intent)
{
    return mConfig->SetConfigureIntent(intent);
}

ECode VpnServiceBuilder::SetMtu(
    /* [in] */ Int32 mtu)
{
    if (mtu <= 0) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return mConfig->SetMtu(mtu);
}

ECode VpnServiceBuilder::AddAddress(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 prefixLength)
{
    mHost->Check(address, prefixLength);

    Boolean isAnyLocalAddress;
    address->IsAnyLocalAddress(&isAnyLocalAddress);

    if (isAnyLocalAddress) {
        Logger::D("VpnService", "Bad address");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ILinkAddress> linkAddress;
    CLinkAddress::New(address, prefixLength, (ILinkAddress**)&linkAddress);
    mAddresses->Add(linkAddress);
    mConfig->UpdateAllowedFamilies(address);

    return NOERROR;
}

ECode VpnServiceBuilder::AddAddress(
    /* [in] */ const String& address,
    /* [in] */ Int32 prefixLength)
{
    AutoPtr<IInetAddressHelper> addressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addressHelper);

    AutoPtr<IInetAddress> addressObj;
    addressHelper->ParseNumericAddress(address, (IInetAddress**)&addressObj);

    return AddAddress(addressObj, prefixLength);
}

ECode VpnServiceBuilder::AddRoute(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 prefixLength)
{
    mHost->Check(address, prefixLength);

    Int32 offset = prefixLength / 8;

    AutoPtr< ArrayOf<Byte> > bytes;
    address->GetAddress((ArrayOf<Byte>**)&bytes);

    Int32 length = bytes->GetLength();

    if (offset < length) {
        for ((*bytes)[offset] <<= prefixLength % 8; offset < length; ++offset) {
            if ((*bytes)[offset] != 0) {
                Logger::D("VpnService", "Bad address");
                return E_ILLEGAL_STATE_EXCEPTION;
            }
        }
    }
    AutoPtr<ILinkAddress> newLinkAddress;
    CLinkAddress::New(address, prefixLength, (ILinkAddress**)&newLinkAddress);
    AutoPtr<IRouteInfo> newRouteInfo;
    CRouteInfo::New(newLinkAddress, NULL, (IRouteInfo**)&newRouteInfo);
    mRoutes->Add(newRouteInfo);
    mConfig->UpdateAllowedFamilies(address);

    return NOERROR;
}

ECode VpnServiceBuilder::AddRoute(
    /* [in] */ const String& address,
    /* [in] */ Int32 prefixLength)
{
    AutoPtr<IInetAddressHelper> addressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addressHelper);

    AutoPtr<IInetAddress> addressObj;
    addressHelper->ParseNumericAddress(address, (IInetAddress**)&addressObj);

    return AddRoute(addressObj, prefixLength);
}

ECode VpnServiceBuilder::AddDnsServer(
    /* [in] */ IInetAddress* address)
{
    Boolean isLoopbackAddress;
    address->IsLoopbackAddress(&isLoopbackAddress);

    Boolean isAnyLocalAddress;
    address->IsAnyLocalAddress(&isAnyLocalAddress);

    if (isLoopbackAddress || isAnyLocalAddress) {
        Logger::D("VpnService", "Bad address");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IList> dnsServers;
    mConfig->GetDnsServers((IList**)&dnsServers);

    if (dnsServers == NULL) {
        CArrayList::New((IList**)&dnsServers);
        mConfig->SetDnsServers(dnsServers);
    }

    String hostAddress;
    address->GetHostAddress(&hostAddress);

    AutoPtr<ICharSequence> elmt;
    CString::New(hostAddress, (ICharSequence**)&elmt);
    return dnsServers->Add(elmt);
}

ECode VpnServiceBuilder::AddDnsServer(
    /* [in] */ const String& address)
{
    AutoPtr<IInetAddressHelper> addressHelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&addressHelper);

    AutoPtr<IInetAddress> addressObj;
    addressHelper->ParseNumericAddress(address, (IInetAddress**)&addressObj);

    return AddDnsServer(addressObj);
}

ECode VpnServiceBuilder::AddSearchDomain(
    /* [in] */ const String& domain)
{
    AutoPtr<IList> searchDomains;
    mConfig->GetSearchDomains((IList**)&searchDomains);

    if (searchDomains == NULL) {
        CArrayList::New((IList**)&searchDomains);
        mConfig->SetSearchDomains(searchDomains);
    }

    AutoPtr<ICharSequence> elmt;
    CString::New(domain, (ICharSequence**)&elmt);
    return searchDomains->Add(elmt);
}

ECode VpnServiceBuilder::AllowFamily(
    /* [in] */ Int32 family)
{
    if (family == OsConstants::_AF_INET) {
        mConfig->SetAllowIPv4(TRUE);
    } else if (family == OsConstants::_AF_INET6) {
        mConfig->SetAllowIPv6(TRUE);
    } else {
        Logger::E("VpnServiceBuilder", "%d is neither %d nor %d", family, OsConstants::_AF_INET, OsConstants::_AF_INET6);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode VpnServiceBuilder::VerifyApp(
    /* [in] */ const String& packageName)
{
    AutoPtr<IBinder> binder = IBinder::Probe(ServiceManager::GetService(String("package")));
    AutoPtr<IIPackageManager> pm = IIPackageManager::Probe(binder);
        // try {
    Int32 userId = UserHandle::GetCallingUserId();
    AutoPtr<IApplicationInfo> appInfo;
    ECode ec = pm->GetApplicationInfo(packageName, 0, userId, (IApplicationInfo**)&appInfo);
        // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::E("VpnService", "%d", ec);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        return ec;
    }
        // }
    return NOERROR;
}

ECode VpnServiceBuilder::AddAllowedApplication(
    /* [in] */ const String& packageName)
{
    AutoPtr<IList> list;
    mConfig->GetDisallowedApplications((IList**)&list);
    if (list != NULL) {
        Logger::E("VpnService", "addDisallowedApplication already called");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    VerifyApp(packageName);
    list = NULL;
    mConfig->GetAllowedApplications((IList**)&list);
    if (list == NULL) {
        AutoPtr<IList> newArrayList;
        CArrayList::New((IList**)&newArrayList);
        mConfig->SetAllowedApplications(newArrayList);
        list = newArrayList;
    }
    list->Add(StringUtils::ParseCharSequence(packageName));
    return NOERROR;
}

ECode VpnServiceBuilder::AddDisallowedApplication(
    /* [in] */ const String& packageName)
{
    AutoPtr<IList> list;
    mConfig->GetAllowedApplications((IList**)&list);
    if (list != NULL) {
        Logger::E("VpnService", "addAllowedApplication already called");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    VerifyApp(packageName);
    list = NULL;
    mConfig->GetDisallowedApplications((IList**)&list);
    if (list == NULL) {
        AutoPtr<IList> newArrayList;
        CArrayList::New((IList**)&newArrayList);
        mConfig->SetDisallowedApplications(newArrayList);
        list = newArrayList;
    }
    list->Add(StringUtils::ParseCharSequence(packageName));
    return NOERROR;
}

ECode VpnServiceBuilder::AllowBypass()
{
    return mConfig->SetAllowBypass(TRUE);
}

ECode VpnServiceBuilder::SetBlocking(
    /* [in] */ Boolean blocking)
{
    return mConfig->SetBlocking(blocking);
}

ECode VpnServiceBuilder::Establish(
    /* [out] */ IParcelFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result);

    mConfig->SetAddresses(mAddresses);
    mConfig->SetRoutes(mRoutes);

    AutoPtr<IParcelFileDescriptor> parcelFd;
    ECode ec = mHost->GetService()->EstablishVpn(mConfig, (IParcelFileDescriptor**)&parcelFd);
    if (FAILED(ec)) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos

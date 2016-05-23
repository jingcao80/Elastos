
#include "elastos/droid/server/net/NetlinkTracker.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/server/net/CDnsServerRepository.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Server::Net::CDnsServerRepository;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::IString;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

const Boolean NetlinkTracker::DBG = FALSE;

CAR_INTERFACE_IMPL(NetlinkTracker, BaseNetworkObserver, INetlinkTracker)

NetlinkTracker::NetlinkTracker()
{
}

ECode NetlinkTracker::constructor(
    /* [in] */ const String& iface,
    /* [in] */ INetlinkTrackerCallback* callback)
{
    TAG = String("NetlinkTracker/") + iface;
    mInterfaceName = iface;
    mCallback = callback;
    CLinkProperties::New((ILinkProperties**)&mLinkProperties);
    mLinkProperties->SetInterfaceName(mInterfaceName);
    CDnsServerRepository::New((IDnsServerRepository**)&mDnsServerRepository);
    return NOERROR;
}

void NetlinkTracker::MaybeLog(
    /* [in] */ const String& operation,
    /* [in] */ const String& iface,
    /* [in] */ ILinkAddress* address)
{
    if (DBG) {
        String str;
        IObject::Probe(address)->ToString(&str);
        Int32 flags, scope;
        address->GetFlags(&flags);
        address->GetScope(&scope);
        Logger::D(TAG, "%s: %s on %s flags %d scope %d" , operation.string(), str.string(), iface.string(), flags, scope);
    }
}

void NetlinkTracker::MaybeLog(
    /* [in] */ const String& operation,
    /* [in] */ IInterface* o)
{
    if (DBG) {
        String str;
        IObject::Probe(o)->ToString(&str);
        Logger::D(TAG, "%s: %s", operation.string(), str.string());
    }
}

ECode NetlinkTracker::AddressUpdated(
    /* [in] */ const String& iface,
    /* [in] */ ILinkAddress* address)
{
    if (mInterfaceName.Equals(iface)) {
        MaybeLog(String("addressUpdated"), iface, address);
        Boolean changed;
        {    AutoLock syncLock(this);
            mLinkProperties->AddLinkAddress(address, &changed);
        }
        if (changed) {
            AutoPtr<ILinkProperties> lp;
            CLinkProperties::New(mLinkProperties, (ILinkProperties**)&lp);
            mCallback->Update(lp);
        }
    }
    return NOERROR;
}

ECode NetlinkTracker::AddressRemoved(
    /* [in] */ const String& iface,
    /* [in] */ ILinkAddress* address)
{
    if (mInterfaceName.Equals(iface)) {
        MaybeLog(String("addressRemoved"), iface, address);
        Boolean changed;
        {    AutoLock syncLock(this);
            mLinkProperties->RemoveLinkAddress(address, &changed);
        }
        if (changed) {
            AutoPtr<ILinkProperties> lp;
            CLinkProperties::New(mLinkProperties, (ILinkProperties**)&lp);
            mCallback->Update(lp);
        }
    }
    return NOERROR;
}

ECode NetlinkTracker::RouteUpdated(
    /* [in] */ IRouteInfo* route)
{
    String result;
    route->GetInterface(&result);
    if (mInterfaceName.Equals(result)) {
        MaybeLog(String("routeUpdated"), IInterface::Probe(route));
        Boolean changed;
        {    AutoLock syncLock(this);
            mLinkProperties->AddRoute(route, &changed);
        }
        if (changed) {
            AutoPtr<ILinkProperties> lp;
            CLinkProperties::New(mLinkProperties, (ILinkProperties**)&lp);
            mCallback->Update(lp);
        }
    }
    return NOERROR;
}

ECode NetlinkTracker::RouteRemoved(
    /* [in] */ IRouteInfo* route)
{
    String result;
    route->GetInterface(&result);
    if (mInterfaceName.Equals(result)) {
        MaybeLog(String("routeRemoved"), IInterface::Probe(route));
        Boolean changed;
        {    AutoLock syncLock(this);
            mLinkProperties->RemoveRoute(route, &changed);
        }
        if (changed) {
            AutoPtr<ILinkProperties> lp;
            CLinkProperties::New(mLinkProperties, (ILinkProperties**)&lp);
            mCallback->Update(lp);
        }
    }
    return NOERROR;
}

ECode NetlinkTracker::InterfaceDnsServerInfo(
    /* [in] */ const String& iface,
    /* [in] */ Int64 lifetime,
    /* [in] */ ArrayOf<String>* addresses)
{
    if (mInterfaceName.Equals(iface)) {
        String str = Arrays::ToString(addresses);
        AutoPtr<IString> strObj;
        CString::New(str, (IString**)&strObj);
        MaybeLog(String("interfaceDnsServerInfo"), IInterface::Probe(strObj));
        Boolean changed;
        mDnsServerRepository->AddServers(lifetime, addresses, &changed);
        if (changed) {
            {    AutoLock syncLock(this);
                mDnsServerRepository->SetDnsServersOn(mLinkProperties);
            }
            AutoPtr<ILinkProperties> lp;
            CLinkProperties::New(mLinkProperties, (ILinkProperties**)&lp);
            mCallback->Update(lp);
        }
    }
    return NOERROR;
}

ECode NetlinkTracker::GetLinkProperties(
    /* [out] */ ILinkProperties** lp)
{
    VALIDATE_NOT_NULL(lp)
    {    AutoLock syncLock(this);
        CLinkProperties::New(mLinkProperties.Get(), lp);
    }
    return NOERROR;
}

ECode NetlinkTracker::ClearLinkProperties()
{
    {    AutoLock syncLock(this);
        // Clear the repository before clearing mLinkProperties. That way, if a clear() happens
        // while interfaceDnsServerInfo() is being called, we'll end up with no DNS servers in
        // mLinkProperties, as desired.
        CDnsServerRepository::New((IDnsServerRepository**)&mDnsServerRepository);
        mLinkProperties->Clear();
        mLinkProperties->SetInterfaceName(mInterfaceName);
    }
    return NOERROR;
}

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos

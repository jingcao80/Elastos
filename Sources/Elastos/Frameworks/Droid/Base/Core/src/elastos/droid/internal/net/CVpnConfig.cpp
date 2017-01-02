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

#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/internal/net/CVpnConfig.h"
#include "elastos/droid/app/CPendingIntentHelper.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/net/CLinkAddress.h"
#include "elastos/droid/net/CRouteInfo.h"
#include "elastos/droid/os/CParcel.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Net::CLinkAddress;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::CRouteInfo;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Net::IIpPrefix;
using Elastos::Droid::Os::UserHandle;
using Elastos::Core::StringUtils;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IInetAddressHelper;;
using Elastos::Net::IInet4Address;
using Elastos::Utility::CArrayList;

typedef Elastos::Droid::Os::CParcel CPARCEL;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CAR_INTERFACE_IMPL_2(CVpnConfig, Object, IVpnConfig, IParcelable)

CAR_OBJECT_IMPL(CVpnConfig)

CVpnConfig::CVpnConfig()
    : mMtu(-1)
    , mStartTime(-1)
    , mLegacy(FALSE)
    , mBlocking(FALSE)
    , mAllowBypass(FALSE)
    , mAllowIPv4(FALSE)
    , mAllowIPv6(FALSE)
{
    CArrayList::New((IList**)&mAddresses);
    CArrayList::New((IList**)&mRoutes);
}

ECode CVpnConfig::constructor()
{
    return NOERROR;
}

ECode CVpnConfig::GetIntentForConfirmation(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    CIntent::New(intent);
    String str;
    CResources::GetSystem()->GetString(
        R::string::config_customVpnConfirmDialogComponent, &str);
    AutoPtr<IComponentName> componentName;
    CComponentName::UnflattenFromString(str, (IComponentName**)&componentName);
    String pkgName, clsName;
    componentName->GetPackageName(&pkgName);
    componentName->GetClassName(&clsName);
    (*intent)->SetClassName(pkgName, clsName);
    return NOERROR;
}

ECode CVpnConfig::GetIntentForStatusPanel(
    /* [in] */ IContext* context,
    /* [out] */ IPendingIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    AutoPtr<IIntent> newIntent;
    CIntent::New((IIntent**)&newIntent);
    newIntent->SetClassName(DIALOGS_PACKAGE, DIALOGS_PACKAGE + ".ManageDialog");
    newIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_NO_HISTORY |
                IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    return helper->GetActivityAsUser(context, 0, newIntent, 0, NULL, UserHandle::CURRENT, intent);
}

ECode CVpnConfig::GetVpnLabel(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName,
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label)
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIntent> intent;
    CIntent::New(SERVICE_INTERFACE, (IIntent**)&intent);
    intent->SetPackage(packageName);
    AutoPtr<IList> services;
    FAIL_RETURN(pm->QueryIntentServices(intent, 0 /* flags */, (IList**)&services))
    Int32 size;
    if (services != NULL && (services->GetSize(&size), size) == 1) {
        // This app contains exactly one VPN service. Call loadLabel, which will attempt to
        // load the service's label, and fall back to the app label if none is present.
        AutoPtr<IInterface> item;
        services->Get(0, (IInterface**)&item);
        return IResolveInfo::Probe(item)->LoadLabel(pm, label);
    }
    else {
        AutoPtr<IApplicationInfo> appInfo;
        pm->GetApplicationInfo(packageName, 0, (IApplicationInfo**)&appInfo);
        return IPackageItemInfo::Probe(appInfo)->LoadLabel(pm, label);
    }
}

ECode CVpnConfig::UpdateAllowedFamilies(
    /* [in] */ IInetAddress* address)
{
    if (IInet4Address::Probe(address)) {
        mAllowIPv4 = TRUE;
    }
    else {
        mAllowIPv6 = TRUE;
    }
    return NOERROR;
}

ECode CVpnConfig::AddLegacyRoutes(
    /* [in] */ const String& routesStr)
{
    if (routesStr.Trim().Equals("")) {
        return NOERROR;
    }
    AutoPtr<ArrayOf<String> > routes;
    StringUtils::Split(routesStr.Trim(), " ", (ArrayOf<String>**)&routes);
    for (Int32 i = 0; i < routes->GetLength(); i++) {
        String route = (*routes)[i];
        //each route is ip/prefix
        AutoPtr<ArrayOf<String> > split;
        StringUtils::Split(route, "/", (ArrayOf<String>**)&split);
        AutoPtr<IInetAddressHelper> helper;
        CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
        AutoPtr<IInetAddress> interAddr;
        helper->ParseNumericAddress((*split)[0], (IInetAddress**)&interAddr);
        AutoPtr<ILinkAddress> address;
        CLinkAddress::New(interAddr, StringUtils::ParseInt32((*split)[1]), (ILinkAddress**)&address);
        AutoPtr<IRouteInfo> info;
        CRouteInfo::New(address, NULL, (IRouteInfo**)&info);
        mRoutes->Add(info);
        AutoPtr<IIpPrefix> dest;
        info->GetDestination((IIpPrefix**)&dest);
        AutoPtr<IInetAddress> addr;
        dest->GetAddress((IInetAddress**)&addr);
        UpdateAllowedFamilies(addr);
    }
    return NOERROR;
}

ECode CVpnConfig::AddLegacyAddresses(
    /* [in] */ const String& addressesStr)
{
    if (addressesStr.Trim().Equals("")) {
        return NOERROR;
    }
    AutoPtr<ArrayOf<String> > addresses;
    StringUtils::Split(addressesStr.Trim(), " ", (ArrayOf<String>**)&addresses);
    for (Int32 i = 0; i < addresses->GetLength(); i++) {
        String address = (*addresses)[i];
        //each address is ip/prefix
        AutoPtr<ArrayOf<String> > split;
        StringUtils::Split(address, "/", (ArrayOf<String>**)&split);
        AutoPtr<IInetAddressHelper> helper;
        CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
        AutoPtr<IInetAddress> interAddr;
        helper->ParseNumericAddress((*split)[0], (IInetAddress**)&interAddr);
        AutoPtr<ILinkAddress> addr;
        CLinkAddress::New(interAddr, StringUtils::ParseInt32((*split)[1]), (ILinkAddress**)&addr);
        mRoutes->Add(addr);
        AutoPtr<IInetAddress> add;
        addr->GetAddress((IInetAddress**)&add);
        UpdateAllowedFamilies(add);
    }
    return NOERROR;
}

ECode CVpnConfig::GetUser(
    /* [out] */ String* user)
{
    VALIDATE_NOT_NULL(user);
    *user = mUser;
    return NOERROR;
}

ECode CVpnConfig::SetUser(
    /* [in] */ const String& user)
{
    mUser = user;
    return NOERROR;
}

ECode CVpnConfig::GetInterfaze(
    /* [out] */ String* interfaze)
{
    VALIDATE_NOT_NULL(interfaze);
    *interfaze = mInterfaze;
    return NOERROR;
}

ECode CVpnConfig::SetInterfaze(
    /* [in] */ const String& interfaze)
{
    mInterfaze = interfaze;
    return NOERROR;
}

ECode CVpnConfig::GetSession(
    /* [out] */ String* session)
{
    VALIDATE_NOT_NULL(session);
    *session = mSession;
    return NOERROR;
}

ECode CVpnConfig::SetSession(
    /* [in] */ const String& session)
{
    mSession = session;
    return NOERROR;
}

ECode CVpnConfig::GetMtu(
    /* [out] */ Int32* mtu)
{
    VALIDATE_NOT_NULL(mtu);
    *mtu = mMtu;
    return NOERROR;
}

ECode CVpnConfig::SetMtu(
    /* [in] */ Int32 mtu)
{
    mMtu = mtu;
    return NOERROR;
}

ECode CVpnConfig::GetAddresses(
    /* [out] */ IList** addresses)
{
    VALIDATE_NOT_NULL(addresses);
    *addresses = mAddresses;
    REFCOUNT_ADD(*addresses);
    return NOERROR;
}

ECode CVpnConfig::SetAddresses(
    /* [in] */ IList* addresses)
{
    mAddresses = addresses;
    return NOERROR;
}

ECode CVpnConfig::GetRoutes(
    /* [out] */ IList** routes)
{
    VALIDATE_NOT_NULL(routes);
    *routes = mRoutes;
    REFCOUNT_ADD(*routes);
    return NOERROR;
}

ECode CVpnConfig::SetRoutes(
    /* [in] */ IList* routes)
{
    mRoutes = routes;
    return NOERROR;
}

ECode CVpnConfig::GetDnsServers(
    /* [out] */ IList** dnsServers)
{
    VALIDATE_NOT_NULL(dnsServers);

    *dnsServers = mDnsServers;
    REFCOUNT_ADD(*dnsServers);
    return NOERROR;
}

ECode CVpnConfig::SetDnsServers(
    /* [in] */ IList* dnsServers)
{
    mDnsServers = dnsServers;
    return NOERROR;
}

ECode CVpnConfig::GetSearchDomains(
    /* [out] */ IList** searchDomains)
{
    VALIDATE_NOT_NULL(searchDomains);

    *searchDomains = mSearchDomains;
    REFCOUNT_ADD(*searchDomains);
    return NOERROR;
}

ECode CVpnConfig::SetSearchDomains(
    /* [in] */ IList* searchDomains)
{
    mSearchDomains = searchDomains;
    return NOERROR;
}

ECode CVpnConfig::GetAllowedApplications(
    /* [out] */ IList** allowedApplications)
{
    VALIDATE_NOT_NULL(allowedApplications);

    *allowedApplications = mAllowedApplications;
    REFCOUNT_ADD(*allowedApplications);
    return NOERROR;
}

ECode CVpnConfig::SetAllowedApplications(
    /* [in] */ IList* allowedApplications)
{
    mAllowedApplications = allowedApplications;
    return NOERROR;
}

ECode CVpnConfig::GetDisallowedApplications(
    /* [out] */ IList** disallowedApplications)
{
    VALIDATE_NOT_NULL(disallowedApplications);

    *disallowedApplications = mDisallowedApplications;
    REFCOUNT_ADD(*disallowedApplications);
    return NOERROR;
}

ECode CVpnConfig::SetDisallowedApplications(
    /* [in] */ IList* disallowedApplications)
{
    mDisallowedApplications = disallowedApplications;
    return NOERROR;
}

ECode CVpnConfig::GetConfigureIntent(
    /* [out] */ IPendingIntent** configureIntent)
{
    VALIDATE_NOT_NULL(configureIntent);

    *configureIntent = mConfigureIntent;
    REFCOUNT_ADD(*configureIntent);
    return NOERROR;
}

ECode CVpnConfig::SetConfigureIntent(
    /* [in] */ IPendingIntent* configureIntent)
{
    mConfigureIntent = configureIntent;
    return NOERROR;
}

ECode CVpnConfig::GetStartTime(
    /* [out] */ Int64* startTime)
{
    VALIDATE_NOT_NULL(startTime);

    *startTime = mStartTime;
    return NOERROR;
}

ECode CVpnConfig::SetStartTime(
    /* [in] */ Int64 startTime)
{
    mStartTime = startTime;
    return NOERROR;
}

ECode CVpnConfig::GetLegacy(
    /* [out] */ Boolean* legacy)
{
    VALIDATE_NOT_NULL(legacy);

    *legacy = mLegacy;
    return NOERROR;
}

ECode CVpnConfig::SetLegacy(
    /* [in] */ Boolean legacy)
{
    mLegacy = legacy;
    return NOERROR;
}

ECode CVpnConfig::GetBlocking(
    /* [out] */ Boolean* blocking)
{
    VALIDATE_NOT_NULL(blocking);

    *blocking = mBlocking;
    return NOERROR;
}

ECode CVpnConfig::SetBlocking(
    /* [in] */ Boolean blocking)
{
    mBlocking = blocking;
    return NOERROR;
}

ECode CVpnConfig::GetAllowBypass(
    /* [out] */ Boolean* allowBypass)
{
    VALIDATE_NOT_NULL(allowBypass);

    *allowBypass = mAllowBypass;
    return NOERROR;
}

ECode CVpnConfig::SetAllowBypass(
    /* [in] */ Boolean allowBypass)
{
    mAllowBypass = allowBypass;
    return NOERROR;
}

ECode CVpnConfig::GetAllowIPv4(
    /* [out] */ Boolean* allowIPv4)
{
    VALIDATE_NOT_NULL(allowIPv4);

    *allowIPv4 = mAllowIPv4;
    return NOERROR;
}

ECode CVpnConfig::SetAllowIPv4(
    /* [in] */ Boolean allowIPv4)
{
    mAllowIPv4 = allowIPv4;
    return NOERROR;
}

ECode CVpnConfig::GetAllowIPv6(
    /* [out] */ Boolean* allowIPv6)
{
    VALIDATE_NOT_NULL(allowIPv6);

    *allowIPv6 = mAllowIPv6;
    return NOERROR;
}

ECode CVpnConfig::SetAllowIPv6(
    /* [in] */ Boolean allowIPv6)
{
    mAllowIPv6 = allowIPv6;
    return NOERROR;
}

ECode CVpnConfig::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mUser);
    source->ReadString(&mInterfaze);
    source->ReadString(&mSession);
    source->ReadInt32(&mMtu);
    CPARCEL::ReadTypedList(source, mAddresses);
    CPARCEL::ReadTypedList(source, mRoutes);
    mDnsServers = IList::Probe(CPARCEL::CreateStringArrayList(source));
    mSearchDomains = IList::Probe(CPARCEL::CreateStringArrayList(source));
    mAllowedApplications = IList::Probe(CPARCEL::CreateStringArrayList(source));
    mDisallowedApplications = IList::Probe(CPARCEL::CreateStringArrayList(source));
    mConfigureIntent = NULL;
    source->ReadInterfacePtr((Handle32*)&mConfigureIntent);
    source->ReadInt64(&mStartTime);
    source->ReadBoolean(&mLegacy);
    source->ReadBoolean(&mBlocking);
    source->ReadBoolean(&mAllowBypass);
    source->ReadBoolean(&mAllowIPv4);
    source->ReadBoolean(&mAllowIPv6);
    return NOERROR;
}

ECode CVpnConfig::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mUser);
    dest->WriteString(mInterfaze);
    dest->WriteString(mSession);
    dest->WriteInt32(mMtu);
    CPARCEL::WriteTypedList(dest, mAddresses);
    CPARCEL::WriteTypedList(dest, mRoutes);
    CPARCEL::WriteStringList(dest, mDnsServers);
    CPARCEL::WriteStringList(dest, mSearchDomains);
    CPARCEL::WriteStringList(dest, mAllowedApplications);
    CPARCEL::WriteStringList(dest, mDisallowedApplications);
    dest->WriteInterfacePtr(mConfigureIntent);
    dest->WriteInt64(mStartTime);
    dest->WriteBoolean(mLegacy);
    dest->WriteBoolean(mBlocking);
    dest->WriteBoolean(mAllowBypass);
    dest->WriteBoolean(mAllowIPv4);
    dest->WriteBoolean(mAllowIPv6);
    return NOERROR;
}

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos

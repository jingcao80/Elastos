
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/CConnectivityManager.h"
#include "elastos/droid/net/CNetwork.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/net/CNetworkCapabilities.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/os/CMessenger.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/R.h"

#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IITelephony;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::EIID_INetworkActivityListener;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::INetworkActivityListener;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Utility::CArrayMap;

using Elastos::Core::IThread;
using Elastos::Core::StringUtils;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IInetAddressHelper;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Libcore::Net::Event::CNetworkEventDispatcherHelper;
using Libcore::Net::Event::INetworkEventDispatcher;
using Libcore::Net::Event::INetworkEventDispatcherHelper;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_OBJECT_IMPL(CConnectivityManager)

CAR_INTERFACE_IMPL(CConnectivityManager, Object, IConnectivityManager)

AutoPtr<IHashMap> CreateHashMap()
{
    AutoPtr<IHashMap> rev;
    CHashMap::New((IHashMap**)&rev);
    return rev;
}

AutoPtr<IAtomicInteger32> CreateAtomicInterger32()
{
    AutoPtr<IAtomicInteger32> rev;
    CAtomicInteger32::New(0, (IAtomicInteger32**)&rev);
    return rev;
}

const String CConnectivityManager::TAG("ConnectivityManager");
const Boolean CConnectivityManager::LEGACY_DBG = TRUE;
const AutoPtr<IHashMap> CConnectivityManager::sLegacyRequests = CreateHashMap();
const Int32 CConnectivityManager::BASE = 0x00080000;
const Int32 CConnectivityManager::EXPIRE_LEGACY_REQUEST = BASE + 10;
const String CConnectivityManager::CallbackHandler::TAG("ConnectivityManager.CallbackHandler");
const AutoPtr<IHashMap> CConnectivityManager::sNetworkCallback = CreateHashMap();
const AutoPtr<IAtomicInteger32> CConnectivityManager::sCallbackRefCount = CreateAtomicInterger32();
AutoPtr<IHandler> CConnectivityManager::sCallbackHandler = NULL;
const Int32 CConnectivityManager::LISTEN  = 1;
const Int32 CConnectivityManager::REQUEST = 2;

CConnectivityManager::CConnectivityManager()
{
    CArrayMap::New((IArrayMap**)&mNetworkActivityListeners);
}

ECode CConnectivityManager::IsNetworkTypeValid(
    /* [in] */ Int32 networkType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = ((networkType >= 0) && (networkType <= MAX_NETWORK_TYPE));
    return NOERROR;
}

ECode CConnectivityManager::GetNetworkTypeName(
    /* [in] */ Int32 type,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    switch (type) {
        case TYPE_MOBILE:
            *result = String("MOBILE");
            break;
        case TYPE_WIFI:
            *result = String("WIFI");
            break;
        case TYPE_MOBILE_MMS:
            *result = String("MOBILE_MMS");
            break;
        case TYPE_MOBILE_SUPL:
            *result = String("MOBILE_SUPL");
            break;
        case TYPE_MOBILE_DUN:
            *result = String("MOBILE_DUN");
            break;
        case TYPE_MOBILE_HIPRI:
            *result = String("MOBILE_HIPRI");
            break;
        case TYPE_WIMAX:
            *result = String("WIMAX");
            break;
        case TYPE_BLUETOOTH:
            *result = String("BLUETOOTH");
            break;
        case TYPE_DUMMY:
            *result = String("DUMMY");
            break;
        case TYPE_ETHERNET:
            *result = String("ETHERNET");
            break;
        case TYPE_MOBILE_FOTA:
            *result = String("MOBILE_FOTA");
            break;
        case TYPE_MOBILE_IMS:
            *result = String("MOBILE_IMS");
            break;
        case TYPE_MOBILE_CBS:
            *result = String("MOBILE_CBS");
            break;
        case TYPE_WIFI_P2P:
            *result = String("WIFI_P2P");
            break;
        case TYPE_MOBILE_IA:
            *result = String("MOBILE_IA");
            break;
        case TYPE_MOBILE_EMERGENCY:
            *result = String("MOBILE_EMERGENCY");
            break;
        case TYPE_PROXY:
            *result = String("PROXY");
            break;
        default:
            *result = StringUtils::ToString(type);
            break;
    }
    return NOERROR;
}

ECode CConnectivityManager::IsNetworkTypeMobile(
    /* [in] */ Int32 networkType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    switch (networkType) {
        case TYPE_MOBILE:
        case TYPE_MOBILE_MMS:
        case TYPE_MOBILE_SUPL:
        case TYPE_MOBILE_DUN:
        case TYPE_MOBILE_HIPRI:
        case TYPE_MOBILE_FOTA:
        case TYPE_MOBILE_IMS:
        case TYPE_MOBILE_CBS:
        case TYPE_MOBILE_IA:
        case TYPE_MOBILE_EMERGENCY:
            *result = TRUE;
            break;
        default:
            *result = FALSE;
            break;
    }
    return NOERROR;
}

ECode CConnectivityManager::IsNetworkTypeWifi(
    /* [in] */ Int32 networkType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    switch (networkType) {
        case TYPE_WIFI:
        case TYPE_WIFI_P2P:
            *result = TRUE;
            break;
        default:
            *result = FALSE;
            break;
    }
    return NOERROR;
}

ECode CConnectivityManager::SetNetworkPreference(
    /* [in] */ Int32 preference)
{
    return NOERROR;
}

ECode CConnectivityManager::GetNetworkPreference(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TYPE_NONE;

    return NOERROR;
}

ECode CConnectivityManager::GetActiveNetworkInfo(
    /* [out] */ INetworkInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<INetworkInfo> networkInfo;
    ECode ec = mService->GetActiveNetworkInfo((INetworkInfo**)&networkInfo);
    if (FAILED(ec)) return NOERROR;
    *result = networkInfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::GetActiveNetworkInfoForUid(
    /* [in] */ Int32 uid,
    /* [out] */ INetworkInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<INetworkInfo> networkInfo;
    ECode ec = mService->GetActiveNetworkInfoForUid(uid, (INetworkInfo**)&networkInfo);
    if (FAILED(ec)) return NOERROR;
    *result = networkInfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::GetNetworkInfo(
    /* [in] */ Int32 networkType,
    /* [out] */ INetworkInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<INetworkInfo> networkInfo;
    ECode ec = mService->GetNetworkInfo(networkType, (INetworkInfo**)&networkInfo);
    if (FAILED(ec)) return NOERROR;
    *result = networkInfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::GetNetworkInfo(
    /* [in] */ INetwork* network,
    /* [out] */ INetworkInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    VALIDATE_NOT_NULL(network)

    AutoPtr<INetworkInfo> networkInfo;
    ECode ec = mService->GetNetworkInfoForNetwork(network, (INetworkInfo**)&networkInfo);
    if (FAILED(ec)) return NOERROR;
    *result = networkInfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::GetAllNetworkInfo(
    /* [out, callee] */ ArrayOf<INetworkInfo*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ArrayOf<INetworkInfo*> > outputarray;
    ECode ec = mService->GetAllNetworkInfo((ArrayOf<INetworkInfo*>**)&outputarray);
    if (FAILED(ec)) return NOERROR;
    *result = outputarray;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::GetNetworkForType(
    /* [in] */ Int32 networkType,
    /* [out] */ INetwork** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<INetwork> network;
    ECode ec = mService->GetNetworkForType(networkType, (INetwork**)&network);
    if (FAILED(ec)) return NOERROR;
    *result = network;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::GetAllNetworks(
    /* [out] */ ArrayOf<INetwork*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<ArrayOf<INetwork*> > outputarray;
    ECode ec = mService->GetAllNetworks((ArrayOf<INetwork*>**)&outputarray);
    if (FAILED(ec)) return NOERROR;
    *result = outputarray;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::GetProvisioningOrActiveNetworkInfo(
    /* [out] */ INetworkInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<INetworkInfo> networkInfo;
    ECode ec = mService->GetProvisioningOrActiveNetworkInfo((INetworkInfo**)&networkInfo);
    if (FAILED(ec)) return NOERROR;
    *result = networkInfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::GetActiveLinkProperties(
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ILinkProperties> linkp;
    ECode ec = mService->GetActiveLinkProperties((ILinkProperties**)&linkp);
    if (FAILED(ec)) return NOERROR;
    *result = linkp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::GetLinkProperties(
    /* [in] */ Int32 networkType,
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ILinkProperties> linkp;
    ECode ec = mService->GetLinkPropertiesForType(networkType, (ILinkProperties**)&linkp);
    if (FAILED(ec)) return NOERROR;
    *result = linkp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::GetLinkProperties(
    /* [in] */ INetwork* network,
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<ILinkProperties> linkp;
    ECode ec = mService->GetLinkProperties(network, (ILinkProperties**)&linkp);
    if (FAILED(ec)) return NOERROR;
    *result = linkp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::GetNetworkCapabilities(
    /* [in] */ INetwork* network,
    /* [out] */ INetworkCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    VALIDATE_NOT_NULL(network)

    AutoPtr<INetworkCapabilities> networkCapabilities;
    ECode ec = mService->GetNetworkCapabilities(network, (INetworkCapabilities**)&networkCapabilities);
    if (FAILED(ec)) return NOERROR;
    *result = networkCapabilities;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::StartUsingNetworkFeature(
    /* [in] */ Int32 networkType,
    /* [in] */ const String& feature,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPhoneConstants::APN_REQUEST_FAILED;

    AutoPtr<INetworkCapabilities> netCap;
    FAIL_RETURN(NetworkCapabilitiesForFeature(networkType, feature, (INetworkCapabilities**)&netCap))
    if (NULL == netCap) {
        Logger::D(TAG, "Can't satisfy startUsingNetworkFeature for %d, %s", networkType, feature.string());
        *result = IPhoneConstants::APN_REQUEST_FAILED;
        return NOERROR;
    }

    AutoPtr<INetworkRequest> request;
    synchronized(sLegacyRequests) {
        if (LEGACY_DBG) {
            String s;
            IObject::Probe(netCap)->ToString(&s);
            Int32 code;
            IObject::Probe(netCap)->GetHashCode(&code);
            Logger::D(TAG, "Looking for legacyRequest for netCap with hash: %s (%d)", s.string(), code);
            Logger::D(TAG, "sLegacyRequests has:");
            AutoPtr<ISet> keySet;
            FAIL_RETURN(sLegacyRequests->GetKeySet((ISet**)&keySet))
            AutoPtr<IIterator> it;
            FAIL_RETURN(keySet->GetIterator((IIterator**)&it))
            Boolean hasNext = FALSE;
            AutoPtr<INetworkCapabilities> nc;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> tmp;
                it->GetNext((IInterface**)&tmp);
                nc = INetworkCapabilities::Probe(tmp);
                IObject::Probe(nc)->GetHashCode(&code);
                IObject::Probe(nc)->ToString(&s);
                Logger::D(TAG, "  %s (%d)", s.string(), code);
            }
        }

        AutoPtr<IInterface> l;
        sLegacyRequests->Get(netCap, (IInterface**)&l);
        if (l != NULL) {
            LegacyRequest* lr = (LegacyRequest*)IObject::Probe(l);
            String s;
            IObject::Probe(lr->mNetworkRequest)->ToString(&s);
            Logger::D(TAG, "renewing startUsingNetworkFeature request %s", s.string());
            FAIL_RETURN(RenewRequestLocked(lr));
            if (lr->mCurrentNetwork != NULL) {
                *result = IPhoneConstants::APN_ALREADY_ACTIVE;
            }
            else {
               *result = IPhoneConstants::APN_REQUEST_STARTED;
            }
            return NOERROR;
        }

        FAIL_RETURN(RequestNetworkForFeatureLocked(netCap, (INetworkRequest**)&request))
    }

    if (request != NULL) {
        String s;
        IObject::Probe(request)->ToString(&s);
        Logger::D(TAG, "starting startUsingNetworkFeature for request %s", s.string());
        *result = IPhoneConstants::APN_REQUEST_STARTED;
    }
    else {
        Logger::D(TAG, " request Failed");
        *result = IPhoneConstants::APN_REQUEST_FAILED;
    }
    return NOERROR;
}

ECode CConnectivityManager::StartUsingNetworkFeatureForSubscription(
    /* [in] */ Int32 networkType,
    /* [in] */ const String& feature,
    /* [in] */ const String& subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPhoneConstants::APN_REQUEST_FAILED;

    Logger::D(TAG, "startUsingNetworkFeatureForSubscription: for %d feature = %s subId = %s", networkType, feature.string(), subId.string());
    AutoPtr<INetworkCapabilities> netCap;
    FAIL_RETURN(NetworkCapabilitiesForFeature(networkType, feature, (INetworkCapabilities**)&netCap));
    if (NULL == netCap) {
        Logger::D(TAG, "Can't satisfy startUsingNetworkFeature for %d, %s", networkType, feature.string());
        return NOERROR;
    }

    netCap->SetNetworkSpecifier(subId);
    AutoPtr<INetworkRequest> request;
    synchronized(sLegacyRequests) {
        if (LEGACY_DBG) {
            String s;
            IObject::Probe(netCap)->ToString(&s);
            Int32 code;
            IObject::Probe(netCap)->GetHashCode(&code);
            Logger::D(TAG, "Looking for legacyRequest for netCap with hash: %s (&d)", s.string(), code);
            Logger::D(TAG, "sLegacyRequests has:");
            AutoPtr<ISet> keySet;
            FAIL_RETURN(sLegacyRequests->GetKeySet((ISet**)&keySet));
            AutoPtr<IIterator> it;
            FAIL_RETURN(keySet->GetIterator((IIterator**)&it));
            Boolean hasNext = FALSE;
            AutoPtr<INetworkCapabilities> nc;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> tmp;
                it->GetNext((IInterface**)&tmp);
                nc = INetworkCapabilities::Probe(tmp);
                IObject::Probe(nc)->GetHashCode(&code);
                IObject::Probe(nc)->ToString(&s);
                Logger::D(TAG, "  %s (%d)", s.string(), code);
            }
        }

        AutoPtr<IInterface> l;
        sLegacyRequests->Get(netCap, (IInterface**)&l);
        if (l != NULL) {
            LegacyRequest* lr = (LegacyRequest*)IObject::Probe(l);
            String s;
            IObject::Probe(lr->mNetworkRequest)->ToString(&s);
            Logger::D(TAG, "renewing startUsingNetworkFeature request %s", s.string());
            FAIL_RETURN(RenewRequestLocked(lr));
            if (lr->mCurrentNetwork != NULL) {
                *result = IPhoneConstants::APN_ALREADY_ACTIVE;
            }
            else {
                *result = IPhoneConstants::APN_REQUEST_STARTED;
            }
            return NOERROR;
        }

        FAIL_RETURN(RequestNetworkForFeatureLocked(netCap, (INetworkRequest**)&request));
    }

    if (request != NULL) {
        String s;
        IObject::Probe(request)->ToString(&s);
        Logger::D(TAG, "starting startUsingNetworkFeature for request %s", s.string());
        *result = IPhoneConstants::APN_REQUEST_STARTED;
    }
    else {
        Logger::D(TAG, " request Failed");
        *result = IPhoneConstants::APN_REQUEST_FAILED;
    }
    return NOERROR;
}

ECode CConnectivityManager::StopUsingNetworkFeature(
    /* [in] */ Int32 networkType,
    /* [in] */ const String& feature,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    AutoPtr<INetworkCapabilities> netCap;
    FAIL_RETURN(NetworkCapabilitiesForFeature(networkType, feature, (INetworkCapabilities**)&netCap))
    if (NULL == netCap) {
        Logger::D(TAG, "Can't satisfy stopUsingNetworkFeature for %d, %s", networkType, feature.string());
        *result = -1;
        return NOERROR;
    }

    AutoPtr<IConnectivityManagerNetworkCallback> networkCallback;
    FAIL_RETURN(RemoveRequestForFeature(netCap, (IConnectivityManagerNetworkCallback**)&networkCallback))
    if (networkCallback != NULL) {
        Logger::D(TAG, "stopUsingNetworkFeature for %d, %s", networkType, feature.string());
        FAIL_RETURN(UnregisterNetworkCallback(networkCallback))
    }
    *result = 1;
    return NOERROR;
}

ECode CConnectivityManager::StopUsingNetworkFeatureForSubscription(
    /* [in] */ Int32 networkType,
    /* [in] */ const String& feature,
    /* [in] */ const String& subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    Logger::D(TAG, "stopUsingNetworkFeatureForSubscription: for %d feature = %s subId = %s", networkType, feature.string(), subId.string());
    AutoPtr<INetworkCapabilities> netCap;
    FAIL_RETURN(NetworkCapabilitiesForFeature(networkType, feature, (INetworkCapabilities**)&netCap));
    if (NULL == netCap) {
        Logger::D(TAG, "Can't satisfy stopUsingNetworkFeature for %d, %s", networkType, feature.string());
        return NOERROR;
    }

    netCap->SetNetworkSpecifier(subId);
    AutoPtr<IConnectivityManagerNetworkCallback> networkCallback;
    FAIL_RETURN(RemoveRequestForFeature(netCap, (IConnectivityManagerNetworkCallback**)&networkCallback));
    if (networkCallback != NULL) {
        Logger::D(TAG, "stopUsingNetworkFeature for %d, %s", networkType, feature.string());
        FAIL_RETURN(UnregisterNetworkCallback(networkCallback))
    }
    *result = 1;
    return NOERROR;
}

ECode CConnectivityManager::MaybeMarkCapabilitiesRestricted(
    /* [in] */ INetworkCapabilities* nc)
{
    VALIDATE_NOT_NULL(nc)

    AutoPtr<ArrayOf<Int32> > cap;
    FAIL_RETURN(nc->GetCapabilities((ArrayOf<Int32>**)&cap))
    for (Int32 i = 0; i < cap->GetLength(); ++i) {
        switch ((*cap)[i]) {
            case INetworkCapabilities::NET_CAPABILITY_CBS:
            case INetworkCapabilities::NET_CAPABILITY_DUN:
            case INetworkCapabilities::NET_CAPABILITY_EIMS:
            case INetworkCapabilities::NET_CAPABILITY_FOTA:
            case INetworkCapabilities::NET_CAPABILITY_IA:
            case INetworkCapabilities::NET_CAPABILITY_IMS:
            case INetworkCapabilities::NET_CAPABILITY_RCS:
            case INetworkCapabilities::NET_CAPABILITY_XCAP:
            case INetworkCapabilities::NET_CAPABILITY_NOT_RESTRICTED: //there by default
                continue;
            default:
                // At least one capability usually provided by unrestricted
                // networks. Conclude that this network is unrestricted.
                return NOERROR;
        }
    }
    // All the capabilities are typically provided by restricted networks.
    // Conclude that this network is restricted.
    return nc->RemoveCapability(INetworkCapabilities::NET_CAPABILITY_NOT_RESTRICTED);
}

ECode CConnectivityManager::NetworkCapabilitiesForFeature(
    /* [in] */ Int32 networkType,
    /* [in] */ const String& feature,
    /* [out] */ INetworkCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (TYPE_MOBILE == networkType) {
        Int32 cap = -1;
        if (feature.Equals("enableMMS")) {
            cap = INetworkCapabilities::NET_CAPABILITY_MMS;
        }
        else if (feature.Equals("enableSUPL")) {
            cap = INetworkCapabilities::NET_CAPABILITY_SUPL;
        }
        else if (feature.Equals("enableDUN") || feature.Equals("enableDUNAlways")) {
            cap = INetworkCapabilities::NET_CAPABILITY_DUN;
        }
        else if (feature.Equals("enableHIPRI")) {
            cap = INetworkCapabilities::NET_CAPABILITY_INTERNET;
        }
        else if (feature.Equals("enableFOTA")) {
            cap = INetworkCapabilities::NET_CAPABILITY_FOTA;
        }
        else if (feature.Equals("enableIMS")) {
            cap = INetworkCapabilities::NET_CAPABILITY_IMS;
        }
        else if (feature.Equals("enableCBS")) {
            cap = INetworkCapabilities::NET_CAPABILITY_CBS;
        }
        else {
            *result = NULL;
        }
        AutoPtr<INetworkCapabilities> netCap;
        CNetworkCapabilities::New((INetworkCapabilities**)&netCap);
        FAIL_RETURN(netCap->AddTransportType(INetworkCapabilities::TRANSPORT_CELLULAR))
        FAIL_RETURN(netCap->AddCapability(cap))
        FAIL_RETURN(MaybeMarkCapabilitiesRestricted(netCap))
        *result = netCap;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (TYPE_WIFI == networkType) {
        if (feature.Equals("p2p")) {
            AutoPtr<INetworkCapabilities> netCap;
            CNetworkCapabilities::New((INetworkCapabilities**)&netCap);
            FAIL_RETURN(netCap->AddTransportType(INetworkCapabilities::TRANSPORT_WIFI))
            FAIL_RETURN(netCap->AddCapability(INetworkCapabilities::NET_CAPABILITY_WIFI_P2P))
            FAIL_RETURN(MaybeMarkCapabilitiesRestricted(netCap))
            *result = netCap;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode CConnectivityManager::InferLegacyTypeForNetworkCapabilities(
    /* [in] */ INetworkCapabilities* netCap,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TYPE_NONE;

    if (NULL == netCap) {
        *result = TYPE_NONE;
        return NOERROR;
    }
    Boolean b;
    if (netCap->HasTransport(INetworkCapabilities::TRANSPORT_CELLULAR, &b), !b) {
        *result = TYPE_NONE;
        return NOERROR;
    }
    AutoPtr<INetworkCapabilities> networkCapabilities;
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_CBS, &b), b) {
        FAIL_RETURN(NetworkCapabilitiesForFeature(TYPE_MOBILE, String("enableCBS"), (INetworkCapabilities**)&networkCapabilities))
        if (IObject::Probe(netCap)->Equals(networkCapabilities, &b), b) {
            *result = TYPE_MOBILE_CBS;
            return NOERROR;
        }
        else {
            *result = TYPE_NONE;
            return NOERROR;
        }
    }
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_IMS, &b), b) {
        FAIL_RETURN(NetworkCapabilitiesForFeature(TYPE_MOBILE, String("enableIMS"), (INetworkCapabilities**)&networkCapabilities))
        if (IObject::Probe(netCap)->Equals(networkCapabilities, &b), b) {
            *result = TYPE_MOBILE_IMS;
            return NOERROR;
        }
        else {
            *result = TYPE_NONE;
            return NOERROR;
        }
    }
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_FOTA, &b), b) {
        FAIL_RETURN(NetworkCapabilitiesForFeature(TYPE_MOBILE, String("enableFOTA"), (INetworkCapabilities**)&networkCapabilities))
        if (IObject::Probe(netCap)->Equals(networkCapabilities, &b), b) {
            *result = TYPE_MOBILE_FOTA;
            return NOERROR;
        }
        else {
            *result = TYPE_NONE;
            return NOERROR;
        }
    }
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_DUN, &b), b) {
        FAIL_RETURN(NetworkCapabilitiesForFeature(TYPE_MOBILE, String("enableDUN"), (INetworkCapabilities**)&networkCapabilities))
        if (IObject::Probe(netCap)->Equals(networkCapabilities, &b), b) {
            *result = TYPE_MOBILE_DUN;
            return NOERROR;
        }
        else {
            *result = TYPE_NONE;
            return NOERROR;
        }
    }
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_SUPL, &b), b) {
        FAIL_RETURN(NetworkCapabilitiesForFeature(TYPE_MOBILE, String("enableSUPL"), (INetworkCapabilities**)&networkCapabilities))
        if (IObject::Probe(netCap)->Equals(networkCapabilities, &b), b) {
            *result = TYPE_MOBILE_SUPL;
            return NOERROR;
        }
        else {
            *result = TYPE_NONE;
            return NOERROR;
        }
    }
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_MMS, &b), b) {
        FAIL_RETURN(NetworkCapabilitiesForFeature(TYPE_MOBILE, String("enableMMS"), (INetworkCapabilities**)&networkCapabilities))
        if (IObject::Probe(netCap)->Equals(networkCapabilities, &b), b) {
            *result = TYPE_MOBILE_MMS;
            return NOERROR;
        }
        else {
            *result = TYPE_NONE;
            return NOERROR;
        }
    }
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_INTERNET, &b), b) {
        FAIL_RETURN(NetworkCapabilitiesForFeature(TYPE_MOBILE, String("enableHIPRI"), (INetworkCapabilities**)&networkCapabilities))
        if (IObject::Probe(netCap)->Equals(networkCapabilities, &b), b) {
            *result = TYPE_MOBILE_HIPRI;
            return NOERROR;
        }
        else {
            *result = TYPE_NONE;
            return NOERROR;
        }
    }
    *result = TYPE_NONE;
    return NOERROR;
}

ECode CConnectivityManager::LegacyTypeForNetworkCapabilities(
    /* [in] */ INetworkCapabilities* netCap,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TYPE_NONE;

    if (NULL == netCap) {
        *result = TYPE_NONE;
        return NOERROR;
    }
    Boolean b;
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_CBS, &b), b) {
       *result = TYPE_MOBILE_CBS;
       return NOERROR;
    }
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_IMS, &b), b) {
       *result = TYPE_MOBILE_IMS;
       return NOERROR;
       }
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_FOTA, &b), b) {
       *result = TYPE_MOBILE_FOTA;
       return NOERROR;
    }
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_DUN, &b), b) {
       *result = TYPE_MOBILE_DUN;
       return NOERROR;
    }
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_SUPL, &b), b) {
       *result = TYPE_MOBILE_SUPL;
       return NOERROR;
    }
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_MMS, &b), b) {
       *result = TYPE_MOBILE_MMS;
       return NOERROR;
    }
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_INTERNET, &b), b) {
       *result = TYPE_MOBILE_HIPRI;
       return NOERROR;
    }
    if (netCap->HasCapability(INetworkCapabilities::NET_CAPABILITY_WIFI_P2P, &b), b) {
       *result = TYPE_WIFI_P2P;
       return NOERROR;
    }
   *result = TYPE_NONE;
   return NOERROR;
}

ECode CConnectivityManager::FindRequestForFeature(
    /* [in] */ INetworkCapabilities* netCap,
    /* [out] */ INetworkRequest** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    VALIDATE_NOT_NULL(netCap)

    synchronized(sLegacyRequests) {
        AutoPtr<IInterface> l;
        sLegacyRequests->Get(netCap, (IInterface**)&l);
        if (l != NULL) {
            LegacyRequest* lr = (LegacyRequest*) IObject::Probe(l.Get());
            *result = lr->mNetworkRequest;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode CConnectivityManager::RenewRequestLocked(
    /* [in] */ LegacyRequest* l)
{
    VALIDATE_NOT_NULL(l)

    l->mExpireSequenceNumber++;
    Logger::D(TAG, (String("renewing request to seqNum ") + StringUtils::ToString(l->mExpireSequenceNumber)).string());
    FAIL_RETURN(SendExpireMsgForFeature(l->mNetworkCapabilities, l->mExpireSequenceNumber, l->mDelay))
    return NOERROR;
}

ECode CConnectivityManager::ExpireRequest(
    /* [in] */ INetworkCapabilities* netCap,
    /* [in] */ Int32 sequenceNum)
{
    VALIDATE_NOT_NULL(netCap)

    Int32 ourSeqNum = -1;
    synchronized(sLegacyRequests) {
        AutoPtr<IInterface> l;
        sLegacyRequests->Get(netCap, (IInterface**)&l);
        LegacyRequest* lr = (LegacyRequest*) IObject::Probe(l.Get());
        if (NULL == lr)
            return NOERROR;
        ourSeqNum = lr->mExpireSequenceNumber;
        if (lr->mExpireSequenceNumber == sequenceNum) {
            FAIL_RETURN(UnregisterNetworkCallback(lr->mNetworkCallback))
            sLegacyRequests->Remove(netCap);
        }
    }
    Logger::D(TAG, "expireRequest with %s, %s", StringUtils::ToString(ourSeqNum).string(), StringUtils::ToString(sequenceNum).string());
    return NOERROR;
}

ECode CConnectivityManager::RequestNetworkForFeatureLocked(
    /* [in] */ INetworkCapabilities* netCap,
    /* [out] */ INetworkRequest** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    VALIDATE_NOT_NULL(netCap)

    Int32 delay = -1;
    Int32 type;
    FAIL_RETURN(LegacyTypeForNetworkCapabilities(netCap, &type))
    // try {
        // delay = mService.getRestoreDefaultNetworkDelay(type);
    // } catch (RemoteException e) {}
    ECode ec = mService->GetRestoreDefaultNetworkDelay(type, &delay);
    if (FAILED(ec) && ec != (ECode)E_REMOTE_EXCEPTION) return ec;
    LegacyRequest* pl = new LegacyRequest;
    AutoPtr<IInterface> l = pl->Probe(EIID_IObject);
    pl->mNetworkCapabilities = netCap;
    pl->mDelay = delay;
    pl->mExpireSequenceNumber = 0;
    FAIL_RETURN(SendRequestForNetwork(netCap, pl->mNetworkCallback, 0, REQUEST, type, (INetworkRequest**)&(pl->mNetworkRequest)))
    if (NULL == pl->mNetworkRequest) {
        *result = NULL;
        return NOERROR;
    }
    sLegacyRequests->Put(netCap, l);
    FAIL_RETURN(SendExpireMsgForFeature(netCap, pl->mExpireSequenceNumber, delay))
    *result = pl->mNetworkRequest;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CConnectivityManager::SendExpireMsgForFeature(
    /* [in] */ INetworkCapabilities* netCap,
    /* [in] */ Int32 seqNum,
    /* [in] */ Int32 delay)
{
    VALIDATE_NOT_NULL(netCap)

    if (delay >= 0) {
        Logger::D(TAG, (String("sending expire msg with seqNum ") + StringUtils::ToString(seqNum) + " and delay " + StringUtils::ToString(delay)).string());
        AutoPtr<IMessage> msg;
        FAIL_RETURN(sCallbackHandler->ObtainMessage(EXPIRE_LEGACY_REQUEST, seqNum, 0, netCap, (IMessage**)&msg))
        Boolean b;
        FAIL_RETURN(sCallbackHandler->SendMessageDelayed(msg, delay, &b))
    }
    return NOERROR;
}

ECode CConnectivityManager::RemoveRequestForFeature(
    /* [in] */ INetworkCapabilities* netCap,
    /* [out] */ IConnectivityManagerNetworkCallback** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    VALIDATE_NOT_NULL(netCap)

    synchronized(sLegacyRequests) {
        AutoPtr<IInterface> pl;
        FAIL_RETURN(sLegacyRequests->Remove(netCap, (IInterface**)&pl))
        LegacyRequest* l = (LegacyRequest*) IObject::Probe(pl);
        if (NULL == l) {
            *result = NULL;
            return NOERROR;
        }
        *result = l->mNetworkCallback;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode CConnectivityManager::RequestRouteToHost(
    /* [in] */ Int32 networkType,
    /* [in] */ Int32 hostAddress,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IInetAddress> inetAddress;
    FAIL_RETURN(NetworkUtils::IntToInetAddress(hostAddress, (IInetAddress**)&inetAddress))
    return RequestRouteToHostAddress(networkType, inetAddress, result);
}

ECode CConnectivityManager::RequestRouteToHostAddress(
    /* [in] */ Int32 networkType,
    /* [in] */ IInetAddress* hostAddress,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(hostAddress)

    // try {
    //     return mService.requestRouteToHostAddress(networkType, hostAddress.getAddress());
    // } catch (RemoteException e) {
    //     return false;
    // }
    AutoPtr<ArrayOf<Byte> > address;
    hostAddress->GetAddress((ArrayOf<Byte>**)&address);
    ECode ec = mService->RequestRouteToHostAddress(networkType, address, result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = FALSE;
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::GetBackgroundDataSetting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode CConnectivityManager::SetBackgroundDataSetting(
    /* [in] */ Boolean allowBackgroundData)
{
    return NOERROR;
}

ECode CConnectivityManager::GetActiveNetworkQuotaInfo(
    /* [out] */ INetworkQuotaInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<INetworkQuotaInfo> quotaInfo;
    ECode ec = mService->GetActiveNetworkQuotaInfo((INetworkQuotaInfo**)&quotaInfo);
    if (FAILED(ec)) return NOERROR;
    *result = quotaInfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::GetMobileDataEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::TELEPHONY_SERVICE);
    AutoPtr<IBinder> b = IBinder::Probe(obj);
    if (b != NULL) {
        // try {
        //     ITelephony it = ITelephony.Stub.asInterface(b);
        //     return it.getDataEnabled();
        // } catch (RemoteException e) { }
        AutoPtr<IITelephony> it = IITelephony::Probe(b);
        ECode ec = it->GetDataEnabled(result);
        if ((ECode)E_REMOTE_EXCEPTION == ec) {
            return NOERROR;
        }
        return ec;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CConnectivityManager::GetNetworkManagementService(
    /* [out] */ IINetworkManagementService** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    synchronized(this) {
        if (mNMService != NULL) {
            *result = mNMService;
            return NOERROR;
        }
        AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::TELEPHONY_SERVICE);
        AutoPtr<IBinder> b = IBinder::Probe(obj);
        mNMService = IINetworkManagementService::Probe(b);
        *result = mNMService;
    }
    return NOERROR;
}

ECode CConnectivityManager::AddDefaultNetworkActiveListener(
    /* [in] */ IConnectivityManagerOnNetworkActiveListener* l)
{
    VALIDATE_NOT_NULL(l)

    class InnerSub_INetworkActivityListener
        : public Object
        , public INetworkActivityListener
    {
    public:
        CAR_INNER_INTERFACE_IMPL(Object, INetworkActivityListener)

        InnerSub_INetworkActivityListener(IConnectivityManagerOnNetworkActiveListener* l)
            : mListener(l)
        {}

        // @Override
        ECode OnNetworkActive()
        {
            mListener->OnNetworkActive();
            return NOERROR;
        }
    private:
        IConnectivityManagerOnNetworkActiveListener* mListener;
    };

    AutoPtr<INetworkActivityListener> rl = new InnerSub_INetworkActivityListener(l);

    // try {
    //     getNetworkManagementService().registerNetworkActivityListener(rl);
    //     mNetworkActivityListeners.put(l, rl);
    // } catch (RemoteException e) {
    // }
    AutoPtr<IINetworkManagementService> nms;
    ECode ec = GetNetworkManagementService((IINetworkManagementService**)&nms);
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec)
            return NOERROR;
        return ec;
    }
    ec = nms->RegisterNetworkActivityListener(rl);
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec)
            return NOERROR;
        return ec;
    }
    ec = IMap::Probe(mNetworkActivityListeners)->Put(l, rl);
    if ((ECode)E_REMOTE_EXCEPTION == ec)
        return NOERROR;
    return ec;
}

ECode CConnectivityManager::RemoveDefaultNetworkActiveListener(
    /* [in] */ IConnectivityManagerOnNetworkActiveListener* l)
{
    AutoPtr<IInterface> itmp;
    IMap::Probe(mNetworkActivityListeners)->Get(l, (IInterface**)&itmp);
    AutoPtr<INetworkActivityListener> rl = INetworkActivityListener::Probe(itmp);
    if (NULL == rl) {
        Logger::E(TAG, "Listener not registered: %s", TO_CSTR(l));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    //     getNetworkManagementService().unregisterNetworkActivityListener(rl);
    // } catch (RemoteException e) {
    // }
    AutoPtr<IINetworkManagementService> nms;
    ECode ec = GetNetworkManagementService((IINetworkManagementService**)&nms);
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION == ec)
            return NOERROR;
        return ec;
    }
    ec = nms->RegisterNetworkActivityListener(rl);
    if ((ECode)E_REMOTE_EXCEPTION == ec)
        return NOERROR;
    return ec;
}

ECode CConnectivityManager::IsDefaultNetworkActive(
    /* [out] */ Boolean* result)
{
    // try {
    //     return getNetworkManagementService().isNetworkActive();
    // } catch (RemoteException e) {
    // }
    AutoPtr<IINetworkManagementService> nms;
    ECode ec = GetNetworkManagementService((IINetworkManagementService**)&nms);
    if (FAILED(ec)) {
        if ((ECode)E_REMOTE_EXCEPTION != ec) {
            return ec;
        }
        *result = FALSE;
        return NOERROR;
    }
    ec = nms->IsNetworkActive(result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = FALSE;
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::constructor(
    /* [in] */ IIConnectivityManager* service)
{
    VALIDATE_NOT_NULL(service);

    mService = service;
    return NOERROR;
}

ECode CConnectivityManager::From(
    /* [in] */ IContext* ctx,
    /* [out] */ IConnectivityManager** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IInterface> obj;
    ctx->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    *result = IConnectivityManager::Probe(obj);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CConnectivityManager::EnforceTetherChangePermission(
    /* [in] */ IContext* context)
{
    VALIDATE_NOT_NULL(context)

    AutoPtr<IResources> res;
    FAIL_RETURN(context->GetResources((IResources**)&res))
    AutoPtr<ArrayOf<String> > stringArray;
    FAIL_RETURN(res->GetStringArray(Elastos::Droid::R::array::config_mobile_hotspot_provision_app, (ArrayOf<String>**)&stringArray))
    if(stringArray->GetLength() == 2) {
        // Have a provisioning app - must only let system apps (which check this app)
        // turn on tethering
        FAIL_RETURN(context->EnforceCallingOrSelfPermission(
                Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL, String("ConnectivityService")))
    }
    else {
        FAIL_RETURN(context->EnforceCallingOrSelfPermission(
                Elastos::Droid::Manifest::permission::CHANGE_NETWORK_STATE, String("ConnectivityService")))
    }
    return NOERROR;
}

ECode CConnectivityManager::GetTetherableIfaces(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);

    ECode ec = mService->GetTetherableIfaces(result);
    if (FAILED(ec)) {
        *result = ArrayOf<String>::Alloc(0);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    return NOERROR;
}

ECode CConnectivityManager::GetTetheredIfaces(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);

    ECode ec = mService->GetTetheredIfaces(result);
    if (FAILED(ec)) {
        *result = ArrayOf<String>::Alloc(0);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    return NOERROR;
}

ECode CConnectivityManager::GetTetheringErroredIfaces(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);

    ECode ec = mService->GetTetheringErroredIfaces(result);
    if (FAILED(ec)) {
        *result = ArrayOf<String>::Alloc(0);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    return NOERROR;
}

ECode CConnectivityManager::GetTetheredDhcpRanges(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)

    ECode ec = mService->GetTetheredDhcpRanges(result);
    if (FAILED(ec)) {
        *result = ArrayOf<String>::Alloc(0);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return NOERROR;
}

ECode  CConnectivityManager::Tether(
    /* [in] */ const String& iface,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TETHER_ERROR_UNSUPPORTED;

    ECode ec = mService->Tether(iface, result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = TETHER_ERROR_SERVICE_UNAVAIL;
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::Untether(
    /* [in] */ const String& iface,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TETHER_ERROR_UNSUPPORTED;

    ECode ec = mService->Untether(iface, result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = TETHER_ERROR_SERVICE_UNAVAIL;
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::IsTetheringSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    result = FALSE;

    ECode ec = mService->IsTetheringSupported(result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = FALSE;
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::GetTetherableUsbRegexs(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr <ArrayOf<String> > outputArray;
    ECode ec = mService->GetTetherableUsbRegexs((ArrayOf<String>**)&outputArray);
    if ((ECode)E_REMOTE_EXCEPTION == ec) outputArray = ArrayOf<String>::Alloc(0);
    *result = outputArray;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::GetTetherableWifiRegexs(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ArrayOf<String> > outputArray;
    ECode ec = mService->GetTetherableWifiRegexs((ArrayOf<String>**)&outputArray);
    if ((ECode)E_REMOTE_EXCEPTION == ec) outputArray = ArrayOf<String>::Alloc(0);
    *result = outputArray;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::GetTetherableBluetoothRegexs(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ArrayOf<String> > outputArray;
    ECode ec = mService->GetTetherableBluetoothRegexs((ArrayOf<String>**)&outputArray);
    if ((ECode)E_REMOTE_EXCEPTION == ec) outputArray = ArrayOf<String>::Alloc(0);
    *result = outputArray.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CConnectivityManager::SetUsbTethering(
    /* [in] */ Boolean enabled,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TETHER_ERROR_UNSUPPORTED;

    ECode ec = mService->SetUsbTethering(enabled, result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = TETHER_ERROR_SERVICE_UNAVAIL;
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::GetTetherConnectedSta(
    /* [out] */ IList** wifiDevices)
{
    VALIDATE_NOT_NULL(wifiDevices);
    *wifiDevices = NULL;

    AutoPtr<IList> list;
    ECode ec = mService->GetTetherConnectedSta((IList**)&list);
    if (FAILED(ec)) {
        return (ECode)E_REMOTE_EXCEPTION == ec ? NOERROR : ec;
    }

    *wifiDevices = list;
    REFCOUNT_ADD(*wifiDevices);
    return NOERROR;
}

ECode CConnectivityManager::GetLastTetherError(
    /* [in] */ const String& iface,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TETHER_ERROR_UNSUPPORTED;

    ECode ec = mService->GetLastTetherError(iface, result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = TETHER_ERROR_SERVICE_UNAVAIL;
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::ReportInetCondition(
    /* [in] */ Int32 networkType,
    /* [in] */ Int32 percentage)
{
    ECode ec = mService->ReportInetCondition(networkType, percentage);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::ReportBadNetwork(
    /* [in] */ INetwork* network)
{
    VALIDATE_NOT_NULL(network)

    ECode ec = mService->ReportBadNetwork(network);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::SetGlobalProxy(
    /* [in] */ IProxyInfo* proxyp)
{
    VALIDATE_NOT_NULL(proxyp)

    ECode ec = mService->SetGlobalProxy(proxyp);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::GetGlobalProxy(
    /* [out] */ IProxyInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    ECode ec = mService->GetGlobalProxy(result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = NULL;
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::GetProxy(
    /* [out] */ IProxyInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    ECode ec = mService->GetProxy(result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = NULL;
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::SetDataDependency(
    /* [in] */ Int32 networkType,
    /* [in] */ Boolean met)
{
    ECode ec = mService->SetDataDependency(networkType, met);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::IsNetworkSupported(
    /* [in] */ Int32 networkType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    ECode ec = mService->IsNetworkSupported(networkType, result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = FALSE;
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::IsActiveNetworkMetered(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    ECode ec = mService->IsActiveNetworkMetered(result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = FALSE;
        return NOERROR;
    }
    return ec;
}


ECode CConnectivityManager::UpdateLockdownVpn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    ECode ec = mService->UpdateLockdownVpn(result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode CConnectivityManager::CaptivePortalCheckCompleted(
    /* [in] */ INetworkInfo* info,
    /* [in] */ Boolean isCaptivePortal)
{
    VALIDATE_NOT_NULL(info)

    // try {
    //     mService.captivePortalCheckCompleted(info, isCaptivePortal);
    // } catch (RemoteException e) {
    // }
    ECode ec = mService->CaptivePortalCheckCompleted(info, isCaptivePortal);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::SupplyMessenger(
    /* [in] */ Int32 networkType,
    /* [in] */ IMessenger* messenger)
{
    VALIDATE_NOT_NULL(messenger)

    // try {
    ECode ec = mService->SupplyMessenger(networkType, messenger);
    //} catch (RemoteException e) {
    //}
    if ((ECode)E_REMOTE_EXCEPTION == ec)
        return NOERROR;
    return ec;
}

ECode CConnectivityManager::CheckMobileProvisioning(
    /* [in] */ Int32 suggestedTimeOutMs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 timeOutMs = -1;
    // try {
    ECode ec = mService->CheckMobileProvisioning(suggestedTimeOutMs, &timeOutMs);
    // } catch (RemoteException e) {
    // }
    if ((ECode)E_REMOTE_EXCEPTION == ec)
        return NOERROR;
    *result = timeOutMs;
    return ec;
}

ECode CConnectivityManager::GetMobileProvisioningUrl(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    // try {
    ECode ec = mService->GetMobileProvisioningUrl(result);
    // } catch (RemoteException e) {
    // }
    if (FAILED(ec)) {
        *result = String(NULL);
    }
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::GetMobileRedirectedProvisioningUrl(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    // try {
    ECode ec = mService->GetMobileRedirectedProvisioningUrl(result);
    // } catch (RemoteException e) {
    // }
    if (FAILED(ec)) {
        *result = String(NULL);
    }
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::GetLinkQualityInfo(
    /* [in] */ Int32 networkType,
    /* [out] */ ILinkQualityInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // try {
    //     LinkQualityInfo li = mService.getLinkQualityInfo(networkType);
    //     return li;
    // } catch (RemoteException e) {
    //     return NULL;
    // }

    ECode ec = mService->GetLinkQualityInfo(networkType, result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = NULL;
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::GetActiveLinkQualityInfo(
    /* [out] */ ILinkQualityInfo** result)
{
    // try {
    //     LinkQualityInfo li = mService.getActiveLinkQualityInfo();
    //     return li;
    // } catch (RemoteException e) {
    //     return NULL;
    // }
    ECode ec = mService->GetActiveLinkQualityInfo(result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = NULL;
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::GetAllLinkQualityInfo(
    /* [out, callee] */ ArrayOf<ILinkQualityInfo*>** result)
{
    // try {
    //     LinkQualityInfo[] li = mService.getAllLinkQualityInfo();
    //     return li;
    // } catch (RemoteException e) {
    //     return NULL;
    // }
    ECode ec = mService->GetAllLinkQualityInfo(result);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        *result = NULL;
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::SetProvisioningNotificationVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 networkType,
    /* [in] */ const String& action)
{
    // try {
    //     mService.setProvisioningNotificationVisible(visible, networkType, action);
    // } catch (RemoteException e) {
    // }
    ECode ec = mService->SetProvisioningNotificationVisible(visible, networkType, action);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::SetAirplaneMode(
        /* [in] */ Boolean enable)
{
    // try {
    //     mService.setAirplaneMode(enable);
    // } catch (RemoteException e) {
    // }
    ECode ec = mService->SetAirplaneMode(enable);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::RegisterNetworkFactory(
    /* [in] */ IMessenger* messenger,
    /* [in] */ const String& name)
{
    // try {
    //     mService.registerNetworkFactory(messenger, name);
    // } catch (RemoteException e) { }
    ECode ec = mService->RegisterNetworkFactory(messenger, name);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::UnregisterNetworkFactory(
    /* [in] */ IMessenger* messenger)
{
    // try {
    //     mService.unregisterNetworkFactory(messenger);
    // } catch (RemoteException e) { }
    ECode ec = mService->UnregisterNetworkFactory(messenger);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::RegisterNetworkAgent(
    /* [in] */ IMessenger* messenger,
    /* [in] */ INetworkInfo* ni,
    /* [in] */ ILinkProperties* lp,
    /* [in] */ INetworkCapabilities* nc,
    /* [in] */ Int32 score,
    /* [in] */ INetworkMisc* misc)
{
    // try {
    //     mService.registerNetworkAgent(messenger, ni, lp, nc, score, misc);
    // } catch (RemoteException e) { }
    ECode ec = mService->RegisterNetworkAgent(messenger, ni, lp, nc, score, misc);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::IncCallbackHandlerRefCount()
{
    synchronized(sCallbackRefCount) {
        Int32 num;
        FAIL_RETURN(sCallbackRefCount->IncrementAndGet(&num))
        if (1 == num) {
            // TODO - switch this over to a ManagerThread or expire it when done
            AutoPtr<IHandlerThread> callbackThread;
            CHandlerThread::New(String("ConnectivityManager"), (IHandlerThread**)&callbackThread);
            FAIL_RETURN(IThread::Probe(callbackThread)->Start())
            AutoPtr<ILooper> looper;
            FAIL_RETURN(callbackThread->GetLooper((ILooper**)&looper))
            sCallbackHandler = new CallbackHandler(looper, sNetworkCallback, sCallbackRefCount, this);
        }
    }
    return NOERROR;
}

ECode CConnectivityManager::DecCallbackHandlerRefCount()
{
    synchronized(sCallbackRefCount) {
        Int32 num;
        FAIL_RETURN(sCallbackRefCount->DecrementAndGet(&num))
        if (0 == num) {
            AutoPtr<IMessage> msg;
            FAIL_RETURN(sCallbackHandler->ObtainMessage(CALLBACK_EXIT, (IMessage**)&msg))
            FAIL_RETURN(msg->SendToTarget())
            sCallbackHandler = NULL;
        }
    }
    return NOERROR;
}

ECode CConnectivityManager::SendRequestForNetwork(
        /* [in] */ INetworkCapabilities* need,
        /* [in] */ IConnectivityManagerNetworkCallback* networkCallback,
        /* [in] */ Int32 timeoutSec,
        /* [in] */ Int32 action,
        /* [in] */ Int32 legacyType,
        /* [out] */ INetworkRequest** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (NULL == networkCallback) {
        Logger::E(TAG, "NULL NetworkCallback");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (NULL == need) {
        Logger::E(TAG, "NULL NetworkCapabilities");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    ECode ec = IncCallbackHandlerRefCount();
    if (FAILED(ec) && ec != (ECode)E_REMOTE_EXCEPTION) {
        return ec;
    }
    synchronized(sNetworkCallback) {
        AutoPtr<IMessenger> msg;
        AutoPtr<IBinder> binder;
        CMessenger::New(sCallbackHandler, (IMessenger**)&msg);
        CBinder::New((IBinder**)&binder);
        if (LISTEN == action) {
            ec = mService->ListenForNetwork(need, msg, binder, (INetworkRequest**)&(((ConnectivityManagerNetworkCallback*)networkCallback)->mNetworkRequest));
            if (FAILED(ec) && ec != (ECode)E_REMOTE_EXCEPTION) {
                return ec;
            }
        }
        else {
            ec = mService->RequestNetwork(need, msg, timeoutSec, binder, legacyType, (INetworkRequest**)&(((ConnectivityManagerNetworkCallback*)networkCallback)->mNetworkRequest));
            if (FAILED(ec) && ec != (ECode)E_REMOTE_EXCEPTION) {
                return ec;
            }
        }
        if (((ConnectivityManagerNetworkCallback*)networkCallback)->mNetworkRequest != NULL) {
            sNetworkCallback->Put(((ConnectivityManagerNetworkCallback*)networkCallback)->mNetworkRequest, networkCallback);
        }
    }
    // } catch (RemoteException e) {}
    if (NULL == ((ConnectivityManagerNetworkCallback*)networkCallback)->mNetworkRequest) {
        FAIL_RETURN(DecCallbackHandlerRefCount())
    }
    *result = ((ConnectivityManagerNetworkCallback*)networkCallback)->mNetworkRequest;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CConnectivityManager::RequestNetwork(
    /* [in] */ INetworkRequest* request,
    /* [in] */ IConnectivityManagerNetworkCallback* networkCallback)
{
    VALIDATE_NOT_NULL(request)
    VALIDATE_NOT_NULL(networkCallback)

    AutoPtr<INetworkCapabilities> nc;
    request->GetNetworkCapabilities((INetworkCapabilities**)&nc);
    Int32 type;
    FAIL_RETURN(InferLegacyTypeForNetworkCapabilities(nc, &type))
    AutoPtr<INetworkRequest> nr;
    FAIL_RETURN(SendRequestForNetwork(nc, networkCallback, 0,
            REQUEST, type, (INetworkRequest**)&nr))
    return NOERROR;
}

ECode CConnectivityManager::RequestNetwork(
        /* [in] */ INetworkRequest* request,
        /* [in] */ IConnectivityManagerNetworkCallback* networkCallback,
        /* [in] */ Int32 timeoutMs)
{
    VALIDATE_NOT_NULL(request)
    VALIDATE_NOT_NULL(networkCallback)

    AutoPtr<INetworkCapabilities> nc;
    request->GetNetworkCapabilities((INetworkCapabilities**)&nc);
    Int32 type;
    FAIL_RETURN(InferLegacyTypeForNetworkCapabilities(nc, &type))
    AutoPtr<INetworkRequest> nr;
    FAIL_RETURN(SendRequestForNetwork(nc, networkCallback, timeoutMs,
            REQUEST, type, (INetworkRequest**)&nr))
    return NOERROR;
}

ECode CConnectivityManager::RequestNetwork(
        /* [in] */ INetworkRequest* request,
        /* [in] */ IPendingIntent* operation)
{
    VALIDATE_NOT_NULL(request)
    VALIDATE_NOT_NULL(operation)

    // try {
    //     mService.pendingRequestForNetwork(request.networkCapabilities, operation);
    // } catch (RemoteException e) {}
    AutoPtr<INetworkCapabilities> nc;
    request->GetNetworkCapabilities((INetworkCapabilities**)&nc);
    AutoPtr<INetworkRequest> nr;
    ECode ec = mService->PendingRequestForNetwork(nc, operation, (INetworkRequest**)&nr);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::RegisterNetworkCallback(
    /* [in] */ INetworkRequest* request,
    /* [in] */ IConnectivityManagerNetworkCallback* networkCallback)
{
    VALIDATE_NOT_NULL(request)
    VALIDATE_NOT_NULL(networkCallback)

    AutoPtr<INetworkCapabilities> nc;
    FAIL_RETURN(request->GetNetworkCapabilities((INetworkCapabilities**)&nc))
    AutoPtr<INetworkRequest> nr;
    return SendRequestForNetwork(nc, networkCallback, 0, LISTEN, TYPE_NONE, (INetworkRequest**)&nr);
}

ECode CConnectivityManager::UnregisterNetworkCallback(
    /* [in] */ IConnectivityManagerNetworkCallback* networkCallback)
{
    if (NULL == networkCallback) {
        Logger::E(TAG, "Invalid NetworkCallback");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else {
        if (NULL == ((ConnectivityManagerNetworkCallback*)networkCallback)->mNetworkRequest) {
            Logger::E(TAG, "Invalid NetworkCallback");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        else {
            Int32 requestId;
            ((ConnectivityManagerNetworkCallback*)networkCallback)->mNetworkRequest->GetRequestId(&requestId);
            if (REQUEST_ID_UNSET == requestId) {
                Logger::E(TAG, "Invalid NetworkCallback");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }
    // try {
    //     mService.releaseNetworkRequest(networkCallback.networkRequest);
    // } catch (RemoteException e) {}
    ECode ec = mService->ReleaseNetworkRequest(((ConnectivityManagerNetworkCallback*)networkCallback)->mNetworkRequest);
    if ((ECode)E_REMOTE_EXCEPTION == ec) {
        return NOERROR;
    }
    return ec;
}

ECode CConnectivityManager::SetProcessDefaultNetwork(
        /* [in] */ INetwork* network,
        /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 netId;
    if (NULL == network) {
        netId = NETID_UNSET;
    }
    else {
        network->GetNetId(&netId);
    }
    Int32 netBound;
    FAIL_RETURN(NetworkUtils::GetNetworkBoundToProcess(&netBound))
    if (netBound == netId) {
        *result = TRUE;
        return NOERROR;
    }
    Boolean b;
    FAIL_RETURN(NetworkUtils::BindProcessToNetwork(netId, &b))
    if (b) {
        // Must flush DNS cache as new network may have different DNS resolutions.
        AutoPtr<IInetAddressHelper> inetAddressHelper;
        CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetAddressHelper);
        FAIL_RETURN(inetAddressHelper->ClearDnsCache())
        // Must flush socket pool as idle sockets will be bound to previous network and may
        // cause subsequent fetches to be performed on old network.
        AutoPtr<INetworkEventDispatcherHelper> networkEventDispatcherHelper;
        CNetworkEventDispatcherHelper::AcquireSingleton((INetworkEventDispatcherHelper**)&networkEventDispatcherHelper);
        AutoPtr<INetworkEventDispatcher> networkEventDispatcher;
        FAIL_RETURN(networkEventDispatcherHelper->GetInstance((INetworkEventDispatcher**)&networkEventDispatcher))
        FAIL_RETURN(networkEventDispatcher->OnNetworkConfigurationChanged())
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CConnectivityManager::GetProcessDefaultNetwork(
    /* [out] */ INetwork** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 netId;
    NetworkUtils::GetNetworkBoundToProcess(&netId);
    if (NETID_UNSET == netId) {
        *result = NULL;
        return NOERROR;
    }
    CNetwork::New(netId, result);
    return NOERROR;
}

ECode CConnectivityManager::SetProcessDefaultNetworkForHostResolution(
    /* [in] */ INetwork* network,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 netId = NETID_UNSET;
    if (NULL == network) {
        network->GetNetId(&netId);
    }
    NetworkUtils::BindProcessToNetworkForHostResolution(netId, result);
    return NOERROR;
}

CConnectivityManager::LegacyRequest::LegacyRequest()
    : mDelay(-1)
    , mNetworkCallback(new InnerSub_ConnectivityManagerNetworkCallback(this))
{}

CConnectivityManager::CallbackHandler::CallbackHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ IHashMap* callbackMap,
    /* [in] */ IAtomicInteger32* refCount,
    /* [in] */ CConnectivityManager* cm)
    : Handler(looper)
    , mCallbackMap(callbackMap)
    , mRefCount(refCount)
    , mCm(cm)
{}

ECode CConnectivityManager::CallbackHandler::HandleMessage(
    /* [in] */ IMessage* message)
{
    VALIDATE_NOT_NULL(message)

    Int32 what;
    message->GetWhat(&what);
    Logger::D(TAG, (String("CM callback handler got msg ") + StringUtils::ToString(what)).string());
    switch (what) {
        case CALLBACK_PRECHECK: {
            AutoPtr<IInterface> request;
            GetObject(message, String("CNetworkRequest"), (IInterface**)&request);
            AutoPtr<IConnectivityManagerNetworkCallback> callbacks;
            GetCallbacks(INetworkRequest::Probe(request), (IConnectivityManagerNetworkCallback**)&callbacks);
            if (callbacks != NULL) {
                AutoPtr<IInterface> network;
                GetObject(message, String("CNetwork"), (IInterface**)&network);
                callbacks->OnPreCheck(INetwork::Probe(network));
            }
            else {
                Logger::E(TAG, String("callback not found for PRECHECK message").string());
            }
            break;
        }
        case CALLBACK_AVAILABLE: {
            AutoPtr<IInterface> request;
            GetObject(message, String("CNetworkRequest"), (IInterface**)&request);
            AutoPtr<IConnectivityManagerNetworkCallback> callbacks;
            GetCallbacks(INetworkRequest::Probe(request), (IConnectivityManagerNetworkCallback**)&callbacks);
            if (callbacks != NULL) {
                AutoPtr<IInterface> network;
                GetObject(message, String("CNetwork"), (IInterface**)&network);
                callbacks->OnAvailable(INetwork::Probe(network));
            }
            else {
                Logger::E(TAG, String("callback not found for AVAILABLE message").string());
            }
            break;
        }
        case CALLBACK_LOSING: {
            AutoPtr<IInterface> request;
            GetObject(message, String("CNetworkRequest"), (IInterface**)&request);

            AutoPtr<IConnectivityManagerNetworkCallback> callbacks;
            GetCallbacks(INetworkRequest::Probe(request), (IConnectivityManagerNetworkCallback**)&callbacks);
            if (callbacks != NULL) {
                AutoPtr<IInterface> network;
                GetObject(message, String("CNetwork"), (IInterface**)&network);
                Int32 arg1;
                message->GetArg1(&arg1);
                callbacks->OnLosing(INetwork::Probe(network), arg1);
            }
            else {
                Logger::E(TAG, String("callback not found for LOSING message").string());
            }
            break;
        }
        case CALLBACK_LOST: {
            AutoPtr<IInterface> request;
            GetObject(message,String("CNetworkRequest"), (IInterface**)&request);
            AutoPtr<IConnectivityManagerNetworkCallback> callbacks;
            GetCallbacks(INetworkRequest::Probe(request), (IConnectivityManagerNetworkCallback**)&callbacks);
            if (callbacks != NULL) {
                AutoPtr<IInterface> network;
                GetObject(message, String("CNetwork"), (IInterface**)&network);
                callbacks->OnLost(INetwork::Probe(network));
            }
            else {
                Logger::E(TAG, String("callback not found for LOST message").string());
            }
            break;
        }
        case CALLBACK_UNAVAIL: {
            AutoPtr<IInterface> request;
            GetObject(message, String("CNetworkRequest"), (IInterface**)&request);
            AutoPtr<IInterface> callbacks = NULL;
            synchronized(mCallbackMap) {
                mCallbackMap->Get(request, (IInterface**)&callbacks);
            }
            if (callbacks != NULL) {
                AutoPtr<IInterface> network;
                GetObject(message, String("CNetwork"), (IInterface**)&network);
                IConnectivityManagerNetworkCallback::Probe(callbacks)->OnUnavailable();
            }
            else {
                Logger::E(TAG, String("callback not found for UNAVAIL message").string());
            }
            break;
        }
        case CALLBACK_CAP_CHANGED: {
            AutoPtr<IInterface> request;
            GetObject(message, String("CNetworkRequest"), (IInterface**)&request);
            AutoPtr<IConnectivityManagerNetworkCallback> callbacks;
            GetCallbacks(INetworkRequest::Probe(request), (IConnectivityManagerNetworkCallback**)&callbacks);
            if (callbacks != NULL) {
                AutoPtr<IInterface> network;
                GetObject(message, String("CNetwork"), (IInterface**)&network);
                AutoPtr<IInterface> cap;
                GetObject(message, String("CNetworkCapabilities"), (IInterface**)&cap);
                callbacks->OnCapabilitiesChanged(INetwork::Probe(network), INetworkCapabilities::Probe(cap));
            }
            else {
                Logger::E(TAG, String("callback not found for CAP_CHANGED message").string());
            }
            break;
        }
        case CALLBACK_IP_CHANGED: {
            AutoPtr<IInterface> request;
            GetObject(message, String("CNetworkRequest"), (IInterface**)&request);
            AutoPtr<IConnectivityManagerNetworkCallback> callbacks;
            GetCallbacks(INetworkRequest::Probe(request), (IConnectivityManagerNetworkCallback**)&callbacks);
            if (callbacks != NULL) {
                AutoPtr<IInterface> network;
                GetObject(message, String("CNetwork"), (IInterface**)&network);
                AutoPtr<IInterface> lp;
                GetObject(message, String("CLinkProperties"), (IInterface**)&lp);

                callbacks->OnLinkPropertiesChanged(INetwork::Probe(network), ILinkProperties::Probe(lp));
            }
            else {
                Logger::E(TAG, String("callback not found for IP_CHANGED message").string());
            }
            break;
        }
        case CALLBACK_RELEASED: {
            AutoPtr<IInterface> req;
            GetObject(message, String("CNetworkRequest"), (IInterface**)&req);
            AutoPtr<IInterface> callbacks = NULL;
            synchronized(mCallbackMap) {
                mCallbackMap->Remove(req, (IInterface**)&callbacks);
            }
            if (callbacks != NULL) {
                synchronized(mRefCount) {
                    Int32 count;
                    mRefCount->DecrementAndGet(&count);
                    if (0 == count) {
                        AutoPtr<ILooper> looper;
                        GetLooper((ILooper**)&looper);
                        looper->Quit();
                    }
                }
            }
            else {
                Logger::E(TAG, String("callback not found for CANCELED message").string());
            }
            break;
        }
        case CALLBACK_EXIT: {
            Logger::D(TAG, String("Listener quiting").string());
            AutoPtr<ILooper> looper;
            GetLooper((ILooper**)&looper);
            looper->Quit();
            break;
        }
        case EXPIRE_LEGACY_REQUEST: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            Int32 arg1;
            message->GetArg1(&arg1);
            mCm->ExpireRequest(INetworkCapabilities::Probe(obj), arg1);
            break;
        }
    }
    return NOERROR;
}

CConnectivityManager::LegacyRequest::InnerSub_ConnectivityManagerNetworkCallback::InnerSub_ConnectivityManagerNetworkCallback(LegacyRequest* const host)
    :   mHost(host)
{}

ECode CConnectivityManager::LegacyRequest::InnerSub_ConnectivityManagerNetworkCallback::OnAvailable(
    /* [in] */ INetwork* network)
{
    mHost->mCurrentNetwork = network;
    String s;
    IObject::Probe(network)->ToString(&s);
    Logger::D(TAG, (String("startUsingNetworkFeature got Network:") + s).string());
    Boolean b;
    CConnectivityManager::SetProcessDefaultNetworkForHostResolution(network, &b);
    return NOERROR;
}

ECode CConnectivityManager::LegacyRequest::InnerSub_ConnectivityManagerNetworkCallback::OnLost(
    /* [in] */ INetwork* network)
{
    VALIDATE_NOT_NULL(network)

    Boolean b;
    if (IObject::Probe(network)->Equals(mHost->mCurrentNetwork, &b), b) {
        mHost->mCurrentNetwork = NULL;
        SetProcessDefaultNetworkForHostResolution(NULL, &b);
    }
    String s;
    IObject::Probe(network)->ToString(&s);
    Logger::D(TAG, (String("startUsingNetworkFeature lost Network:") + s).string());
    return NOERROR;
}

ECode CConnectivityManager::CallbackHandler::GetObject(
    /* [in] */ IMessage* msg,
    /* [in] */ const String& clsName,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    VALIDATE_NOT_NULL(msg)

    AutoPtr<IBundle> data;
    msg->GetData((IBundle**)&data);
    AutoPtr<IParcelable> p;
    data->GetParcelable(clsName, (IParcelable**)&p);
    *result = p.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CConnectivityManager::CallbackHandler::GetCallbacks(
    /* [in] */ INetworkRequest* req,
   /* [out] */ IConnectivityManagerNetworkCallback** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    VALIDATE_NOT_NULL(req)

    synchronized(mCallbackMap) {
        AutoPtr<IInterface> value;
        mCallbackMap->Get(req, (IInterface**)&value);
        *result = IConnectivityManagerNetworkCallback::Probe(value);
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos

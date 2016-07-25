
#include "elastos/droid/internal/telephony/dataconnection/DataCallResponse.h"
#include "elastos/droid/internal/telephony/dataconnection/DcFailCause.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/net/CLinkAddress.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/CRouteInfo.h"
#include "elastos/droid/os/SystemProperties.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Net::CLinkAddress;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::CRouteInfo;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Net::NetworkUtils;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::Net::IInet4Address;
using Elastos::Net::IInetAddress;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DataCallResponse::SetupResult
//=============================================================================
CAR_INTERFACE_IMPL(DataCallResponse::SetupResult, Object, IDataCallResponseSetupResult)

DataCallResponse::SetupResult::SetupResult()
{
    DcFailCause::FromInt32(0, (IDcFailCause**)&mFailCause);
}

ECode DataCallResponse::SetupResult::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    String rev;
    rev.AppendFormat("%d  SetupResult.mFailCause=%s", mEnumValue, TO_CSTR(mFailCause));
    *result = rev;
    return NOERROR;
}

//=============================================================================
// DataCallResponse
//=============================================================================
CAR_INTERFACE_IMPL(DataCallResponse, Object, IDataCallResponse)

DataCallResponse::DataCallResponse()
    : DBG(TRUE)
    , LOG__TAG("DataCallResponse")
    , mVersion(0)
    , mStatus(0)
    , mCid(0)
    , mActive(0)
    , mType("")
    , mIfname("")
    , mAddresses(ArrayOf<String>::Alloc(0))
    , mDnses(ArrayOf<String>::Alloc(0))
    , mGateways(ArrayOf<String>::Alloc(0))
    , mSuggestedRetryTime(-1)
    , mPcscf(ArrayOf<String>::Alloc(0))
    , mMtu(IPhoneConstants::UNSET_MTU)
{}

ECode DataCallResponse::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuffer sb;
    sb.Append("DataCallResponse: {");
    sb.Append("version=");
    sb.Append(mVersion);
    sb.Append(" status=");
    sb.Append(mStatus);
    sb.Append(" retry=");
    sb.Append(mSuggestedRetryTime);
    sb.Append(" cid=");
    sb.Append(mCid);
    sb.Append(" active=");
    sb.Append(mActive);
    sb.Append(" type=");
    sb.Append(mType);
    sb.Append(" ifname=");
    sb.Append(mIfname);
    sb.Append(" mtu=");
    sb.Append(mMtu);
    sb.Append(" addresses=[");
    for (Int32 i = 0; i < mAddresses->GetLength(); ++i) {
        String addr = (*mAddresses)[i];
        sb.Append(addr);
        sb.Append(",");
    }
    if (mAddresses->GetLength() > 0) sb.DeleteCharAt(sb.GetLength()-1);
    sb.Append("] dnses=[");
    for (Int32 i = 0; i < mDnses->GetLength(); ++i) {
        String addr = (*mDnses)[i];
        sb.Append(addr);
        sb.Append(",");
    }
    if (mDnses->GetLength() > 0) sb.DeleteCharAt(sb.GetLength()-1);
    sb.Append("] gateways=[");
    for (Int32 i = 0; i < mGateways->GetLength(); ++i) {
        String addr = (*mGateways)[i];
        sb.Append(addr);
        sb.Append(",");
    }
    if (mGateways->GetLength() > 0) sb.DeleteCharAt(sb.GetLength()-1);
    sb.Append("] pcscf=[");
    for (Int32 i = 0; i < mPcscf->GetLength(); ++i) {
        String addr = (*mPcscf)[i];
        sb.Append(addr);
        sb.Append(",");
    }
    if (mPcscf->GetLength() > 0) sb.DeleteCharAt(sb.GetLength()-1);
    sb.Append("]}");
    return sb.ToString(result);
}

ECode DataCallResponse::SetLinkProperties(
    /* [in] */ ILinkProperties* lp,
    /* [in] */ Boolean okToUseSystemPropertyDns,
    /* [out] */ IDataCallResponseSetupResult** _result)
{
    VALIDATE_NOT_NULL(_result)

    AutoPtr<ILinkProperties> linkProperties = lp;
    AutoPtr<DataCallResponse::SetupResult> result = new DataCallResponse::SetupResult();
    // Start with clean network properties and if we have
    // a failure we'll clear again at the bottom of this code.
    if (linkProperties == NULL)
        CLinkProperties::New((ILinkProperties**)&linkProperties);
    else
        linkProperties->Clear();
    if (mStatus == DcFailCause_NONE) {
        String propertyPrefix = String("net.") + mIfname + ".";
        // try {
        ECode ec;
        do {
            // set interface name
            ec = linkProperties->SetInterfaceName(mIfname);
            if (FAILED(ec)) break;
            // set link addresses
            if (mAddresses != NULL && mAddresses->GetLength() > 0) {
                for (Int32 i = 0; i < mAddresses->GetLength(); ++i) {
                    String addr = (*mAddresses)[i];
                    addr = addr.Trim();
                    if (addr.IsEmpty()) continue;
                    AutoPtr<ILinkAddress> la;
                    Int32 addrPrefixLen;
                    AutoPtr<ArrayOf<String> > ap;
                    StringUtils::Split(addr, "/", (ArrayOf<String>**)&ap);
                    if (ap->GetLength() == 2) {
                        addr = (*ap)[0];
                        String s;
                        StringUtils::ReplaceAll((*ap)[1], "[\\D]", "", &s);
                        addrPrefixLen = StringUtils::ParseInt32(s);
                    } else {
                        addrPrefixLen = 0;
                    }
                    AutoPtr<IInetAddress> ia;
                    // try {
                    ec = NetworkUtils::NumericToInetAddress(addr, (IInetAddress**)&ia);
                    // } catch (IllegalArgumentException e) {
                    if ((ECode) E_ILLEGAL_ARGUMENT_EXCEPTION == ec) {
                        Logger::E(LOG__TAG, "Non-numeric ip addr=%s", addr.string());
                        ec = E_UNKNOWN_HOST_EXCEPTION;
                        break;
                    }
                    // }
                    Boolean isAnyLocalAddress;
                    ec = ia->IsAnyLocalAddress(&isAnyLocalAddress);
                    if (FAILED(ec)) break;
                    if (! isAnyLocalAddress) {
                        if (addrPrefixLen == 0) {
                            // Assume point to point
                            addrPrefixLen = (IInet4Address::Probe(ia) != NULL) ? 32 : 128;
                        }
                        if (DBG) Logger::D(LOG__TAG, "addr/pl=%s/%d", addr.string(), addrPrefixLen);
                        ec = CLinkAddress::New(ia, addrPrefixLen, (ILinkAddress**)&la);
                        if (FAILED(ec)) break;
                        Boolean b;
                        ec = linkProperties->AddLinkAddress(la, &b);
                        if (FAILED(ec)) break;
                    }
                }
            } else {
                ec = E_UNKNOWN_HOST_EXCEPTION;
                Logger::E(LOG__TAG, "no address for ifname=%s", mIfname.string());
                break;
            }
            // set dns servers
            if (mDnses != NULL && mDnses->GetLength() > 0) {
                for (Int32 i = 0; i < mDnses->GetLength(); ++i) {
                    String addr = (*mDnses)[i];
                    addr = addr.Trim();
                    if (addr.IsEmpty()) continue;
                    AutoPtr<IInetAddress> ia;
                    // try {
                    ec = NetworkUtils::NumericToInetAddress(addr, (IInetAddress**)&ia);
                    // } catch (IllegalArgumentException e) {
                    if ((ECode) E_ILLEGAL_ARGUMENT_EXCEPTION == ec) {
                        ec = E_UNKNOWN_HOST_EXCEPTION;
                        Logger::E(LOG__TAG, "Non-numeric dns addr=%s", addr.string());
                        break;
                    }
                    // }
                    Boolean isAnyLocalAddress;
                    ec = ia->IsAnyLocalAddress(&isAnyLocalAddress);
                    if (FAILED(ec)) break;
                    if (!isAnyLocalAddress) {
                        Boolean b;
                        ec = linkProperties->AddDnsServer(ia, &b);
                        if (FAILED(ec)) break;
                    }
                }
            } else if (okToUseSystemPropertyDns){
                AutoPtr<ArrayOf<String> > dnsServers = ArrayOf<String>::Alloc(2);
                SystemProperties::Get(propertyPrefix + "dns1", &(*dnsServers)[0]);
                SystemProperties::Get(propertyPrefix + "dns2", &(*dnsServers)[1]);
                for (Int32 i = 0; i < dnsServers->GetLength(); ++i) {
                    String dnsAddr = (*dnsServers)[i];
                    dnsAddr = dnsAddr.Trim();
                    if (dnsAddr.IsEmpty()) continue;
                    AutoPtr<IInetAddress> ia;
                    // try {
                    ec = NetworkUtils::NumericToInetAddress(dnsAddr, (IInetAddress**)&ia);
                    // } catch (IllegalArgumentException e) {
                    if ((ECode) E_ILLEGAL_ARGUMENT_EXCEPTION == ec) {
                        ec = E_UNKNOWN_HOST_EXCEPTION;
                        Logger::E(LOG__TAG, "Non-numeric dns addr=%s", dnsAddr.string());
                        break;
                    }
                    // }
                    Boolean isAnyLocalAddress;
                    ec = ia->IsAnyLocalAddress(&isAnyLocalAddress);
                    if (FAILED(ec)) break;
                    if (!isAnyLocalAddress) {
                        Boolean b;
                        ec = linkProperties->AddDnsServer(ia, &b);
                        if (FAILED(ec)) break;
                    }
                }
            } else {
                ec = E_UNKNOWN_HOST_EXCEPTION;
                Logger::E(LOG__TAG, "Empty dns response and no system default dns");
            }
            // set gateways
            if ((mGateways == NULL) || (mGateways->GetLength() == 0)) {
                String sysGateways;
                SystemProperties::Get(propertyPrefix + "gw", &sysGateways);
                if (sysGateways != NULL) {
                    mGateways = NULL;
                    StringUtils::Split(sysGateways, " ", (ArrayOf<String>**)&mGateways);
                } else {
                    mGateways = ArrayOf<String>::Alloc(0);
                }
            }
            for (Int32 i = 0; i < mGateways->GetLength(); ++i) {
                String addr = (*mGateways)[i];
                addr = addr.Trim();
                if (addr.IsEmpty()) continue;
                AutoPtr<IInetAddress> ia;
                // try {
                ec = NetworkUtils::NumericToInetAddress(addr, (IInetAddress**)&ia);
                // } catch (IllegalArgumentException e) {
                if ((ECode) E_ILLEGAL_ARGUMENT_EXCEPTION == ec) {
                    ec = E_UNKNOWN_HOST_EXCEPTION;
                    Logger::E("Non-numeric gateway addr=%s", addr.string());
                    break;
                }
                // }
                // Allow 0.0.0.0 or :: as a gateway; this indicates a point-to-point interface.
                AutoPtr<IRouteInfo> ri;
                ec = CRouteInfo::New(ia, (IRouteInfo**)&ri);
                if (FAILED(ec)) break;
                Boolean b;
                ec = linkProperties->AddRoute(ri, &b);
                if (FAILED(ec)) break;
            }
            // set interface MTU
            // this may clobber the setting read from the APN db, but that's ok
            ec = linkProperties->SetMtu(mMtu);
            if (FAILED(ec)) break;
            result->mEnumValue = DataCallResponse_SUCCESS;
        } while(FALSE);
        // } catch (UnknownHostException e) {
        if ((ECode) E_UNKNOWN_HOST_EXCEPTION == ec) {
            Logger::D(LOG__TAG, "setLinkProperties: UnknownHostException %d", ec);
            // e->PrintStackTrace();
            result->mEnumValue = DataCallResponse_ERR_UnacceptableParameter;
        }
        // }
    } else {
        if (mVersion < 4) {
            result->mEnumValue = DataCallResponse_ERR_GetLastErrorFromRil;
        } else {
            result->mEnumValue = DataCallResponse_ERR_RilError;
        }
    }
    // An error occurred so clear properties
    if (result->mEnumValue != DataCallResponse_SUCCESS) {
        if (DBG) {
            Logger::D(LOG__TAG, "setLinkProperties: error clearing LinkProperties "
                    "status=%d result=%d", mStatus, TO_CSTR(result));
        }
        linkProperties->Clear();
    }
    *_result = result.Get();
    REFCOUNT_ADD(*_result)
    return NOERROR;
}

ECode DataCallResponse::GetVersion(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVersion;
    return NOERROR;
}

ECode DataCallResponse::SetVersion(
    /* [in] */ Int32 version)
{
    mVersion = version;
    return NOERROR;
}

ECode DataCallResponse::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStatus;
    return NOERROR;
}

ECode DataCallResponse::SetStatus(
    /* [in] */ Int32 status)
{
    mStatus = status;
    return NOERROR;
}

ECode DataCallResponse::GetCid(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCid;
    return NOERROR;
}

ECode DataCallResponse::SetCid(
    /* [in] */ Int32 cid)
{
    mCid = cid;
    return NOERROR;
}

ECode DataCallResponse::GetActive(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mActive;
    return NOERROR;
}

ECode DataCallResponse::SetActive(
    /* [in] */ Int32 active)
{
    mActive = active;
    return NOERROR;
}

ECode DataCallResponse::GetType(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mType;
    return NOERROR;
}

ECode DataCallResponse::SetType(
    /* [in] */ const String& type)
{
    mType = type;
    return NOERROR;
}

ECode DataCallResponse::GetIfname(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIfname;
    return NOERROR;
}

ECode DataCallResponse::SetIfname(
    /* [in] */ const String& ifname)
{
    mIfname = ifname;
    return NOERROR;
}

ECode DataCallResponse::GetAddresses(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAddresses;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DataCallResponse::SetAddresses(
    /* [in] */ ArrayOf<String>* addresses)
{
    mAddresses = addresses;
    return NOERROR;
}

ECode DataCallResponse::GetDnses(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDnses;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DataCallResponse::SetDnses(
    /* [in] */ ArrayOf<String>* dnses)
{
    mDnses = dnses;
    return NOERROR;
}

ECode DataCallResponse::GetGateways(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGateways;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DataCallResponse::SetGateways(
    /* [in] */ ArrayOf<String>* gateways)
{
    mGateways = gateways;
    return NOERROR;
}

ECode DataCallResponse::GetSuggestedRetryTime(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSuggestedRetryTime;
    return NOERROR;
}

ECode DataCallResponse::SetSuggestedRetryTime(
    /* [in] */ Int32 suggestedRetryTime)
{
    mSuggestedRetryTime = suggestedRetryTime;
    return NOERROR;
}

ECode DataCallResponse::GetPcscf(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPcscf;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DataCallResponse::SetPcscf(
    /* [in] */ ArrayOf<String>* pcscf)
{
    mPcscf = pcscf;
    return NOERROR;
}

ECode DataCallResponse::GetMtu(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMtu;
    return NOERROR;
}

ECode DataCallResponse::SetMtu(
    /* [in] */ Int32 mtu)
{
    mMtu = mtu;
    return NOERROR;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#include "elastos/droid/internal/telephony/dataconnection/DataCallResponse.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(DataCallResponse, Object, IDataCallResponse)

DataCallResponse::DataCallResponse()
    : DBG(true)
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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        StringBuffer sb = new StringBuffer();
        sb.append("DataCallResponse: {")
           .append("version=").append(version)
           .append(" status=").append(status)
           .append(" retry=").append(suggestedRetryTime)
           .append(" cid=").append(cid)
           .append(" active=").append(active)
           .append(" type=").append(type)
           .append(" ifname=").append(ifname)
           .append(" mtu=").append(mtu)
           .append(" addresses=[");
        for (String addr : addresses) {
            sb.append(addr);
            sb.append(",");
        }
        if (addresses.length > 0) sb.deleteCharAt(sb.length()-1);
        sb.append("] dnses=[");
        for (String addr : dnses) {
            sb.append(addr);
            sb.append(",");
        }
        if (dnses.length > 0) sb.deleteCharAt(sb.length()-1);
        sb.append("] gateways=[");
        for (String addr : gateways) {
            sb.append(addr);
            sb.append(",");
        }
        if (gateways.length > 0) sb.deleteCharAt(sb.length()-1);
        sb.append("] pcscf=[");
        for (String addr : pcscf) {
            sb.append(addr);
            sb.append(",");
        }
        if (pcscf.length > 0) sb.deleteCharAt(sb.length()-1);
        sb.append("]}");
        return sb.toString();

#endif
}

ECode DataCallResponse::SetLinkProperties(
    /* [in] */ ILinkProperties* linkProperties,
    /* [in] */ Boolean okToUseSystemPropertyDns,
    /* [out] */ DataCallResponseSetupResult* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        SetupResult result;
        // Start with clean network properties and if we have
        // a failure we'll clear again at the bottom of this code.
        if (linkProperties == null)
            linkProperties = new LinkProperties();
        else
            linkProperties.clear();
        if (status == DcFailCause.NONE.getErrorCode()) {
            String propertyPrefix = "net." + ifname + ".";
            try {
                // set interface name
                linkProperties.setInterfaceName(ifname);
                // set link addresses
                if (addresses != null && addresses.length > 0) {
                    for (String addr : addresses) {
                        addr = addr.trim();
                        if (addr.isEmpty()) continue;
                        LinkAddress la;
                        int addrPrefixLen;
                        String [] ap = addr.split("/");
                        if (ap.length == 2) {
                            addr = ap[0];
                            addrPrefixLen = Integer.parseInt(ap[1].replaceAll("[\\D]",""));
                        } else {
                            addrPrefixLen = 0;
                        }
                        InetAddress ia;
                        try {
                            ia = NetworkUtils.numericToInetAddress(addr);
                        } catch (IllegalArgumentException e) {
                            throw new UnknownHostException("Non-numeric ip addr=" + addr);
                        }
                        if (! ia.isAnyLocalAddress()) {
                            if (addrPrefixLen == 0) {
                                // Assume point to point
                                addrPrefixLen = (ia instanceof Inet4Address) ? 32 : 128;
                            }
                            if (DBG) Rlog.d(LOG__TAG, "addr/pl=" + addr + "/" + addrPrefixLen);
                            la = new LinkAddress(ia, addrPrefixLen);
                            linkProperties.addLinkAddress(la);
                        }
                    }
                } else {
                    throw new UnknownHostException("no address for ifname=" + ifname);
                }
                // set dns servers
                if (dnses != null && dnses.length > 0) {
                    for (String addr : dnses) {
                        addr = addr.trim();
                        if (addr.isEmpty()) continue;
                        InetAddress ia;
                        try {
                            ia = NetworkUtils.numericToInetAddress(addr);
                        } catch (IllegalArgumentException e) {
                            throw new UnknownHostException("Non-numeric dns addr=" + addr);
                        }
                        if (! ia.isAnyLocalAddress()) {
                            linkProperties.addDnsServer(ia);
                        }
                    }
                } else if (okToUseSystemPropertyDns){
                    String dnsServers[] = new String[2];
                    dnsServers[0] = SystemProperties.get(propertyPrefix + "dns1");
                    dnsServers[1] = SystemProperties.get(propertyPrefix + "dns2");
                    for (String dnsAddr : dnsServers) {
                        dnsAddr = dnsAddr.trim();
                        if (dnsAddr.isEmpty()) continue;
                        InetAddress ia;
                        try {
                            ia = NetworkUtils.numericToInetAddress(dnsAddr);
                        } catch (IllegalArgumentException e) {
                            throw new UnknownHostException("Non-numeric dns addr=" + dnsAddr);
                        }
                        if (! ia.isAnyLocalAddress()) {
                            linkProperties.addDnsServer(ia);
                        }
                    }
                } else {
                    throw new UnknownHostException("Empty dns response and no system default dns");
                }
                // set gateways
                if ((gateways == null) || (gateways.length == 0)) {
                    String sysGateways = SystemProperties.get(propertyPrefix + "gw");
                    if (sysGateways != null) {
                        gateways = sysGateways.split(" ");
                    } else {
                        gateways = new String[0];
                    }
                }
                for (String addr : gateways) {
                    addr = addr.trim();
                    if (addr.isEmpty()) continue;
                    InetAddress ia;
                    try {
                        ia = NetworkUtils.numericToInetAddress(addr);
                    } catch (IllegalArgumentException e) {
                        throw new UnknownHostException("Non-numeric gateway addr=" + addr);
                    }
                    // Allow 0.0.0.0 or :: as a gateway; this indicates a point-to-point interface.
                    linkProperties.addRoute(new RouteInfo(ia));
                }
                // set interface MTU
                // this may clobber the setting read from the APN db, but that's ok
                linkProperties.setMtu(mtu);
                result = SetupResult.SUCCESS;
            } catch (UnknownHostException e) {
                Rlog.d(LOG__TAG, "setLinkProperties: UnknownHostException " + e);
                e.printStackTrace();
                result = SetupResult.ERR_UnacceptableParameter;
            }
        } else {
            if (version < 4) {
                result = SetupResult.ERR_GetLastErrorFromRil;
            } else {
                result = SetupResult.ERR_RilError;
            }
        }
        // An error occurred so clear properties
        if (result != SetupResult.SUCCESS) {
            if(DBG) {
                Rlog.d(LOG__TAG, "setLinkProperties: error clearing LinkProperties " +
                        "status=" + status + " result=" + result);
            }
            linkProperties.clear();
        }
        return result;

#endif
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
    /* [in] */ String type)
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
    /* [in] */ String ifname)
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
    /* [out] */ ArrayOf<String>** result)
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

    CARAPI SetMtu(
        /* [in] */ Int32 mtu);

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

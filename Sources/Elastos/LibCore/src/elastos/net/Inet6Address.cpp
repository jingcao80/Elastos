#include "elastos/net/Inet6Address.h"
#include "elastos/net/CInet6Address.h"
#include "elastos/net/NetworkInterface.h"
#include "droid/system/OsConstants.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::System::OsConstants;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Net {

static AutoPtr<IInetAddress> InitANY()
{
    AutoPtr<ArrayOf<Byte> > ipAddress = ArrayOf<Byte>::Alloc(16);
    AutoPtr<CInet6Address> ipv6Addr;
    CInet6Address::NewByFriend(ipAddress, String(NULL), 0, (CInet6Address**)&ipv6Addr);
    AutoPtr<IInetAddress> res = IInetAddress::Probe(ipv6Addr);
    return res;
}

static AutoPtr<IInetAddress> InitLOOPBACK()
{
    AutoPtr<ArrayOf<Byte> > ipAddress = ArrayOf<Byte>::Alloc(16);
    (*ipAddress)[15] = 1;
    AutoPtr<CInet6Address> ipv6Addr;
    CInet6Address::NewByFriend(ipAddress, String("localhost"), 0, (CInet6Address**)&ipv6Addr);
    AutoPtr<IInetAddress> res = IInetAddress::Probe(ipv6Addr);
    return res;
}

AutoPtr<IInetAddress> Inet6Address::ANY = InitANY();
AutoPtr<IInetAddress> Inet6Address::LOOPBACK = InitLOOPBACK();

CAR_INTERFACE_IMPL(Inet6Address, InetAddress, IInet6Address)

Inet6Address::Inet6Address()
    : mScopeId(0)
    , mScopeIdSet(FALSE)
    , mScopeIfnameSet(FALSE)
{}

ECode Inet6Address::constructor(
    /* [in] */ ArrayOf<Byte>* address,
    /* [in] */ const String& hostName,
    /* [in] */ Int32 scopeId)
{
    FAIL_RETURN(InetAddress::constructor(OsConstants::_AF_INET6, address, hostName))
    mScopeId = scopeId;
    mScopeIdSet = (scopeId != 0);
    return NOERROR;
}

ECode Inet6Address::GetByAddress(
    /* [in] */ const String& host,
    /* [in] */ ArrayOf<Byte>* addr,
    /* [in] */ Int32 scopeId,
    /* [out] */ IInet6Address** address)
{
    VALIDATE_NOT_NULL(address);
    *address = NULL;

    if (addr == NULL || addr->GetLength() != 16) {
//        throw new UnknownHostException("Illegal IPv6 address");
        return E_UNKNOWN_HOST_EXCEPTION;
    }
    if (scopeId < 0) {
        scopeId = 0;
    }

    AutoPtr<CInet6Address> obj;
    CInet6Address::NewByFriend(addr, host, scopeId, (CInet6Address**)&obj);
    *address = IInet6Address::Probe(obj);
    REFCOUNT_ADD(*address);
    return NOERROR;
}

ECode Inet6Address::GetByAddress(
    /* [in] */ const String& host,
    /* [in] */ ArrayOf<Byte>* addr,
    /* [in] */ INetworkInterface* nif,
    /* [out] */ IInet6Address** address)
{
    FAIL_RETURN(GetByAddress(host, addr, 0, address));

    // if nif is null, nothing needs to be set.
    if (NULL == nif) {
        return NOERROR;
    }

    AutoPtr<IEnumeration> em;
    nif->GetInetAddresses((IEnumeration**)&em);
    Boolean hasNext;
    while (em->HasMoreElements(&hasNext), hasNext) {
        AutoPtr<IInterface> ia;
        em->GetNextElement((IInterface**)&ia);
        AutoPtr<ArrayOf<Byte> > addresses;
        IInetAddress::Probe(ia)->GetAddress((ArrayOf<Byte>**)&addresses);
        if(addresses->GetLength() == 16)
        {
            AutoPtr<IInet6Address> v6ia = IInet6Address::Probe(ia);
            if(v6ia != NULL){
                CInet6Address* cv6ia = (CInet6Address*)(v6ia.Get());
                Boolean isSameType = cv6ia->CompareLocalType((IInet6Address*)*address);
                if(isSameType)
                {
                    ((CInet6Address*)(*address))->mScopeIdSet = TRUE;
                    ((CInet6Address*)(*address))->mScopeId = cv6ia->mScopeId;
                    ((CInet6Address*)(*address))->mScopeIfnameSet = TRUE;
                    nif->GetName(&(((CInet6Address*)(*address))->mIfname));
                    break;
                }
            }
        }
    }
    if(!((CInet6Address*)(*address))->mScopeIdSet)
    {
        return E_UNKNOWN_HOST_EXCEPTION;
    }
    return NOERROR;
}

Boolean Inet6Address::CompareLocalType(
    /* [in] */ IInet6Address* ia)
{
    if (ia == NULL) {
        return FALSE;
    }

    Boolean iaSLA, isSLA;
    IInetAddress::Probe(ia)->IsSiteLocalAddress(&iaSLA);
    IsSiteLocalAddress(&isSLA);
    if (iaSLA && isSLA) {
        return TRUE;
    }
    Boolean iaLLA, isLLA;
    IInetAddress::Probe(ia)->IsLinkLocalAddress(&iaLLA);
    IsLinkLocalAddress(&isLLA);
    if (iaLLA && isLLA) {
        return TRUE;
    }
    if (!iaSLA && !iaLLA) {
        return TRUE;
    }
    return FALSE;
}

ECode Inet6Address::IsAnyLocalAddress(
    /* [out] */ Boolean* isAnyLocalAddress)
{
    VALIDATE_NOT_NULL(isAnyLocalAddress);
    AutoPtr<ArrayOf<Byte> > anyIpaddress;
    ANY->GetAddress((ArrayOf<Byte>**)&anyIpaddress);
    *isAnyLocalAddress = Arrays::Equals(mIpAddress, anyIpaddress);
    return NOERROR;
}


ECode Inet6Address::IsIPv4CompatibleAddress(
    /* [out] */ Boolean* isIPv4Compatible)
{
    VALIDATE_NOT_NULL(isIPv4Compatible);
    for (Int32 i = 0; i < 12; i++) {
        if ((*mIpAddress)[i] != 0) {
            *isIPv4Compatible = FALSE;
            return NOERROR;
        }
    }
    *isIPv4Compatible = TRUE;
    return NOERROR;
}

ECode Inet6Address::IsLinkLocalAddress(
    /* [out] */ Boolean* isLinkLocalAddress)
{
    VALIDATE_NOT_NULL(isLinkLocalAddress);
    // the first 10 bits need to be 1111111010 (1018)
    *isLinkLocalAddress = (((*mIpAddress)[0] & 0xff) == 0xfe) &&
            (((*mIpAddress)[1] & 0xc0) == 0x80);
    return NOERROR;
}

ECode Inet6Address::IsLoopbackAddress(
    /* [out] */ Boolean* isLoopbackAddress)
{
    VALIDATE_NOT_NULL(isLoopbackAddress);
    // The last word must be 1
    AutoPtr<ArrayOf<Byte> > loopBackAddress;
    LOOPBACK->GetAddress((ArrayOf<Byte>**)&loopBackAddress);
    *isLoopbackAddress = Arrays::Equals(mIpAddress, loopBackAddress);
    return NOERROR;
}

ECode Inet6Address::IsMCGlobal(
    /* [out] */ Boolean* isMCGlobal)
{
    VALIDATE_NOT_NULL(isMCGlobal);
    // the first byte should be 0xFF and the lower 4 bits
    // of the second byte should be 0xE
    *isMCGlobal = (((*mIpAddress)[0] & 0xff) == 0xff) &&
            (((*mIpAddress)[1] & 0x0f) == 0x0e);
    return NOERROR;
}

ECode Inet6Address::IsMCNodeLocal(
    /* [out] */ Boolean* isMCNodeLocal)
{
    VALIDATE_NOT_NULL(isMCNodeLocal);
    // the first byte should be 0xFF and the lower 4 bits
    // of the second byte should be 0x1
    *isMCNodeLocal = (((*mIpAddress)[0] & 0xff) == 0xff) &&
            (((*mIpAddress)[1] & 0x0f) == 0x01);
    return NOERROR;
}

ECode Inet6Address::IsMCLinkLocal(
    /* [out] */ Boolean* isMCLinkLocal)
{
    VALIDATE_NOT_NULL(isMCLinkLocal);
    // the first byte should be 0xFF and the lower 4 bits
    // of the second byte should be 0x2
    *isMCLinkLocal = (((*mIpAddress)[0] & 0xff) == 0xff) &&
            (((*mIpAddress)[1] & 0x0f) == 0x02);
    return NOERROR;
}

ECode Inet6Address::IsMCSiteLocal(
    /* [out] */ Boolean* isMCSiteLocal)
{
    VALIDATE_NOT_NULL(isMCSiteLocal);
    // the first byte should be 0xFF and the lower 4 bits
    // of the second byte should be 0x5
    *isMCSiteLocal = (((*mIpAddress)[0] & 0xff) == 0xff) &&
            (((*mIpAddress)[1] & 0x0f) == 0x05);
    return NOERROR;
}

ECode Inet6Address::IsMCOrgLocal(
    /* [out] */ Boolean* isMCOrgLocal)
{
    VALIDATE_NOT_NULL(isMCOrgLocal);
    // the first byte should be 0xFF and the lower 4 bits
    // of the second byte should be 0x8
    *isMCOrgLocal = (((*mIpAddress)[0] & 0xff) == 0xff) &&
            (((*mIpAddress)[1] & 0x0f) == 0x08);
    return NOERROR;
}

ECode Inet6Address::IsMulticastAddress(
    /* [out] */ Boolean* isMulticastAddress)
{
    VALIDATE_NOT_NULL(isMulticastAddress);
    // Multicast addresses are prefixed with 11111111 (255)
    *isMulticastAddress = (((*mIpAddress)[0] & 0xff) == 0xff);
    return NOERROR;
}

ECode Inet6Address::IsSiteLocalAddress(
    /* [out] */ Boolean* isSiteLocalAddress)
{
    VALIDATE_NOT_NULL(isSiteLocalAddress);
    // the first 10 bits need to be 1111111011 (1019)
    *isSiteLocalAddress = (((*mIpAddress)[0] & 0xff) == 0xfe) &&
            (((*mIpAddress)[1] & 0xc0) == 0xc0);
    return NOERROR;
}

ECode Inet6Address::GetScopeId(
    /* [out] */ Int32* scopeId)
{
    VALIDATE_NOT_NULL(scopeId);
    if (mScopeIdSet) {
        *scopeId = mScopeId;
        return NOERROR;
    }
    *scopeId = 0;
    return NOERROR;
}

ECode Inet6Address::GetScopedInterface(
    /* [out] */ INetworkInterface** scopedInterface)
{
    VALIDATE_NOT_NULL(scopedInterface);
    if (mScopeIfnameSet && (mIfname.IsNull())) {
        NetworkInterface::GetByName(mIfname, scopedInterface);
    }
    else{
        scopedInterface = NULL;
    }
    return NOERROR;
}

void Inet6Address::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
//     ObjectOutputStream.PutField fields = stream.putFields();
//     if (ipaddress == null) {
//         fields.put("ipaddress", null);
//     } else {
//         fields.put("ipaddress", ipaddress);
//     }

//     fields.put("scope_id", scope_id);
//     fields.put("scope_id_set", scope_id_set);
//     fields.put("scope_ifname_set", scope_ifname_set);
//     fields.put("ifname", ifname);
//     stream.writeFields();
}

void  Inet6Address::ReadObject(
    /* [in] */ IObjectOutputStream* stream)
{
//     ObjectInputStream.GetField fields = stream.readFields();
//     ipaddress = (byte[]) fields.get("ipaddress", null);
//     scope_id = fields.get("scope_id", 0);
//     scope_id_set = fields.get("scope_id_set", false);
//     ifname = (String) fields.get("ifname", null);
//     scope_ifname_set = fields.get("scope_ifname_set", false);
//     if (scope_ifname_set && null != ifname) {
//         scopedIf = NetworkInterface.getByName(ifname);
//     }
}

ECode Inet6Address::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String addr;
    InetAddress::ToString(&addr);
    StringBuilder sb(addr);
    if (!mIfname.IsNull()){
        sb.Append("%");
        sb.Append(mIfname);
    }
    else if (0 != mScopeId){
        sb.Append("%");
        sb.Append(mScopeId);
    }

    *result = sb.ToString();
    return NOERROR;
}

ECode Inet6Address::GetANY(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)

    *result = ANY;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Inet6Address::GetLOOPBACK(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)

    *result = LOOPBACK;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Net
} // namespace Elastos

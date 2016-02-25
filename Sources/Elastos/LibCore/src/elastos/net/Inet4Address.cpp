#include "Inet4Address.h"
#include "CInet4Address.h"
#include "droid/system/OsConstants.h"

using Elastos::Droid::System::OsConstants;

namespace Elastos {
namespace Net {

static AutoPtr<IInetAddress> InitANY()
{
    AutoPtr<ArrayOf<Byte> > ipAddress = ArrayOf<Byte>::Alloc(4);
    AutoPtr<CInet4Address> ipv4Addr;
    CInet4Address::NewByFriend(ipAddress, String(NULL), (CInet4Address**)&ipv4Addr);
    AutoPtr<IInetAddress> addr = IInetAddress::Probe(ipv4Addr);
    return addr;
}

static AutoPtr<IInetAddress> InitALL()
{
    AutoPtr<ArrayOf<Byte> > ipAddress = ArrayOf<Byte>::Alloc(4);
    (*ipAddress)[0] = (Byte)255;
    (*ipAddress)[1] = (Byte)255;
    (*ipAddress)[2] = (Byte)255;
    (*ipAddress)[3] = (Byte)255;
    AutoPtr<CInet4Address> ipv4Addr;
    CInet4Address::NewByFriend(ipAddress, String(NULL), (CInet4Address**)&ipv4Addr);
    AutoPtr<IInetAddress> addr = IInetAddress::Probe(ipv4Addr);
    return addr;
}

static AutoPtr<IInetAddress> InitLOOPBACK()
{
    AutoPtr<ArrayOf<Byte> > ipAddress = ArrayOf<Byte>::Alloc(4);
    (*ipAddress)[0] = 127;
    (*ipAddress)[1] = 0;
    (*ipAddress)[2] = 0;
    (*ipAddress)[3] = 1;
    AutoPtr<CInet4Address> ipv4Addr;
    CInet4Address::NewByFriend(ipAddress, String("localhost"), (CInet4Address**)&ipv4Addr);
    AutoPtr<IInetAddress> addr = IInetAddress::Probe(ipv4Addr);
    return addr;
}

AutoPtr<IInetAddress> Inet4Address::ANY = InitANY();
AutoPtr<IInetAddress> Inet4Address::ALL = InitALL();
AutoPtr<IInetAddress> Inet4Address::LOOPBACK = InitLOOPBACK();

CAR_INTERFACE_IMPL(Inet4Address, InetAddress, IInet4Address)

ECode Inet4Address::GetANY(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)

    *result = ANY;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Inet4Address::GetALL(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)

    *result = ALL;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Inet4Address::GetLOOPBACK(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)

    *result = LOOPBACK;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Inet4Address::constructor(
    /* [in] */ ArrayOf<Byte>* address,
    /* [in] */ const String& name)
{
    return InetAddress::constructor(OsConstants::_AF_INET, address, name);
}

ECode Inet4Address::IsAnyLocalAddress(
    /* [out] */ Boolean* isAnyLocalAddress)
{
    VALIDATE_NOT_NULL(isAnyLocalAddress);
    if(((*mIpAddress)[0] == 0) && ((*mIpAddress)[1] == 0) &&
       ((*mIpAddress)[2] == 0) && ((*mIpAddress)[3] == 0)) {
        *isAnyLocalAddress = TRUE;
    }
    else{
        *isAnyLocalAddress = FALSE;
    }
    return NOERROR;
}

ECode Inet4Address::IsLinkLocalAddress(
    /* [out] */ Boolean* isLinkLocalAddress)
{
    VALIDATE_NOT_NULL(isLinkLocalAddress);
    // The reference implementation does not return true for loopback
    // addresses even though RFC 3484 says to do so
    *isLinkLocalAddress = ((((*mIpAddress)[0] & 255) == 169) &&
                           (((*mIpAddress)[1] & 255) == 254));
    return NOERROR;
}

ECode Inet4Address::IsLoopbackAddress(
    /* [out] */ Boolean* isLoopbackAddress)
{
    VALIDATE_NOT_NULL(isLoopbackAddress);
    *isLoopbackAddress = (((*mIpAddress)[0] & 255) == 127);
    return NOERROR;
}

ECode Inet4Address::IsMCGlobal(
    /* [out] */ Boolean* isMCGlobal)
{
    VALIDATE_NOT_NULL(isMCGlobal);
    *isMCGlobal = FALSE;

    // Check if we have a prefix of 1110
    Boolean isMulticastAddress;
    IsMulticastAddress(&isMulticastAddress);
    if (!isMulticastAddress) {
        return NOERROR;
    }

    Int32 address = InetAddress::BytesToInt32(*mIpAddress, 0);
    /*
     * Now check the boundaries of the global space if we have an address
     * that is prefixed by something less than 111000000000000000000001
     * (fortunately we don't have to worry about sign after shifting 8 bits
     * right) it is not multicast. ( < 224.0.1.0)
     */
    if (((UInt32)address) >> 8 < 0xE00001) {
        return NOERROR;
    }

    /*
     * Now check the high boundary which is prefixed by 11101110 = 0xEE. If
     * the value is higher than this than it is not MCGlobal ( >
     * 238.255.255.255 )
     */
    if (((UInt32)address) >> 24 > 0xEE) {
        return NOERROR;
    }

    *isMCGlobal = TRUE;
    return NOERROR;
}

ECode Inet4Address::IsMCLinkLocal(
    /* [out] */ Boolean* isMCLinkLocal)
{
    VALIDATE_NOT_NULL(isMCLinkLocal);
    *isMCLinkLocal = (((*mIpAddress)[0] & 0xff) == 224) &&
                     ((*mIpAddress)[1] == 0) &&
                     ((*mIpAddress)[2] == 0);
    return NOERROR;
}

ECode Inet4Address::IsMCNodeLocal(
    /* [out] */ Boolean* isMCNodeLocal)
{
    VALIDATE_NOT_NULL(isMCNodeLocal);
    *isMCNodeLocal = FALSE;
    return NOERROR;
}

ECode Inet4Address::IsMCOrgLocal(
    /* [out] */ Boolean* isMCOrgLocal)
{
    VALIDATE_NOT_NULL(isMCOrgLocal);
    *isMCOrgLocal = (((*mIpAddress)[0] & 0xff) == 239) &&
                    (((*mIpAddress)[1] & 0xfc) == 192);
    return NOERROR;
}

ECode Inet4Address::IsMCSiteLocal(
    /* [out] */ Boolean* isMCSiteLocal)
{
    VALIDATE_NOT_NULL(isMCSiteLocal);
    *isMCSiteLocal = (((*mIpAddress)[0] & 0xff) == 239) &&
                     (((*mIpAddress)[1] & 0xff) == 255);
    return NOERROR;
}

ECode Inet4Address::IsMulticastAddress(
    /* [out] */ Boolean* isMulticastAddress)
{
    VALIDATE_NOT_NULL(isMulticastAddress);
    *isMulticastAddress = (((*mIpAddress)[0] & 0xF0) == 0xE0);
    return NOERROR;
}

ECode Inet4Address::IsSiteLocalAddress(
    /* [out] */ Boolean* isSiteLocalAddress)
{
    ArrayOf<Byte>& ipAddress = *mIpAddress;
    VALIDATE_NOT_NULL(isSiteLocalAddress);
    *isSiteLocalAddress = ((ipAddress[0] & 0xFF) == 10) ||
        (((ipAddress[0] & 0xFF) == 172) && ((ipAddress[1] & 0xF0) == 16)) ||
        (((ipAddress[0] & 0xFF) == 192) && ((ipAddress[1] & 0xFF) == 168));
    return NOERROR;
}

AutoPtr<IInterface> Inet4Address::WriteReplace()
{
    AutoPtr<IInet4Address> addr;
    CInet4Address::New(mIpAddress, mHostname, (IInet4Address**)&addr);
    AutoPtr<IInterface> obj = TO_IINTERFACE(addr);
    return obj;
}

} // namespace Net
} // namespace Elastos
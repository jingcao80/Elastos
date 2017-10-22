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

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/net/CRouteInfo.h"
#include "elastos/droid/utility/CPair.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <netutils/ifc.h>
#include <cutils/properties.h>
#include <netd/NetdClient.h>

using Elastos::Droid::Net::CRouteInfo;
using Elastos::Droid::Utility::CPair;

using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IInet6Address;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetAddressHelper;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

extern "C" {
int ifc_enable(const char *ifname);
int ifc_disable(const char *ifname);
int ifc_reset_connections(const char *ifname, int reset_mask);

int dhcp_do_request(const char * const ifname,
                    const char *ipaddr,
                    const char *gateway,
                    uint32_t *prefixLength,
                    const char *dns[],
                    const char *server,
                    uint32_t *lease,
                    const char *vendorInfo,
                    const char *domains,
                    const char *mtu);

int dhcp_do_request_renew(const char * const ifname,
                    const char *ipaddr,
                    const char *gateway,
                    uint32_t *prefixLength,
                    const char *dns[],
                    const char *server,
                    uint32_t *lease,
                    const char *vendorInfo,
                    const char *domains,
                    const char *mtu);

int dhcp_stop(const char *ifname);
int dhcp_release_lease(const char *ifname);
char *dhcp_get_errmsg();
}

namespace Elastos {
namespace Droid {
namespace Net {

const String NetworkUtils::TAG("NetworkUtils");

NetworkUtils::NetworkUtils()
{}

ECode NetworkUtils::ResetConnections(
    /* [in] */ const String& interfaceName,
    /* [in] */ Int32 mask,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    const char *nameStr = interfaceName;

    Logger::D(TAG, "droid_net_utils_resetConnections in iface=%s mask=0x%x\n",
            nameStr, mask);
    int n = ::ifc_reset_connections(nameStr, mask);
    *result = (Int32)n;
    return NOERROR;
}

ECode NetworkUtils::RunDhcp(
    /* [in] */ const String& interfaceName,
    /* [in] */ IDhcpResults* dhcpResults,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeRunDhcpCommon(interfaceName, dhcpResults, FALSE);
    return NOERROR;
}

ECode NetworkUtils::RunDhcpRenew(
    /* [in] */ const String& interfaceName,
    /* [in] */ IDhcpResults* dhcpResults,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = NativeRunDhcpCommon(interfaceName, dhcpResults, TRUE);
    return NOERROR;
}

ECode NetworkUtils::StopDhcp(
    /* [in] */ const String& interfaceName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    const char *nameStr = interfaceName.string();
    int res = ::dhcp_stop(nameStr);
    *result = (Boolean)(res == 0);
    return NOERROR;
}

ECode NetworkUtils::ReleaseDhcpLease(
    /* [in] */ const String& interfaceName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    const char *nameStr = interfaceName.string();
    int res = ::dhcp_release_lease(nameStr);
    *result = (Boolean)(res == 0);
    return NOERROR;
}

ECode NetworkUtils::GetDhcpError(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = String(::dhcp_get_errmsg());
    return NOERROR;
}

ECode NetworkUtils::BindProcessToNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = !setNetworkForProcess(netId);
    return NOERROR;
}

ECode NetworkUtils::GetNetworkBoundToProcess(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = getNetworkForProcess();
    return NOERROR;
}

ECode NetworkUtils::BindProcessToNetworkForHostResolution(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = !setNetworkForResolv(netId);
    return NOERROR;
}

ECode NetworkUtils::BindSocketToNetwork(
    /* [in] */ Int32 socketfd,
    /* [in] */ Int32 netId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = setNetworkForSocket(netId, socketfd);
    return NOERROR;
}

ECode NetworkUtils::ProtectFromVpn(
    /* [in] */ Int32 socketfd,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = !protectFromVpn(socketfd);
    return NOERROR;
}

ECode NetworkUtils::IntToInetAddress(
    /* [in] */ Int32 hostAddress,
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > addressBytes = ArrayOf<Byte>::Alloc(4);
    (*addressBytes)[0] = (0xff & hostAddress);
    (*addressBytes)[1] = (0xff & (hostAddress >> 8));
    (*addressBytes)[2] = (0xff & (hostAddress >> 16));
    (*addressBytes)[3] = (0xff & (hostAddress >> 24));
        // try {
    AutoPtr<IInetAddressHelper> helper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
    ECode ec = helper->GetByAddress(addressBytes, result);
        // } catch (UnknownHostException e) {
    if (FAILED(ec)) {
        if (ec == (ECode)E_UNKNOWN_HOST_EXCEPTION)
           return E_ASSERTION_ERROR;
        return ec;
    }
        // }
    return NOERROR;
}

ECode NetworkUtils::InetAddressToInt(
    /* [in] */ IInet4Address* inetAddr,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Byte> > addr;
    IInetAddress::Probe(inetAddr)->GetAddress((ArrayOf<Byte>**)&addr);
    *result = (((*addr)[3] & 0xff) << 24) | (((*addr)[2] & 0xff) << 16) |
            (((*addr)[1] & 0xff) << 8) | ((*addr)[0] & 0xff);
    return NOERROR;
}

ECode NetworkUtils::PrefixLengthToNetmaskInt(
    /* [in] */ Int32 prefixLength,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (prefixLength < 0 || prefixLength > 32) {
        Logger::E(TAG, "Invalid prefix length 0 <= prefix <= 32)");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 value = (Int32)0xffffffff << (32 - prefixLength);
    *result = value;
    return NOERROR;
}

ECode NetworkUtils::NetmaskIntToPrefixLength(
    /* [in] */ Int32 netmask,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = Elastos::Core::Math::BitCount(netmask);
    return NOERROR;
}

ECode NetworkUtils::NumericToInetAddress(
    /* [in] */ const String& addrString,
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IInetAddressHelper> inetaddresshelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetaddresshelper);
    AutoPtr<IInetAddress> inetAddress;
    FAIL_RETURN(inetaddresshelper->ParseNumericAddress(addrString, (IInetAddress**)&inetAddress));
    *result = inetAddress;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkUtils::ParcelInetAddress(
    /* [in] */ IParcel* parcel,
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 flags)
{
    AutoPtr<ArrayOf<Byte> > addressArray;
    if (address != NULL) {
        address->GetAddress((ArrayOf<Byte>**)&addressArray);
    }
    parcel->WriteArrayOf((Handle32)addressArray.Get());
    return NOERROR;
}

ECode NetworkUtils::UnparcelInetAddress(
    /* [in] */ IParcel* in,
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ArrayOf<Byte> > addressArray;
    in->ReadArrayOf((Handle32*)&addressArray);
    if (addressArray == NULL) {
        *result = NULL;
        return NOERROR;
    }
        // try {
    AutoPtr<IInetAddressHelper> inetaddresshelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetaddresshelper);
    ECode ec = inetaddresshelper->GetByAddress(addressArray, result);
        // } catch (UnknownHostException e) {
    if (FAILED(ec)) {
        *result = NULL;
        return ec == (ECode)E_UNKNOWN_HOST_EXCEPTION ? NOERROR : ec;
    }
        // }
    return NOERROR;
}

ECode NetworkUtils::MaskRawAddress(
    /* [in] */ ArrayOf<Byte>* array,
    /* [in] */ Int32 prefixLength)
{
    if (prefixLength < 0 || prefixLength > array->GetLength() * 8) {
        Logger::E(TAG, "IP address with %d bytes has invalid prefix length %d", array->GetLength(), prefixLength);
        return E_RUNTIME_EXCEPTION;
    }

    Int32 offset = prefixLength / 8;
    Int32 remainder = prefixLength % 8;
    Byte mask = (Byte)(0xFF << (8 - remainder));

    if (offset < array->GetLength())
        array->Set(offset, (Byte)((*array)[offset] & mask));

    offset++;

    for (; offset < array->GetLength(); offset++) {
        array->Set(offset, 0);
    }
    return NOERROR;
}

ECode NetworkUtils::GetNetworkPart(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 prefixLength,
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ArrayOf<Byte> > array;
    address->GetAddress((ArrayOf<Byte>**)&array);
    MaskRawAddress(array, prefixLength);

    AutoPtr<IInetAddress> netPart;
    AutoPtr<IInetAddressHelper> inetaddresshelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetaddresshelper);
    inetaddresshelper->GetByAddress(array, (IInetAddress**)&netPart);
    *result = netPart;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkUtils::ParseIpAndMask(
    /* [in] */ const String& ipAndMaskString,
    /* [out] */ IPair** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IInetAddress> address;
    Int32 prefixLength = -1;
    // try {
    AutoPtr<ArrayOf<String> > pieces;
    StringUtils::Split(ipAndMaskString, "/", 2, (ArrayOf<String>**)&pieces);
    AutoPtr<IInetAddressHelper> inetaddresshelper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&inetaddresshelper);
    if (pieces->GetLength() > 1) {
        prefixLength = StringUtils::ParseInt32((*pieces)[1]);
        ECode ec = inetaddresshelper->ParseNumericAddress((*pieces)[0], (IInetAddress**)&address);
        if (FAILED(ec)) {
            if (ec != (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION && ec != (ECode)E_NUMBER_FORMAT_EXCEPTION) {
                return ec;
            }
        }
    }

    // } catch (NullPointerException e) {            // Null string.
    // } catch (ArrayIndexOutOfBoundsException e) {  // No prefix length.
    // } catch (NumberFormatException e) {           // Non-numeric prefix.
    // } catch (IllegalArgumentException e) {        // Invalid IP address.
    // }
    if (address == NULL || prefixLength == -1) {
        Logger::E(TAG, "Invalid IP address and mask %s", ipAndMaskString.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInteger32> i32;
    CInteger32::New(prefixLength, (IInteger32**)&i32);
    return CPair::New(address, i32, result);
}

ECode NetworkUtils::AddressTypeMatches(
    /* [in] */ IInetAddress* left,
    /* [in] */ IInetAddress* right,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (((IInet4Address::Probe(left) != NULL) && (IInet4Address::Probe(right) != NULL)) ||
            ((IInet6Address::Probe(left) != NULL) && (IInet6Address::Probe(right) != NULL)));
    return NOERROR;
}

ECode NetworkUtils::HexToInet6Address(
    /* [in] */ const String& addrHexString,
    /* [out] */ IInetAddress** result)
{    VALIDATE_NOT_NULL(result);

//    try {
            // return numericToInetAddress(String.format(Locale.US, "%s:%s:%s:%s:%s:%s:%s:%s",
            //         addrHexString.substring(0,4),   addrHexString.substring(4,8),
            //         addrHexString.substring(8,12),  addrHexString.substring(12,16),
            //         addrHexString.substring(16,20), addrHexString.substring(20,24),
            //         addrHexString.substring(24,28), addrHexString.substring(28,32)));
    StringBuilder builder(addrHexString.Substring(0,4));
    builder += String(":");
    builder += addrHexString.Substring(4,8);
    builder += String(":");
    builder += addrHexString.Substring(8,12);
    builder += String(":");
    builder += addrHexString.Substring(12,16);
    builder += String(":");
    builder += addrHexString.Substring(16,20);
    builder += String(":");
    builder += addrHexString.Substring(20,24);
    builder += String(":");
    builder += addrHexString.Substring(24,28);
    builder += String(":");
    builder += addrHexString.Substring(28,32);
    AutoPtr<IInetAddress> inetaddress;
    NumericToInetAddress(builder.ToString(), (IInetAddress**)&inetaddress);

 //   }
//    catch (Exception e) {
//        Log.e("NetworkUtils", "error in hexToInet6Address(" + addrHexString + "): " + e);
//        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//    }
    *result = inetaddress;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkUtils::MakeStrings(
    /* [in] */ ICollection* addrs,
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 n;
    addrs->GetSize(&n);
    AutoPtr<ArrayOf<String> > strs = ArrayOf<String>::Alloc(n);

    AutoPtr<IIterator> it;
    addrs->GetIterator((IIterator**)&it);
    Int32 i = 0;
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> addr;
        it->GetNext((IInterface**)&addr);
        String s;
        IInetAddress::Probe(addr)->GetHostAddress(&s);
        (*strs)[i++] = s;
    }
    *result = strs;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkUtils::TrimV4AddrZeros(
    /* [in] */ const String& addr,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    if (addr.IsNull()) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<ArrayOf<String> > octets;
    StringUtils::Split(addr, String("\\."), (ArrayOf<String>**)&octets);
    if (octets->GetLength() != 4) {
        *result = addr;
        return NOERROR;
    }
    StringBuffer buff;
    for (Int32 i = 0; i < 4; i++) {
//        try {
            if ((*octets)[i].GetLength() > 3) {
                *result = addr;
        return NOERROR;
            }
            buff += (*octets)[i];
//        } catch (NumberFormatException e) {
//            return addr;
//        }
        if (i < 3)
            buff += String(".");
    }
    *result = buff.ToString();
    return NOERROR;
}

Boolean NetworkUtils::NativeRunDhcpCommon(
    /* [in] */ const String& ifname,
    /* [in] */ IDhcpResults* dhcpResults,
    /* [in] */ Boolean renew)
{
    Int32 num;
    char  ipaddr[PROPERTY_VALUE_MAX];
    uint32_t prefixLength;
    char gateway[PROPERTY_VALUE_MAX];
    char    dns1[PROPERTY_VALUE_MAX];
    char    dns2[PROPERTY_VALUE_MAX];
    char    dns3[PROPERTY_VALUE_MAX];
    char    dns4[PROPERTY_VALUE_MAX];
    const char *dns[5] = {dns1, dns2, dns3, dns4, NULL};
    char  server[PROPERTY_VALUE_MAX];
    uint32_t lease;
    char vendorInfo[PROPERTY_VALUE_MAX];
    char domains[PROPERTY_VALUE_MAX];
    char mtu[PROPERTY_VALUE_MAX];

    const char *nameStr = ifname.string();
    if (nameStr == NULL) {
        return FALSE;
    }
    if (renew) {
        num = ::dhcp_do_request_renew(nameStr, ipaddr, gateway, &prefixLength,
                dns, server, &lease, vendorInfo, domains, mtu);
    }
    else {
        num = ::dhcp_do_request(nameStr, ipaddr, gateway, &prefixLength,
                dns, server, &lease, vendorInfo, domains, mtu);
    }
    if (num != 0) {
        Logger::D(TAG, "dhcp_do_request failed : %s (%s)", nameStr, renew ? "renew" : "new");
    }

    Boolean b;
    if (num == 0) {
        dhcpResults->Clear();
        // set the linkAddress
        // dhcpResults->addLinkAddress(inetAddress, prefixLength)
        num = dhcpResults->SetIpAddress(String(ipaddr), prefixLength, &b);
    }
    if (num == 0) {
        // set the gateway
        num = dhcpResults->SetGateway(String(gateway), &b);
    }
    if (num == 0) {
        // dhcpResults->addDns(new InetAddress(dns1))
        num = dhcpResults->AddDns(String(dns1), &b);
    }

    if (num == 0) {
        dhcpResults->SetDomains(String(domains));

        num = dhcpResults->AddDns(String(dns2), &b);

        if (num == 0) {
            num = dhcpResults->AddDns(String(dns3), &b);
            if (num == 0) {
                num = dhcpResults->AddDns(String(dns4), &b);
            }
        }
    }

    if (num == 0) {
        // dhcpResults->setServerAddress(new InetAddress(server))
        num = dhcpResults->SetServerAddress(String(server), &b);
    }

    if (num == 0) {
        // dhcpResults->setLeaseDuration(lease)
        dhcpResults->SetLeaseDuration(lease);

        // dhcpResults->setVendorInfo(vendorInfo)
        dhcpResults->SetVendorInfo(String(vendorInfo));
    }
    return (Boolean)(num == 0);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos

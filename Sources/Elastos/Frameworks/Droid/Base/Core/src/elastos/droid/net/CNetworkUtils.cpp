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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CNetworkUtils.h"
#include "elastos/droid/net/NetworkUtils.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CNetworkUtils, Singleton, INetworkUtils)

CAR_SINGLETON_IMPL(CNetworkUtils)

ECode CNetworkUtils::ResetConnections(
    /* [in] */ const String& interfaceName,
    /* [in] */ Int32 mask,
    /* [out] */ Int32* result)
{
    return NetworkUtils::ResetConnections(interfaceName, mask, result);
}

ECode CNetworkUtils::RunDhcp(
    /* [in] */ const String& interfaceName,
    /* [in] */ IDhcpResults* dhcpResults,
    /* [out] */ Boolean* result)
{
    return NetworkUtils::RunDhcp(interfaceName, dhcpResults, result);
}

ECode CNetworkUtils::RunDhcpRenew(
    /* [in] */ const String& interfaceName,
    /* [in] */ IDhcpResults* dhcpResults,
    /* [out] */ Boolean* result)
{
    return NetworkUtils::RunDhcpRenew(interfaceName, dhcpResults, result);
}

ECode CNetworkUtils::StopDhcp(
    /* [in] */ const String& interfaceName,
    /* [out] */ Boolean* result)
{
    return NetworkUtils::StopDhcp(interfaceName, result);
}

ECode CNetworkUtils::ReleaseDhcpLease(
    /* [in] */ const String& interfaceName,
    /* [out] */ Boolean* result)
{
    return NetworkUtils::ReleaseDhcpLease(interfaceName, result);
}

ECode CNetworkUtils::GetDhcpError(
    /* [out] */ String* result)
{
    return NetworkUtils::GetDhcpError(result);
}

ECode CNetworkUtils::BindProcessToNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    return NetworkUtils::BindProcessToNetwork(netId, result);
}

ECode CNetworkUtils::GetNetworkBoundToProcess(
    /* [out] */ Int32* result)
{
    return NetworkUtils::GetNetworkBoundToProcess(result);
}

ECode CNetworkUtils::BindProcessToNetworkForHostResolution(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* result)
{
    return NetworkUtils::BindProcessToNetworkForHostResolution(netId, result);
}

ECode CNetworkUtils::BindSocketToNetwork(
    /* [in] */ Int32 socketfd,
    /* [in] */ Int32 netId,
    /* [out] */ Int32* result)
{
    return NetworkUtils::BindSocketToNetwork(socketfd, netId, result);
}

ECode CNetworkUtils::ProtectFromVpn(
    /* [in] */ Int32 socketfd,
    /* [out] */ Boolean* result)
{
    return NetworkUtils::ProtectFromVpn(socketfd, result);
}

ECode CNetworkUtils::IntToInetAddress(
    /* [in] */ Int32 hostAddress,
    /* [out] */ IInetAddress** result)
{
    return NetworkUtils::IntToInetAddress(hostAddress, result);
}

ECode CNetworkUtils::InetAddressToInt(
    /* [in] */ IInet4Address* inetAddr,
    /* [out] */ Int32* result)
{
    return NetworkUtils::InetAddressToInt(inetAddr, result);
}

ECode CNetworkUtils::PrefixLengthToNetmaskInt(
    /* [in] */ Int32 prefixLength,
    /* [out] */ Int32* result)
{
    return NetworkUtils::PrefixLengthToNetmaskInt(prefixLength, result);
}

ECode CNetworkUtils::NetmaskIntToPrefixLength(
    /* [in] */ Int32 netmask,
    /* [out] */ Int32* result)
{
    return NetworkUtils::NetmaskIntToPrefixLength(netmask, result);
}

ECode CNetworkUtils::NumericToInetAddress(
    /* [in] */ const String& addrString,
    /* [out] */ IInetAddress** result)
{
    return NetworkUtils::NumericToInetAddress(addrString, result);
}

ECode CNetworkUtils::ParcelInetAddress(
    /* [in] */ IParcel* parcel,
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 flags)
{
    return NetworkUtils::ParcelInetAddress(parcel, address, flags);
}

ECode CNetworkUtils::UnparcelInetAddress(
    /* [in] */ IParcel* in,
    /* [out] */ IInetAddress** result)
{
    return NetworkUtils::UnparcelInetAddress(in, result);
}

ECode CNetworkUtils::MaskRawAddress(
    /* [in] */ ArrayOf<Byte>* array,
    /* [in] */ Int32 prefixLength)
{
    return NetworkUtils::MaskRawAddress(array, prefixLength);
}

ECode CNetworkUtils::GetNetworkPart(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 prefixLength,
    /* [out] */ IInetAddress** result)
{
    return NetworkUtils::GetNetworkPart(address, prefixLength, result);
}

ECode CNetworkUtils::ParseIpAndMask(
    /* [in] */ const String& ipAndMaskString,
    /* [out] */ IPair** result)
{
    return NetworkUtils::ParseIpAndMask(ipAndMaskString, result);
}

ECode CNetworkUtils::AddressTypeMatches(
    /* [in] */ IInetAddress* left,
    /* [in] */ IInetAddress* right,
    /* [out] */ Boolean* result)
{
    return NetworkUtils::AddressTypeMatches(left, right, result);
}

ECode CNetworkUtils::HexToInet6Address(
    /* [in] */ const String& addrHexString,
    /* [out] */ IInetAddress** result)
{
    return NetworkUtils::HexToInet6Address(addrHexString, result);
}

ECode CNetworkUtils::MakeStrings(
    /* [in] */ ICollection* addrs,
    /* [out, callee] */ ArrayOf<String>** result)
{
    return NetworkUtils::MakeStrings(addrs, result);
}

ECode CNetworkUtils::TrimV4AddrZeros(
    /* [in] */ const String& addr,
    /* [out] */ String* result)
{
    return NetworkUtils::TrimV4AddrZeros(addr, result);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos


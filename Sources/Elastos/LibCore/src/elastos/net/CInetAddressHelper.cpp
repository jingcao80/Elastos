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

#include "CInetAddressHelper.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CInetAddressHelper, Singleton, IInetAddressHelper)

CAR_SINGLETON_IMPL(CInetAddressHelper)

ECode CInetAddressHelper::GetUNSPECIFIED(
    /* [out] */ IInetAddress** addr)
{
    return InetAddress::GetUNSPECIFIED(addr);
}

ECode CInetAddressHelper::GetAllByName(
    /* [in] */ const String& host,
    /* [out, callee] */ ArrayOf<IInetAddress*>** interAddress)
{
    return InetAddress::GetAllByName(host, interAddress);
}

ECode CInetAddressHelper::GetAllByNameOnNet(
    /* [in] */ const String& host,
    /* [in] */ Int32 netId,
    /* [out, callee] */ ArrayOf<IInetAddress*>** addresses)
{
    return InetAddress::GetAllByNameOnNet(host, netId, addresses);
}

ECode CInetAddressHelper::GetByName(
    /* [in] */ const String& host,
    /* [out] */ IInetAddress** interAddr)
{
    return InetAddress::GetByName(host, interAddr);
}

CARAPI CInetAddressHelper::GetByNameOnNet(
    /* [in] */ const String& host,
    /* [in] */ Int32 netId,
    /* [out] */ IInetAddress** address)
{
    return InetAddress::GetByNameOnNet(host, netId, address);
}

ECode CInetAddressHelper::GetLocalHost(
    /* [out] */ IInetAddress** interAddr)
{
    return InetAddress::GetLocalHost(interAddr);
}

ECode CInetAddressHelper::ClearDnsCache()
{
    return InetAddress::ClearDnsCache();
}

ECode CInetAddressHelper::IsNumeric(
    /* [in] */ const String& address,
    /* [out] */ Boolean* isNumeric)
{
    return InetAddress::IsNumeric(address, isNumeric);
}

ECode CInetAddressHelper::ParseNumericAddress(
    /* [in] */ const String& numericAddress,
    /* [out] */ IInetAddress** interAddr)
{
    return InetAddress::ParseNumericAddress(numericAddress, interAddr);
}

ECode CInetAddressHelper::GetLoopbackAddress(
    /* [out] */ IInetAddress** interAddr)
{
    return InetAddress::GetLoopbackAddress(interAddr);
}

ECode CInetAddressHelper::GetByAddress(
    /* [in] */ ArrayOf<Byte>* ipAddress,
    /* [out] */ IInetAddress** interAddr)
{
    return InetAddress::GetByAddress(ipAddress, interAddr);
}

ECode CInetAddressHelper::GetByAddress(
    /* [in] */ const String& hostName,
    /* [in] */ ArrayOf<Byte>* ipAddress,
    /* [out] */ IInetAddress** interAddr)
{
    return InetAddress::GetByAddress(hostName, ipAddress, interAddr);
}

} // namespace Net
} // namespace Elastos

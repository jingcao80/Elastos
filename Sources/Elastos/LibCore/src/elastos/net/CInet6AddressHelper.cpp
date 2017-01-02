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

#include "elastos/net/CInet6AddressHelper.h"
#include "elastos/net/Inet6Address.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CInet6AddressHelper, Singleton, IInet6AddressHelper)

CAR_SINGLETON_IMPL(CInet6AddressHelper)

ECode CInet6AddressHelper::GetByAddress(
    /* [in] */ const String& host,
    /* [in] */ ArrayOf<Byte>* addr,
    /* [in] */ Int32 scope_id,
    /* [out] */ IInet6Address** address)
{
    return Inet6Address::GetByAddress(host, addr, scope_id, address);
}

ECode CInet6AddressHelper::GetByAddress(
    /* [in] */ const String& host,
    /* [in] */ ArrayOf<Byte>* addr,
    /* [in] */ INetworkInterface* nif,
    /* [out] */ IInet6Address** address)
{
    return Inet6Address::GetByAddress(host, addr, nif, address);
}

ECode CInet6AddressHelper::GetANY(
    /* [out] */ IInetAddress** result)
{
    return Inet6Address::GetANY(result);
}

ECode CInet6AddressHelper::GetLOOPBACK(
    /* [out] */ IInetAddress** result)
{
    return Inet6Address::GetLOOPBACK(result);
}

} // namespace Net
} // namespace Elastos

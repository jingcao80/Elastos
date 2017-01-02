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

#ifndef __ELASTOS_NET_INTERFACEADDRESS_H__
#define __ELASTOS_NET_INTERFACEADDRESS_H__

#include "Elastos.CoreLibrary.Net.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Net {

class InterfaceAddress
    : public Object
    , public IInterfaceAddress
{
public:
    CAR_INTERFACE_DECL()

    /**
     * For IPv4.
     */
    InterfaceAddress(
        /* [in] */ IInetAddress* address,
        /* [in] */ IInetAddress* broadcastAddress,
        /* [in] */ IInetAddress* mask);

    /**
     * For IPv6.
     */
    InterfaceAddress(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int16 prefixLength);

    /**
     * For IPv4.
     */
    CARAPI constructor(
        /* [in] */ IInetAddress* address,
        /* [in] */ IInetAddress* broadcastAddress,
        /* [in] */ IInetAddress* mask);

    /**
     * For IPv6.
     */
    CARAPI constructor(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int16 prefixLength);

    /**
     * Tests whether this object is equal to another one. Returns true if
     * the address, broadcast address and prefix length are all equal.
     *
     * @param obj the object to be compared.
     * @return true if 'obj' is equal to this InterfaceAddress, false otherwise.
     */
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equals);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    /**
     * Returns a string containing this interface's address, prefix length, and broadcast address.
     * For example: {@code "/172.18.103.112/23 [/172.18.103.255]"} or
     * {@code "/0:0:0:0:0:0:0:1%1/128 [null]"}.
     */
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Returns the InetAddress for this address.
     */
    CARAPI GetAddress(
        /* [out] */ IInetAddress** address);

    /**
     * Returns the subnet-directed broadcast address if this is an IPv4 interface, null otherwise.
     */
    CARAPI GetBroadcast(
        /* [out] */ IInetAddress** broadcast);

    /**
     * Returns the network prefix length in bits.
     * (In IPv4 parlance, this is known as the subnet mask,
     * but this method applies to IPv6 addresses too.)
     */
    CARAPI GetNetworkPrefixLength(
        /* [out] */ Int16* length);

protected:
    InterfaceAddress();

private:
    static CARAPI_(Int16) CountPrefixLength(
        /* [in] */ IInetAddress* mask);

protected:
    /**
     * An IPv4 or IPv6 address.
     */
    AutoPtr<IInetAddress> mAddress;

    /**
     * The IPv4 broadcast address, or null for IPv6.
     */
    AutoPtr<IInetAddress> mBroadcastAddress;

    Int16 mPrefixLength;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_INTERFACEADDRESS_H__

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

#ifndef __ELASTOS_NET_INET6ADDRESS_H__
#define __ELASTOS_NET_INET6ADDRESS_H__

#include "InetAddress.h"

using Elastos::IO::IObjectOutputStream;

namespace Elastos {
namespace Net {

class Inet6Address
    : public InetAddress
    , public IInet6Address
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* address,
        /* [in] */ const String& hostName,
        /* [in] */ Int32 scopeId);

    /**
     * Constructs an IPv6 address according to the given {@code host}, {@code
     * addr} and {@code scope_id}.
     *
     * @param host
     *            the host name associated with the address.
     * @param addr
     *            the network address.
     * @param scope_id
     *            the scope id for link- or site-local addresses.
     * @return the Inet6Address instance representing the IP address.
     * @throws UnknownHostException
     *             if the address is null or has an invalid length.
     */
    static CARAPI GetByAddress(
        /* [in] */ const String& host,
        /* [in] */ ArrayOf<Byte>* addr,
        /* [in] */ Int32 scopeId,
        /* [out] */ IInet6Address** address);

    /**
     * Gets an IPv6 address instance according to the given {@code host},
     * {@code addr} and {@code nif}. {@code scope_id} is set according to the
     * given {@code nif} and the {@code addr} type (for example site-local or
     * link-local).
     *
     * @param host
     *            the hostname associated with the address.
     * @param addr
     *            the network address.
     * @param nif
     *            the network interface that this address is associated with.
     * @return the Inet6Address instance representing the IP address.
     * @throws UnknownHostException
     *             if the address is {@code null} or has an invalid length or
     *             the interface doesn't have a numeric scope id for the given
     *             address type.
     */
    static CARAPI GetByAddress(
        /* [in] */ const String& host,
        /* [in] */ ArrayOf<Byte>* addr,
        /* [in] */ INetworkInterface* nif,
        /* [out] */ IInet6Address** address);

    CARAPI IsAnyLocalAddress(
        /* [out] */ Boolean* isAnyLocalAddress);

    CARAPI IsIPv4CompatibleAddress(
        /* [out] */ Boolean* isIPv4Compatible);

    CARAPI IsLinkLocalAddress(
        /* [out] */ Boolean* isLinkLocalAddress);

    CARAPI IsLoopbackAddress(
        /* [out] */ Boolean* isLoopbackAddress);

    CARAPI IsMCGlobal(
        /* [out] */ Boolean* isMCGlobal);

    CARAPI IsMCLinkLocal(
        /* [out] */ Boolean* isMCLinkLocal);

    CARAPI IsMCNodeLocal(
        /* [out] */ Boolean* isMCNodeLocal);

    CARAPI IsMCOrgLocal(
        /* [out] */ Boolean* isMCOrgLocal);

    CARAPI IsMCSiteLocal(
        /* [out] */ Boolean* isMCSiteLocal);

    CARAPI IsMulticastAddress(
        /* [out] */ Boolean* isMulticastAddress);

    CARAPI IsSiteLocalAddress(
        /* [out] */ Boolean* isSiteLocalAddress);

    CARAPI GetScopeId(
        /* [out] */ Int32* scopeId);

    CARAPI GetScopedInterface(
        /* [out] */ INetworkInterface** scopedInterface);

    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * @hide
     */
    static CARAPI GetANY(
        /* [out] */ IInetAddress** result);

    /**
     * @hide
     */
    static CARAPI GetLOOPBACK(
        /* [out] */ IInetAddress** result);

protected:
    Inet6Address();

private:
    AutoPtr<IInterface> WriteReplace();

    CARAPI_(Boolean) CompareLocalType(
        /* [in] */ IInet6Address* ia);

    CARAPI_(void)  WriteObject(
        /* [in] */ IObjectOutputStream* stream);

    CARAPI_(void)  ReadObject(
        /* [in] */ IObjectOutputStream* stream);
public:
    /**
     * @hide
     */
    static AutoPtr<IInetAddress> ANY;

    /**
     * @hide
     */
    static AutoPtr<IInetAddress> LOOPBACK;

private:
    Int32 mScopeId;

    String mIfname;

    // AutoPtr<INetworkInterface> mScopedIf;

    Boolean mScopeIdSet;
    Boolean mScopeIfnameSet;

    //static AutoPtr<ArryaOf<ObjectStreamField*> > mSerialPersistentFields;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_INET4ADDRESS_H__
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

#ifndef __ELASTOS_NET_INET4ADDRESS_H__
#define __ELASTOS_NET_INET4ADDRESS_H__

#include "InetAddress.h"

namespace Elastos {
namespace Net {

class Inet4Address
    : public InetAddress
    , public IInet4Address
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* address,
        /* [in] */ const String& name);

    CARAPI IsAnyLocalAddress(
        /* [out] */ Boolean* isAnyLocalAddress);

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

    /**
     * @hide
     */
    static CARAPI GetANY(
        /* [out] */ IInetAddress** result);

    /**
     * @hide
     */
    static CARAPI GetALL(
        /* [out] */ IInetAddress** result);

    /**
     * @hide
     */
    static CARAPI GetLOOPBACK(
        /* [out] */ IInetAddress** result);

public:
    /**
     * @hide
     */
    static AutoPtr<IInetAddress> ANY;

    /**
     * @hide
     */
    static AutoPtr<IInetAddress> ALL;

    /**
     * @hide
     */
    static AutoPtr<IInetAddress> LOOPBACK;

private:
    AutoPtr<IInterface> WriteReplace();
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_INET4ADDRESS_H__
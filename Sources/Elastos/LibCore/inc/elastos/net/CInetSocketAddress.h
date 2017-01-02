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

#ifndef __ELASTOS_NET_CINETSOCKETADDRESS_H__
#define __ELASTOS_NET_CINETSOCKETADDRESS_H__

#include "_Elastos_Net_CInetSocketAddress.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;
using Elastos::IO::ISerializable;
using Elastos::Net::ISocketAddress;
using Elastos::Net::IInetSocketAddress;

namespace Elastos {
namespace Net {

CarClass(CInetSocketAddress)
    , public Object
    , public IInetSocketAddress
    , public ISocketAddress
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 port);

    CARAPI constructor(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port);

    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port);

    CARAPI constructor(
        /* [in] */ const String& hostname,
        /* [in] */ Int32 port,
        /* [in] */ Boolean needResolved);

    static CARAPI CreateUnresolved(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [out] */ IInetSocketAddress** addr);

    CARAPI GetPort(
        /* [out] */ Int32* port);

    CARAPI SetPort(
        /* [in] */ Int32 port);

    CARAPI GetAddress(
        /* [out] */ IInetAddress** address);

    CARAPI SetAddress(
        /* [in] */ IInetAddress* address);

    CARAPI GetHostName(
        /* [out] */ String* hostname);

    CARAPI GetHostString(
        /* [out] */ String* hostString);

    CARAPI IsUnresolved(
        /* [out] */ Boolean* isUnresolved);

    CARAPI Equals(
        /*[in] */ IInterface* socketAddr,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);


public:
    AutoPtr<IInetAddress> mAddr;

    Int32 mPort;

private:
    static const Int64 sSerialVersionUID;

    String mHostname;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CINETSOCKETADDRESS_H__

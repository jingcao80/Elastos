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

#ifndef __ELASTOS_NET_CPROXY_H__
#define __ELASTOS_NET_CPROXY_H__

#include "_Elastos_Net_CProxy.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Net {

CarClass(CProxy)
    , public Object
    , public IProxy
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ProxyType type,
        /* [in] */ ISocketAddress* sa);

    CARAPI GetType(
        /* [out] */ ProxyType* type);

    CARAPI GetAddress(
        /* [out] */ ISocketAddress** address);

    CARAPI static GetNO_PROXY(
        /* [out] */ IProxy** proxy);

    /**
     * Gets a textual representation of this {@code Proxy} instance. The string
     * includes the two parts {@code type.toString()} and {@code
     * address.toString()} if {@code address} is not {@code null}.
     *
     * @return the representing string of this proxy.
     */
    CARAPI ToString(
        /* [out] */ String * str);

    CARAPI Equals(
        /* [in] */ IInterface * o,
        /* [out] */ Boolean* resutl);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

public:
    static AutoPtr<IProxy> NO_PROXY;// = new Proxy();

private:
    ProxyType mType;

    AutoPtr<ISocketAddress> mAddress;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CPROXY_H__

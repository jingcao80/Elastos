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

#ifndef __ELASTOS_NET_CINET6ADDRESSHELPER_H__
#define __ELASTOS_NET_CINET6ADDRESSHELPER_H__

#include "_Elastos_Net_CInet6AddressHelper.h"
#include "elastos/net/InetAddress.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Net {

CarClass(CInet6AddressHelper)
    , public Singleton
    , public IInet6AddressHelper
{
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetByAddress(
        /* [in] */ const String& host,
        /* [in] */ ArrayOf<Byte>* addr,
        /* [in] */ Int32 scope_id,
        /* [out] */ IInet6Address** address);

    CARAPI GetByAddress(
        /* [in] */ const String& host,
        /* [in] */ ArrayOf<Byte>* addr,
        /* [in] */ INetworkInterface* nif,
        /* [out] */ IInet6Address** address);

    /**
     * @hide
     */
    CARAPI GetANY(
        /* [out] */ IInetAddress** result);

    /**
     * @hide
     */
    CARAPI GetLOOPBACK(
        /* [out] */ IInetAddress** result);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CINET6ADDRESSHELPER_H__

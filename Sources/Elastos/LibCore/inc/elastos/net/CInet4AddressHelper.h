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

#ifndef __ELASTOS_NET_CINET4ADDRESSHELPER_H__
#define __ELASTOS_NET_CINET4ADDRESSHELPER_H__

#include "_Elastos_Net_CInet4AddressHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Net {

/**
 * An IPv4 address. See {@link InetAddress}.
 */
CarClass(CInet4AddressHelper)
    , public Singleton
    , public IInet4AddressHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * @hide
     */
    CARAPI GetANY(
        /* [out] */ IInetAddress** result);

    /**
     * @hide
     */
    CARAPI GetALL(
        /* [out] */ IInetAddress** result);

    /**
     * @hide
     */
    CARAPI GetLOOPBACK(
        /* [out] */ IInetAddress** result);

};

} // namespace Net
} // namespace Elastos

#endif //  __ELASTOS_NET_CINET4ADDRESSHELPER_H__

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

#ifndef __ELASTOS_NET_CINETADDRESSHELPER_H__
#define __ELASTOS_NET_CINETADDRESSHELPER_H__

#include "_Elastos_Net_CInetAddressHelper.h"
#include "InetAddress.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Net {

CarClass(CInetAddressHelper)
    , public Singleton
    , public IInetAddressHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetUNSPECIFIED(
        /* [out] */ IInetAddress** addr);

    CARAPI GetAllByName(
        /* [in] */ const String& host,
        /* [out, callee] */ ArrayOf<IInetAddress*>** interAddress);

    CARAPI GetAllByNameOnNet(
        /* [in] */ const String& host,
        /* [in] */ Int32 netId,
        /* [out, callee] */ ArrayOf<IInetAddress*>** addresses);

    CARAPI GetByName(
        /* [in] */ const String& host,
        /* [out] */ IInetAddress** interAddr);

    CARAPI GetByNameOnNet(
        /* [in] */ const String& host,
        /* [in] */ Int32 netId,
        /* [out] */ IInetAddress** address);

    CARAPI GetLocalHost(
        /* [out] */ IInetAddress** interAddr);

    CARAPI ClearDnsCache();

    CARAPI IsNumeric(
        /* [in] */ const String& address,
        /* [out] */ Boolean* isNumeric);

    CARAPI ParseNumericAddress(
        /* [in] */ const String& numericAddress,
        /* [out] */ IInetAddress** interAddr);

    CARAPI GetLoopbackAddress(
        /* [out] */ IInetAddress** interAddr);

    CARAPI GetByAddress(
        /* [in] */ ArrayOf<Byte>* ipAddress,
        /* [out] */ IInetAddress** interAddr);

    CARAPI GetByAddress(
        /* [in] */ const String& hostName,
        /* [in] */ ArrayOf<Byte>* ipAddress,
        /* [out] */ IInetAddress** interAddr);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CINETADDRESSHELPER_H__

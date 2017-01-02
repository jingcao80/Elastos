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

#ifndef __ELASTOS_DROID_TELECOM_GATEWAYINFO_H__
#define __ELASTOS_DROID_TELECOM_GATEWAYINFO_H__

#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telecom.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * When calls are made, they may contain gateway information for services which route phone calls
 * through their own service/numbers. The data consists of a number to call and the package name of
 * the service. This data is used in two ways:
 * <ol>
 * <li> Call the appropriate routing number
 * <li> Display information about how the call is being routed to the user
 * </ol>
 * @hide
 */
class GatewayInfo
    : public Object
    , public IGatewayInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /** @hide */
    CARAPI constructor(
        /* [in] */ const String& packageName,
        /* [in] */ IUri* gatewayUri,
        /* [in] */ IUri* originalAddress);

    /**
     * Package name of the gateway provider service. used to place the call with.
     */
    CARAPI GetGatewayProviderPackageName(
        /* [out] */ String* result);

    /**
     * Gateway provider address to use when actually placing the call.
     */
    CARAPI GetGatewayAddress(
        /* [out] */ IUri** result);

    /**
     * The actual call address that the user is trying to connect to via the gateway.
     */
    CARAPI GetOriginalAddress(
        /* [out] */ IUri** result);

    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * {@inheritDoc}
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* destination);

private:
    String mGatewayProviderPackageName;
    AutoPtr<IUri> mGatewayAddress;
    AutoPtr<IUri> mOriginalAddress;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_GATEWAYINFO_H__
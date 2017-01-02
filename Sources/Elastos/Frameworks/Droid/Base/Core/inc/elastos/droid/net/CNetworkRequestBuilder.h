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

#ifndef __ELASTOS_DROID_NET_CNETWORKREQUESTBUILDER_H__
#define __ELASTOS_DROID_NET_CNETWORKREQUESTBUILDER_H__

#include "_Elastos_Droid_Net_CNetworkRequestBuilder.h"
#include "elastos/droid/net/NetworkRequest.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Builder used to create {@link NetworkRequest} objects.  Specify the Network features
 * needed in terms of {@link NetworkCapabilities} features
 */
// inner class of NetworkRequest
CarClass(CNetworkRequestBuilder)
    , public NetworkRequestBuilder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKREQUESTBUILDER_H__

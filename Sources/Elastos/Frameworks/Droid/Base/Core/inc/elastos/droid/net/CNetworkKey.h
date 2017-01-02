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

#ifndef __ELASTOS_DROID_NET_CNETWORKKEY_H__
#define __ELASTOS_DROID_NET_CNETWORKKEY_H__

#include "_Elastos_Droid_Net_CNetworkKey.h"
#include "elastos/droid/net/NetworkKey.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Information which identifies a specific network.
 *
 * @hide
 */
// @SystemApi
// NOTE: Ideally, we would abstract away the details of what identifies a network of a specific
// type, so that all networks appear the same and can be scored without concern to the network type
// itself. However, because no such cross-type identifier currently exists in the Android framework,
// and because systems might obtain information about networks from sources other than Android
// devices, we need to provide identifying details about each specific network type (wifi, cell,
// etc.) so that clients can pull out these details depending on the type of network.
CarClass(CNetworkKey)
    , public NetworkKey
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKKEY_H__

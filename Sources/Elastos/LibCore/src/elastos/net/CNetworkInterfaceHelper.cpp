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

#include <CNetworkInterfaceHelper.h>
#include "NetworkInterface.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CNetworkInterfaceHelper, Singleton, INetworkInterfaceHelper)

CAR_SINGLETON_IMPL(CNetworkInterfaceHelper)

ECode CNetworkInterfaceHelper::GetByName(
   /* [in] */ const String& interfaceName,
   /* [out] */ INetworkInterface** networkInterface)
{
    return NetworkInterface::GetByName(interfaceName, networkInterface);
}

ECode CNetworkInterfaceHelper::GetByInetAddress(
   /* [in] */ IInetAddress* address,
   /* [out] */ INetworkInterface** networkInterface)
{
    return NetworkInterface::GetByInetAddress(address, networkInterface);
}

ECode CNetworkInterfaceHelper::GetByIndex(
   /* [in] */ Int32 index,
   /* [out] */ INetworkInterface** networkInterface)
{
    return NetworkInterface::GetByIndex(index, networkInterface);
}

ECode CNetworkInterfaceHelper::GetNetworkInterfaces(
   /* [out] */ IEnumeration** networkInterfaceList)
{
    return NetworkInterface::GetNetworkInterfaces(networkInterfaceList);
}

} // namespace Net
} // namespace Elastos

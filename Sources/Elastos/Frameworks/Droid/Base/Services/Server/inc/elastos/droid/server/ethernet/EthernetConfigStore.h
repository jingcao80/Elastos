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

#ifndef __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETCONFIGSTORE_H__
#define __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETCONFIGSTORE_H__

#include "elastos/droid/server/net/IpConfigStore.h"

using Elastos::Droid::Net::IIpConfiguration;
using Elastos::Droid::Server::Net::IpConfigStore;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Ethernet {

/**
 * This class provides an API to store and manage Ethernet network configuration.
 */
class EthernetConfigStore : public IpConfigStore
{
public:
    CARAPI constructor();

    CARAPI_(AutoPtr<IIpConfiguration>) ReadIpAndProxyConfigurations();

    CARAPI_(void) WriteIpAndProxyConfigurations(
        /* [in] */ IIpConfiguration* config);

private:
    static const String TAG;
    static const String sIpConfigFile;
};

} // namespace Ethernet
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETCONFIGSTORE_H__

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

#ifndef __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETSERVICE_H__
#define __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETSERVICE_H__

#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/ethernet/CEthernetServiceImpl.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Server::SystemService;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Ethernet {

class EthernetService
    : public SystemService
{
public:
    EthernetService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

public:
    AutoPtr<CEthernetServiceImpl> mImpl;

private:
    static const String TAG;
};

} // namespace Ethernet
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETSERVICE_H__


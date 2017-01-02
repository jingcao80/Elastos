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

#include "elastos/droid/server/ethernet/EthernetService.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Ethernet {

//=====================================================================
//                             EthernetService
//=====================================================================
const String EthernetService::TAG("EthernetService");

EthernetService::EthernetService()
{
}

ECode EthernetService::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    CEthernetServiceImpl::NewByFriend(context, (CEthernetServiceImpl**)&mImpl);
    return NOERROR;
}

ECode EthernetService::OnStart()
{
    Logger::I(TAG, "Registering service %s", IContext::ETHERNET_SERVICE.string());
    PublishBinderService(IContext::ETHERNET_SERVICE, mImpl);
    return NOERROR;
}

ECode EthernetService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == ISystemService::PHASE_SYSTEM_SERVICES_READY) {
         mImpl->Start();
    }
    return NOERROR;
}

} // namespace Ethernet
} // namespace Server
} // namespace Droid
} // namespace Elastos

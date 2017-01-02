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

#include "elastos/droid/server/net/CBaseNetworkObserver.h"
#include "elastos/droid/server/net/CNetlinkTracker.h"
#include "elastos/droid/server/net/CDnsServerEntry.h"
#include "elastos/droid/server/net/CDnsServerRepository.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

CAR_OBJECT_IMPL(CBaseNetworkObserver)
CAR_OBJECT_IMPL(CNetlinkTracker)
CAR_OBJECT_IMPL(CDnsServerEntry)
CAR_OBJECT_IMPL(CDnsServerRepository)

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos
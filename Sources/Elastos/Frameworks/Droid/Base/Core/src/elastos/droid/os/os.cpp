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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/CBatteryStatsHistoryItem.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/os/CCancellationSignalTransport.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/CRegistrantList.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/CUserEnvironment.h"
#include "elastos/droid/os/CPatternMatcher.h"
#include "elastos/droid/os/CParcelFileDescriptor.h"
#include "elastos/droid/os/CParcelFileDescriptorAutoCloseInputStream.h"
#include "elastos/droid/os/CParcelFileDescriptorAutoCloseOutputStream.h"
#include "elastos/droid/os/CRemoteCallbackLocalCallback.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_OBJECT_IMPL(CBatteryStatsHistoryItem)

CAR_OBJECT_IMPL(CBinder)

CAR_OBJECT_IMPL(CCancellationSignalTransport)

CAR_OBJECT_IMPL(CHandler)

CAR_OBJECT_IMPL(CRegistrant)

CAR_OBJECT_IMPL(CRegistrantList)

CAR_OBJECT_IMPL(CUserHandle)

CAR_OBJECT_IMPL(CUserEnvironment)

CAR_OBJECT_IMPL(CPatternMatcher)

CAR_OBJECT_IMPL(CParcelFileDescriptor)

CAR_OBJECT_IMPL(CParcelFileDescriptorAutoCloseInputStream)

CAR_OBJECT_IMPL(CParcelFileDescriptorAutoCloseOutputStream)

CAR_OBJECT_IMPL(CRemoteCallbackLocalCallback)

} // namespace Os
} // namespace Droid
} // namespace Elastos

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

#include "elastos/droid/server/am/CActivityContainer.h"
#include "elastos/droid/server/am/CBatteryStatsService.h"
#include "elastos/droid/server/am/CVirtualActivityContainer.h"
#include "elastos/droid/server/am/CActivityManagerBootCompletedReceiver.h"
#include "elastos/droid/server/am/CActivityManagerSwitchUserReceiver.h"
#include "elastos/droid/server/am/CActivityManagerNeedStartReceiver.h"
#include "elastos/droid/server/am/CActivityManagerPreBootCompletedReceiver.h"
#include "elastos/droid/server/am/CActivityManagerSystemReadyReceiver.h"
#include "elastos/droid/server/am/CActivityManagerShutdownReceiver.h"
#include "elastos/droid/server/am/CActivityManagerStoppingReceiver.h"
#include "elastos/droid/server/am/CActivityManagerDispatchUserSwitchCallback.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_OBJECT_IMPL(CBatteryStatsService)
CAR_OBJECT_IMPL(CActivityContainer)
CAR_OBJECT_IMPL(CVirtualActivityContainer)
CAR_OBJECT_IMPL(CActivityManagerBootCompletedReceiver)
CAR_OBJECT_IMPL(CActivityManagerSwitchUserReceiver)
CAR_OBJECT_IMPL(CActivityManagerNeedStartReceiver)
CAR_OBJECT_IMPL(CActivityManagerPreBootCompletedReceiver)
CAR_OBJECT_IMPL(CActivityManagerSystemReadyReceiver)
CAR_OBJECT_IMPL(CActivityManagerShutdownReceiver)
CAR_OBJECT_IMPL(CActivityManagerStoppingReceiver)
CAR_OBJECT_IMPL(CActivityManagerDispatchUserSwitchCallback)


} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

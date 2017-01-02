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

#include "elastos/droid/server/notification/CConditionProviders.h"
#include "elastos/droid/server/notification/CNotificationIntrusivenessExtractor.h"
#include "elastos/droid/server/notification/CNotificationListeners.h"
#include "elastos/droid/server/notification/CNotificationManagerBinderService.h"
#include "elastos/droid/server/notification/CNotificationManagerService.h"
#include "elastos/droid/server/notification/CPackagePriorityExtractor.h"
#include "elastos/droid/server/notification/CPackageVisibilityExtractor.h"
#include "elastos/droid/server/notification/CStatusBarNotificationHolder.h"
#include "elastos/droid/server/notification/CValidateNotificationPeople.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

CAR_OBJECT_IMPL(CConditionProviders)
CAR_OBJECT_IMPL(CNotificationIntrusivenessExtractor)
CAR_OBJECT_IMPL(CNotificationListeners)
CAR_OBJECT_IMPL(CNotificationManagerBinderService);
CAR_OBJECT_IMPL(CNotificationManagerService);
CAR_OBJECT_IMPL(CPackagePriorityExtractor)
CAR_OBJECT_IMPL(CPackageVisibilityExtractor)
CAR_OBJECT_IMPL(CStatusBarNotificationHolder);
CAR_OBJECT_IMPL(CValidateNotificationPeople)

} // Notification
} // Server
} // Droid
} // Elastos

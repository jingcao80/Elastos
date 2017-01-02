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

#include "elastos/droid/service/notification/CNotificationListenerServiceRanking.h"
#include "elastos/droid/service/notification/CNotificationListenerServiceRankingMap.h"
#include "elastos/droid/service/notification/CINotificationListenerWrapper.h"
#include "elastos/droid/service/notification/CCondition.h"
#include "elastos/droid/service/notification/CConditionProviderServiceProvider.h"
#include "elastos/droid/service/notification/CNotificationRankingUpdate.h"
#include "elastos/droid/service/notification/CStatusBarNotification.h"
#include "elastos/droid/service/notification/CZenModeConfig.h"
#include "elastos/droid/service/notification/CZenModeConfigDowntimeInfo.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

CAR_OBJECT_IMPL(CNotificationListenerServiceRanking)
CAR_OBJECT_IMPL(CNotificationListenerServiceRankingMap)
CAR_OBJECT_IMPL(CINotificationListenerWrapper)
CAR_OBJECT_IMPL(CCondition)
CAR_OBJECT_IMPL(CConditionProviderServiceProvider)
CAR_OBJECT_IMPL(CNotificationRankingUpdate)
CAR_OBJECT_IMPL(CStatusBarNotification)
CAR_OBJECT_IMPL(CZenModeConfig)
CAR_OBJECT_IMPL(CZenModeConfigDowntimeInfo)

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos
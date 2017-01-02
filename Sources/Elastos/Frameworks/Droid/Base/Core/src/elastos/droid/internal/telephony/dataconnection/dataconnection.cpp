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

#include "elastos/droid/internal/telephony/dataconnection/CApnContext.h"
#include "elastos/droid/internal/telephony/dataconnection/CApnProfileOmh.h"
#include "elastos/droid/internal/telephony/dataconnection/CApnSetting.h"
#include "elastos/droid/internal/telephony/dataconnection/CCdmaApnProfileTracker.h"
#include "elastos/droid/internal/telephony/dataconnection/CDataCallResponse.h"
#include "elastos/droid/internal/telephony/dataconnection/CDataConnection.h"
#include "elastos/droid/internal/telephony/dataconnection/CDataProfile.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcAsyncChannel.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcFailCause.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcRetryAlarmController.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcSwitchAsyncChannel.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcSwitchState.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcTesterDeactivateAll.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcTesterFailBringUpAll.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcTracker.h"
#include "elastos/droid/internal/telephony/dataconnection/CDctController.h"
#include "elastos/droid/internal/telephony/dataconnection/CDdsScheduler.h"
#include "elastos/droid/internal/telephony/dataconnection/CDdsSchedulerAc.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_OBJECT_IMPL(CApnContext)
CAR_OBJECT_IMPL(CApnProfileOmh)
CAR_OBJECT_IMPL(CApnSetting)
CAR_OBJECT_IMPL(CCdmaApnProfileTracker)
CAR_OBJECT_IMPL(CDataCallResponse)
CAR_OBJECT_IMPL(CDataConnection)
CAR_OBJECT_IMPL(CDataProfile)
CAR_OBJECT_IMPL(CDcAsyncChannel)
CAR_OBJECT_IMPL(CDcFailCause)
CAR_OBJECT_IMPL(CDcRetryAlarmController)
CAR_OBJECT_IMPL(CDcSwitchAsyncChannel)
CAR_OBJECT_IMPL(CDcSwitchState)
CAR_OBJECT_IMPL(CDcTesterDeactivateAll)
CAR_OBJECT_IMPL(CDcTesterFailBringUpAll)
CAR_OBJECT_IMPL(CDcTracker)
CAR_OBJECT_IMPL(CDctController)
CAR_OBJECT_IMPL(CDdsScheduler)
CAR_OBJECT_IMPL(CDdsSchedulerAc)

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos


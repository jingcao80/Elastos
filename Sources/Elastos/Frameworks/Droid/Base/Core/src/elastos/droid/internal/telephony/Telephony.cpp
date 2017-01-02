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

#include "elastos/droid/internal/telephony/CSmsAddress.h"
#include "elastos/droid/internal/telephony/CSmsHeader.h"
#include "elastos/droid/internal/telephony/CSmsStorageMonitor.h"
#include "elastos/droid/internal/telephony/CSmsUsageMonitor.h"
#include "elastos/droid/internal/telephony/CSubInfoRecordUpdater.h"
#include "elastos/droid/internal/telephony/CSubscription.h"
#include "elastos/droid/internal/telephony/CSubscriptionData.h"
#include "elastos/droid/internal/telephony/CPhoneProxy.h"
#include "elastos/droid/internal/telephony/CPhoneStateIntentReceiver.h"
#include "elastos/droid/internal/telephony/CPhoneSubInfo.h"
#include "elastos/droid/internal/telephony/CPhoneSubInfoController.h"
#include "elastos/droid/internal/telephony/CPhoneSubInfoProxy.h"
#include "elastos/droid/internal/telephony/CSubscriptionController.h"
#include "elastos/droid/internal/telephony/CSubscriptionHelper.h"
#include "elastos/droid/internal/telephony/CSyntheticSmsMessage.h"
#include "elastos/droid/internal/telephony/CTelephonyDevController.h"
#include "elastos/droid/internal/telephony/CUiccPhoneBookController.h"
#include "elastos/droid/internal/telephony/CUiccSmsController.h"
#include "elastos/droid/internal/telephony/CUUSInfo.h"
#include "elastos/droid/internal/telephony/CWapPushOverSms.h"
#include "elastos/droid/internal/telephony/CWspTypeDecoder.h"
#include "elastos/droid/internal/telephony/CRestrictedState.h"
#include "elastos/droid/internal/telephony/CRetryManager.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_OBJECT_IMPL(CSmsAddress)

CAR_OBJECT_IMPL(CSmsHeader)

CAR_OBJECT_IMPL(CSmsStorageMonitor)

CAR_OBJECT_IMPL(CSmsUsageMonitor)

CAR_OBJECT_IMPL(CSubInfoRecordUpdater)

CAR_OBJECT_IMPL(CSubscription)

CAR_OBJECT_IMPL(CSubscriptionData)

CAR_OBJECT_IMPL(CPhoneProxy)

CAR_OBJECT_IMPL(CPhoneStateIntentReceiver)

CAR_OBJECT_IMPL(CPhoneSubInfo)

CAR_OBJECT_IMPL(CPhoneSubInfoController)

CAR_OBJECT_IMPL(CPhoneSubInfoProxy)

CAR_OBJECT_IMPL(CSubscriptionController)

CAR_OBJECT_IMPL(CSubscriptionHelper)

CAR_OBJECT_IMPL(CSyntheticSmsMessage)

CAR_OBJECT_IMPL(CTelephonyDevController)

CAR_OBJECT_IMPL(CUiccPhoneBookController)

CAR_OBJECT_IMPL(CUiccSmsController)

CAR_OBJECT_IMPL(CUUSInfo)

CAR_OBJECT_IMPL(CWapPushOverSms)

CAR_OBJECT_IMPL(CWspTypeDecoder)

CAR_OBJECT_IMPL(CRestrictedState)

CAR_OBJECT_IMPL(CRetryManager)

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

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

#include "elastos/droid/teleservice/phone/CADNList.h"
#include "elastos/droid/teleservice/phone/COutgoingCallReceiver.h"
#include "elastos/droid/teleservice/phone/CEmergencyCallbackModeServiceBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CEmergencyCallbackModeServiceLocalBinder.h"
#include "elastos/droid/teleservice/phone/CEmergencyCallbackModeExitDialogBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CHfaLogicBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CEmergencyDialerBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CNetworkQueryServiceLocalBinder.h"
#include "elastos/droid/teleservice/phone/CNetworkQueryServiceNetworkQueryService.h"
#include "elastos/droid/teleservice/phone/CNetworkSettingNetworkQueryServiceCallback.h"
#include "elastos/droid/teleservice/phone/CPhoneGlobalsPhoneAppBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CPhoneGlobalsNotificationBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CPhoneGlobalsNotificationBroadcastReceiver.h"
#include "elastos/droid/teleservice/phone/CIccProvider.h"
#include "elastos/droid/teleservice/phone/CIccPanel.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_OBJECT_IMPL(CADNList)
CAR_OBJECT_IMPL(COutgoingCallReceiver)
CAR_OBJECT_IMPL(CEmergencyCallbackModeServiceBroadcastReceiver)
CAR_OBJECT_IMPL(CEmergencyCallbackModeServiceLocalBinder)
CAR_OBJECT_IMPL(CEmergencyCallbackModeExitDialogBroadcastReceiver)
CAR_OBJECT_IMPL(CHfaLogicBroadcastReceiver)
CAR_OBJECT_IMPL(CEmergencyDialerBroadcastReceiver)
CAR_OBJECT_IMPL(CNetworkQueryServiceLocalBinder)
CAR_OBJECT_IMPL(CNetworkQueryServiceNetworkQueryService)
CAR_OBJECT_IMPL(CNetworkSettingNetworkQueryServiceCallback)
CAR_OBJECT_IMPL(CPhoneGlobalsPhoneAppBroadcastReceiver)
CAR_OBJECT_IMPL(CPhoneGlobalsNotificationBroadcastReceiver)
CAR_OBJECT_IMPL(CIccProvider)
CAR_OBJECT_IMPL(CIccPanel)

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

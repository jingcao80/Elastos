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

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_CPSTNINCOMINGCALLNOTIFIERBROADCASTERCEIVER_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_CPSTNINCOMINGCALLNOTIFIERBROADCASTERCEIVER_H__

#include "_Elastos_Droid_TeleService_Services_Telephony_CPstnIncomingCallNotifierBroadcastReceiver.h"
#include "elastos/droid/teleservice/services/telephony/PstnIncomingCallNotifier.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

CarClass(CPstnIncomingCallNotifierBroadcastReceiver)
    , public PstnIncomingCallNotifier::MyBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_CPSTNINCOMINGCALLNOTIFIERBROADCASTERCEIVER_H__
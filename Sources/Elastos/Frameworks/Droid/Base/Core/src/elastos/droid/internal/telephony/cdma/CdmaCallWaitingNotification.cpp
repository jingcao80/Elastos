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

#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaCallWaitingNotification.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                     CdmaCallWaitingNotification
//=====================================================================
CAR_INTERFACE_IMPL(CdmaCallWaitingNotification, Object, ICdmaCallWaitingNotification);

const String CdmaCallWaitingNotification::LOGTAG("CdmaCallWaitingNotification");

CdmaCallWaitingNotification::CdmaCallWaitingNotification()
    : numberPresentation(0)
    , namePresentation(0)
    , numberType(0)
    , numberPlan(0)
    , isPresent(0)
    , signalType(0)
    , alertPitch(0)
    , signal(0)
{
}

ECode CdmaCallWaitingNotification::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = /*super.toString() +*/ String("Call Waiting Notification  ")
        + " number: " + number
        + " numberPresentation: " + StringUtils::ToString(numberPresentation)
        + " name: " + name
        + " namePresentation: " + StringUtils::ToString(namePresentation)
        + " numberType: " + StringUtils::ToString(numberType)
        + " numberPlan: " + StringUtils::ToString(numberPlan)
        + " isPresent: " + StringUtils::ToString(isPresent)
        + " signalType: " + StringUtils::ToString(signalType)
        + " alertPitch: " + StringUtils::ToString(alertPitch)
        + " signal: " + StringUtils::ToString(signal) ;
    return NOERROR;
}

Int32 CdmaCallWaitingNotification::PresentationFromCLIP(
    /* [in] */ Int32 cli)
{
    switch(cli) {
        case 0: return IPhoneConstants::PRESENTATION_ALLOWED;
        case 1: return IPhoneConstants::PRESENTATION_RESTRICTED;
        case 2: return IPhoneConstants::PRESENTATION_UNKNOWN;
        default:
            // This shouldn't happen, just log an error and treat as Unknown
            Logger::D(LOGTAG, "Unexpected presentation %d", cli);
            return IPhoneConstants::PRESENTATION_UNKNOWN;
    }
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

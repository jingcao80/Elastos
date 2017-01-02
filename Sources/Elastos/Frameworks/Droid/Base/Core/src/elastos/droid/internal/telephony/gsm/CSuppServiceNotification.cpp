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

#include "elastos/droid/internal/telephony/gsm/CSuppServiceNotification.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

const Int32 CSuppServiceNotification::NOTIFICATION_TYPE_MO;
const Int32 CSuppServiceNotification::NOTIFICATION_TYPE_MT;
const Int32 CSuppServiceNotification::MO_CODE_UNCONDITIONAL_CF_ACTIVE;
const Int32 CSuppServiceNotification::MO_CODE_SOME_CF_ACTIVE;
const Int32 CSuppServiceNotification::MO_CODE_CALL_FORWARDED;
const Int32 CSuppServiceNotification::MO_CODE_CALL_IS_WAITING;
const Int32 CSuppServiceNotification::MO_CODE_CUG_CALL;
const Int32 CSuppServiceNotification::MO_CODE_OUTGOING_CALLS_BARRED;
const Int32 CSuppServiceNotification::MO_CODE_INCOMING_CALLS_BARRED;
const Int32 CSuppServiceNotification::MO_CODE_CLIR_SUPPRESSION_REJECTED;
const Int32 CSuppServiceNotification::MO_CODE_CALL_DEFLECTED;
const Int32 CSuppServiceNotification::MT_CODE_FORWARDED_CALL;
const Int32 CSuppServiceNotification::MT_CODE_CUG_CALL;
const Int32 CSuppServiceNotification::MT_CODE_CALL_ON_HOLD;
const Int32 CSuppServiceNotification::MT_CODE_CALL_RETRIEVED;
const Int32 CSuppServiceNotification::MT_CODE_MULTI_PARTY_CALL;
const Int32 CSuppServiceNotification::MT_CODE_ON_HOLD_CALL_RELEASED;
const Int32 CSuppServiceNotification::MT_CODE_FORWARD_CHECK_RECEIVED;
const Int32 CSuppServiceNotification::MT_CODE_CALL_CONNECTING_ECT;
const Int32 CSuppServiceNotification::MT_CODE_CALL_CONNECTED_ECT;
const Int32 CSuppServiceNotification::MT_CODE_DEFLECTED_CALL;
const Int32 CSuppServiceNotification::MT_CODE_ADDITIONAL_CALL_FORWARDED;

CAR_INTERFACE_IMPL(CSuppServiceNotification, Object, ISuppServiceNotification)

CAR_OBJECT_IMPL(CSuppServiceNotification)

CSuppServiceNotification::CSuppServiceNotification()
    : mNotificationType(0)
    , mCode(0)
    , mIndex(0)
    , mType(0)
{
}

ECode CSuppServiceNotification::constructor()
{
    return NOERROR;
}

ECode CSuppServiceNotification::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb(" mobile");
    sb += (mNotificationType == NOTIFICATION_TYPE_MO ? " originated " : " terminated ");
    sb += " code: "; sb += mCode;
    sb += " index: "; sb += mIndex;
    sb += " \"";
    String str;
    sb += (PhoneNumberUtils::StringFromStringAndTOA(mNumber, mType, &str), str);
    sb += "\" ";
    *result = sb.ToString();
    return NOERROR;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
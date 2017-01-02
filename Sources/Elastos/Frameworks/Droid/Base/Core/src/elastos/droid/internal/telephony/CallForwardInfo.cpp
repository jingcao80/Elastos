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

#include "elastos/droid/internal/telephony/CallForwardInfo.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"

using Elastos::Droid::Telephony::PhoneNumberUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CallForwardInfo, Object, ICallForwardInfo)
CallForwardInfo::CallForwardInfo()
    : mStatus(0)
    , mReason(0)
    , mServiceClass(0)
    , mToa(0)
    , mTimeSeconds(0)
    , mStartHour(0)
    , mStartMinute(0)
    , mEndHour(0)
    , mEndMinute(0)
{}

ECode CallForwardInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    String value;
    Object::ToString(&value);
    StringBuilder sb(value);
    sb.Append(mStatus == 0 ? " not active " : " active ");
    sb.Append(" mReason: ");
    sb.Append(mReason);
    sb.Append(" mServiceClass: ");
    sb.Append(mServiceClass);
    sb.Append(" \"");

    PhoneNumberUtils::StringFromStringAndTOA(mNumber, mToa, &value);
    sb.Append(value);
    sb.Append("\" ");
    sb.Append(mTimeSeconds);
    sb.Append(" seconds");
    sb.Append(" mStartHour:");
    sb.Append(mStartHour);
    sb.Append(" mStartMinute: ");
    sb.Append(mStartMinute);
    sb.Append(" mEndHour: ");
    sb.Append(mEndHour);
    sb.Append(" mEndMinute: ");
    sb.Append(mEndMinute);
    return sb.ToString(str);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

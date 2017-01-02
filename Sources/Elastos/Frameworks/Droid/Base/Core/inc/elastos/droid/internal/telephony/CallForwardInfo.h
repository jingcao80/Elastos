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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CALLFORWARDINFO_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CALLFORWARDINFO_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telephony::ICallForwardInfo;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * See also RIL_CallForwardInfo in include/telephony/ril.h
 *
 * {@hide}
 */
class ECO_PUBLIC CallForwardInfo
    : public Object
    , public ICallForwardInfo
{
public:
    CAR_INTERFACE_DECL()

    CallForwardInfo();

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

public:
    Int32             mStatus;      /*1 = active, 0 = not active */
    Int32             mReason;      /* from TS 27.007 7.11 "mReason" */
    Int32             mServiceClass; /* Sum of CommandsInterface.SERVICE_CLASS */
    Int32             mToa;         /* "type" from TS 27.007 7.11 */
    String            mNumber;      /* "mNumber" from TS 27.007 7.11 */
    Int32             mTimeSeconds; /* for CF no reply only */
    Int32             mStartHour;
    Int32             mStartMinute;
    Int32             mEndHour;
    Int32             mEndMinute;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CALLFORWARDINFO_H__

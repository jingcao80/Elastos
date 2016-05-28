
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CALLFORWARDINFO_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CALLFORWARDINFO_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

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
class CallForwardInfo
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

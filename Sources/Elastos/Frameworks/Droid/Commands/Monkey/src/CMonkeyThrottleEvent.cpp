
#include "CMonkeyThrottleEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CAR_OBJECT_IMPL(CMonkeyThrottleEvent)

ECode CMonkeyThrottleEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyThrottleEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyThrottleEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyThrottleEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyThrottleEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyThrottleEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyThrottleEvent::constructor(
    /* [in] */ Int64 throttle)
{
    MonkeyThrottleEvent::Init(throttle);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

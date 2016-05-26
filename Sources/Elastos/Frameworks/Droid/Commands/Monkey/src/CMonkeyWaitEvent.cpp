
#include "CMonkeyWaitEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CAR_OBJECT_IMPL(CMonkeyWaitEvent)

ECode CMonkeyWaitEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyWaitEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyWaitEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyWaitEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyWaitEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyWaitEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyWaitEvent::constructor(
    /* [in] */ Int64 waitTime)
{
    MonkeyWaitEvent::Init(waitTime);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

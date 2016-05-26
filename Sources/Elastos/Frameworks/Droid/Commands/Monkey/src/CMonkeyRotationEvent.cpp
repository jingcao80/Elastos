
#include "CMonkeyRotationEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CAR_OBJECT_IMPL(CMonkeyRotationEvent)

ECode CMonkeyRotationEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyRotationEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyRotationEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyRotationEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyRotationEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyRotationEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyRotationEvent::constructor(
    /* [in] */ Int32 degree,
    /* [in] */ Boolean persist)
{
    MonkeyRotationEvent::Init(degree, persist);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

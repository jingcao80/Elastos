
#include "CMonkeyNoopEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CAR_OBJECT_IMPL(CMonkeyNoopEvent)

ECode CMonkeyNoopEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyNoopEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyNoopEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyNoopEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyNoopEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyNoopEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyNoopEvent::constructor()
{
    MonkeyNoopEvent::Init();
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

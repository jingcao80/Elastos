
#include "CMonkeyGetFrameRateEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CAR_OBJECT_IMPL(CMonkeyGetFrameRateEvent)

ECode CMonkeyGetFrameRateEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyGetFrameRateEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyGetFrameRateEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyGetFrameRateEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyGetFrameRateEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyGetFrameRateEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyGetFrameRateEvent::constructor(
    /* [in] */ const String& status,
    /* [in] */ const String& testCaseName)
{
    MonkeyGetFrameRateEvent::Init(status, testCaseName);
    return NOERROR;
}

ECode CMonkeyGetFrameRateEvent::constructor(
    /* [in] */ const String& status)
{
    MonkeyGetFrameRateEvent::Init(status);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

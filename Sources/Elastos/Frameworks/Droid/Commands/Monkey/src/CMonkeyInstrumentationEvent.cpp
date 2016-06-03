
#include "CMonkeyInstrumentationEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CAR_OBJECT_IMPL(CMonkeyInstrumentationEvent)

ECode CMonkeyInstrumentationEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyInstrumentationEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyInstrumentationEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyInstrumentationEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyInstrumentationEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyInstrumentationEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyInstrumentationEvent::constructor(
    /* [in] */ const String& testCaseName,
    /* [in] */ const String& runnerName)
{
    MonkeyInstrumentationEvent::Init(testCaseName, runnerName);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

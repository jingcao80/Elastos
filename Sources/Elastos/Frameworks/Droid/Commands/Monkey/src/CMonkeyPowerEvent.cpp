
#include "CMonkeyPowerEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkeyPowerEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyPowerEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyPowerEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyPowerEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyPowerEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyPowerEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyPowerEvent::constructor(
    /* [in] */ const String& powerLogTag,
    /* [in] */ const String& powerTestResult)
{
    MonkeyPowerEvent::Init(powerLogTag, powerTestResult);
    return NOERROR;
}

ECode CMonkeyPowerEvent::constructor(
    /* [in] */ const String& powerLogTag)
{
    MonkeyPowerEvent::Init(powerLogTag);
    return NOERROR;
}

ECode CMonkeyPowerEvent::constructor()
{
    MonkeyPowerEvent::Init();
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

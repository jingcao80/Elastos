
#include "CMonkeyGetAppFrameRateEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {


ECode CMonkeyGetAppFrameRateEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyGetAppFrameRateEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyGetAppFrameRateEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyGetAppFrameRateEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyGetAppFrameRateEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyGetAppFrameRateEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyGetAppFrameRateEvent::constructor(
    /* [in] */ const String& status,
    /* [in] */ const String& activityName,
    /* [in] */ const String& testCaseName)
{
    MonkeyGetAppFrameRateEvent::Init(status, activityName, testCaseName);
    return NOERROR;
}

ECode CMonkeyGetAppFrameRateEvent::constructor(
    /* [in] */ const String& status,
    /* [in] */ const String& activityName)
{
    MonkeyGetAppFrameRateEvent::Init(status, activityName);
    return NOERROR;
}

ECode CMonkeyGetAppFrameRateEvent::constructor(
    /* [in] */ const String& status)
{
    MonkeyGetAppFrameRateEvent::Init(status);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

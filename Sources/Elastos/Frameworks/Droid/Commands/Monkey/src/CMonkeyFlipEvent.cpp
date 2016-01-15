
#include "CMonkeyFlipEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkeyFlipEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyFlipEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyFlipEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyFlipEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyFlipEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyFlipEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyFlipEvent::constructor(
    /* [in] */ Boolean isKeyboardOpen)
{
    MonkeyFlipEvent::Init(isKeyboardOpen);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos


#include "CMonkeyCommandEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkeyCommandEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyCommandEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyCommandEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyCommandEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyCommandEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyCommandEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyCommandEvent::constructor(
    /* [in] */ const String& cmd)
{
    MonkeyCommandEvent::Init(cmd);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

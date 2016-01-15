
#include "CMonkeyActivityEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkeyActivityEvent::GetEventType(
    /* [out] */ Int32 *eventType)
{
    VALIDATE_NOT_NULL(eventType)
    *eventType = MonkeyActivityEvent::GetEventType();
    return NOERROR;
}

ECode CMonkeyActivityEvent::IsThrottlable(
    /* [out] */ Boolean *isThrottlable)
{
    VALIDATE_NOT_NULL(isThrottlable)
    *isThrottlable = MonkeyActivityEvent::IsThrottlable();
    return NOERROR;
}

ECode CMonkeyActivityEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result)
    *result = MonkeyActivityEvent::InjectEvent(iwm, iam, verbose);
    return NOERROR;
}

ECode CMonkeyActivityEvent::constructor(
    /* [in] */IComponentName *app)
{
    MonkeyActivityEvent::Init(app);
    return NOERROR;
}

ECode CMonkeyActivityEvent::constructor(
    /* [in] */ IComponentName *app,
    /* [in] */ Int64 arg)
{
    MonkeyActivityEvent::Init(app, arg);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

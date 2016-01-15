
#include "CMonkeySourceScript.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkeySourceScript::GetNextEvent(
    /* [out] */ IMonkeyEvent **event)
{
    return MonkeySourceScript::GetNextEvent(event);
}

ECode CMonkeySourceScript::SetVerbose(
    /* [in] */ Int32 verbose)
{
    return MonkeySourceScript::SetVerbose(verbose);
}

ECode CMonkeySourceScript::Validate(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result);
    *result = MonkeySourceScript::Validate();
    return NOERROR;
}

ECode CMonkeySourceScript::constructor(
    /* [in] */ IRandom *random,
    /* [in] */ const String& filename,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle,
    /* [in] */ Int64 profileWaitTime,
    /* [in] */ Int64 deviceSleepTime)
{
    MonkeySourceScript::Init(
        random,
        filename,
        throttle,
        randomizeThrottle,
        profileWaitTime,
        deviceSleepTime);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

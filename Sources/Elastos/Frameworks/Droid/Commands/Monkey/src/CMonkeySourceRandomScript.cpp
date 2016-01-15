
#include "CMonkeySourceRandomScript.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {


ECode CMonkeySourceRandomScript::GetNextEvent(
    /* [out] */IMonkeyEvent **event)
{
    VALIDATE_NOT_NULL(event);
    return MonkeySourceRandomScript::GetNextEvent(event);
}

ECode CMonkeySourceRandomScript::SetVerbose(
    /* [in] */ Int32 verbose)
{
    return MonkeySourceRandomScript::SetVerbose(verbose);
}

ECode CMonkeySourceRandomScript::Validate(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result);
    return MonkeySourceRandomScript::Validate(result);
}

ECode CMonkeySourceRandomScript::constructor(
    /* [in] */ const String& setupFileName,
    /* [in] */ ArrayOf<String> *scriptFileNames,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle,
    /* [in] */ IRandom *random,
    /* [in] */ Int64 profileWaitTime,
    /* [in] */ Int64 deviceSleepTime,
    /* [in] */ Boolean randomizeScript)
{
    return MonkeySourceRandomScript::Init(
        setupFileName,
        scriptFileNames,
        throttle,
        randomizeThrottle,
        random,
        profileWaitTime,
        deviceSleepTime,
        randomizeScript);
}

ECode CMonkeySourceRandomScript::constructor(
    /* [in] */ ArrayOf<String> *scriptFileNames,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle,
    /* [in] */ IRandom *random,
    /* [in] */ Int64 profileWaitTime,
    /* [in] */ Int64 deviceSleepTime,
    /* [in] */ Boolean randomizeScript)
{
    return MonkeySourceRandomScript::Init(
        scriptFileNames,
        throttle,
        randomizeThrottle,
        random,
        profileWaitTime,
        deviceSleepTime,
        randomizeScript);
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

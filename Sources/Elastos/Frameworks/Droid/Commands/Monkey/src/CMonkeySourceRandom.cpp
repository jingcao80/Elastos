
#include "CMonkeySourceRandom.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {


ECode CMonkeySourceRandom::GetNextEvent(
    /* [out] */ IMonkeyEvent **event)
{
    VALIDATE_NOT_NULL(event);
    MonkeySourceRandom::GetNextEvent(event);
    return NOERROR;
}

ECode CMonkeySourceRandom::SetVerbose(
    /* [in] */ Int32 verbose)
{
    return MonkeySourceRandom::SetVerbose(verbose);
}

ECode CMonkeySourceRandom::Validate(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result);
    return MonkeySourceRandom::Validate(result);
}

ECode CMonkeySourceRandom::SetFactors(
    /* [in] */ ArrayOf<Float> *factors)
{
    return MonkeySourceRandom::SetFactors(factors);
}

ECode CMonkeySourceRandom::SetFactors(
    /* [in] */ Int32 index,
    /* [in] */ Float v)
{
    return MonkeySourceRandom::SetFactors(index, v);
}

ECode CMonkeySourceRandom::GenerateActivity()
{
    return MonkeySourceRandom::GenerateActivity();
}

ECode CMonkeySourceRandom::constructor(
    /* [in] */ Elastos::Core::IRandom *random,
    /* [in] */ IObjectContainer *mainApps,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle)
{
    MonkeySourceRandom::Init(
        random,
        mainApps,
        throttle,
        randomizeThrottle);
    return NOERROR;
}

CARAPI CMonkeySourceRandom::GetKeyName(
    /* [in] */ Int32 keycode,
    /* [out] */ String *ret)
{
    VALIDATE_NOT_NULL(ret);
    MonkeySourceRandom::GetKeyName(keycode, ret);
    return NOERROR;
}

CARAPI CMonkeySourceRandom::GetKeyCode(
        /* [in] */ const String& keyName,
        /* [out] */ Int32 *keycode)
{
    VALIDATE_NOT_NULL(keycode);
    MonkeySourceRandom::GetKeyCode(keyName, keycode);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
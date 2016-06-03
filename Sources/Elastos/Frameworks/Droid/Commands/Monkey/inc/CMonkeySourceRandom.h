
#ifndef __CMONKEYSOURCERANDOM_H__
#define __CMONKEYSOURCERANDOM_H__

#include "_CMonkeySourceRandom.h"
#include "MonkeySourceRandom.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeySourceRandom), public MonkeySourceRandom
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetNextEvent(
        /* [out] */ IMonkeyEvent **event);

    CARAPI SetVerbose(
        /* [in] */ Int32 verbose);

    CARAPI Validate(
        /* [out] */ Boolean *result);

    CARAPI SetFactors(
        /* [in] */ ArrayOf<Float> *factors);

    CARAPI SetFactors(
        /* [in] */ Int32 index,
        /* [in] */ Float v);

    CARAPI GenerateActivity();

    CARAPI constructor(
        /* [in] */ IRandom *random,
        /* [in] */ IObjectContainer *mainApps,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle);

    static CARAPI GetKeyName(
    /* [in] */ Int32 keycode,
    /* [out] */ String *ret);

    static CARAPI GetKeyCode(
        /* [in] */ const String& keyName,
        /* [out] */ Int32 *keycode);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYSOURCERANDOM_H__

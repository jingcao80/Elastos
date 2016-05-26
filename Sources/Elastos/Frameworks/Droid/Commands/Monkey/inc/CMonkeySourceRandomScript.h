
#ifndef __CMONKEYSOURCERANDOMSCRIPT_H__
#define __CMONKEYSOURCERANDOMSCRIPT_H__

#include "_CMonkeySourceRandomScript.h"
#include "MonkeySourceRandomScript.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeySourceRandomScript), public MonkeySourceRandomScript
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetNextEvent(
        /* [out] */ IMonkeyEvent **event);

    CARAPI SetVerbose(
        /* [in] */ Int32 verbose);

    CARAPI Validate(
        /* [out] */ Boolean *result);

    CARAPI constructor(
        /* [in] */ const String& setupFileName,
        /* [in] */ ArrayOf<String> *scriptFileNames,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle,
        /* [in] */ IRandom *random,
        /* [in] */ Int64 profileWaitTime,
        /* [in] */ Int64 deviceSleepTime,
        /* [in] */ Boolean randomizeScript);

    CARAPI constructor(
        /* [in] */ ArrayOf<String> *scriptFileNames,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle,
        /* [in] */ IRandom *random,
        /* [in] */ Int64 profileWaitTime,
        /* [in] */ Int64 deviceSleepTime,
        /* [in] */ Boolean randomizeScript);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYSOURCERANDOMSCRIPT_H__


#ifndef __CMONKEYSOURCESCRIPT_H__
#define __CMONKEYSOURCESCRIPT_H__

#include "_CMonkeySourceScript.h"
#include "MonkeySourceScript.h"

using Elastos::Droid::Commands::Monkey::IMonkeyEvent;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeySourceScript), public MonkeySourceScript
{
public:
    CARAPI GetNextEvent(
        /* [out] */ IMonkeyEvent **event);

    CARAPI SetVerbose(
        /* [in] */ Int32 verbose);

    CARAPI Validate(
        /* [out] */ Boolean *result);

    CARAPI constructor(
        /* [in] */ IRandom *random,
        /* [in] */ const String& filename,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle,
        /* [in] */ Int64 profileWaitTime,
        /* [in] */ Int64 deviceSleepTime);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYSOURCESCRIPT_H__

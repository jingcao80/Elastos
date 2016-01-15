
#ifndef __CMONKEYEVENTQUEUE_H__
#define __CMONKEYEVENTQUEUE_H__

#include "_CMonkeyEventQueue.h"
#include "MonkeyEventQueue.h"

using Elastos::Core::IRandom;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyEventQueue), public MonkeyEventQueue
{
public:
    CARAPI IsEmpty(
        /* [out] */ Boolean *isEmpty);

    CARAPI GetFirst(
        /* [out] */ IMonkeyEvent **monkeyEvent);

    CARAPI RemoveFirst();

    CARAPI AddLast(
        /* [in] */ IMonkeyEvent *event);

    CARAPI constructor(
        /* [in] */ IRandom *random,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYEVENTQUEUE_H__

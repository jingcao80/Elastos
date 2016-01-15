#ifndef __MONKEYEVENTQUEUE_H_
#define __MONKEYEVENTQUEUE_H_

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Commands.Monkey_server.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Core::IRandom;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyEventQueue
{
protected:
    CARAPI IsEmpty(
        /* [out] */ Boolean *isEmpty);

    CARAPI RemoveFirst();

    CARAPI AddLast(
        /* [in] */ IMonkeyEvent *e);

    CARAPI GetFirst(
        /* [out] */ IMonkeyEvent **monkeyEvent);

    CARAPI Init(
        /* [in] */ IRandom *random,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle);

private:
    AutoPtr<IRandom> mRandom;
    Int64 mThrottle;
    Boolean mRandomizeThrottle;
    List<AutoPtr<IMonkeyEvent> > mList;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif //__MONKEYEVENTQUEUE_H_

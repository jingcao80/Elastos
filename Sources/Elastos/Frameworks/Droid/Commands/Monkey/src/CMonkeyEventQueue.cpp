
#include "CMonkeyEventQueue.h"

using Elastos::Core::IRandom;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CAR_OBJECT_IMPL(CMonkeyEventQueue)

ECode CMonkeyEventQueue::IsEmpty(
    /* [out] */ Boolean *isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty);
    return MonkeyEventQueue::IsEmpty(isEmpty);
}

ECode CMonkeyEventQueue::GetFirst(
    /* [out] */ IMonkeyEvent **monkeyEvent)
{
    VALIDATE_NOT_NULL(monkeyEvent);
    return MonkeyEventQueue::GetFirst(monkeyEvent);
}

ECode CMonkeyEventQueue::RemoveFirst()
{
    return MonkeyEventQueue::RemoveFirst();
}

ECode CMonkeyEventQueue::AddLast(
    /* [in] */ IMonkeyEvent *event)
{
    return MonkeyEventQueue::AddLast(event);
}

ECode CMonkeyEventQueue::constructor(
    /* [in] */ IRandom *random,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle)
{
    return MonkeyEventQueue::Init(random, throttle, randomizeThrottle);
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

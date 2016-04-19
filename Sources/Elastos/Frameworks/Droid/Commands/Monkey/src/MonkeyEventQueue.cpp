#include "MonkeyEventQueue.h"
#include "CMonkeyThrottleEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode MonkeyEventQueue::IsEmpty(
    /* [out] */ Boolean *isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty)
    *isEmpty = mList.IsEmpty();
    return NOERROR;
}

ECode MonkeyEventQueue::RemoveFirst()
{
    mList.PopFront();
    return NOERROR;
}

ECode MonkeyEventQueue::GetFirst(
    /* [out] */ IMonkeyEvent **monkeyEvent)
{
    VALIDATE_NOT_NULL(monkeyEvent)
    AutoPtr<IMonkeyEvent> temp = mList.GetFront();
    *monkeyEvent = temp;
    REFCOUNT_ADD(*monkeyEvent)
    return NOERROR;
}

ECode MonkeyEventQueue::AddLast(
    /* [in] */ IMonkeyEvent *event)
{
    mList.PushBack(event);
    Boolean isThrottlable = FALSE;
    event->IsThrottlable(&isThrottlable);
    if (isThrottlable) {
        Int64 throttle = mThrottle;
        if (mRandomizeThrottle && mThrottle > 0) {
            mRandom->NextInt64(&throttle);
            if (throttle < 0) {
                throttle = -throttle;
            }
            throttle %= mThrottle;
            ++throttle;
        }
        AutoPtr<IMonkeyThrottleEvent> e;
        CMonkeyThrottleEvent::New(throttle, (IMonkeyThrottleEvent**)&e);
        mList.PushBack(e);
    }
    return NOERROR;
}

ECode MonkeyEventQueue::Init(
    /* [in] */ IRandom *random,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle)
{
    mRandom = random;
    mThrottle = throttle;
    mRandomizeThrottle = randomizeThrottle;
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos


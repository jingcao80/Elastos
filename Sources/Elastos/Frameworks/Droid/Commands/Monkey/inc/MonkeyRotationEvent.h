#ifndef __MONKEYROTATIONEVENT_H__
#define __MONKEYROTATIONEVENT_H__

#include "MonkeyEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyRotationEvent : public MonkeyEvent
{
protected:
    MonkeyRotationEvent();

    MonkeyRotationEvent(
        /* [in] */ Int32 degree,
        /* [in] */ Boolean persist);

    CARAPI Init(
        /* [in] */ Int32 degree,
        /* [in] */ Boolean persist);

    CARAPI_(Int32) InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose);

private:
    const volatile Int32 mRotationDegree;
    const volatile Boolean mPersist;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEYROTATIONEVENT_H__

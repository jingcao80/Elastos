#ifndef __MONKEYWAITEVENT_H__
#define __MONKEYWAITEVENT_H__

#include "MonkeyEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyWaitEvent : public MonkeyEvent
{
protected:
    MonkeyWaitEvent();

    MonkeyWaitEvent(
        /* [in] */ Int64 waitTime);

    CARAPI_(Int32) InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose);
    CARAPI Init(
    /* [in] */ Int64 waitTime);

private:
    Int64 mWaitTime;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEYWAITEVENT_H__

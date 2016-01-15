#ifndef __MONKEYTRACKBALLEVENT_H__
#define __MONKEYTRACKBALLEVENT_H__

#include "MonkeyMotionEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyTrackballEvent : public MonkeyMotionEvent
{
protected:
    MonkeyTrackballEvent();

    MonkeyTrackballEvent(
        /* [in] */ Int32 action);

    CARAPI Init(
        /* [in] */ Int32 action);

    CARAPI_(String) GetTypeLabel();
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif //__MONKEYTRACKBALLEVENT_H__

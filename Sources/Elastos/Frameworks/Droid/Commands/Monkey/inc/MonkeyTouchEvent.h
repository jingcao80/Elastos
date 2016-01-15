#ifndef __MONKEYTOUCHEVENT_H__
#define __MONKEYTOUCHEVENT_H__

#include "MonkeyMotionEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyTouchEvent : public MonkeyMotionEvent
{
protected:
    MonkeyTouchEvent();

    MonkeyTouchEvent(
        /* [in] */ Int32 action);

    CARAPI Init(
        /* [in] */ Int32 action);

protected:
    String GetTypeLabel();
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif //__MONKEYTOUCHEVENT_H__

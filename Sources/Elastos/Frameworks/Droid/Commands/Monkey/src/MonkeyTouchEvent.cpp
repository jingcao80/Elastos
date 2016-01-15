#include "MonkeyTouchEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyTouchEvent::MonkeyTouchEvent()
{}

MonkeyTouchEvent::MonkeyTouchEvent(
    /* [in] */ Int32 action)
    : MonkeyMotionEvent(IMonkeyEvent::EVENT_TYPE_TOUCH
                        , IInputDevice::SOURCE_TOUCHSCREEN
                        , action)
{}

ECode MonkeyTouchEvent::Init(
    /* [in] */ Int32 action)
{
    MonkeyMotionEvent::Init(
        IMonkeyEvent::EVENT_TYPE_TOUCH
        , IInputDevice::SOURCE_TOUCHSCREEN
        , action);
    return NOERROR;
}

String MonkeyTouchEvent::GetTypeLabel()
{
    return String("Touch");
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos


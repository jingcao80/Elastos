#include "MonkeyTrackballEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyTrackballEvent::MonkeyTrackballEvent()
{}

MonkeyTrackballEvent::MonkeyTrackballEvent(
    /* [in] */ Int32 action)
    : MonkeyMotionEvent(IMonkeyEvent::EVENT_TYPE_TRACKBALL
                        , IInputDevice::SOURCE_TRACKBALL
                        , action)
{}

CARAPI MonkeyTrackballEvent::Init(
    /* [in] */ Int32 action)
{
    MonkeyMotionEvent::Init(
        IMonkeyEvent::EVENT_TYPE_TRACKBALL
        , IInputDevice::SOURCE_TRACKBALL
        , action);
    return NOERROR;
}

String MonkeyTrackballEvent::GetTypeLabel()
{
    return String("Trackball");
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos


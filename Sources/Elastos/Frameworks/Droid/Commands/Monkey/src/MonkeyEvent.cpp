#include "MonkeyEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyEvent::~MonkeyEvent()
{
}

MonkeyEvent::MonkeyEvent()
    : mEventType(0)
{
}

MonkeyEvent::MonkeyEvent(
    /* [in] */ Int32 type)
    : mEventType(type)
{
}

Int32 MonkeyEvent::GetEventType()
{
    return mEventType;
}

Boolean MonkeyEvent::IsThrottlable()
{
    return TRUE;
}

ECode MonkeyEvent::Init(
    /* [in] */ Int32 type)
{
    mEventType = type;
    return NOERROR;
}


} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

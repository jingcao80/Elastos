#include "MonkeyNoopEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyNoopEvent::MonkeyNoopEvent()
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_NOOP)
{
}

ECode MonkeyNoopEvent::Init()
{
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_NOOP);
    return NOERROR;
}

Int32 MonkeyNoopEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose)
{
    if(verbose > 1)
        PFL_EX("NOOP\n")
    return IMonkeyEvent::INJECT_SUCCESS;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos


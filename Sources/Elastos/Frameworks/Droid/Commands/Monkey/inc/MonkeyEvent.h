#ifndef __MONKEYEVENT_H__
#define __MONKEYEVENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Commands.Monkey_server.h"

using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::View::IIWindowManager;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

/**
 * abstract class for monkey event
 */
class MonkeyEvent
{
public:

protected:
    MonkeyEvent();

    MonkeyEvent(
        /* [in] */ Int32 type);

    virtual ~MonkeyEvent();

    /**
     * @return event type
     */
    CARAPI_(Int32) GetEventType();

    /**
     * a method for injecting event
     * @param iwm wires to current window manager
     * @param iam wires to current activity manager
     * @param verbose a log switch
     * @return INJECT_SUCCESS if it goes through, and INJECT_FAIL if it fails
     *         in the case of exceptions, return its corresponding error code
     */
    virtual CARAPI_(Int32) InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose) = 0;

protected:
    CARAPI Init(
        /* [in] */ Int32 type);

    /**
     * @return true if it is safe to throttle after this event, and false otherwise.
    */
    CARAPI_(Boolean) IsThrottlable();

protected:
    Int32 mEventType;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif //__MONKEYEVENT_H__

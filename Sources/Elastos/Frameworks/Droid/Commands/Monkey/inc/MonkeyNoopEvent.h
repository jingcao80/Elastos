#ifndef __MONKEYNOOPEVENT_H__
#define __MONKEYNOOPEVENT_H__

#include "MonkeyEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyNoopEvent : public MonkeyEvent
{
protected:
    MonkeyNoopEvent();

    CARAPI Init();

    CARAPI_(Int32) InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEYNOOPEVENT_H__
#ifndef __MONKEYCOMMANDEVENT_H__
#define __MONKEYCOMMANDEVENT_H__

#include "MonkeyEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyCommandEvent : public MonkeyEvent
{
protected:
    MonkeyCommandEvent();

    MonkeyCommandEvent(
        /* [in] */ const String& cmd);

    CARAPI Init(
        /* [in] */ const String& cmd);

    virtual CARAPI_(Int32) InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose);

private:
    String mCmd;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif //__MONKEYCOMMANDEVENT_H__

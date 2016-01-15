#ifndef __MONKEYINSTRUMENTATIONEVENT_H__
#define __MONKEYINSTRUMENTATIONEVENT_H__

#include "MonkeyEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyInstrumentationEvent : public MonkeyEvent
{
protected:
    MonkeyInstrumentationEvent();

    MonkeyInstrumentationEvent(
            /* [in] */ const String& testCaseName,
            /* [in] */ const String& runnerName);

    virtual CARAPI_(Int32) InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose);

    CARAPI Init(
        /* [in] */ const String& testCaseName,
        /* [in] */ const String& runnerName);

private:
    String mRunnerName;
    String mTestCaseName;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif //__MONKEYINSTRUMENTATIONEVENT_H__

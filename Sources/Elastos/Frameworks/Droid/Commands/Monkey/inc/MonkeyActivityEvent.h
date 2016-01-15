#ifndef __MONKEYACTIVITYEVENT_H__
#define __MONKEYACTIVITYEVENT_H__

#include "MonkeyEvent.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyActivityEvent : public MonkeyEvent
{
protected:
    MonkeyActivityEvent();

    MonkeyActivityEvent(
        /* [in] */ IComponentName *app);

    MonkeyActivityEvent(
        /* [in] */ IComponentName *app,
        /* [in] */ Int64 arg);

    CARAPI Init(
        /* [in] */ IComponentName *app);

    CARAPI Init(
        /* [in] */ IComponentName *app,
        /* [in] */ Int64 arg);

    Int32 InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose);

private:
    /**
     * @return Intent for the new activity
     */
    CARAPI_(AutoPtr<IIntent>) GetEvent();

public:
    Int64 mAlarmTime;

private:
    AutoPtr<IComponentName> mApp;
};


} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif //__MONKEYACTIVITYEVENT_H__

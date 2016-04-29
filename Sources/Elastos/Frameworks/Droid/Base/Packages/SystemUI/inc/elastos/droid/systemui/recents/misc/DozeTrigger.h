
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_DOZETRIGGER_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_DOZETRIGGER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/droid/os/Runnable.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Core::IRunnable;
using Elastos::Droid::Os::Runnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Misc {

/**
 * A dozer is a class that fires a trigger after it falls asleep.  You can occasionally poke it to
 * wake it up, but it will fall asleep if left untouched.
 */
class DozeTrigger
    : public Object
{
private:
    class DozeRunnable
        : public Runnable
    {
    public:
        DozeRunnable(
            /* [in] */ DozeTrigger* host);

        // @Override
        CARAPI Run();

    private:
        DozeTrigger* mHost;
    };

public:
    DozeTrigger(
        /* [in] */ Int32 dozeDurationSeconds,
        /* [in] */ IRunnable* sleepRunnable);

    /** Starts dozing. This also resets the trigger flag. */
    CARAPI_(void) StartDozing();

    /** Stops dozing. */
    CARAPI_(void) StopDozing();

    /** Poke this dozer to wake it up for a little bit, if it is dozing. */
    CARAPI_(void) Poke();

    /** Poke this dozer to wake it up for a little bit. */
     CARAPI_(void) ForcePoke();

    /** Returns whether we are dozing or not. */
    CARAPI_(Boolean) IsDozing();

    /** Returns whether the trigger has fired at least once. */
    CARAPI_(Boolean) HasTriggered();

public:
    AutoPtr<IHandler> mHandler;
    Boolean mIsDozing;
    Boolean mHasTriggered;
    Int32 mDozeDurationSeconds;
    AutoPtr<IRunnable> mSleepRunnable;
    AutoPtr<IRunnable> mDozeRunnable;
};

} // namespace Misc
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_DOZETRIGGER_H__

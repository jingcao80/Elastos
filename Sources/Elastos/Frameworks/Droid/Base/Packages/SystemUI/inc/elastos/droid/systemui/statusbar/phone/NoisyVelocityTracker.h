
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_NOISYVELOCITYTRACKER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_NOISYVELOCITYTRACKER_H__

#include "_SystemUI.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/droid/utility/Pools.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::Pools;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Core::Object;
using Elastos::Utility::IArrayDeque;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * A very simple low-pass velocity filter for motion events for noisy touch screens.
 */
class NoisyVelocityTracker
    : public Object
    , public INoisyVelocityTracker
    , public IVelocityTrackerInterface
{
private:
    class MotionEventCopy: public Object
    {
    public:
        MotionEventCopy(
            /* [in] */ Float x2,
            /* [in] */ Float y2,
            /* [in] */ Int64 eventTime);

    public:
        Float mX;
        Float mY;
        Int64 mT;
    };

public:
    CAR_INTERFACE_DECL();

    static CARAPI_(AutoPtr<NoisyVelocityTracker>) Obtain();

    CARAPI AddMovement(
        /* [in] */ IMotionEvent* event);

    CARAPI ComputeCurrentVelocity(
        /* [in] */ Int32 units);

    CARAPI GetXVelocity(
        /* [out] */ Float* result);

    CARAPI GetYVelocity(
        /* [out] */ Float* result);

    CARAPI Recycle();

private:
    NoisyVelocityTracker();

private:
    static AutoPtr<Pools::SynchronizedPool<INoisyVelocityTracker> > sNoisyPool;

    static const Float DECAY;
    static const Boolean DEBUG;

    const Int32 MAX_EVENTS;
    AutoPtr<IArrayDeque/*<MotionEventCopy*/> mEventBuf;
    Float mVX;
    Float mVY;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_NOISYVELOCITYTRACKER_H__

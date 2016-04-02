
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PLATFORMVELOCITYTRACKER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PLATFORMVELOCITYTRACKER_H__

#include "_SystemUI.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/utility/Pools.h>
#include <elastos/core/Object.h>

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::Utility::Pools;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * An implementation of {@link VelocityTrackerInterface} using the platform-standard
 * {@link VelocityTracker}.
 */
class PlatformVelocityTracker
    : public Object
    , public IVelocityTrackerInterface
{
public:
    CAR_INTERFACE_DECL();

    static CARAPI_(AutoPtr<PlatformVelocityTracker>) Obtain();

    CARAPI SetTracker(
        /* [in] */ IVelocityTracker* tracker);

    // @Override
    CARAPI AddMovement(
        /* [in] */ IMotionEvent* event);

    // @Override
    CARAPI ComputeCurrentVelocity(
        /* [in] */ Int32 units);

    // @Override
    CARAPI GetXVelocity(
        /* [out] */ Float* result);

    // @Override
    CARAPI GetYVelocity(
        /* [out] */ Float* result);

    // @Override
    CARAPI Recycle();

private:
    static AutoPtr<Pools::SynchronizedPool<IVelocityTrackerInterface> > sPool;

    AutoPtr<IVelocityTracker> mTracker;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PLATFORMVELOCITYTRACKER_H__

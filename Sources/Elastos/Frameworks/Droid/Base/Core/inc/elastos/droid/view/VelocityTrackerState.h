#ifndef __ELASTOS_DROID_VIEW_VELOCITYTRACKERSTATE_H__
#define __ELASTOS_DROID_VIEW_VELOCITYTRACKERSTATE_H__

#include <input/Input.h>
#include <input/VelocityTracker.h>
#include <utils/BitSet.h>

#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

using android::BitSet32;

class VelocityTrackerState
    : public Object
{
public:
    VelocityTrackerState(
        /* [in] */ const char* strategy);

    virtual ~VelocityTrackerState();

    void Clear();

    void AddMovement(
        /* [in] */ const android::MotionEvent* event);

    void ComputeCurrentVelocity(
        /* [in] */ Int32 units,
        /* [in] */ Float maxVelocity);

    void GetVelocity(
        /* [in] */ Int32 id,
        /* [in] */ Float* outVx,
        /* [in] */ Float* outVy);

    Boolean GetEstimator(
        /* [in] */ Int32 id,
        /* [in] */ android::VelocityTracker::Estimator* outEstimator);

private:
    class Velocity {
    public:
        Float vx, vy;
    };

    android::VelocityTracker mVelocityTracker;
    Int32 mActivePointerId;
    BitSet32 mCalculatedIdBits;
    Velocity mCalculatedVelocity[MAX_POINTERS];

    static const Int32 ACTIVE_POINTER_ID = -1;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif

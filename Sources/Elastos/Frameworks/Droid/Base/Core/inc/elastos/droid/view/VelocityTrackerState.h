#ifndef __ELASTOS_DROID_VIEW_VELOCITYTRACKERSTATE_H__
#define __ELASTOS_DROID_VIEW_VELOCITYTRACKERSTATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "androidfw/VelocityTracker.h"
#include "androidfw/Input.h"
#include "utils/BitSet.h"

using android::VelocityTracker;

namespace Elastos {
namespace Droid {
namespace View {

using android::MotionEvent;
using android::BitSet32;

class VelocityTrackerState : public ElRefBase
{
public:
    VelocityTrackerState(
        /* [in] */ const char* strategy);

    virtual ~VelocityTrackerState();

    void Clear();

    void AddMovement(
        /* [in] */ IMotionEvent* event);

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

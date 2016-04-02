
#include "elastos/droid/systemui/statusbar/phone/NoisyVelocityTracker.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::IInputEvent;
using Elastos::Utility::CArrayDeque;
using Elastos::Utility::IDeque;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

NoisyVelocityTracker::MotionEventCopy::MotionEventCopy(
    /* [in] */ Float x2,
    /* [in] */ Float y2,
    /* [in] */ Int64 eventTime)
    : mX(x2)
    , mY(y2)
    , mT(eventTime)
{
}

AutoPtr<Pools::SynchronizedPool<INoisyVelocityTracker> > NoisyVelocityTracker::sNoisyPool
    = new Pools::SynchronizedPool<INoisyVelocityTracker>(2);
const Float NoisyVelocityTracker::DECAY = 0.75f;
const Boolean NoisyVelocityTracker::DEBUG = FALSE;

CAR_INTERFACE_IMPL_2(NoisyVelocityTracker, Object, INoisyVelocityTracker, IVelocityTrackerInterface);
AutoPtr<NoisyVelocityTracker> NoisyVelocityTracker::Obtain()
{
    AutoPtr<NoisyVelocityTracker> instance = (NoisyVelocityTracker*)sNoisyPool->AcquireItem().Get();
    if (instance == NULL) {
        instance = new NoisyVelocityTracker();
    }
    return instance;
}

NoisyVelocityTracker::NoisyVelocityTracker()
    : MAX_EVENTS(8)
    , mVX(0)
    , mVY(0)
{
    CArrayDeque::New(MAX_EVENTS, (IArrayDeque**)&mEventBuf);
}

ECode NoisyVelocityTracker::AddMovement(
    /* [in] */ IMotionEvent* event)
{
    Int32 size = 0;
    mEventBuf->GetSize(&size);
    if (size == MAX_EVENTS) {
        AutoPtr<IInterface> obj;
        IDeque::Probe(mEventBuf)->Remove((IInterface**)&obj);
    }
    Float x = 0, y = 0;
    event->GetX(&x);
    event->GetY(&y);
    Int64 t = 0;
    IInputEvent::Probe(event)->GetEventTime(&t);
    AutoPtr<MotionEventCopy> me = new MotionEventCopy(x, y, t);
    mEventBuf->Add(me->Probe(EIID_IObject));
    return NOERROR;
}

ECode NoisyVelocityTracker::ComputeCurrentVelocity(
    /* [in] */ Int32 units)
{
    if (NoisyVelocityTracker::DEBUG) {
        Int32 size = 0;
        mEventBuf->GetSize(&size);
        Logger::V("FlingTracker", "computing velocities for %d events", size);
    }
    mVX = mVY = 0;
    AutoPtr<MotionEventCopy> last;
    Int32 i = 0;
    Float totalweight = 0.f;
    Float weight = 10.f;
    AutoPtr<IIterator> iter;
    mEventBuf->GetIterator((IIterator**)&iter);

    Boolean hasNext;
    while(iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);

        AutoPtr<MotionEventCopy> event = (MotionEventCopy*)IObject::Probe(obj);
        if (last != NULL) {
            const Float dt = (Float) (event->mT - last->mT) / units;
            const Float dx = (event->mX - last->mX);
            const Float dy = (event->mY - last->mY);
            if (NoisyVelocityTracker::DEBUG) {
                Logger::V("FlingTracker", "   [%d] (t=%d %.1f,%.1f) dx=%.1f dy=%.1f dt=%f vx=%.1f vy=%.1f",
                        i, event->mT, event->mX, event->mY, dx, dy, dt, (dx / dt), (dy / dt));
            }
            if (event->mT == last->mT) {
                // Really not sure what to do with events that happened at the same time,
                // so we'll skip subsequent events.
                continue;
            }
            mVX += weight * dx / dt;
            mVY += weight * dy / dt;
            totalweight += weight;
            weight *= DECAY;
        }
        last = event;
        i++;
    }

    if (totalweight > 0) {
        mVX /= totalweight;
        mVY /= totalweight;
    }
    else {
        // so as not to contaminate the velocities with NaN
        mVX = mVY = 0;
    }

    if (NoisyVelocityTracker::DEBUG) {
        Logger::V("FlingTracker", "computed: vx=%f vy=%f", mVX, mVY);
    }
    return NOERROR;
}

ECode NoisyVelocityTracker::GetXVelocity(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    if (Elastos::Core::Math::IsNaN(mVX) || Elastos::Core::Math::IsInfinite(mVX)) {
        mVX = 0;
    }
    *result = mVX;
    return NOERROR;
}

ECode NoisyVelocityTracker::GetYVelocity(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    if (Elastos::Core::Math::IsNaN(mVY) || Elastos::Core::Math::IsInfinite(mVX)) {
        mVY = 0;
    }
    *result = mVY;
    return NOERROR;
}

ECode NoisyVelocityTracker::Recycle()
{
    mEventBuf->Clear();
    sNoisyPool->ReleaseItem(this);
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#ifndef __ELASTOS_DROID_WIDGET_SCROLLER_H__
#define __ELASTOS_DROID_WIDGET_SCROLLER_H__

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IPowerManager;

namespace Elastos {
namespace Droid {
namespace Widget {

using Elastos::Droid::View::Animation::IInterpolator;

class Scroller
    : public Object
    , public IScroller
{
protected:
    class ViscousFluidInterpolator
        : public Object
        , public IInterpolator
    {
    public:
        CAR_INTERFACE_DECL();

        static CARAPI_(Float) ViscousFluid(
            /* [in] */ Float x);

        // @Override
        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* interpolation);

    private:
        /** Controls the viscous fluid effect (how much of it). */
        static const Float VISCOUS_FLUID_SCALE;
        static const Float VISCOUS_FLUID_NORMALIZE;
        static const Float VISCOUS_FLUID_OFFSET;
    };

public:
    CAR_INTERFACE_DECL();

    Scroller();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IInterpolator* interpolator);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IInterpolator* interpolator,
        /* [in] */ Boolean flywheel);

    CARAPI SetFriction(
        /* [in] */ Float friction);

    CARAPI IsFinished(
        /* [out] */ Boolean* finished);

    CARAPI ForceFinished(
        /* [in] */ Boolean finished);

    CARAPI GetDuration(
        /* [out] */ Int32* duration);

    CARAPI GetCurrX(
        /* [out] */ Int32* result);

    CARAPI GetCurrY(
        /* [out] */ Int32* result);

    CARAPI GetCurrVelocity(
        /* [out] */ Float* result);

    CARAPI GetStartX(
        /* [out] */ Int32* result);

    CARAPI GetStartY(
        /* [out] */ Int32* result);

    CARAPI GetFinalX(
        /* [out] */ Int32* result);

    CARAPI GetFinalY(
        /* [out] */ Int32* result);

    CARAPI ComputeScrollOffset(
        /* [out] */ Boolean* result);

    CARAPI StartScroll(
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy);

    CARAPI StartScroll(
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 dx,
        /* [in] */ Int32 dy,
        /* [in] */ Int32 duration);

    CARAPI Fling(
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 velocityX,
        /* [in] */ Int32 velocityY,
        /* [in] */ Int32 minX,
        /* [in] */ Int32 maxX,
        /* [in] */ Int32 minY,
        /* [in] */ Int32 maxY);

    CARAPI AbortAnimation();

    CARAPI ExtendDuration(
        /* [in] */ Int32 extend);

    CARAPI TimePassed(
        /* [out] */ Int32* passed);

    CARAPI SetFinalX(
        /* [in] */ Int32 newX);

    CARAPI SetFinalY(
        /* [in] */ Int32 newY);

    CARAPI IsScrollingInDirection(
        /* [in] */ Float xvel,
        /* [in] */ Float yvel,
        /* [out] */ Boolean* scrolling);

private:
    CARAPI_(Float) ComputeDeceleration(
        /* [in] */ Float velocity);

    CARAPI_(Double) GetSplineDeceleration(
        /* [in] */ Float velocity);

    CARAPI_(Int32) GetSplineFlingDuration(
        /* [in] */ Float velocity);

    CARAPI_(Double) GetSplineFlingDistance(
        /* [in] */ Float velocity);

private:
    static const Int32 DEFAULT_DURATION;
    static const Int32 SCROLL_MODE;
    static const Int32 FLING_MODE;

    static Float DECELERATION_RATE;
    static const Float INFLEXION; // Tension lines cross at (INFLEXION, 1)
    static const Float START_TENSION;
    static const Float END_TENSION;
    static const Float P1;// = START_TENSION * INFLEXION;
    static const Float P2;// = 1.0f - END_TENSION * (1.0f - INFLEXION);

    static const Int32 NB_SAMPLES;
    static AutoPtr<ArrayOf<Float> > SPLINE_POSITION;
    static AutoPtr<ArrayOf<Float> > SPLINE_TIME;

private:
    AutoPtr<IInterpolator> mInterpolator;
    Int32 mMode;

    Int32 mStartX;
    Int32 mStartY;
    Int32 mFinalX;
    Int32 mFinalY;

    Int32 mMinX;
    Int32 mMaxX;
    Int32 mMinY;
    Int32 mMaxY;

    Int32 mCurrX;
    Int32 mCurrY;
    Int64 mStartTime;
    Int32 mDuration;
    Float mDurationReciprocal;
    Float mDeltaX;
    Float mDeltaY;
    Boolean mFinished;

    Boolean mFlywheel;
    Float mVelocity;
    Float mCurrVelocity;
    Int32 mDistance;

    Float mFlingFriction;// = ViewConfiguration.getScrollFriction();
    Float mDeceleration;
    Float mPpi;

    AutoPtr<IPowerManager> mPm;

    Float mPhysicalCoeff;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_SCROLLER_H__

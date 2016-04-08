
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_RIPPLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_RIPPLE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CanvasProperty.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::View::IHardwareCanvas;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

/**
 * Draws a Material ripple.
 */
class Ripple
    : public Object
{
private:
    /**
    * Interpolator with a smooth log deceleration
    */
    class LogInterpolator
        : public Object
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* result);

        // @Override
        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);
    };

    class RippleAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        RippleAnimatorListenerAdapter(
            /* [in] */ Ripple* host);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        Ripple* mHost;
    };

public:
    /**
     * Creates a new ripple.
     */
    Ripple(
        /* [in] */ IRippleDrawable* owner,
        /* [in] */ IRect* bounds,
        /* [in] */ Float startingX,
        /* [in] */ Float startingY);

    CARAPI_(void) Setup(
        /* [in] */ Int32 maxRadius,
        /* [in] */ Int32 color,
        /* [in] */ Float density);

    CARAPI_(Boolean) IsHardwareAnimating();

    CARAPI_(void) OnHotspotBoundsChanged();

    CARAPI_(void) SetOpacity(
        /* [in] */ Float a);

    CARAPI_(Float) GetOpacity();

    // @SuppressWarnings("unused")
    CARAPI_(void) SetRadiusGravity(
        /* [in] */ Float r);

    // @SuppressWarnings("unused")
    CARAPI_(Float) GetRadiusGravity();

    // @SuppressWarnings("unused")
    CARAPI_(void) SetXGravity(
        /* [in] */ Float x);

    // @SuppressWarnings("unused")
    CARAPI_(Float) GetXGravity();

    // @SuppressWarnings("unused")
    CARAPI_(void) SetYGravity(
        /* [in] */ Float y);

    // @SuppressWarnings("unused")
    CARAPI_(Float) GetYGravity();

    /**
     * Draws the ripple centered at (0,0) using the specified paint.
     */
    CARAPI_(Boolean) Draw(
        /* [in] */ ICanvas* c,
        /* [in] */ IPaint* p);

    /**
     * Returns the maximum bounds of the ripple relative to the ripple center.
     */
    CARAPI_(void) GetBounds(
        /* [in] */ IRect* bounds);

    /**
     * Specifies the starting position relative to the drawable bounds. No-op if
     * the ripple has already entered.
     */
    CARAPI_(void) Move(
        /* [in] */ Float x,
        /* [in] */ Float y);

    /**
     * Starts the enter animation.
     */
    CARAPI_(void) Enter();

    /**
     * Starts the exit animation.
     */
    CARAPI_(void) Exit();

    /**
     * Jump all animations to their end state. The caller is responsible for
     * removing the ripple from the list of animating ripples.
     */
    CARAPI_(void) Jump();

    /**
     * Cancels all animations. The caller is responsible for removing
     * the ripple from the list of animating ripples.
     */
    CARAPI_(void) Cancel();

private:
    CARAPI_(void) ClampStartingPosition();

    CARAPI_(Boolean) DrawHardware(
        /* [in] */ IHardwareCanvas* c);

    CARAPI_(Boolean) DrawSoftware(
        /* [in] */ ICanvas* c,
        /* [in] */ IPaint* p);

    CARAPI_(void) ExitHardware(
        /* [in] */ Int32 radiusDuration,
        /* [in] */ Int32 opacityDuration);

    CARAPI_(void) EndSoftwareAnimations();

    CARAPI_(AutoPtr<IPaint>) GetTempPaint();

    CARAPI_(void) ExitSoftware(
        /* [in] */ Int32 radiusDuration,
        /* [in] */ Int32 opacityDuration);

    CARAPI_(void) CancelSoftwareAnimations();

    /**
     * Cancels any running hardware animations.
     */
    CARAPI_(void) CancelHardwareAnimations(
        /* [in] */ Boolean cancelPending);

    CARAPI_(void) RemoveSelf();

    CARAPI_(void) InvalidateSelf();

    static CARAPI_(AutoPtr<ITimeInterpolator>) Init_LINEAR_INTERPOLATOR();

private:
    static AutoPtr<ITimeInterpolator> LINEAR_INTERPOLATOR;
    static AutoPtr<ITimeInterpolator> DECEL_INTERPOLATOR;

    static const Float GLOBAL_SPEED;
    static const Float WAVE_TOUCH_DOWN_ACCELERATION;
    static const Float WAVE_TOUCH_UP_ACCELERATION;
    static const Float WAVE_OPACITY_DECAY_VELOCITY;

    static const Int64 RIPPLE_ENTER_DELAY;

    // Hardware animators.
    AutoPtr<IArrayList> mRunningAnimations;/* = new ArrayList<RenderNodeAnimator>()*/
    AutoPtr<IArrayList> mPendingAnimations;/* = new ArrayList<RenderNodeAnimator>()*/

    AutoPtr<IRippleDrawable> mOwner;

    /** Bounds used for computing max radius. */
    AutoPtr<IRect> mBounds;

    /** Full-opacity color for drawing this ripple. */
    Int32 mColorOpaque;

    /** Maximum ripple radius. */
    Float mOuterRadius;

    /** Screen density used to adjust pixel-based velocities. */
    Float mDensity;

    Float mStartingX;
    Float mStartingY;
    Float mClampedStartingX;
    Float mClampedStartingY;

    // Hardware rendering properties.
    AutoPtr<CanvasProperty> mPropPaint;  /*<Paint*/
    AutoPtr<CanvasProperty> mPropRadius;  /*<Float*/
    AutoPtr<CanvasProperty> mPropX;  /*<Float*/
    AutoPtr<CanvasProperty> mPropY;  /*<Float*/

    // Software animators.
    AutoPtr<IObjectAnimator> mAnimRadius;
    AutoPtr<IObjectAnimator> mAnimOpacity;
    AutoPtr<IObjectAnimator> mAnimX;
    AutoPtr<IObjectAnimator> mAnimY;

    // Temporary paint used for creating canvas properties.
    AutoPtr<IPaint> mTempPaint;

    // Software rendering properties.
    Float mOpacity;
    Float mOuterX;
    Float mOuterY;

    // Values used to tween between the start and end positions.
    Float mTweenRadius;
    Float mTweenX;
    Float mTweenY;

    /** Whether we should be drawing hardware animations. */
    Boolean mHardwareAnimating;

    /** Whether we can use hardware acceleration for the exit animation. */
    Boolean mCanUseHardware;

    /** Whether we have an explicit maximum radius. */
    Boolean mHasMaxRadius;

    /** Whether we were canceled externally and should avoid self-removal. */
    Boolean mCanceled;

    AutoPtr<AnimatorListenerAdapter> mAnimationListener;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Graphics::Drawable::Ripple, IInterface);

#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_RIPPLE_H__

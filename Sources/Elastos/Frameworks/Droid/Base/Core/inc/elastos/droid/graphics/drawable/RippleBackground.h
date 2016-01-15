
#ifndef  __ELASTOS_DROID_GRAPHICS_DRAWABLE_RIPPLEBACKGROUND_H__
#define  __ELASTOS_DROID_GRAPHICS_DRAWABLE_RIPPLEBACKGROUND_H__

#include "elastos/droid/graphics/CanvasProperty.h"
#include "elastos/droid/graphics/drawable/Ripple.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IHardwareCanvas;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class RippleDrawable;
/**
 * Draws a Material ripple.
 */
class RippleBackground
    : public Object
{
private:
    class RBAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        RBAnimatorListenerAdapter(
            /* [in] */ RippleBackground* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        RippleBackground* mHost;
    };

    class OuterOpacityAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        OuterOpacityAnimatorListenerAdapter(
            /* [in] */ RippleBackground* host,
            /* [in] */ Int32 duration);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

    private:
        RippleBackground* mHost;
        Int32 mDuration;
    };

public:
    /**
     * Creates a new ripple.
     */
    RippleBackground(
        /* [in] */ RippleDrawable* owner,
        /* [in] */ IRect* bounds);

    CARAPI_(void) Setup(
        /* [in] */ Int32 maxRadius,
        /* [in] */ Int32 color,
        /* [in] */ Float density);

    CARAPI_(Boolean) IsHardwareAnimating();

    CARAPI_(void) OnHotspotBoundsChanged();

    // @SuppressWarnings("unused")
    CARAPI_(void) SetOuterOpacity(
        /* [in] */ Float a);

    // @SuppressWarnings("unused")
    CARAPI_(Float) GetOuterOpacity();

    /**
     * Draws the ripple centered at (0,0) using the specified paint.
     */
    CARAPI_(Boolean) Draw(
        /* [in] */ ICanvas* c,
        /* [in] */ IPaint* p);

    CARAPI_(Boolean) ShouldDraw();

    /**
     * Returns the maximum bounds of the ripple relative to the ripple center.
     */
    CARAPI_(void) GetBounds(
        /* [in] */ IRect* bounds);

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
     * Cancel all animations. The caller is responsible for removing
     * the ripple from the list of animating ripples.
     */
    CARAPI_(void) Cancel();

private:
    static CARAPI_(Boolean) InitStatic();

    CARAPI_(Boolean) DrawHardware(
        /* [in] */ IHardwareCanvas* c);

    CARAPI_(Boolean) DrawSoftware(
        /* [in] */ ICanvas* c,
        /* [in] */ IPaint* p);

    CARAPI_(void) ExitHardware(
        /* [in] */ Int32 opacityDuration,
        /* [in] */ Int32 inflectionDuration,
        /* [in] */ Int32 inflectionOpacity);

    CARAPI_(void) EndSoftwareAnimations();

    CARAPI_(AutoPtr<IPaint>) GetTempPaint();

    CARAPI_(void) ExitSoftware(
        /* [in] */ Int32 opacityDuration,
        /* [in] */ Int32 inflectionDuration,
        /* [in] */ Int32 inflectionOpacity);

    CARAPI_(void) CancelSoftwareAnimations();

    /**
     * Cancels any running hardware animations.
     */
    CARAPI_(void) CancelHardwareAnimations(
        /* [in] */ Boolean cancelPending);

    CARAPI_(void) InvalidateSelf();


private:
    static Boolean sInit;
    static AutoPtr<ITimeInterpolator> LINEAR_INTERPOLATOR;

    static const Float GLOBAL_SPEED;
    static const Float WAVE_OPACITY_DECAY_VELOCITY;
    static const Float WAVE_OUTER_OPACITY_EXIT_VELOCITY_MAX;
    static const Float WAVE_OUTER_OPACITY_EXIT_VELOCITY_MIN;
    static const Float WAVE_OUTER_OPACITY_ENTER_VELOCITY;
    static const Float WAVE_OUTER_SIZE_INFLUENCE_MAX;
    static const Float WAVE_OUTER_SIZE_INFLUENCE_MIN;

    // Hardware animators.
    AutoPtr<IArrayList/*<RenderNodeAnimator*/> mRunningAnimations/* = new ArrayList<RenderNodeAnimator>()*/;
    AutoPtr<IArrayList/*<RenderNodeAnimator*/> mPendingAnimations/* = new ArrayList<RenderNodeAnimator>()*/;

    AutoPtr<RippleDrawable> mOwner;

    /** Bounds used for computing max radius. */
    AutoPtr<IRect> mBounds;

    /** Full-opacity color for drawing this ripple. */
    Int32 mColorOpaque;

    /** Maximum alpha value for drawing this ripple. */
    Int32 mColorAlpha;

    /** Maximum ripple radius. */
    Float mOuterRadius;

    /** Screen density used to adjust pixel-based velocities. */
    Float mDensity;

    // Hardware rendering properties.
    AutoPtr<CanvasProperty/*<Paint*/> mPropOuterPaint;
    AutoPtr<CanvasProperty/*<Float*/> mPropOuterRadius;
    AutoPtr<CanvasProperty/*<Float*/> mPropOuterX;
    AutoPtr<CanvasProperty/*<Float*/> mPropOuterY;

    // Software animators.
    AutoPtr<IObjectAnimator> mAnimOuterOpacity;

    // Temporary paint used for creating canvas properties.
    AutoPtr<IPaint> mTempPaint;

    // Software rendering properties.
    Float mOuterOpacity;
    Float mOuterX;
    Float mOuterY;

    /** Whether we should be drawing hardware animations. */
    Boolean mHardwareAnimating;

    /** Whether we can use hardware acceleration for the exit animation. */
    Boolean mCanUseHardware;

    /** Whether we have an explicit maximum radius. */
    Boolean mHasMaxRadius;

    AutoPtr<RBAnimatorListenerAdapter> mAnimationListener;
    friend class OuterOpacityAnimatorListenerAdapter;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_GRAPHICS_DRAWABLE_RIPPLEBACKGROUND_H__

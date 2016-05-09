
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CRIPPLEBACKGROUND_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CRIPPLEBACKGROUND_H__

#include "_Elastos_Droid_Graphics_Drawable_CRippleBackground.h"
#include "elastos/droid/graphics/CanvasProperty.h"
#include "elastos/droid/graphics/drawable/CRipple.h"
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

/**
 * Draws a Material ripple.
 */
CarClass(CRippleBackground)
    , public Object
    , public IRippleBackground
{
private:
    class RBAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("RippleBackground::RBAnimatorListenerAdapter")

        RBAnimatorListenerAdapter(
            /* [in] */ CRippleBackground* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CRippleBackground* mHost;
    };

    class OuterOpacityAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("RippleBackground::OuterOpacityAnimatorListenerAdapter")

        OuterOpacityAnimatorListenerAdapter(
            /* [in] */ CRippleBackground* host,
            /* [in] */ Int32 duration);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

    private:
        CRippleBackground* mHost;
        Int32 mDuration;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CRippleBackground();

    /**
     * Creates a new ripple.
     */
    CARAPI constructor(
        /* [in] */ IRippleDrawable* owner,
        /* [in] */ IRect* bounds);

    CARAPI Setup(
        /* [in] */ Int32 maxRadius,
        /* [in] */ Int32 color,
        /* [in] */ Float density);

    CARAPI IsHardwareAnimating(
        /* [out] */ Boolean* result);

    CARAPI OnHotspotBoundsChanged();

    // @SuppressWarnings("unused")
    CARAPI SetOuterOpacity(
        /* [in] */ Float a);

    // @SuppressWarnings("unused")
    CARAPI GetOuterOpacity(
        /* [out] */ Float* result);

    /**
     * Draws the ripple centered at (0,0) using the specified paint.
     */
    CARAPI Draw(
        /* [in] */ ICanvas* c,
        /* [in] */ IPaint* p,
        /* [out] */ Boolean* result);

    CARAPI ShouldDraw(
        /* [out] */ Boolean* result);

    /**
     * Returns the maximum bounds of the ripple relative to the ripple center.
     */
    CARAPI GetBounds(
        /* [in] */ IRect* bounds);

    /**
     * Starts the enter animation.
     */
    CARAPI Enter();

    /**
     * Starts the exit animation.
     */
    CARAPI Exit();

    /**
     * Jump all animations to their end state. The caller is responsible for
     * removing the ripple from the list of animating ripples.
     */
    CARAPI Jump();

    /**
     * Cancel all animations. The caller is responsible for removing
     * the ripple from the list of animating ripples.
     */
    CARAPI Cancel();

private:
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
    static AutoPtr<ITimeInterpolator> LINEAR_INTERPOLATOR;

    static const Float GLOBAL_SPEED;
    static const Float WAVE_OPACITY_DECAY_VELOCITY;
    static const Float WAVE_OUTER_OPACITY_EXIT_VELOCITY_MAX;
    static const Float WAVE_OUTER_OPACITY_EXIT_VELOCITY_MIN;
    static const Float WAVE_OUTER_OPACITY_ENTER_VELOCITY;
    static const Float WAVE_OUTER_SIZE_INFLUENCE_MAX;
    static const Float WAVE_OUTER_SIZE_INFLUENCE_MIN;

    // Hardware animators.
    AutoPtr<IArrayList> mRunningAnimations;/* = new ArrayList<RenderNodeAnimator>()*/
    AutoPtr<IArrayList> mPendingAnimations;/* = new ArrayList<RenderNodeAnimator>()*/

    AutoPtr<IWeakReference> mWeakHost; //IRippleDrawable

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
    AutoPtr<CanvasProperty> mPropOuterPaint;  /*<Paint*/
    AutoPtr<CanvasProperty> mPropOuterRadius;  /*<Float*/
    AutoPtr<CanvasProperty> mPropOuterX;  /*<Float*/
    AutoPtr<CanvasProperty> mPropOuterY;  /*<Float*/

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

#endif  // __ELASTOS_DROID_GRAPHICS_DRAWABLE_CRIPPLEBACKGROUND_H__

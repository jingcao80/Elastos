
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_ROTARYSELECTOR_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_ROTARYSELECTOR_H__

#include "Elastos.Droid.Os.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/VelocityTracker.h"
#include "elastos/droid/view/animation/AnimationUtils.h"

using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::View::View;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::VelocityTracker;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::Animation::AnimationUtils;

/**
 * Custom view that presents up to two items that are selectable by rotating a semi-circle from
 * left to right, or right to left.  Used by incoming call screen, and the lock screen when no
 * security pattern is set.
 */
namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class RotarySelector
    : public Elastos::Droid::View::View
    , public IRotarySelector
{
public:
    CAR_INTERFACE_DECL()

    RotarySelector();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Constructor used when this widget is created from a layout file.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Sets the left handle icon to a given resource.
     *
     * The resource should refer to a Drawable object, or use 0 to remove
     * the icon.
     *
     * @param resId the resource ID.
     */
    virtual CARAPI SetLeftHandleResource(
        /* [in] */ Int32 resId);

    /**
     * Sets the right handle icon to a given resource.
     *
     * The resource should refer to a Drawable object, or use 0 to remove
     * the icon.
     *
     * @param resId the resource ID.
     */
    virtual CARAPI SetRightHandleResource(
        /* [in] */ Int32 resId);

    /**
     * Handle touch screen events.
     *
     * @param event The motion event.
     * @return True if the event was handled, FALSE otherwise.
     */
    virtual CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Registers a callback to be invoked when the dial
     * is "triggered" by rotating it one way or the other.
     *
     * @param l the OnDialTriggerListener to attach to this view
     */
    virtual CARAPI SetOnDialTriggerListener(
        /* [in] */ IOnDialTriggerListener* l);

    using View::StartAnimation;

    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(AutoPtr<IBitmap>) GetBitmapFor(
        /* [in] */ Int32 resId);

    CARAPI_(Boolean) IsHoriz();

    /**
     * Assuming bitmap is a bounding box around a piece of an arc drawn by two concentric circles
     * (as the background drawable for the rotary widget is), and given an x coordinate along the
     * drawable, return the y coordinate of a point on the arc that is between the two concentric
     * circles.  The resulting y combined with the incoming x is a point along the circle in
     * between the two concentric circles.
     *
     * @param backgroundWidth The width of the asset (the bottom of the box surrounding the arc).
     * @param innerRadius The radius of the circle that intersects the drawable at the bottom two
     *        corders of the drawable (top two corners in terms of drawing coordinates).
     * @param outerRadius The radius of the circle who's top most point is the top center of the
     *        drawable (bottom center in terms of drawing coordinates).
     * @param x The distance along the x axis of the desired point.    @return The y coordinate, in drawing coordinates, that will place (x, y) along the circle
     *        in between the two concentric circles.
     */
    CARAPI_(Int32) GetYOnArc(
        /* [in] */ Int32 backgroundWidth,
        /* [in] */ Int32 innerRadius,
        /* [in] */ Int32 outerRadius,
        /* [in] */ Int32 x);

    CARAPI_(void) StartAnimation(
        /* [in] */ Int32 startX,
        /* [in] */ Int32 endX,
        /* [in] */ Int32 duration);

    CARAPI_(void) StartAnimationWithVelocity(
        /* [in] */ Int32 startX,
        /* [in] */ Int32 endX,
        /* [in] */ Int32 pixelsPerSecond);

    CARAPI_(void) UpdateAnimation();

    CARAPI_(void) Reset();

    /**
     * Triggers haptic feedback.
     */
    CARAPI Vibrate(
        /* [in] */ Int64 duration);

    /**
     * Draw the bitmap so that it's centered
     * on the point (x,y), then draws it using specified canvas.
     * TODO: is there already a utility method somewhere for this?
     */
    CARAPI_(void) DrawCentered(
        /* [in] */ IBitmap* d,
        /* [in] */ ICanvas* c,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    /**
     * Dispatches a trigger event to our listener.
     */
    CARAPI DispatchTriggerEvent(
        /* [in] */ Int32 whichHandle);

    /**
     * Sets the current grabbed state, and dispatches a grabbed state change
     * event to our listener.
     */
    CARAPI_(void) SetGrabbedState(
        /* [in] */ Int32 newState);

    CARAPI InternalInit(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

private:
    static const String TAG;
    static const Boolean DBG;
    static const Boolean VISUAL_DEBUG;

    // Listener for onDialTrigger() callbacks.
    AutoPtr<IOnDialTriggerListener> mOnDialTriggerListener;

    Float mDensity;

    // UI elements
    AutoPtr<IBitmap> mBackground;
    AutoPtr<IBitmap> mDimple;
    AutoPtr<IBitmap> mDimpleDim;

    AutoPtr<IBitmap> mLeftHandleIcon;
    AutoPtr<IBitmap> mRightHandleIcon;

    AutoPtr<IBitmap> mArrowShortLeftAndRight;
    AutoPtr<IBitmap> mArrowLongLeft;  // Long arrow starting on the left, pointing clockwise
    AutoPtr<IBitmap> mArrowLongRight;  // Long arrow starting on the right, pointing CCW

    // positions of the left and right handle
    Int32 mLeftHandleX;
    Int32 mRightHandleX;

    // current offset of rotary widget along the x axis
    Int32 mRotaryOffsetX;

    // state of the animation used to bring the handle back to its start position when
    // the user lets go before triggering an action
    Boolean mAnimating;
    Int64 mAnimationStartTime;
    Int64 mAnimationDuration;
    Int32 mAnimatingDeltaXStart;   // the animation will interpolate from this delta to zero
    Int32 mAnimatingDeltaXEnd;

    AutoPtr<IDecelerateInterpolator> mInterpolator;

    AutoPtr<IPaint> mPaint;

    // used to rotate the background and arrow assets depending on orientation
    AutoPtr<IMatrix> mBgMatrix;
    AutoPtr<IMatrix> mArrowMatrix;

    /**
     * If the user is currently dragging something.
     */
    Int32 mGrabbedState;

    /**
     * Whether the user has triggered something (e.g dragging the left handle all the way over to
     * the right).
     */
    Boolean mTriggered;

    // Vibration (haptic feedback)
    AutoPtr<IVibrator> mVibrator;
    static const Int64 VIBRATE_SHORT;  // msec
    static const Int64 VIBRATE_LONG;  // msec

    /**
     * The drawable for the arrows need to be scrunched this many dips towards the rotary bg below
     * it.
     */
    static const Int32 ARROW_SCRUNCH_DIP;

    /**
     * How far inset the left and right circles should be
     */
    static const Int32 EDGE_PADDING_DIP;

    /**
     * How far from the edge of the screen the user must drag to trigger the event.
     */
    static const Int32 EDGE_TRIGGER_DIP;

    /**
     * Dimensions of arc in background drawable.
     */
    static const Int32 OUTER_ROTARY_RADIUS_DIP;
    static const Int32 ROTARY_STROKE_WIDTH_DIP;
    static const Int32 SNAP_BACK_ANIMATION_DURATION_MILLIS;
    static const Int32 SPIN_ANIMATION_DURATION_MILLIS;

    Int32 mEdgeTriggerThresh;
    Int32 mDimpleWidth;
    Int32 mBackgroundWidth;
    Int32 mBackgroundHeight;
    Int32 mOuterRadius;
    Int32 mInnerRadius;
    Int32 mDimpleSpacing;

    AutoPtr<VelocityTracker> mVelocityTracker;
    Int32 mMinimumVelocity;
    Int32 mMaximumVelocity;

    /**
     * The number of dimples we are flinging when we do the "spin" animation.  Used to know when to
     * wrap the icons back around so they "rotate back" onto the screen.
     * @see #updateAnimation()
     */
    Int32 mDimplesOfFling;

    /**
     * Either {@link #HORIZONTAL} or {@link #VERTICAL}.
     */
    Int32 mOrientation;

    static AutoPtr<IAudioAttributes> VIBRATION_ATTRIBUTES;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_ROTARYSELECTOR_H__

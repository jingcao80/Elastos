
#ifndef __ELASTOS_DROID_WIDGET_ABSSEEKBAR_H__
#define __ELASTOS_DROID_WIDGET_ABSSEEKBAR_H__

#include "elastos/droid/widget/ProgressBar.h"

using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;

namespace Elastos {
namespace Droid {
namespace Widget {

class ECO_PUBLIC AbsSeekBar
    : public ProgressBar
    , public IAbsSeekBar
{
public:
    CAR_INTERFACE_DECL()

    AbsSeekBar();

    ~AbsSeekBar();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
     * Sets the thumb that will be drawn at the end of the progress meter within the SeekBar.
     * <p>
     * If the thumb is a valid drawable (i.e. not null), half its width will be
     * used as the new thumb offset (@see #setThumbOffset(Int32)).
     *
     * @param thumb Drawable representing the thumb
     */
    CARAPI SetThumb(
        /* [in] */ IDrawable* thumb);

    /**
     * Return the drawable used to represent the scroll thumb - the component that
     * the user can drag back and forth indicating the current value by its position.
     *
     * @return The current thumb drawable
     */
    CARAPI GetThumb(
        /* [out] */ IDrawable** thumb);

    /**
     * Applies a tint to the thumb drawable. Does not modify the current tint
     * mode, which is {@link PorterDuff.Mode#SRC_IN} by default.
     * <p>
     * Subsequent calls to {@link #setThumb(Drawable)} will automatically
     * mutate the drawable and apply the specified tint and tint mode using
     * {@link Drawable#setTintList(ColorStateList)}.
     *
     * @param tint the tint to apply, may be {@code null} to clear tint
     *
     * @attr ref android.R.styleable#SeekBar_thumbTint
     * @see #getThumbTintList()
     * @see Drawable#setTintList(ColorStateList)
     */
    CARAPI SetThumbTintList(
        /* [in] */ IColorStateList* tint);

    /**
     * Returns the tint applied to the thumb drawable, if specified.
     *
     * @return the tint applied to the thumb drawable
     * @attr ref android.R.styleable#SeekBar_thumbTint
     * @see #setThumbTintList(ColorStateList)
     */
    // @Nullable
    CARAPI GetThumbTintList(
        /* [out] */ IColorStateList** tint);

    /**
     * Specifies the blending mode used to apply the tint specified by
     * {@link #setThumbTintList(ColorStateList)}} to the thumb drawable. The
     * default mode is {@link PorterDuff.Mode#SRC_IN}.
     *
     * @param tintMode the blending mode used to apply the tint, may be
     *                 {@code null} to clear tint
     *
     * @attr ref android.R.styleable#SeekBar_thumbTintMode
     * @see #getThumbTintMode()
     * @see Drawable#setTintMode(PorterDuff.Mode)
     */
    CARAPI SetThumbTintMode(
        /* [in] */ PorterDuffMode tintMode);

    /**
     * Returns the blending mode used to apply the tint to the thumb drawable,
     * if specified.
     *
     * @return the blending mode used to apply the tint to the thumb drawable
     * @attr ref android.R.styleable#SeekBar_thumbTintMode
     * @see #setThumbTintMode(PorterDuff.Mode)
     */
    // @Nullable
    CARAPI GetThumbTintMode(
        /* [out] */ PorterDuffMode* tintMode);

    /**
     * @see #setThumbOffset(Int32)
     */
    CARAPI GetThumbOffset(
        /* [out] */ Int32* offset);

    /**
     * Sets the thumb offset that allows the thumb to extend out of the range of
     * the track.
     *
     * @param thumbOffset The offset amount in pixels.
     */
    CARAPI SetThumbOffset(
        /* [in] */ Int32 thumbOffset);

    /**
     * Specifies whether the track should be split by the thumb. When true,
     * the thumb's optical bounds will be clipped out of the track drawable,
     * then the thumb will be drawn into the resulting gap.
     *
     * @param splitTrack Whether the track should be split by the thumb
     */
    CARAPI SetSplitTrack(
        /* [in] */ Boolean splitTrack);

    /**
     * Returns whether the track should be split by the thumb.
     */
    CARAPI GetSplitTrack(
        /* [out] */ Boolean* track);


    /**
     * Sets the amount of progress changed via the arrow keys.
     *
     * @param increment The amount to increment or decrement when the user
     *            presses the arrow keys.
     */
    CARAPI SetKeyProgressIncrement(
        /* [in] */ Int32 increment);

    /**
     * Returns the amount of progress changed via the arrow keys.
     * <p>
     * By default, this will be a value that is derived from the max progress.
     *
     * @return The amount to increment or decrement when the user presses the
     *         arrow keys. This will be positive.
     */
    CARAPI GetKeyProgressIncrement(
        /* [out] */ Int32* increment);

    //@Override
    CARAPI SetMax(
        /* [in] */ Int32 max);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* res);

    //@Override
    CARAPI JumpDrawablesToCurrentState();

    //@Override
    CARAPI DrawableHotspotChanged(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI OnResolveDrawables(
        /* [in] */ Int32 layoutDirection);

    //@Override
    CARAPI SetProgress(
        /* [in] */ Int32 progress,
        /* [in] */ Boolean fromUser);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    //@Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    //@Override
    CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnRtlPropertiesChanged(
        /* [in] */ Int32 layoutDirection);

protected:
    //@Override
    CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

    //@Override
    CARAPI DrawableStateChanged();

    //@Override
    CARAPI_(void) OnProgressRefresh(
        /* [in] */ Float scale,
        /* [in] */ Boolean fromUser);

    //@Overide
    CARAPI_(void) OnAnimatePosition(
        /* [in] */ Float scale,
        /* [in] */ Boolean fromUser);

    //@Overide
    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    //@Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI_(void) DrawTrack(
        /* [in] */ ICanvas* canvas);

    /**
     * Draw the thumb.
     */
    CARAPI_(void) DrawThumb(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //java package access permission
    /**
     * This is called when the user has started touching this widget.
     */
    virtual CARAPI_(void) OnStartTrackingTouch();

    /**
     * This is called when the user either releases his touch or the touch is
     * canceled.
     */
    virtual CARAPI_(void) OnStopTrackingTouch();

    /**
     * Called when the user changes the seekbar's progress by using a key event.
     */
    virtual CARAPI_(void) OnKeyChange();

    virtual CARAPI_(Boolean) IsAnimationRunning();

    virtual CARAPI_(void) AnimateSetProgress(
        /* [in] */ Int32 progress);

    virtual CARAPI_(Int32) UpdateTouchProgress(
        /* [in] */ Int32 lastProgress,
        /* [in] */ Int32 newProgress);

private:
    CARAPI_(void) ApplyThumbTint();

    CARAPI_(void) updateThumbAndTrackPos(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI_(Float) GetScale();

    CARAPI_(void) SetThumbPos(
        /* [in] */ Float scale);

    /**
     * Updates the thumb drawable bounds.
     *
     * @param w Width of the view, including padding
     * @param thumb Drawable used for the thumb
     * @param scale Current progress between 0 and 1
     * @param offset Vertical offset for centering. If set to
     *            {@link Integer#MIN_VALUE}, the current offset will be used.
     */
    CARAPI_(void) SetThumbPos(
        /* [in] */ Int32 w,
        /* [in] */ IDrawable* thumb,
        /* [in] */ Float scale,
        /* [in] */ Int32 offset);

    CARAPI_(void) SetHotspot(
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) TrackTouchEvent(
        /* [in] */ IMotionEvent* event);

    /**
     * Tries to claim the user's drag motion, and requests disallowing any
     * ancestors from stealing events in the drag.
     */
    CARAPI_(void) AttemptClaimDrag();

private:
    AutoPtr<IRect> mTempRect;

    AutoPtr<IDrawable> mThumb;
    AutoPtr<IColorStateList> mThumbTintList;
    PorterDuffMode mThumbTintMode;
    Boolean mHasThumbTint;
    Boolean mHasThumbTintMode;

    Int32 mThumbOffset;
    Boolean mSplitTrack;

    /**
     * On key presses (right or left), the amount to increment/decrement the
     * progress.
     */
    Int32 mKeyProgressIncrement;
    AutoPtr<IObjectAnimator> mPositionAnimator;
    ECO_LOCAL static const Int32 PROGRESS_ANIMATION_DURATION;

    ECO_LOCAL static const Int32 NO_ALPHA;
    Float mDisabledAlpha;

    Int32 mScaledTouchSlop;
    Float mTouchDownX;
    Boolean mIsDragging;
protected:
    /**
     * On touch, this offset plus the scaled value from the position of the
     * touch will form the progress value. Usually 0.
     */
    Float mTouchProgressOffset;

    /**
     * Whether this is user seekable.
     */
    Boolean mIsUserSeekable;
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif

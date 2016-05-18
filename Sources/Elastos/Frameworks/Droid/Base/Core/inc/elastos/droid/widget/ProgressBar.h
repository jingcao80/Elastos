
#ifndef __ELASTOS_DROID_WIDGET_PROGRESSBAR_H__
#define __ELASTOS_DROID_WIDGET_PROGRESSBAR_H__

#include "elastos/droid/R.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/utility/Pools.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::R;
using Elastos::Droid::Graphics::Drawable::Shapes::IShape;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Utility::Pools;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Widget {

using Elastos::Droid::View::View;
using Elastos::Droid::View::Animation::IInterpolator;

class ECO_PUBLIC ProgressBar
    : public View
    , public IProgressBar
{
public:
    class ProgressBarSavedState
        : public View::BaseSavedState
        , public IProgressBarSavedState
    {
    public:
        CAR_INTERFACE_DECL()

        ProgressBarSavedState();

        ~ProgressBarSavedState();

        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

    public:
        Int32 mProgress;
        Int32 mSecondaryProgress;
    };

private:
    class RefreshProgressRunnable
        : public Runnable
    {
    public:
        CARAPI Run();

        RefreshProgressRunnable(
            /* [in] */ IWeakReference* host);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };// RefreshProgressRunnable

    /**
     * Command for sending an accessibility event.
     */
    class AccessibilityEventSender
        : public Runnable
    {
    public:
        CARAPI Run();

        AccessibilityEventSender(
            /* [in] */ IWeakReference* host);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };// AccessibilityEventSender

    class RefreshData
        : public Object
    {
    public:
        CARAPI Recycle();

        static CARAPI_(AutoPtr<RefreshData>) Obtain(
            /* [in] */ Int32 id,
            /* [in] */ Float progress,
            /* [in] */ Boolean fromUser,
            /* [in] */ Boolean animate);

    private:
        RefreshData();

    public:
        Int32 mId;
        Float mProgress;
        Boolean mFromUser;
        Boolean mAnimate;

    private:
        static const Int32 POOL_MAX;
        static AutoPtr<Pools::SynchronizedPool<RefreshData> > sPool;
    };// RefreshData

    class ProgressTintInfo : public Object
    {
    public:
        ProgressTintInfo();

    public:
        AutoPtr<IColorStateList> mIndeterminateTintList;
        PorterDuffMode mIndeterminateTintMode;
        Boolean mHasIndeterminateTint;
        Boolean mHasIndeterminateTintMode;

        AutoPtr<IColorStateList> mProgressTintList;
        PorterDuffMode mProgressTintMode;
        Boolean mHasProgressTint;
        Boolean mHasProgressTintMode;

        AutoPtr<IColorStateList> mProgressBackgroundTintList;
        PorterDuffMode mProgressBackgroundTintMode;
        Boolean mHasProgressBackgroundTint;
        Boolean mHasProgressBackgroundTintMode;

        AutoPtr<IColorStateList> mSecondaryProgressTintList;
        PorterDuffMode mSecondaryProgressTintMode;
        Boolean mHasSecondaryProgressTint;
        Boolean mHasSecondaryProgressTintMode;
    };

public:
    CAR_INTERFACE_DECL()

    ProgressBar();

    /**
     * Create a new progress bar with range 0...100 and initial progress of 0.
     * @param context the application environment
     */
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

    virtual CARAPI_(AutoPtr<IShape>) GetDrawableShape();

    /**
     * <p>Indicate whether this progress bar is in indeterminate mode.</p>
     *
     * @return TRUE if the progress bar is in indeterminate mode
     */
    //synchronized
    virtual CARAPI IsIndeterminate(
        /* [out] */ Boolean* result);

    /**
     * <p>Change the indeterminate mode for this progress bar. In indeterminate
     * mode, the progress is ignored and the progress bar shows an infinite
     * animation instead.</p>
     *
     * If this progress bar's style only supports indeterminate mode (such as the circular
     * progress bars), then this will be ignored.
     *
     * @param indeterminate TRUE to enable the indeterminate mode
     */
    //synchronized
    virtual CARAPI SetIndeterminate(
        /* [in] */ Boolean indeterminate);

    /**
     * <p>Get the drawable used to draw the progress bar in
     * indeterminate mode.</p>
     *
     * @return a {@link android.graphics.drawable.Drawable} instance
     *
     * @see #setIndeterminateDrawable(android.graphics.drawable.Drawable)
     * @see #setIndeterminate(Boolean)
     */
    virtual CARAPI GetIndeterminateDrawable(
        /* [out] */ IDrawable** drawable);

    /**
     * <p>Define the drawable used to draw the progress bar in
     * indeterminate mode.</p>
     *
     * @param d the new drawable
     *
     * @see #getIndeterminateDrawable()
     * @see #setIndeterminate(Boolean)
     */
    virtual CARAPI SetIndeterminateDrawable(
        /* [in] */ IDrawable* d);

    /**
     * Applies a tint to the indeterminate drawable. Does not modify the
     * current tint mode, which is {@link PorterDuff.Mode#SRC_IN} by default.
     * <p>
     * Subsequent calls to {@link #setIndeterminateDrawable(Drawable)} will
     * automatically mutate the drawable and apply the specified tint and
     * tint mode using
     * {@link Drawable#setTintList(ColorStateList)}.
     *
     * @param tint the tint to apply, may be {@code null} to clear tint
     *
     * @attr ref android.R.styleable#ProgressBar_indeterminateTint
     * @see #getIndeterminateTintList()
     * @see Drawable#setTintList(ColorStateList)
     */
    CARAPI SetIndeterminateTintList(
        /* [in] */ /*@Nullable*/ IColorStateList* tint);

    /**
     * @return the tint applied to the indeterminate drawable
     * @attr ref android.R.styleable#ProgressBar_indeterminateTint
     * @see #setIndeterminateTintList(ColorStateList)
     */
    // @Nullable
    CARAPI GetIndeterminateTintList(
        /* [out] */ IColorStateList** list);

    /**
     * Specifies the blending mode used to apply the tint specified by
     * {@link #setIndeterminateTintList(ColorStateList)} to the indeterminate
     * drawable. The default mode is {@link PorterDuff.Mode#SRC_IN}.
     *
     * @param tintMode the blending mode used to apply the tint, may be
     *                 {@code null} to clear tint
     * @attr ref android.R.styleable#ProgressBar_indeterminateTintMode
     * @see #setIndeterminateTintList(ColorStateList)
     * @see Drawable#setTintMode(PorterDuff.Mode)
     */
    CARAPI SetIndeterminateTintMode(
        /* [in] */ /*@Nullable*/ PorterDuffMode tintMode);

    /**
     * Returns the blending mode used to apply the tint to the indeterminate
     * drawable, if specified.
     *
     * @return the blending mode used to apply the tint to the indeterminate
     *         drawable
     * @attr ref android.R.styleable#ProgressBar_indeterminateTintMode
     * @see #setIndeterminateTintMode(PorterDuff.Mode)
     */
    // @Nullable
    CARAPI GetIndeterminateTintMode(
        /* [out] */ PorterDuffMode* mode);

    /**
     * Define the tileable drawable used to draw the progress bar in
     * indeterminate mode.
     * <p>
     * If the drawable is a BitmapDrawable or contains BitmapDrawables, a
     * tiled copy will be generated for display as a progress bar.
     *
     * @param d the new drawable
     * @see #getIndeterminateDrawable()
     * @see #setIndeterminate(boolean)
     */
    CARAPI SetIndeterminateDrawableTiled(
        /* [in] */ IDrawable* d);

    /**
     * <p>Get the drawable used to draw the progress bar in
     * progress mode.</p>
     *
     * @return a {@link android.graphics.drawable.Drawable} instance
     *
     * @see #setProgressDrawable(android.graphics.drawable.Drawable)
     * @see #setIndeterminate(Boolean)
     */
    virtual CARAPI GetProgressDrawable(
        /* [out] */ IDrawable** drawable);

    /**
     * <p>Define the drawable used to draw the progress bar in
     * progress mode.</p>
     *
     * @param d the new drawable
     *
     * @see #getProgressDrawable()
     * @see #setIndeterminate(Boolean)
     */
    virtual CARAPI SetProgressDrawable(
        /* [in] */ IDrawable* d);

    /**
     * Applies a tint to the progress indicator, if one exists, or to the
     * entire progress drawable otherwise. Does not modify the current tint
     * mode, which is {@link PorterDuff.Mode#SRC_IN} by default.
     * <p>
     * The progress indicator should be specified as a layer with
     * id {@link android.R.id#progress} in a {@link LayerDrawable}
     * used as the progress drawable.
     * <p>
     * Subsequent calls to {@link #setProgressDrawable(Drawable)} will
     * automatically mutate the drawable and apply the specified tint and
     * tint mode using
     * {@link Drawable#setTintList(ColorStateList)}.
     *
     * @param tint the tint to apply, may be {@code null} to clear tint
     *
     * @attr ref android.R.styleable#ProgressBar_progressTint
     * @see #getProgressTintList()
     * @see Drawable#setTintList(ColorStateList)
     */
    CARAPI SetProgressTintList(
        /* [in] */ /*@Nullable*/ IColorStateList* tint);

    /**
     * Returns the tint applied to the progress drawable, if specified.
     *
     * @return the tint applied to the progress drawable
     * @attr ref android.R.styleable#ProgressBar_progressTint
     * @see #setProgressTintList(ColorStateList)
     */
    // @Nullable
    CARAPI GetProgressTintList(
        /* [out] */ IColorStateList** list);

    /**
     * Specifies the blending mode used to apply the tint specified by
     * {@link #setProgressTintList(ColorStateList)}} to the progress
     * indicator. The default mode is {@link PorterDuff.Mode#SRC_IN}.
     *
     * @param tintMode the blending mode used to apply the tint, may be
     *                 {@code null} to clear tint
     * @attr ref android.R.styleable#ProgressBar_progressTintMode
     * @see #getProgressTintMode()
     * @see Drawable#setTintMode(PorterDuff.Mode)
     */
    CARAPI SetProgressTintMode(
        /* [in] */ /*@Nullable*/ PorterDuffMode tintMode);

    /**
     * Returns the blending mode used to apply the tint to the progress
     * drawable, if specified.
     *
     * @return the blending mode used to apply the tint to the progress
     *         drawable
     * @attr ref android.R.styleable#ProgressBar_progressTintMode
     * @see #setProgressTintMode(PorterDuff.Mode)
     */
    // @Nullable
    CARAPI GetProgressTintMode(
        /* [out] */ PorterDuffMode* mode);

    /**
     * Applies a tint to the progress background, if one exists. Does not
     * modify the current tint mode, which is
     * {@link PorterDuff.Mode#SRC_ATOP} by default.
     * <p>
     * The progress background must be specified as a layer with
     * id {@link android.R.id#background} in a {@link LayerDrawable}
     * used as the progress drawable.
     * <p>
     * Subsequent calls to {@link #setProgressDrawable(Drawable)} where the
     * drawable contains a progress background will automatically mutate the
     * drawable and apply the specified tint and tint mode using
     * {@link Drawable#setTintList(ColorStateList)}.
     *
     * @param tint the tint to apply, may be {@code null} to clear tint
     *
     * @attr ref android.R.styleable#ProgressBar_progressBackgroundTint
     * @see #getProgressBackgroundTintList()
     * @see Drawable#setTintList(ColorStateList)
     */
    CARAPI SetProgressBackgroundTintList(
        /* [in] */ /*@Nullable*/ IColorStateList* tint);

    /**
     * Returns the tint applied to the progress background, if specified.
     *
     * @return the tint applied to the progress background
     * @attr ref android.R.styleable#ProgressBar_progressBackgroundTint
     * @see #setProgressBackgroundTintList(ColorStateList)
     */
    // @Nullable
    CARAPI GetProgressBackgroundTintList(
        /* [out] */ IColorStateList** list);

    /**
     * Specifies the blending mode used to apply the tint specified by
     * {@link #setProgressBackgroundTintList(ColorStateList)}} to the progress
     * background. The default mode is {@link PorterDuff.Mode#SRC_IN}.
     *
     * @param tintMode the blending mode used to apply the tint, may be
     *                 {@code null} to clear tint
     * @attr ref android.R.styleable#ProgressBar_progressBackgroundTintMode
     * @see #setProgressBackgroundTintList(ColorStateList)
     * @see Drawable#setTintMode(PorterDuff.Mode)
     */
    CARAPI SetProgressBackgroundTintMode(
        /* [in] */ /*@Nullable*/ PorterDuffMode tintMode);

    /**
     * @return the blending mode used to apply the tint to the progress
     *         background
     * @attr ref android.R.styleable#ProgressBar_progressBackgroundTintMode
     * @see #setProgressBackgroundTintMode(PorterDuff.Mode)
     */
    // @Nullable
    CARAPI GetProgressBackgroundTintMode(
        /* [out] */ PorterDuffMode* mode);

    /**
     * Applies a tint to the secondary progress indicator, if one exists.
     * Does not modify the current tint mode, which is
     * {@link PorterDuff.Mode#SRC_ATOP} by default.
     * <p>
     * The secondary progress indicator must be specified as a layer with
     * id {@link android.R.id#secondaryProgress} in a {@link LayerDrawable}
     * used as the progress drawable.
     * <p>
     * Subsequent calls to {@link #setProgressDrawable(Drawable)} where the
     * drawable contains a secondary progress indicator will automatically
     * mutate the drawable and apply the specified tint and tint mode using
     * {@link Drawable#setTintList(ColorStateList)}.
     *
     * @param tint the tint to apply, may be {@code null} to clear tint
     *
     * @attr ref android.R.styleable#ProgressBar_secondaryProgressTint
     * @see #getSecondaryProgressTintList()
     * @see Drawable#setTintList(ColorStateList)
     */
    CARAPI SetSecondaryProgressTintList(
        /* [in] */ /*@Nullable*/ IColorStateList* tint);

    /**
     * Returns the tint applied to the secondary progress drawable, if
     * specified.
     *
     * @return the tint applied to the secondary progress drawable
     * @attr ref android.R.styleable#ProgressBar_secondaryProgressTint
     * @see #setSecondaryProgressTintList(ColorStateList)
     */
    // @Nullable
    CARAPI GetSecondaryProgressTintList(
        /* [out] */ IColorStateList** list);

    /**
     * Specifies the blending mode used to apply the tint specified by
     * {@link #setSecondaryProgressTintList(ColorStateList)}} to the secondary
     * progress indicator. The default mode is
     * {@link PorterDuff.Mode#SRC_ATOP}.
     *
     * @param tintMode the blending mode used to apply the tint, may be
     *                 {@code null} to clear tint
     * @attr ref android.R.styleable#ProgressBar_secondaryProgressTintMode
     * @see #setSecondaryProgressTintList(ColorStateList)
     * @see Drawable#setTintMode(PorterDuff.Mode)
     */
    CARAPI SetSecondaryProgressTintMode(
        /* [in] */ /*@Nullable*/ PorterDuffMode tintMode);

    /**
     * Returns the blending mode used to apply the tint to the secondary
     * progress drawable, if specified.
     *
     * @return the blending mode used to apply the tint to the secondary
     *         progress drawable
     * @attr ref android.R.styleable#ProgressBar_secondaryProgressTintMode
     * @see #setSecondaryProgressTintMode(PorterDuff.Mode)
     */
    // @Nullable
    CARAPI GetSecondaryProgressTintMode(
        /* [out] */ PorterDuffMode* mode);

    /**
     * Define the tileable drawable used to draw the progress bar in
     * progress mode.
     * <p>
     * If the drawable is a BitmapDrawable or contains BitmapDrawables, a
     * tiled copy will be generated for display as a progress bar.
     *
     * @param d the new drawable
     * @see #getProgressDrawable()
     * @see #setIndeterminate(boolean)
     */
    CARAPI SetProgressDrawableTiled(
        /* [in] */ IDrawable* d);

    /**
     * @return The drawable currently used to draw the progress bar
     */
    virtual CARAPI_(AutoPtr<IDrawable>) GetCurrentDrawable();

    using View::PostInvalidate;

    //@Override
    CARAPI PostInvalidate();

    virtual CARAPI_(void) OnProgressRefresh(
        /* [in] */ Float scale,
        /* [in] */ Boolean fromUser);

    /**
     * <p>Set the current progress to the specified value. Does not do anything
     * if the progress bar is in indeterminate mode.</p>
     *
     * @param progress the new progress, between 0 and {@link #getMax()}
     *
     * @see #setIndeterminate(Boolean)
     * @see #isIndeterminate()
     * @see #getProgress()
     * @see #incrementProgressBy(Int32)
     */
    //synchronized
    virtual CARAPI SetProgress(
        /* [in] */ Int32 progress);

    //synchronized
    virtual CARAPI SetProgress(
        /* [in] */ Int32 progress,
        /* [in] */ Boolean fromUser);

    /**
     * <p>
     * Set the current secondary progress to the specified value. Does not do
     * anything if the progress bar is in indeterminate mode.
     * </p>
     *
     * @param secondaryProgress the new secondary progress, between 0 and {@link #getMax()}
     * @see #setIndeterminate(Boolean)
     * @see #isIndeterminate()
     * @see #getSecondaryProgress()
     * @see #incrementSecondaryProgressBy(Int32)
     */
    //synchronized
    virtual CARAPI SetSecondaryProgress(
        /* [in] */ Int32 secondaryProgress);

    /**
     * <p>Get the progress bar's current level of progress. Return 0 when the
     * progress bar is in indeterminate mode.</p>
     *
     * @return the current progress, between 0 and {@link #getMax()}
     *
     * @see #setIndeterminate(Boolean)
     * @see #isIndeterminate()
     * @see #setProgress(Int32)
     * @see #setMax(Int32)
     * @see #getMax()
     */
    //synchronized
    virtual CARAPI GetProgress(
        /* [out] */ Int32* progress);

    /**
     * <p>Get the progress bar's current level of secondary progress. Return 0 when the
     * progress bar is in indeterminate mode.</p>
     *
     * @return the current secondary progress, between 0 and {@link #getMax()}
     *
     * @see #setIndeterminate(Boolean)
     * @see #isIndeterminate()
     * @see #setSecondaryProgress(Int32)
     * @see #setMax(Int32)
     * @see #getMax()
     */
    //synchronized
    virtual CARAPI GetSecondaryProgress(
        /* [out] */ Int32* progress);

    /**
     * <p>Return the upper limit of this progress bar's range.</p>
     *
     * @return a positive integer
     *
     * @see #setMax(Int32)
     * @see #getProgress()
     * @see #getSecondaryProgress()
     */
    //synchronized
    virtual CARAPI GetMax(
        /* [out] */ Int32* max);

    /**
     * <p>Set the range of the progress bar to 0...<tt>max</tt>.</p>
     *
     * @param max the upper range of this progress bar
     *
     * @see #getMax()
     * @see #setProgress(Int32)
     * @see #setSecondaryProgress(Int32)
     */
    //synchronized
    virtual CARAPI SetMax(
        /* [in] */ Int32 max);

    /**
     * <p>Increase the progress bar's progress by the specified amount.</p>
     *
     * @param diff the amount by which the progress must be increased
     *
     * @see #setProgress(Int32)
     */
    //synchronized
    CARAPI IncrementProgressBy(
        /* [in] */ Int32 diff);

    /**
     * <p>Increase the progress bar's secondary progress by the specified amount.</p>
     *
     * @param diff the amount by which the secondary progress must be increased
     *
     * @see #setSecondaryProgress(Int32)
     */
    //synchronized
    CARAPI IncrementSecondaryProgressBy(
        /* [in] */ Int32 diff);

    using View::StartAnimation;

    /**
     * <p>Start the indeterminate progress animation.</p>
     */
    virtual CARAPI_(void) StartAnimation();

    /**
     * <p>Stop the indeterminate progress animation.</p>
     */
    virtual CARAPI_(void) StopAnimation();

    /**
     * Sets the acceleration curve for the indeterminate animation.
     * The interpolator is loaded as a resource from the specified context.
     *
     * @param context The application environment
     * @param resID The resource identifier of the interpolator to load
     */
    virtual CARAPI SetInterpolator(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resID);

    /**
     * Sets the acceleration curve for the indeterminate animation.
     * Defaults to a linear interpolation.
     *
     * @param interpolator The interpolator which defines the acceleration curve
     */
    virtual CARAPI SetInterpolator(
        /* [in] */ IInterpolator* interpolator);

    /**
     * Gets the acceleration curve type for the indeterminate animation.
     *
     * @return the {@link Interpolator} associated to this animation
     */
    virtual CARAPI GetInterpolator(
        /* [out] */ IInterpolator** interpolator);

    virtual CARAPI SetVisibility(
        /* [in] */ Int32 v);

    //@Override
    virtual CARAPI InvalidateDrawable(
            /* [in] */ IDrawable* dr);

    //@Override
    virtual CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    //@Override
    virtual CARAPI_(void) OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    CARAPI JumpDrawablesToCurrentState();

    CARAPI OnResolveDrawables(
        /* [in] */ Int32 layoutDirection);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    // @Override
    CARAPI DrawableHotspotChanged(
        /* [in] */ Float x,
        /* [in] */ Float y);

protected:
    //@Override
    virtual CARAPI_(Boolean) VerifyDrawable(
            /* [in] */ IDrawable* who);

    //@Override
    virtual CARAPI OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

    //@Override
    virtual CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    //@Override
    //synchronized
    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    /**
     * Draws the progress bar track.
     */
    virtual CARAPI_(void) DrawTrack(
        /* [in] */ ICanvas* canvas);

    //@Override
    //synchronized
    virtual CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    virtual CARAPI DrawableStateChanged();

    //@Override
    virtual CARAPI OnAttachedToWindow();

    //@Override
    virtual CARAPI OnDetachedFromWindow();

    /**
     * Called when a ProgressBar is animating its position.
     *
     * @param scale Current position/progress between 0 and 1.
     * @param fromUser True if the progress change was initiated by the user.
     */
    virtual CARAPI_(void) OnAnimatePosition(
        /* [in] */ Float scale,
        /* [in] */ Boolean fromUser);

    /**
     * Sets the progress value without going through the entire refresh process.
     *
     * @see #setProgress(int, boolean)
     * @param progress The new progress, between 0 and {@link #getMax()}
     */
    virtual CARAPI_(void) SetProgressValueOnly(
        /* [in] */ Int32 progress);

    virtual CARAPI_(void) SetAnimationPosition(
        /* [in] */ Float position);

    virtual CARAPI_(Float) GetAnimationPosition();

private:
    CARAPI InitFromAttributes(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
     * Converts a drawable to a tiled version of itself. It will recursively
     * traverse layer and state list drawables.
     */
    CARAPI_(AutoPtr<IDrawable>) Tileify(
        /* [in] */ IDrawable* drawable,
        /* [in] */ Boolean clip);

    /**
     * Convert a AnimationDrawable for use as a barberpole animation.
     * Each frame of the animation is wrapped in a ClipDrawable and
     * given a tiling BitmapShader.
     */
    CARAPI_(AutoPtr<IDrawable>) TileifyIndeterminate(
        /* [in] */ IDrawable* drawable);

    /**
     * <p>
     * Initialize the progress bar's default values:
     * </p>
     * <ul>
     * <li>progress = 0</li>
     * <li>max = 100</li>
     * <li>animation duration = 4000 ms</li>
     * <li>indeterminate = FALSE</li>
     * <li>behavior = repeat</li>
     * </ul>
     */
    CARAPI_(void) InitProgressBar();

    //synchronized
    CARAPI_(void) RefreshProgress(
        /* [in] */ Int32 id,
        /* [in] */ Float progress,
        /* [in] */ Boolean fromUser);

    //synchronized
    CARAPI_(void) RefreshProgress(
        /* [in] */ Int32 id,
        /* [in] */ Float progress,
        /* [in] */ Boolean fromUser,
        /* [in] */ Boolean animate);

    CARAPI_(void) UpdateDrawableBounds(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI_(void) UpdateDrawableState();

    CARAPI_(void) ScheduleAccessibilityEventSender();

    CARAPI_(void) ApplyIndeterminateTint();

    /**
     * Applies the progress tints in order of increasing specificity.
     */
    CARAPI_(void) ApplyProgressTints();

    /**
     * Should only be called if we've already verified that mProgressDrawable
     * and mProgressTintInfo are non-null.
     */
    CARAPI_(void) ApplyPrimaryProgressTint();

    /**
     * Should only be called if we've already verified that mProgressDrawable
     * and mProgressTintInfo are non-null.
     */
    CARAPI_(void) ApplyProgressBackgroundTint();

    /**
     * Should only be called if we've already verified that mProgressDrawable
     * and mProgressTintInfo are non-null.
     */
    CARAPI_(void) ApplySecondaryProgressTint();

    /**
     * Returns the drawable to which a tint or tint mode should be applied.
     *
     * @param layerId id of the layer to modify
     * @param shouldFallback whether the base drawable should be returned
     *                       if the id does not exist
     * @return the drawable to modify
     */
    // @Nullable
    CARAPI_(AutoPtr<IDrawable>) GetTintTarget(
        /* [in] */ Int32 layerId,
        /* [in] */ Boolean shouldFallback);

    CARAPI_(void) SetDrawableTint(
        /* [in] */ Int32 id,
        /* [in] */ IColorStateList* tint,
        /* [in] */ PorterDuffMode tintMode,
        /* [in] */ Boolean fallback);

    CARAPI_(Float) GetScale(
        /* [in] */ Float progress);

    //synchronized
    CARAPI_(void) DoRefreshProgress(
        /* [in] */ Int32 id,
        /* [in] */ Float progress,
        /* [in] */ Boolean fromUser,
        /* [in] */ Boolean callBackToApp);

    //synchronized
    CARAPI_(void) DoRefreshProgress(
        /* [in] */ Int32 id,
        /* [in] */ Float progress,
        /* [in] */ Boolean fromUser,
        /* [in] */ Boolean callBackToApp,
        /* [in] */ Boolean animate);

private:
    ECO_LOCAL static const Int32 MAX_LEVEL = 10000;
    ECO_LOCAL static const Int32 TIMEOUT_SEND_ACCESSIBILITY_EVENT = 200;

protected:
    Int32 mMinWidth;
    Int32 mMaxWidth;
    Int32 mMinHeight;
    Int32 mMaxHeight;
    AutoPtr<IBitmap> mSampleTile;
    Boolean mMirrorForRtl;

private:
    Int32 mProgress;
    Int32 mSecondaryProgress;
    Int32 mMax;

    Int32 mBehavior;
    Int32 mDuration;
    Boolean mIndeterminate;
    Boolean mOnlyIndeterminate;
    AutoPtr<ITransformation> mTransformation;
    AutoPtr<IAnimation> mAnimation; //AlphaAnimation
    Boolean mHasAnimation;
    AutoPtr<IDrawable> mIndeterminateDrawable;
    AutoPtr<IDrawable> mProgressDrawable;
    AutoPtr<IDrawable> mCurrentDrawable;
    AutoPtr<ProgressTintInfo> mProgressTintInfo;

    Boolean mNoInvalidate;
    AutoPtr<IInterpolator> mInterpolator;
    AutoPtr<RefreshProgressRunnable> mRefreshProgressRunnable;
    Int64 mUiThreadId;
    Boolean mShouldStartAnimationDrawable;
    Float mAnimationPosition;

    Boolean mInDrawing;

    Boolean mAttached;
    Boolean mRefreshIsPosted;

    List<AutoPtr<RefreshData> > mRefreshData;

    AutoPtr<AccessibilityEventSender> mAccessibilityEventSender;
protected:
    Object mLock;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Widget::ProgressBar::RefreshData, IInterface)

#endif //__ELASTOS_DROID_WIDGET_PROGRESSBAR_H__

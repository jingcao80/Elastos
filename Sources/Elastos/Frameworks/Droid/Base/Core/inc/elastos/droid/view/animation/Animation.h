//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_VIEW_ANIMATION_ANIMATION_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_ANIMATION_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Core::ICloneable;
using Elastos::Core::ICloseGuard;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

class ECO_PUBLIC Animation
    : public Object
    , public IAnimation
    , public ICloneable
{
    friend class AnimationSet;
public:
    class SetListenerHandlerRunable
        : public Runnable
    {
    public:
        SetListenerHandlerRunable(
            /* [in] */ Animation* host,
            /* [in] */ Int32 type);

        ~SetListenerHandlerRunable();

        CARAPI Run();

    private:
        Animation* mHost;
        const Int32 mType;

    public:
        static const Int32 ONSTART_TYPE;
        static const Int32 ONREPEAT_TYPE;
        static const Int32 ONEND_TYPE;
    };

    /**
     * Utility class to parse a string description of a size.
     */
    class Description
        : public Object
    {
    public:
        Description();

        ~Description();

        CARAPI constructor();

        /**
         * Size descriptions can appear inthree forms:
         * <ol>
         * <li>An absolute size. This is represented by a number.</li>
         * <li>A size relative to the size of the object being animated. This
         * is represented by a number followed by "%".</li> *
         * <li>A size relative to the size of the parent of object being
         * animated. This is represented by a number followed by "%p".</li>
         * </ol>
         * @param value The typed value to parse
         * @return The parsed version of the description
         */
        static CARAPI_(AutoPtr<Description>) ParseValue(
            /* [in] */ ITypedValue* value);

    public:
        /**
        * One of Animation.ABSOLUTE, Animation.RELATIVE_TO_SELF, or
        * Animation.RELATIVE_TO_PARENT.
        */
        Int32 mType;

        /**
        * The absolute or relative dimension for this Description.
        */
        Float mValue;
    };

public:
    CAR_INTERFACE_DECL()

    Animation();

    ~Animation();

    CARAPI constructor();

    /**
     * Creates a new animation whose parameters come from the specified context and
     * attributes Set.
     *
     * @param context the application environment
     * @param attrs the Set of attributes holding the animation parameters
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Reset the initialization state of this animation.
     *
     * @see #initialize(Int32, Int32, Int32, Int32)
     */
    CARAPI Reset();

    /**
     * Cancel the animation. Cancelling an animation invokes the animation
     * listener, if Set, to notify the end of the animation.
     *
     * If you cancel an animation manually, you must call {@link #reset()}
     * before starting the animation again.
     *
     * @see #reset()
     * @see #start()
     * @see #startNow()
     */
    CARAPI Cancel();

    /**
     * @hide
     */
    CARAPI Detach();

    /**
     * Whether or not the animation has been initialized.
     *
     * @return Has this animation been initialized.
     * @see #initialize(Int32, Int32, Int32, Int32)
     */
    CARAPI IsInitialized(
        /* [out] */ Boolean* init);

    /**
     * Initialize this animation with the dimensions of the object being
     * animated as well as the objects parents. (This is to support animation
     * sizes being specified relative to these dimensions.)
     *
     * <p>Objects that interpret Animations should call this method when
     * the sizes of the object being animated and its parent are known, and
     * before calling {@link #getTransformation}.
     *
     *
     * @param width Width of the object being animated
     * @param height Height of the object being animated
     * @param parentWidth Width of the animated object's parent
     * @param parentHeight Height of the animated object's parent
     */
    CARAPI Initialize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 parentWidth,
        /* [in] */ Int32 parentHeight);

    /**
     * Sets the handler used to invoke listeners.
     *
     * @hide
     */
    CARAPI SetListenerHandler(
        /* [in] */ IHandler* handler);

    /**
     * Sets the acceleration curve for this animation. The interpolator is loaded as
     * a resource from the specified context.
     *
     * @param context The application environment
     * @param resID The resource identifier of the interpolator to load
     * @attr ref android.R.styleable#IAnimation::interpolator
     */
    CARAPI SetInterpolator(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resID);

    /**
     * Sets the acceleration curve for this animation. Defaults to a linear
     * interpolation.
     *
     * @param i The interpolator which defines the acceleration curve
     * @attr ref android.R.styleable#IAnimation::interpolator
     */
    CARAPI SetInterpolator(
        /* [in] */ IInterpolator* i);

    /**
     * When this animation should start relative to the start time. This is most
     * useful when composing complex animations using an {@link AnimationSet }
     * where some of the animations components start at different times.
     *
     * @param startOffset When this Animation should start, in milliseconds from
     *                    the start time of the root AnimationSet.
     * @attr ref android.R.styleable#IAnimation::startOffset
     */
    CARAPI SetStartOffset(
        /* [in] */ Int64 startOffset);

    /**
     * How Int64 this animation should last. The duration cannot be negative.
     *
     * @param durationMillis Duration in milliseconds
     *
     * @throws java.lang.IllegalArgumentException if the duration is < 0
     *
     * @attr ref android.R.styleable#IAnimation::duration
     */
    CARAPI SetDuration(
        /* [in] */ Int64 durationMillis);

    /**
     * Ensure that the duration that this animation will run is not longer
     * than <var>durationMillis</var>.  In addition to adjusting the duration
     * itself, this ensures that the repeat count also will not make it run
     * longer than the given time.
     *
     * @param durationMillis The maximum duration the animation is allowed
     * to run.
     */
    CARAPI RestrictDuration(
        /* [in] */ Int64 durationMillis);

    /**
     * How much to scale the duration by.
     *
     * @param scale The amount to scale the duration.
     */
    CARAPI ScaleCurrentDuration(
        /* [in] */ Float scale);

    /**
     * When this animation should start. When the start time is Set to
     * {@link #START_ON_FIRST_FRAME}, the animation will start the first time
     * {@link #getTransformation(Int64, Transformation)} is invoked. The time passed
     * to this method should be obtained by calling
     * {@link AnimationUtils#currentAnimationTimeMillis()} instead of
     * {@link System#currentTimeMillis()}.
     *
     * @param startTimeMillis the start time in milliseconds
     */
    CARAPI SetStartTime(
        /* [in] */ Int64 startTimeMillis);

    /**
     * Convenience method to start the animation the first time
     * {@link #getTransformation(Int64, Transformation)} is invoked.
     */
    CARAPI Start();

    /**
     * Convenience method to start the animation at the current time in
     * milliseconds.
     */
    CARAPI StartNow();

    /**
     * Defines what this animation should do when it reaches the end. This
     * Setting is applied only when the repeat count is either greater than
     * 0 or {@link #INFINITE}. Defaults to {@link #IAnimation::RESTART}.
     *
     * @param repeatMode {@link #IAnimation::RESTART} or {@link #REVERSE}
     * @attr ref android.R.styleable#IAnimation::repeatMode
     */
    CARAPI SetRepeatMode(
        /* [in] */ Int32 repeatMode);

    /**
     * Sets how many times the animation should be repeated. If the repeat
     * count is 0, the animation is never repeated. If the repeat count is
     * greater than 0 or {@link #INFINITE}, the repeat mode will be taken
     * into account. The repeat count is 0 by default.
     *
     * @param repeatCount the number of times the animation should be repeated
     * @attr ref android.R.styleable#IAnimation::repeatCount
     */
    CARAPI SetRepeatCount(
        /* [in] */ Int32 repeatCount);

    /**
     * If fillEnabled is true, this animation will apply the value of fillBefore.
     *
     * @return true if the animation will take fillBefore into account
     * @attr ref android.R.styleable#IAnimation::fillEnabled
     */
    CARAPI IsFillEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * If fillEnabled is true, the animation will apply the value of fillBefore.
     * Otherwise, fillBefore is ignored and the animation
     * transformation is always applied until the animation ends.
     *
     * @param fillEnabled true if the animation should take the value of fillBefore into account
     * @attr ref android.R.styleable#IAnimation::fillEnabled
     *
     * @see #setFillBefore(boolean)
     * @see #setFillAfter(boolean)
     */
    CARAPI SetFillEnabled(
        /* [in] */ Boolean fillEnabled);

    /**
     * If fillBefore is true, this animation will apply its transformation
     * before the start time of the animation. Defaults to true if
     * {@link #setFillEnabled(boolean)} is not set to true.
     * Note that this applies when using an {@link
     * android.view.animation.AnimationSet AnimationSet} to chain
     * animations. The transformation is not applied before the AnimationSet
     * itself starts.
     *
     * @param fillBefore true if the animation should apply its transformation before it starts
     * @attr ref android.R.styleable#IAnimation::fillBefore
     *
     * @see #setFillEnabled(boolean)
     */
    CARAPI SetFillBefore(
        /* [in] */ Boolean fillBefore);

    /**
     * If fillAfter is true, the transformation that this animation performed
     * will persist when it is finished. Defaults to false if not set.
     * Note that this applies to individual animations and when using an {@link
     * android.view.animation.AnimationSet AnimationSet} to chain
     * animations.
     *
     * @param fillAfter true if the animation should apply its transformation after it ends
     * @attr ref android.R.styleable#IAnimation::fillAfter
     *
     * @see #setFillEnabled(boolean)
     */
    CARAPI SetFillAfter(
        /* [in] */ Boolean fillAfter);

    /**
     * Set the Z ordering mode to use while running the animation.
     *
     * @param zAdjustment The desired mode, one of {@link #ZORDER_NORMAL},
     * {@link #ZORDER_TOP}, or {@link #ZORDER_BOTTOM}.
     * @attr ref android.R.styleable#IAnimation::zAdjustment
     */
    CARAPI SetZAdjustment(
        /* [in] */ Int32 zAdjustment);

    /**
     * Set background behind animation.
     *
     * @param bg The background color.  If 0, no background.  Currently must
     * be black, with any desired alpha level.
     */
    CARAPI SetBackgroundColor(
        /* [in] */ Int32 bg);

    CARAPI GetBackgroundColor(
        /* [out] */ Int32* bg);

    /**
     * If detachWallpaper is TRUE, and this is a window animation of a window
     * that has a wallpaper background, then the window will be detached from
     * the wallpaper while it runs.  That is, the animation will only be applied
     * to the window, and the wallpaper behind it will remain static.
     *
     * @param detachWallpaper TRUE if the wallpaper should be detached from the animation
     * @attr ref android.R.styleable#IAnimation::detachWallpaper
     */
    CARAPI SetDetachWallpaper(
        /* [in] */ Boolean detachWallpaper);

    /**
     * Gets the acceleration curve type for this animation.
     *
     * @return the {@link Interpolator} associated to this animation
     * @attr ref android.R.styleable#IAnimation::interpolator
     */
    CARAPI GetInterpolator(
        /* [out] */ IInterpolator** interpolator);

    /**
     * When this animation should start. If the animation has not startet yet,
     * this method might return {@link #START_ON_FIRST_FRAME}.
     *
     * @return the time in milliseconds when the animation should start or
     *         {@link #START_ON_FIRST_FRAME}
     */
    CARAPI GetStartTime(
        /* [out] */ Int64* time);

    /**
     * How Int64 this animation should last
     *
     * @return the duration in milliseconds of the animation
     * @attr ref android.R.styleable#IAnimation::duration
     */
    CARAPI GetDuration(
        /* [out] */ Int64* duration);

    /**
     * When this animation should start, relative to StartTime
     *
     * @return the start offset in milliseconds
     * @attr ref android.R.styleable#IAnimation::startOffset
     */
    CARAPI GetStartOffset(
        /* [out] */ Int64* startOffset);

    /**
     * Defines what this animation should do when it reaches the end.
     *
     * @return either one of {@link #REVERSE} or {@link #IAnimation::RESTART}
     * @attr ref android.R.styleable#IAnimation::repeatMode
     */
    CARAPI GetRepeatMode(
        /* [out] */ Int32* mode);

    /**
     * Defines how many times the animation should repeat. The default value
     * is 0.
     *
     * @return the number of times the animation should repeat, or {@link #INFINITE}
     * @attr ref android.R.styleable#IAnimation::repeatCount
     */
    CARAPI GetRepeatCount(
        /* [out] */ Int32* count);

    /**
     * If fillBefore is true, this animation will apply its transformation
     * before the start time of the animation. If fillBefore is false and
     * {@link #isFillEnabled() fillEnabled} is true, the transformation will not be applied until
     * the start time of the animation.
     *
     * @return true if the animation applies its transformation before it starts
     * @attr ref android.R.styleable#IAnimation::fillBefore
     */
    CARAPI GetFillBefore(
        /* [out] */ Boolean* result);

    /**
     * If fillAfter is TRUE, this animation will apply its transformation
     * after the end time of the animation.
     *
     * @return TRUE if the animation applies its transformation after it ends
     * @attr ref android.R.styleable#IAnimation::fillAfter
     */
    CARAPI GetFillAfter(
        /* [out] */ Boolean* result);

    /**
     * Returns the Z ordering mode to use while running the animation as
     * previously Set by {@link #setZAdjustment}.
     *
     * @return Returns one of {@link #ZORDER_NORMAL},
     * {@link #ZORDER_TOP}, or {@link #ZORDER_BOTTOM}.
     * @attr ref android.R.styleable#IAnimation::zAdjustment
     */
    CARAPI GetZAdjustment(
        /* [out] */ Int32* zAdjustment);

    /**
     * Return value of {@link #setDetachWallpaper(Boolean)}.
     * @attr ref android.R.styleable#IAnimation::detachWallpaper
     */
    CARAPI GetDetachWallpaper(
        /* [out] */ Boolean* result);

    /**
     * <p>Indicates whether or not this animation will affect the transformation
     * matrix. For instance, a fade animation will not affect the matrix whereas
     * a scale animation will.</p>
     *
     * @return TRUE if this animation will change the transformation matrix
     */
    CARAPI WillChangeTransformationMatrix(
        /* [out] */ Boolean* result);

    /**
     * <p>Indicates whether or not this animation will affect the bounds of the
     * animated view. For instance, a fade animation will not affect the bounds
     * whereas a 200% scale animation will.</p>
     *
     * @return TRUE if this animation will change the view's bounds
     */
    CARAPI WillChangeBounds(
        /* [out] */ Boolean* result);

    /**
     * <p>Binds an animation listener to this animation. The animation listener
     * is notified of animation events such as the end of the animation or the
     * repetition of the animation.</p>
     *
     * @param listener the animation listener to be notified
     */
    CARAPI SetAnimationListener(
        /* [in] */ IAnimationAnimationListener* listener);

    /**
     * Compute a hint at how Int64 the entire animation may last, in milliseconds.
     * Animations can be written to cause themselves to run for a different
     * duration than what is computed here, but generally this should be
     * accurate.
     */
    CARAPI ComputeDurationHint(
        /* [out] */ Int64* hint);

    /**
     * Gets the transformation to apply at a specified point in time. Implementations of this
     * method should always replace the specified Transformation or document they are doing
     * otherwise.
     *
     * @param currentTime Where we are in the animation. This is wall clock time.
     * @param outTransformation A tranformation object that is provided by the
     *        caller and will be filled in by the animation.
     * @return True if the animation is still running
     */
    CARAPI GetTransformation(
        /* [in] */ Int64 currentTime,
        /* [in] */ ITransformation* outTransformation,
        /* [out] */ Boolean* result);

    CARAPI GetTransformation(
        /* [in] */ Int64 currentTime,
        /* [in, out] */ ITransformation* outTransformation,
        /* [in] */ Float scale,
        /* [out] */ Boolean* result);

    /**
     * <p>Indicates whether this animation has started or not.</p>
     *
     * @return TRUE if the animation has started, FALSE otherwise
     */
    CARAPI HasStarted(
        /* [out] */ Boolean* has);

    /**
     * <p>Indicates whether this animation has ended or not.</p>
     *
     * @return TRUE if the animation has ended, FALSE otherwise
     */
    CARAPI HasEnded(
        /* [out] */ Boolean* has);

    CARAPI HasAlpha(
        /* [out] */ Boolean* has);

    /**
     * @param left
     * @param top
     * @param right
     * @param bottom
     * @param invalidate
     * @param transformation
     *
     * @hide
     */
    CARAPI GetInvalidateRegion(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [in] */ IRectF* invalidate,
        /* [in] */ ITransformation* transformation);

    /**
     * @param left
     * @param top
     * @param right
     * @param bottom
     *
     * @hide
     */
    CARAPI InitializeInvalidateRegion(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

protected:
    /**
     * Gurantees that this animation has an interpolator. Will use
     * a AccelerateDecelerateInterpolator is nothing else was specified.
     */
    virtual CARAPI_(void) EnsureInterpolator();

    /**
     * Helper for getTransformation. Subclasses should implement this to apply
     * their transforms given an interpolation value.  Implementations of this
     * method should always replace the specified Transformation or document
     * they are doing otherwise.
     *
     * @param interpolatedTime The value of the normalized time (0.0 to 1.0)
     *        after it has been run through the interpolation function.
     * @param t The Transofrmation object to fill in with the current
     *        transforms.
     */
    virtual CARAPI_(void) ApplyTransformation(
        /* [in] */ Float interpolatedTime,
        /* [in] */ ITransformation* t);

    /**
     * Convert the information in the description of a size to an actual
     * dimension
     *
     * @param type One of Animation.ABSOLUTE, Animation.RELATIVE_TO_SELF, or
     *             Animation.RELATIVE_TO_PARENT.
     * @param value The dimension associated with the type parameter
     * @param size The size of the object being animated
     * @param parentSize The size of the parent of the object being animated
     * @return The dimension to use for the animation
     */
    virtual CARAPI_(Float) ResolveSize(
        /* [in] */ Int32 type,
        /* [in] */ Float value,
        /* [in] */ Int32 size,
        /* [in] */ Int32 parentSize);

    /**
     * The scale factor is set by the call to <code>getTransformation</code>. Overrides of
     * {@link #getTransformation(long, Transformation, float)} will get this value
     * directly. Overrides of {@link #applyTransformation(float, Transformation)} can
     * call this method to get the value.
     *
     * @return float The scale factor that should be applied to pre-scaled values in
     * an Animation such as the pivot points in {@link ScaleAnimation} and {@link RotateAnimation}.
     */
    virtual CARAPI_(Float) GetScaleFactor();

    virtual CARAPI_(void) Finalize();

    CARAPI CloneImpl(
        /* [in] */ IAnimation* object);

private:
    CARAPI_(void) FireAnimationStart();

    CARAPI_(void) FireAnimationRepeat();

    CARAPI_(void) FireAnimationEnd();

protected:
    /**
     * Set by {@link #getTransformation(Int64, Transformation)} when the animation ends.
     */
    Boolean mEnded;

    /**
     * Set by {@link #getTransformation(Int64, Transformation)} when the animation starts.
     */
    Boolean mStarted;

    /**
     * Set by {@link #getTransformation(Int64, Transformation)} when the animation repeats
     * in REVERSE mode.
     */
    Boolean mCycleFlip;

    /**
     * This value must be Set to TRUE by {@link #initialize(Int32, Int32, Int32, Int32)}. It
     * indicates the animation was successfully initialized and can be played.
     */
    Boolean mInitialized;

    /**
     * Indicates whether the animation transformation should be applied before the
     * animation starts. The value of this variable is only relevant if mFillEnabled is true;
     * otherwise it is assumed to be true.
     */
    Boolean mFillBefore;

    /**
     * Indicates whether the animation transformation should be applied after the
     * animation ends.
     */
    Boolean mFillAfter;

    /**
     * Indicates whether fillBefore should be taken into account.
     */
    Boolean mFillEnabled;

    /**
     * The time in milliseconds at which the animation must start;
     */
    Int64 mStartTime;

    /**
     * The delay in milliseconds after which the animation must start. When the
     * start offset is > 0, the start time of the animation is startTime + startOffset.
     */
    Int64 mStartOffset;

    /**
     * The duration of one animation cycle in milliseconds.
     */
    Int64 mDuration;

    /**
     * The number of times the animation must repeat. By default, an animation repeats
     * indefinitely.
     */
    Int32 mRepeatCount;

    /**
     * Indicates how many times the animation was repeated.
     */
    Int32 mRepeated;

    /**
     * The behavior of the animation when it repeats. The repeat mode is either
     * {@link #Animation_RESTART} or {@link #REVERSE}.
     *
     */
    Int32 mRepeatMode;

    /**
     * The interpolator used by the animation to smooth the movement.
     */
    AutoPtr<IInterpolator> mInterpolator;

    /**
     * The animation listener to be notified when the animation starts, ends or repeats.
     */
    AutoPtr<IAnimationAnimationListener> mListener;

    AutoPtr<IRectF> mPreviousRegion;
    AutoPtr<IRectF> mRegion;
    AutoPtr<ITransformation> mTransformation;
    AutoPtr<ITransformation> mPreviousTransformation;

private:
    /**
     * Desired Z order mode during animation.
     */
    Int32 mZAdjustment;

    /**
     * Desired background color behind animation.
     */
    Int32 mBackgroundColor;

    /**
     * scalefactor to apply to pivot points, etc. during animation. Subclasses retrieve the
     * value via getScaleFactor().
     */
    Float mScaleFactor;

    /**
     * Don't animate the wallpaper.
     */
    Boolean mDetachWallpaper;

    Boolean mMore;
    Boolean mOneMoreTime;
    static Boolean USE_CLOSEGUARD;

    AutoPtr<ICloseGuard> mGuard;

    AutoPtr<IHandler> mListenerHandler;
    AutoPtr<IRunnable> mOnStart;
    AutoPtr<IRunnable> mOnRepeat;
    AutoPtr<IRunnable> mOnEnd;
};

}// namespace Animation
}// namespace View
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_VIEW_ANIMATION_ANIMATION_H__

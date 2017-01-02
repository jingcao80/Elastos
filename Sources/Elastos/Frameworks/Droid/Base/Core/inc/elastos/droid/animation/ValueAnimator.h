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

#ifndef __ELASTOS_DROID_ANIMATION_VALUEANIMATOR_H__
#define __ELASTOS_DROID_ANIMATION_VALUEANIMATOR_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/animation/Animator.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::IChoreographer;
using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Animation {

class ObjectAnimator;

class ValueAnimator
    : public Animator
    , public IValueAnimator
{
public:
    class AnimationHandler
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("ValueAnimator::AnimationHandler")

        /**
         * Start animating on the next frame.
         */
        CARAPI_(void) Start();

        CARAPI Run();

    private:
        AnimationHandler();

        ~AnimationHandler();

        CARAPI_(void) DoAnimationFrame(
            /* [in] */ Int64 frameTime);

        CARAPI_(void) ScheduleAnimation();

    private:
        friend class ValueAnimator;
        friend class ObjectAnimator;

        // The per-thread list of all active animations
        List<AutoPtr<IValueAnimator> > mAnimations;

        // Used in doAnimationFrame() to avoid concurrent modifications of mAnimations
        List<AutoPtr<IValueAnimator> > mTmpAnimations;

        // The per-thread set of animations to be started on the next animation frame
        List<AutoPtr<IValueAnimator> > mPendingAnimations;

        /**
         * Internal per-thread collections used to avoid set collisions as animations start and end
         * while being processed.
         */
        List<AutoPtr<IValueAnimator> > mDelayedAnims;
        List<AutoPtr<IValueAnimator> > mEndingAnims;
        List<AutoPtr<IValueAnimator> > mReadyAnims;

        AutoPtr<IChoreographer> mChoreographer;
        Boolean mAnimationScheduled;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new ValueAnimator object. This default constructor is primarily for
     * use internally; the factory methods which take parameters are more generally
     * useful.
     */
    ValueAnimator();

    virtual ~ValueAnimator();

    CARAPI constructor();

    static CARAPI_(void) SetDurationScale(
        /* [in] */ Float durationScale);

    static CARAPI_(Float) GetDurationScale();

    /**
     * Constructs and returns a ValueAnimator that animates between int values. A single
     * value implies that that value is the one being animated to. However, this is not typically
     * useful in a ValueAnimator object because there is no way for the object to determine the
     * starting value for the animation (unlike ObjectAnimator, which can derive that value
     * from the target object and property being animated). Therefore, there should typically
     * be two or more values.
     *
     * @param values A set of values that the animation will animate between over time.
     * @return A ValueAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IValueAnimator>) OfInt32(
        /* [in] */ ArrayOf<Int32>* values);

    /**
     * Constructs and returns a ValueAnimator that animates between color values. A single
     * value implies that that value is the one being animated to. However, this is not typically
     * useful in a ValueAnimator object because there is no way for the object to determine the
     * starting value for the animation (unlike ObjectAnimator, which can derive that value
     * from the target object and property being animated). Therefore, there should typically
     * be two or more values.
     *
     * @param values A set of values that the animation will animate between over time.
     * @return A ValueAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IValueAnimator>) OfArgb(
        /* [in] */ ArrayOf<Int32>* values);

    /**
     * Constructs and returns a ValueAnimator that animates between float values. A single
     * value implies that that value is the one being animated to. However, this is not typically
     * useful in a ValueAnimator object because there is no way for the object to determine the
     * starting value for the animation (unlike ObjectAnimator, which can derive that value
     * from the target object and property being animated). Therefore, there should typically
     * be two or more values.
     *
     * @param values A set of values that the animation will animate between over time.
     * @return A ValueAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IValueAnimator>) OfFloat(
        /* [in] */ ArrayOf<Float>* values);

    /**
     * Constructs and returns a ValueAnimator that animates between the values
     * specified in the PropertyValuesHolder objects.
     *
     * @param values A set of PropertyValuesHolder objects whose values will be animated
     * between over time.
     * @return A ValueAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IValueAnimator>) OfPropertyValuesHolder(
        /* [in] */ ArrayOf<IPropertyValuesHolder*>* values);

    /**
     * Constructs and returns a ValueAnimator that animates between Object values. A single
     * value implies that that value is the one being animated to. However, this is not typically
     * useful in a ValueAnimator object because there is no way for the object to determine the
     * starting value for the animation (unlike ObjectAnimator, which can derive that value
     * from the target object and property being animated). Therefore, there should typically
     * be two or more values.
     *
     * <p>Since ValueAnimator does not know how to animate between arbitrary Objects, this
     * factory method also takes a TypeEvaluator object that the ValueAnimator will use
     * to perform that interpolation.
     *
     * @param evaluator A TypeEvaluator that will be called on each animation frame to
     * provide the ncessry interpolation between the Object values to derive the animated
     * value.
     * @param values A set of values that the animation will animate between over time.
     * @return A ValueAnimator object that is set up to animate between the given values.
     */
    static CARAPI_(AutoPtr<IValueAnimator>) OfObject(
        /* [in] */ ITypeEvaluator* evaluator,
        /* [in] */ ArrayOf<IInterface*>* values);


    /**
     * Sets Int32 values that will be animated between. A single
     * value implies that that value is the one being animated to. However, this is not typically
     * useful in a ValueAnimator object because there is no way for the object to determine the
     * starting value for the animation (unlike ObjectAnimator, which can derive that value
     * from the target object and property being animated). Therefore, there should typically
     * be two or more values.
     *
     * <p>If there are already multiple sets of values defined for this ValueAnimator via more
     * than one PropertyValuesHolder object, this method will set the values for the first
     * of those objects.</p>
     *
     * @param values A set of values that the animation will animate between over time.
     */
    virtual CARAPI SetInt32Values(
        /* [in] */ ArrayOf<Int32>* values);

    /**
     * Sets Float values that will be animated between. A single
     * value implies that that value is the one being animated to. However, this is not typically
     * useful in a ValueAnimator object because there is no way for the object to determine the
     * starting value for the animation (unlike ObjectAnimator, which can derive that value
     * from the target object and property being animated). Therefore, there should typically
     * be two or more values.
     *
     * <p>If there are already multiple sets of values defined for this ValueAnimator via more
     * than one PropertyValuesHolder object, this method will set the values for the first
     * of those objects.</p>
     *
     * @param values A set of values that the animation will animate between over time.
     */
    virtual CARAPI SetFloatValues(
        /* [in] */ ArrayOf<Float>* values);

    /**
     * Sets the values to animate between for this animation. A single
     * value implies that that value is the one being animated to. However, this is not typically
     * useful in a ValueAnimator object because there is no way for the object to determine the
     * starting value for the animation (unlike ObjectAnimator, which can derive that value
     * from the target object and property being animated). Therefore, there should typically
     * be two or more values.
     *
     * <p>If there are already multiple sets of values defined for this ValueAnimator via more
     * than one PropertyValuesHolder object, this method will set the values for the first
     * of those objects.</p>
     *
     * <p>There should be a TypeEvaluator set on the ValueAnimator that knows how to interpolate
     * between these value objects. ValueAnimator only knows how to interpolate between the
     * primitive types specified in the other setValues() methods.</p>
     *
     * @param values The set of values to animate between.
     */
    virtual CARAPI SetObjectValues(
        /* [in] */ ArrayOf<IInterface*>* values);

    /**
     * Sets the values, per property, being animated between. This function is called internally
     * by the constructors of ValueAnimator that take a list of values. But a ValueAnimator can
     * be constructed without values and this method can be called to set the values manually
     * instead.
     *
     * @param values The set of values, per property, being animated between.
     */
    virtual CARAPI SetValues(
        /* [in] */ ArrayOf<IPropertyValuesHolder*>* values);

    /**
     * Returns the values that this ValueAnimator animates between. These values are stored in
     * PropertyValuesHolder objects, even if the ValueAnimator was created with a simple list
     * of value objects instead.
     *
     * @return PropertyValuesHolder[] An array of PropertyValuesHolder objects which hold the
     * values, per property, that define the animation.
     */
    virtual CARAPI GetValues(
        /* [out, callee] */ ArrayOf<IPropertyValuesHolder*>** values);

    /**
     * Sets the length of the animation. The default duration is 300 milliseconds.
     *
     * @param duration The length of the animation, in milliseconds. This value cannot
     * be negative.
     * @return ValueAnimator The object called with setDuration(). This return
     * value makes it easier to compose statements together that construct and then set the
     * duration, as in <code>ValueAnimator.ofInt(0, 10).setDuration(500).start()</code>.
     */
    virtual CARAPI SetDuration(
        /* [in] */ Int64 duration);

    /**
     * Gets the length of the animation. The default duration is 300 milliseconds.
     *
     * @return The length of the animation, in milliseconds.
     */
    virtual CARAPI GetDuration(
        /* [out] */ Int64* duration);

    /**
     * Sets the position of the animation to the specified point in time. This time should
     * be between 0 and the total duration of the animation, including any repetition. If
     * the animation has not yet been started, then it will not advance forward after it is
     * set to this time; it will simply set the time to this value and perform any appropriate
     * actions based on that time. If the animation is already running, then setCurrentPlayTime()
     * will set the current playing time to this value and continue playing from that point.
     *
     * @param playTime The time, in milliseconds, to which the animation is advanced or rewound.
     */
    virtual CARAPI SetCurrentPlayTime(
        /* [in] */ Int64 playTime);

    /**
     * Gets the current position of the animation in time, which is equal to the current
     * time minus the time that the animation started. An animation that is not yet started will
     * return a value of zero.
     *
     * @return The current position in time of the animation.
     */
    virtual CARAPI GetCurrentPlayTime(
        /* [out] */ Int64* playTime);

    /**
     * The amount of time, in milliseconds, to delay starting the animation after
     * {@link #start()} is called.
     *
     * @return the number of milliseconds to delay running the animation
     */
    virtual CARAPI GetStartDelay(
        /* [out] */ Int64* delay);

    /**
     * The amount of time, in milliseconds, to delay starting the animation after
     * {@link #start()} is called.

     * @param startDelay The amount of the delay, in milliseconds
     */
    virtual CARAPI SetStartDelay(
        /* [in] */ Int64 startDelay);

    /**
     * The amount of time, in milliseconds, between each frame of the animation. This is a
     * requested time that the animation will attempt to honor, but the actual delay between
     * frames may be different, depending on system load and capabilities. This is a static
     * function because the same delay will be applied to all animations, since they are all
     * run off of a single timing loop.
     *
     * The frame delay may be ignored when the animation system uses an external timing
     * source, such as the display refresh rate (vsync), to govern animations.
     *
     * @return the requested time between frames, in milliseconds
     */
    static CARAPI_(Int64) GetFrameDelay();

    /**
     * The amount of time, in milliseconds, between each frame of the animation. This is a
     * requested time that the animation will attempt to honor, but the actual delay between
     * frames may be different, depending on system load and capabilities. This is a static
     * function because the same delay will be applied to all animations, since they are all
     * run off of a single timing loop.
     *
     * The frame delay may be ignored when the animation system uses an external timing
     * source, such as the display refresh rate (vsync), to govern animations.
     *
     * @param frameDelay the requested time between frames, in milliseconds
     */
    static CARAPI SetFrameDelay(
        /* [in]*/ Int64 delay);

    /**
     * The most recent value calculated by this <code>ValueAnimator</code> when there is just one
     * property being animated. This value is only sensible while the animation is running. The main
     * purpose for this read-only property is to retrieve the value from the <code>ValueAnimator</code>
     * during a call to {@link AnimatorUpdateListener#onAnimationUpdate(ValueAnimator)}, which
     * is called during each animation frame, immediately after the value is calculated.
     *
     * @return animatedValue The value most recently calculated by this <code>ValueAnimator</code> for
     * the single property being animated. If there are several properties being animated
     * (specified by several PropertyValuesHolder objects in the constructor), this function
     * returns the animated value for the first of those objects.
     */
    virtual CARAPI GetAnimatedValue(
        /* [out] */ IInterface** value);

    /**
     * The most recent value calculated by this <code>ValueAnimator</code> for <code>propertyName</code>.
     * The main purpose for this read-only property is to retrieve the value from the
     * <code>ValueAnimator</code> during a call to
     * {@link AnimatorUpdateListener#onAnimationUpdate(ValueAnimator)}, which
     * is called during each animation frame, immediately after the value is calculated.
     *
     * @return animatedValue The value most recently calculated for the named property
     * by this <code>ValueAnimator</code>.
     */
    virtual CARAPI GetAnimatedValue(
        /* [in] */ const String& propertyName,
        /* [out] */ IInterface** value);

    /**
     * Sets how many times the animation should be repeated. If the repeat
     * count is 0, the animation is never repeated. If the repeat count is
     * greater than 0 or {@link #INFINITE}, the repeat mode will be taken
     * into account. The repeat count is 0 by default.
     *
     * @param value the number of times the animation should be repeated
     */
    virtual CARAPI SetRepeatCount(
        /* [in] */ Int32 value);

    /**
     * Defines how many times the animation should repeat. The default value
     * is 0.
     *
     * @return the number of times the animation should repeat, or {@link #INFINITE}
     */
    virtual CARAPI GetRepeatCount(
        /* [out] */ Int32* count);

    /**
     * Defines what this animation should do when it reaches the end. This
     * setting is applied only when the repeat count is either greater than
     * 0 or {@link #INFINITE}. Defaults to {@link #RESTART}.
     *
     * @param value {@link #RESTART} or {@link #REVERSE}
     */
    virtual CARAPI SetRepeatMode(
        /* [in] */ Int32 value);

    /**
     * Defines what this animation should do when it reaches the end.
     *
     * @return either one of {@link #REVERSE} or {@link #RESTART}
     */
    virtual CARAPI GetRepeatMode(
        /* [out] */ Int32* mode);

    /**
     * Adds a listener to the set of listeners that are sent update events through the life of
     * an animation. This method is called on all listeners for every frame of the animation,
     * after the values for the animation have been calculated.
     *
     * @param listener the listener to be added to the current set of listeners for this animation.
     */
    virtual CARAPI AddUpdateListener(
        /* [in] */ IAnimatorUpdateListener* listener);

    /**
     * Removes all listeners from the set listening to frame updates for this animation.
     */
    virtual CARAPI RemoveAllUpdateListeners();

    /**
     * Removes a listener from the set listening to frame updates for this animation.
     *
     * @param listener the listener to be removed from the current set of update listeners
     * for this animation.
     */
    virtual CARAPI RemoveUpdateListener(
        /* [in] */ IAnimatorUpdateListener* listener);

    /**
     * The time interpolator used in calculating the elapsed fraction of this animation. The
     * interpolator determines whether the animation runs with linear or non-linear motion,
     * such as acceleration and deceleration. The default value is
     * {@link android.view.animation.AccelerateDecelerateInterpolator}
     *
     * @param value the interpolator to be used by this animation. A value of <code>NULL</code>
     * will result in linear interpolation.
     */
    //@Override
    virtual CARAPI SetInterpolator(
        /* [in] */ ITimeInterpolator* value);

    /**
     * Returns the timing interpolator that this ValueAnimator uses.
     *
     * @return The timing interpolator for this ValueAnimator.
     */
    virtual CARAPI GetInterpolator(
        /* [out] */ ITimeInterpolator** interpolator);

    /**
     * The type evaluator to be used when calculating the animated values of this animation.
     * The system will automatically assign a Float or Int32 evaluator based on the type
     * of <code>startValue</code> and <code>endValue</code> in the constructor. But if these values
     * are not one of these primitive types, or if different evaluation is desired (such as is
     * necessary with Int32 values that represent colors), a custom evaluator needs to be assigned.
     * For example, when running an animation on color values, the {@link ArgbEvaluator}
     * should be used to get correct RGB color interpolation.
     *
     * <p>If this ValueAnimator has only one set of values being animated between, this evaluator
     * will be used for that set. If there are several sets of values being animated, which is
     * the case if PropertyValuesHOlder objects were set on the ValueAnimator, then the evaluator
     * is assigned just to the first PropertyValuesHolder object.</p>
     *
     * @param value the evaluator to be used this animation
     */
    virtual CARAPI SetEvaluator(
        /* [in] */ ITypeEvaluator* value);

    //@Override
    virtual CARAPI Start();

    //@Override
    virtual CARAPI Cancel();

    //@Override
    virtual CARAPI End();

    // @Override
    virtual CARAPI Resume();

    // @Override
    virtual CARAPI Pause();

    //@Override
    virtual CARAPI IsRunning(
        /* [out] */ Boolean* running);

    //@Override
    virtual CARAPI IsStarted(
        /* [out] */ Boolean* started);

    /**
     * Plays the ValueAnimator in reverse. If the animation is already running,
     * it will stop itself and play backwards from the point reached when reverse was called.
     * If the animation is not currently running, then it will start from the end and
     * play backwards. This behavior is only set for the current animation; future playing
     * of the animation will use the default behavior of playing forward.
     */
    virtual CARAPI Reverse();

    /**
     * @hide
     */
    // @Override
    virtual CARAPI CanReverse(
        /* [out] */ Boolean* can);

    /**
     * Returns the current animation fraction, which is the elapsed/interpolated fraction used in
     * the most recent frame update on the animation.
     *
     * @return Elapsed/interpolated fraction of the animation.
     */
    virtual CARAPI GetAnimatedFraction(
        /* [out] */ Float* fraction);

    //@Override
    virtual CARAPI Clone(
        /* [out] */ IInterface** object);

    /**
     * Return the number of animations currently running.
     *
     * Used by StrictMode internally to annotate violations.
     * May be called on arbitrary threads!
     *
     * @hide
     */
    static CARAPI_(Int32) GetCurrentAnimationsCount();

    /**
     * Clear all animations on this thread, without canceling or ending them.
     * This should be used with caution.
     *
     * @hide
     */
    static CARAPI_(void) ClearAllAnimations();

    //@Override
    CARAPI ToString(
        /* [out] */ String* info);

    /**
     * <p>Whether or not the ValueAnimator is allowed to run asynchronously off of
     * the UI thread. This is a hint that informs the ValueAnimator that it is
     * OK to run the animation off-thread, however ValueAnimator may decide
     * that it must run the animation on the UI thread anyway. For example if there
     * is an {@link AnimatorUpdateListener} the animation will run on the UI thread,
     * regardless of the value of this hint.</p>
     *
     * <p>Regardless of whether or not the animation runs asynchronously, all
     * listener callbacks will be called on the UI thread.</p>
     *
     * <p>To be able to use this hint the following must be true:</p>
     * <ol>
     * <li>{@link #getAnimatedFraction()} is not needed (it will return undefined values).</li>
     * <li>The animator is immutable while {@link #isStarted()} is true. Requests
     *    to change values, duration, delay, etc... may be ignored.</li>
     * <li>Lifecycle callback events may be asynchronous. Events such as
     *    {@link Animator.AnimatorListener#onAnimationEnd(Animator)} or
     *    {@link Animator.AnimatorListener#onAnimationRepeat(Animator)} may end up delayed
     *    as they must be posted back to the UI thread, and any actions performed
     *    by those callbacks (such as starting new animations) will not happen
     *    in the same frame.</li>
     * <li>State change requests ({@link #cancel()}, {@link #end()}, {@link #reverse()}, etc...)
     *    may be asynchronous. It is guaranteed that all state changes that are
     *    performed on the UI thread in the same frame will be applied as a single
     *    atomic update, however that frame may be the current frame,
     *    the next frame, or some future frame. This will also impact the observed
     *    state of the Animator. For example, {@link #isStarted()} may still return true
     *    after a call to {@link #end()}. Using the lifecycle callbacks is preferred over
     *    queries to {@link #isStarted()}, {@link #isRunning()}, and {@link #isPaused()}
     *    for this reason.</li>
     * </ol>
     * @hide
     */
    // @Override
    virtual CARAPI SetAllowRunningAsynchronously(
        /* [in] */ Boolean mayRunAsync);

protected:
    /**
     * This function is called immediately before processing the first animation
     * frame of an animation. If there is a nonzero <code>startDelay</code>, the
     * function is called after that delay ends.
     * It takes care of the final initialization steps for the
     * animation.
     *
     *  <p>Overrides of this method should call the superclass method to ensure
     *  that internal mechanisms for the animation are set up correctly.</p>
     */
    virtual CARAPI InitAnimation();

    /**
     * Called internally to end an animation by removing it from the animations list. Must be
     * called on the UI thread.
     */
    CARAPI_(void) EndAnimation(
        /* [in] */ AnimationHandler* handler);

    /**
     * Returns the name of this animator for debugging purposes.
     */
    CARAPI_(String) GetNameForTrace();

    /**
     * This internal function processes a single animation frame for a given animation. The
     * currentTime parameter is the timing pulse sent by the handler, used to calculate the
     * elapsed duration, and therefore
     * the elapsed fraction, of the animation. The return value indicates whether the animation
     * should be ended (which happens when the elapsed time of the animation exceeds the
     * animation's duration, including the repeatCount).
     *
     * @param currentTime The current time, as tracked by the static timing handler
     * @return TRUE if the animation's duration, including any repetitions due to
     * <code>repeatCount</code> has been exceeded and the animation should be ended.
     */
    virtual CARAPI_(Boolean) AnimationFrame(
        /* [in] */ Int64 currentTime);

    /**
     * Processes a frame of the animation, adjusting the start time if needed.
     *
     * @param frameTime The frame time.
     * @return TRUE if the animation has ended.
     */
    virtual CARAPI_(Boolean) DoAnimationFrame(
        /* [in] */ Int64 frameTime);

    /**
     * This method is called with the elapsed fraction of the animation during every
     * animation frame. This function turns the elapsed fraction into an interpolated fraction
     * and then into an animated value (from the evaluator. The function is called mostly during
     * animation updates, but it is also called when the <code>end()</code>
     * function is called, to set the final value on the property.
     *
     * <p>Overrides of this method must call the superclass to perform the calculation
     * of the animated value.</p>
     *
     * @param fraction The elapsed fraction of the animation.
     */
    virtual CARAPI AnimateValue(
        /* [in] */ Float fraction);

    CARAPI CloneImpl(
        /* [in] */ IValueAnimator* anim);

private:
    CARAPI_(void) UpdateScaledDuration();

    CARAPI_(void) NotifyStartListeners();

    /**
     * Start the animation playing. This version of start() takes a Boolean flag that indicates
     * whether the animation should play in reverse. The flag is usually FALSE, but may be set
     * to TRUE if called from the reverse() method.
     *
     * <p>The animation started by calling this method will be run on the thread that called
     * this method. This thread should have a Looper on it (a runtime exception will be thrown if
     * this is not the case). Also, if the animation will animate
     * properties of objects in the view hierarchy, then the calling thread should be the UI
     * thread for that view hierarchy.</p>
     *
     * @param playBackwards Whether the ValueAnimator should start playing in reverse.
     */
    CARAPI Start(
        /* [in] */ Boolean playBackwards);

    /**
     * Called internally to start an animation by adding it to the active animations list. Must be
     * called on the UI thread.
     */
    CARAPI_(void) StartAnimation(
        /* [in] */ AnimationHandler* handler);

    /**
     * Internal function called to process an animation frame on an animation that is currently
     * sleeping through its <code>startDelay</code> phase. The return value indicates whether it
     * should be woken up and put on the active animations queue.
     *
     * @param currentTime The current animation time, used to calculate whether the animation
     * has exceeded its <code>startDelay</code> and should be started.
     * @return True if the animation's <code>startDelay</code> has been exceeded and the animation
     * should be added to the set of active animations.
     */
    CARAPI_(Boolean) DelayedAnimationFrame(
        /* [in] */ Int64 currentTime);

    CARAPI_(AutoPtr<AnimationHandler>) GetOrCreateAnimationHandler();

public:
    // The static sAnimationHandler processes the internal timing loop on which all animations
    // are based
    static pthread_key_t sAnimationHandler;
    static Boolean sHaveKey;

protected:
    /**
     * Values used with internal variable mPlayingState to indicate the current state of an
     * animation.
     */
    static const Int32 STOPPED; // Not yet playing
    static const Int32 RUNNING; // Playing normally
    static const Int32 SEEKED; // Seeked to some time value

    /**
     * Internal variables
     * NOTE: This object implements the clone() method, making a deep copy of any referenced
     * objects. As other non-trivial fields are added to this class, make sure to add logic
     * to clone() to make deep copies of them.
     */

    // The first time that the animation's animateFrame() method is called. This time is used to
    // determine elapsed time (and therefore the elapsed fraction) in subsequent calls
    // to animateFrame()
    Int64 mStartTime;

    /**
     * Set when setCurrentPlayTime() is called. If negative, animation is not currently seeked
     * to a value.
     */
    Int64 mSeekTime;

    /**
     * Flag that represents the current state of the animation. Used to figure out when to start
     * an animation (if state == STOPPED). Also used to end an animation that
     * has been cancel()'d or end()'d since the last animation frame. Possible values are
     * STOPPED, RUNNING, SEEKED.
     */
    Int32 mPlayingState;

    /**
     * Flag that denotes whether the animation is set up and ready to go. Used to
     * set up animation that has not yet been started.
     */
    Boolean mInitialized;

    /**
     * The property/value sets being animated.
     */
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > mValues;

    /**
     * A hashmap of the PropertyValuesHolder objects. This map is used to lookup animated values
     * by property name during calls to getAnimatedValue(String).
     */
    HashMap<String, AutoPtr<IPropertyValuesHolder> > mValuesMap;
    typedef HashMap<String, AutoPtr<IPropertyValuesHolder> >::Iterator StrMapIterator;

private:
    /**
     * Internal constants
     */
    static Float sDurationScale;

    /**
     * Set on the next frame after pause() is called, used to calculate a new startTime
     * or delayStartTime which allows the animator to continue from the point at which
     * it was paused. If negative, has not yet been set.
     */
    Int64 mPauseTime;

    /**
     * Set when an animator is resumed. This triggers logic in the next frame which
     * actually resumes the animator.
     */
    Boolean mResumed;

    // The time interpolator to be used if none is set on the animation
    static const AutoPtr<ITimeInterpolator> sDefaultInterpolator;

    /**
     * Used to indicate whether the animation is currently playing in reverse. This causes the
     * elapsed fraction to be inverted to calculate the appropriate values.
     */
    Boolean mPlayingBackwards;

    /**
     * This variable tracks the current iteration that is playing. When mCurrentIteration exceeds the
     * repeatCount (if repeatCount!=INFINITE), the animation ends
     */
    Int32 mCurrentIteration;

    /**
     * Tracks current elapsed/eased fraction, for querying in getAnimatedFraction().
     */
    Float mCurrentFraction;

    /**
     * Tracks whether a startDelay'd animation has begun playing through the startDelay.
     */
    Boolean mStartedDelay;

    /**
     * Tracks the time at which the animation began playing through its startDelay. This is
     * different from the mStartTime variable, which is used to track when the animation became
     * active (which is when the startDelay expired and the animation was added to the active
     * animations list).
     */
    Int64 mDelayStartTime;

    /**
     * Additional playing state to indicate whether an animator has been start()'d. There is
     * some lag between a call to start() and the first animation frame. We should still note
     * that the animation has been started, even if it's first animation frame has not yet
     * happened, and reflect that state in isRunning().
     * Note that delayed animations are different: they are not started until their first
     * animation frame, which occurs after their delay elapses.
     */
    Boolean mRunning;

    /**
     * Additional playing state to indicate whether an animator has been start()'d, whether or
     * not there is a nonzero startDelay.
     */
    Boolean mStarted;

    /**
     * Tracks whether we've notified listeners of the onAnimationStart() event. This can be
     * complex to keep track of since we notify listeners at different times depending on
     * startDelay and whether start() was called before end().
     */
    Boolean mStartListenersCalled;

    //
    // Backing variables
    //

    // How Int64 the animation should last in ms
    Int64 mDuration;
    Int64 mUnscaledDuration;

    // The amount of time in ms to delay starting the animation after start() is called
    Int64 mStartDelay;
    Int64 mUnscaledStartDelay;

    // The number of times the animation will repeat. The default is 0, which means the animation
    // will play only once
    Int32 mRepeatCount;

    /**
     * The type of repetition that will occur when repeatMode is nonzero. RESTART means the
     * animation will start from the beginning on every new cycle. REVERSE means the animation
     * will reverse directions on each iteration.
     */
    Int32 mRepeatMode;

    /**
     * The time interpolator to be used. The elapsed fraction of the animation will be passed
     * through this interpolator to calculate the interpolated fraction, which is then used to
     * calculate the animated values.
     */
    AutoPtr<ITimeInterpolator> mInterpolator;

    /**
     * The set of listeners to be sent events through the life of an animation.
     */
    AutoPtr<IArrayList> mUpdateListeners;
};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_VALUEANIMATOR_H__

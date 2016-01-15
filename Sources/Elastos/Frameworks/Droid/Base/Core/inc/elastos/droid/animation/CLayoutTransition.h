
#ifndef  __ELASTOS_DROID_ANIMATION_CLAYOUTTRANSITION_H__
#define  __ELASTOS_DROID_ANIMATION_CLAYOUTTRANSITION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.View.h"
#define HASH_FOR_VIEW
#include "elastos/droid/ext/frameworkhash.h"
#include "_Elastos_Droid_Animation_CLayoutTransition.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include "elastos/droid/animation/AnimatorListenerAdapter.h"

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::View::IViewOnLayoutChangeListener;
using Elastos::Droid::View::EIID_IViewOnLayoutChangeListener;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Animation {

/**
 * This class enables automatic animations on layout changes in ViewGroup objects. To enable
 * transitions for a layout container, create a LayoutTransition object and set it on any
 * ViewGroup by calling {@link ViewGroup#setLayoutTransition(LayoutTransition)}. This will cause
 * default animations to run whenever items are added to or removed from that container. To specify
 * custom animations, use the {@link LayoutTransition#setAnimator(int, Animator)
 * setAnimator()} method.
 *
 * <p>One of the core concepts of these transition animations is that there are two types of
 * changes that cause the transition and four different animations that run because of
 * those changes. The changes that trigger the transition are items being added to a container
 * (referred to as an "appearing" transition) or removed from a container (also known as
 * "disappearing"). Setting the visibility of views (between GONE and VISIBLE) will trigger
 * the same add/remove logic. The animations that run due to those events are one that animates
 * items being added, one that animates items being removed, and two that animate the other
 * items in the container that change due to the add/remove occurrence. Users of
 * the transition may want different animations for the changing items depending on whether
 * they are changing due to an appearing or disappearing event, so there is one animation for
 * each of these variations of the changing event. Most of the API of this class is concerned
 * with setting up the basic properties of the animations used in these four situations,
 * or with setting up custom animations for any or all of the four.</p>
 *
 * <p>By default, the DISAPPEARING animation begins immediately, as does the CHANGE_APPEARING
 * animation. The other animations begin after a delay that is set to the default duration
 * of the animations. This behavior facilitates a sequence of animations in transitions as
 * follows: when an item is being added to a layout, the other children of that container will
 * move first (thus creating space for the new item), then the appearing animation will run to
 * animate the item being added. Conversely, when an item is removed from a container, the
 * animation to remove it will run first, then the animations of the other children in the
 * layout will run (closing the gap created in the layout when the item was removed). If this
 * default choreography behavior is not desired, the {@link #setDuration(int, Int64)} and
 * {@link #setStartDelay(int, Int64)} of any or all of the animations can be changed as
 * appropriate.</p>
 *
 * <p>The animations specified for the transition, both the defaults and any custom animations
 * set on the transition object, are templates only. That is, these animations exist to hold the
 * basic animation properties, such as the duration, start delay, and properties being animated.
 * But the actual target object, as well as the start and end values for those properties, are
 * set automatically in the process of setting up the transition each time it runs. Each of the
 * animations is cloned from the original copy and the clone is then populated with the dynamic
 * values of the target being animated (such as one of the items in a layout container that is
 * moving as a result of the layout event) as well as the values that are changing (such as the
 * position and size of that object). The actual values that are pushed to each animation
 * depends on what properties are specified for the animation. For example, the default
 * CHANGE_APPEARING animation animates the <code>left</code>, <code>top</code>, <code>right</code>,
 * <code>bottom</code>, <code>scrollX</code>, and <code>scrollY</code> properties.
 * Values for these properties are updated with the pre- and post-layout
 * values when the transition begins. Custom animations will be similarly populated with
 * the target and values being animated, assuming they use ObjectAnimator objects with
 * property names that are known on the target object.</p>
 *
 * <p>This class, and the associated XML flag for containers, animateLayoutChanges="true",
 * provides a simple utility meant for automating changes in straightforward situations.
 * Using LayoutTransition at multiple levels of a nested view hierarchy may not work due to the
 * interrelationship of the various levels of layout. Also, a container that is being scrolled
 * at the same time as items are being added or removed is probably not a good candidate for
 * this utility, because the before/after locations calculated by LayoutTransition
 * may not match the actual locations when the animations finish due to the container
 * being scrolled as the animations are running. You can work around that
 * particular issue by disabling the 'changing' animations by setting the CHANGE_APPEARING
 * and CHANGE_DISAPPEARING animations to null, and setting the startDelay of the
 * other animations appropriately.</p>
 */
CarClass(CLayoutTransition)
    , public Object
    , public ILayoutTransition
{
private:
    class _AnimatorListenerAdapter : public AnimatorListenerAdapter
    {
    public:
        _AnimatorListenerAdapter(
            /* [in] */ CLayoutTransition* host,
            /* [in] */ IView* child);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CLayoutTransition* mHost;
        AutoPtr<IView> mChild;
    };

    class _AnimatorListenerAdapterEx : public AnimatorListenerAdapter
    {
    public:
        _AnimatorListenerAdapterEx(
            /* [in] */ CLayoutTransition* host,
            /* [in] */ IView* child,
            /* [in] */ IViewGroup* parent,
            /* [in] */ Boolean changeReason,
            /* [in] */ IViewOnLayoutChangeListener* mListener);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CLayoutTransition* mHost;
        AutoPtr<IView> mChild;
        AutoPtr<IViewGroup> mParent;
        Boolean mChangeReason;
        AutoPtr<IViewOnLayoutChangeListener> mListener;
    };

    class _AnimatorListenerAdapterEx2 : public AnimatorListenerAdapter
    {
    public:
        _AnimatorListenerAdapterEx2(
            /* [in] */ CLayoutTransition* host,
            /* [in] */ IView* child,
            /* [in] */ IViewGroup* parent);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CLayoutTransition* mHost;
        AutoPtr<IView> mChild;
        AutoPtr<IViewGroup> mParent;
    };

    class _AnimatorListenerAdapterEx3 : public AnimatorListenerAdapter
    {
    public:
        _AnimatorListenerAdapterEx3(
            /* [in] */ CLayoutTransition* host,
            /* [in] */ IView* child,
            /* [in] */ IViewGroup* parent,
            /* [in] */ Float preAnimAlpha);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CLayoutTransition* mHost;
        AutoPtr<IView> mChild;
        AutoPtr<IViewGroup> mParent;
        Float mPreAnimAlpha;
    };

    class _OnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:

        CAR_INTERFACE_DECL()

        _OnPreDrawListener(
            /* [in] */ CLayoutTransition* host,
            /* [in] */ IViewGroup* parent);

        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

    private:
        CLayoutTransition* mHost;
        AutoPtr<IViewGroup> mParent;
    };

    class ViewOnLayoutChangeListener
            : public IViewOnLayoutChangeListener
            , public Object
    {
    public:
        CAR_INTERFACE_DECL()

        ViewOnLayoutChangeListener(
            /* [in] */ CLayoutTransition* host,
            /* [in] */ Int32 changeReason,
            /* [in] */ Int64 duration,
            /* [in] */ IView* child,
            /* [in] */ IViewGroup* parent);
        /**
         * Called when the focus state of a view has changed.
         *
         * @param v The view whose state has changed.
         * @param left The new value of the view's left property.
         * @param top The new value of the view's top property.
         * @param right The new value of the view's right property.
         * @param bottom The new value of the view's bottom property.
         * @param oldLeft The previous value of the view's left property.
         * @param oldTop The previous value of the view's top property.
         * @param oldRight The previous value of the view's right property.
         * @param oldBottom The previous value of the view's bottom property.
         */
        CARAPI OnLayoutChange(
            /* [in] */ IView* v,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom,
            /* [in] */ Int32 oldLeft,
            /* [in] */ Int32 oldTop,
            /* [in] */ Int32 oldRight,
            /* [in] */ Int32 oldBottom);

    private:
        CLayoutTransition* mHost;
        Int32 mChangeReason;
        Int64 mDuration;
        AutoPtr<IView> mChild;
        AutoPtr<IAnimator> mAnim;
        AutoPtr<IViewGroup> mParent;
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    /**
     * Constructs a LayoutTransition object. By default, the object will listen to layout
     * events on any ViewGroup that it is set on and will run default animations for each
     * type of layout event.
     */
    CLayoutTransition();

    CARAPI constructor();
   /**
     * Sets the duration to be used by all animations of this transition object. If you want to
     * set the duration of just one of the animations in particular, use the
     * {@link #setDuration(Int32, Int64)} method.
     *
     * @param duration The length of time, in milliseconds, that the transition animations
     * should last.
     */
    CARAPI SetDuration(
        /* [in] */ Int64 duration);

    /**
     * Enables the specified transitionType for this LayoutTransition object.
     * By default, a LayoutTransition listens for changes in children being
     * added/remove/hidden/shown in the container, and runs the animations associated with
     * those events. That is, all transition types besides {@link #CHANGING} are enabled by default.
     * You can also enable {@link #CHANGING} animations by calling this method with the
     * {@link #CHANGING} transitionType.
     *
     * @param transitionType One of {@link #CHANGE_APPEARING}, {@link #CHANGE_DISAPPEARING},
     * {@link #CHANGING}, {@link #APPEARING}, or {@link #DISAPPEARING}.
     */
    CARAPI EnableTransitionType(
        /* [in] */ Int32 transitionType);

    /**
     * Disables the specified transitionType for this LayoutTransition object.
     * By default, all transition types except {@link #CHANGING} are enabled.
     *
     * @param transitionType One of {@link #CHANGE_APPEARING}, {@link #CHANGE_DISAPPEARING},
     * {@link #CHANGING}, {@link #APPEARING}, or {@link #DISAPPEARING}.
     */
    CARAPI DisableTransitionType(
        /* [in] */ Int32 transitionType);

    /**
     * Returns whether the specified transitionType is enabled for this LayoutTransition object.
     * By default, all transition types except {@link #CHANGING} are enabled.
     *
     * @param transitionType One of {@link #CHANGE_APPEARING}, {@link #CHANGE_DISAPPEARING},
     * {@link #CHANGING}, {@link #APPEARING}, or {@link #DISAPPEARING}.
     * @return true if the specified transitionType is currently enabled, false otherwise.
     */
    CARAPI IsTransitionTypeEnabled(
        /* [in] */ Int32 transitionType,
        /* [out] */ Boolean* enabled);

    /**
     * Sets the start delay on one of the animation objects used by this transition. The
     * <code>transitionType</code> parameter determines the animation whose start delay
     * is being set.
     *
     * @param transitionType One of {@link #CHANGE_APPEARING}, {@link #CHANGE_DISAPPEARING},
     * {@link #CHANGING}, {@link #APPEARING}, or {@link #DISAPPEARING}, which determines
     * the animation whose start delay is being set.
     * @param delay The length of time, in milliseconds, to delay before starting the animation.
     * @see Animator#setStartDelay(Int64)
     */
    CARAPI SetStartDelay(
        /* [in] */ Int32 transitionType,
        /* [in] */ Int64 delay);

    /**
     * Gets the start delay on one of the animation objects used by this transition. The
     * <code>transitionType</code> parameter determines the animation whose start delay
     * is returned.
     *
     * @param transitionType One of {@link #CHANGE_APPEARING}, {@link #CHANGE_DISAPPEARING},
     * {@link #CHANGING}, {@link #APPEARING}, or {@link #DISAPPEARING}, which determines
     * the animation whose start delay is returned.
     * @return Int64 The start delay of the specified animation.
     * @see Animator#getStartDelay()
     */
    CARAPI GetStartDelay(
        /* [in] */ Int32 transitionType,
        /* [out] */ Int64* delay);

    /**
     * Sets the duration on one of the animation objects used by this transition. The
     * <code>transitionType</code> parameter determines the animation whose duration
     * is being set.
     *
     * @param transitionType One of {@link #CHANGE_APPEARING}, {@link #CHANGE_DISAPPEARING},
     * {@link #CHANGING}, {@link #APPEARING}, or {@link #DISAPPEARING}, which determines
     * the animation whose duration is being set.
     * @param duration The length of time, in milliseconds, that the specified animation should run.
     * @see Animator#setDuration(Int64)
     */
    CARAPI SetDuration(
        /* [in] */ Int32 transitionType,
        /* [in] */ Int64 duration);

    /**
     * Gets the duration on one of the animation objects used by this transition. The
     * <code>transitionType</code> parameter determines the animation whose duration
     * is returned.
     *
     * @param transitionType One of {@link #CHANGE_APPEARING}, {@link #CHANGE_DISAPPEARING},
     * {@link #CHANGING}, {@link #APPEARING}, or {@link #DISAPPEARING}, which determines
     * the animation whose duration is returned.
     * @return Int64 The duration of the specified animation.
     * @see Animator#getDuration()
     */
    CARAPI GetDuration(
        /* [in] */ Int32 transitionType,
        /* [out] */ Int64* druation);

    /**
     * Sets the length of time to delay between starting each animation during one of the
     * change animations.
     *
     * @param transitionType A value of {@link #CHANGE_APPEARING}, {@link #CHANGE_DISAPPEARING}, or
     * {@link #CHANGING}.
     * @param duration The length of time, in milliseconds, to delay before launching the next
     * animation in the sequence.
     */
    CARAPI SetStagger(
        /* [in] */ Int32 transitionType,
        /* [in] */ Int64 duration);

    /**
     * Gets the length of time to delay between starting each animation during one of the
     * change animations.
     *
     * @param transitionType A value of {@link #CHANGE_APPEARING}, {@link #CHANGE_DISAPPEARING}, or
     * {@link #CHANGING}.
     * @return Int64 The length of time, in milliseconds, to delay before launching the next
     * animation in the sequence.
     */
    CARAPI GetStagger(
        /* [in] */ Int32 transitionType,
        /* [out] */ Int64* stagger);

    /**
     * Sets the interpolator on one of the animation objects used by this transition. The
     * <code>transitionType</code> parameter determines the animation whose interpolator
     * is being set.
     *
     * @param transitionType One of {@link #CHANGE_APPEARING}, {@link #CHANGE_DISAPPEARING},
     * {@link #CHANGING}, {@link #APPEARING}, or {@link #DISAPPEARING}, which determines
     * the animation whose interpolator is being set.
     * @param interpolator The interpolator that the specified animation should use.
     * @see Animator#setInterpolator(TimeInterpolator)
     */
    CARAPI SetInterpolator(
        /* [in] */ Int32 transitionType,
        /* [in] */ ITimeInterpolator* interpolator);

    /**
     * Gets the interpolator on one of the animation objects used by this transition. The
     * <code>transitionType</code> parameter determines the animation whose interpolator
     * is returned.
     *
     * @param transitionType One of {@link #CHANGE_APPEARING}, {@link #CHANGE_DISAPPEARING},
     * {@link #CHANGING}, {@link #APPEARING}, or {@link #DISAPPEARING}, which determines
     * the animation whose interpolator is being returned.
     * @return TimeInterpolator The interpolator that the specified animation uses.
     * @see Animator#setInterpolator(TimeInterpolator)
     */
    CARAPI GetInterpolator(
        /* [in] */ Int32 transitionType,
        /* [out] */ ITimeInterpolator** polator);

    /**
     * Sets the animation used during one of the transition types that may run. Any
     * Animator object can be used, but to be most useful in the context of layout
     * transitions, the animation should either be a ObjectAnimator or a AnimatorSet
     * of animations including PropertyAnimators. Also, these ObjectAnimator objects
     * should be able to get and set values on their target objects automatically. For
     * example, a ObjectAnimator that animates the property "left" is able to set and get the
     * <code>left</code> property from the View objects being animated by the layout
     * transition. The transition works by setting target objects and properties
     * dynamically, according to the pre- and post-layoout values of those objects, so
     * having animations that can handle those properties appropriately will work best
     * for custom animation. The dynamic setting of values is only the case for the
     * CHANGE animations; the APPEARING and DISAPPEARING animations are simply run with
     * the values they have.
     *
     * <p>It is also worth noting that any and all animations (and their underlying
     * PropertyValuesHolder objects) will have their start and end values set according
     * to the pre- and post-layout values. So, for example, a custom animation on "alpha"
     * as the CHANGE_APPEARING animation will inherit the real value of alpha on the target
     * object (presumably 1) as its starting and ending value when the animation begins.
     * Animations which need to use values at the beginning and end that may not match the
     * values queried when the transition begins may need to use a different mechanism
     * than a standard ObjectAnimator object.</p>
     *
     * @param transitionType One of {@link #CHANGE_APPEARING}, {@link #CHANGE_DISAPPEARING},
     * {@link #CHANGING}, {@link #APPEARING}, or {@link #DISAPPEARING}, which determines the
     * animation whose animator is being set.
     * @param animator The animation being assigned. A value of <code>null</code> means that no
     * animation will be run for the specified transitionType.
     */
    CARAPI SetAnimator(
        /* [in] */ Int32 transitionType,
        /* [in] */ IAnimator* animator);

    /**
     * Gets the animation used during one of the transition types that may run.
     *
     * @param transitionType One of {@link #CHANGE_APPEARING}, {@link #CHANGE_DISAPPEARING},
     * {@link #CHANGING}, {@link #APPEARING}, or {@link #DISAPPEARING}, which determines
     * the animation whose animator is being returned.
     * @return Animator The animation being used for the given transition type.
     * @see #setAnimator(Int32, Animator)
     */
    CARAPI GetAnimator(
        /* [in] */ Int32 transitionType,
        /* [out] */ IAnimator** mator);

    /**
     * This flag controls whether CHANGE_APPEARING or CHANGE_DISAPPEARING animations will
     * cause the default changing animation to be run on the parent hierarchy as well. This allows
     * containers of transitioning views to also transition, which may be necessary in situations
     * where the containers bounds change between the before/after states and may clip their
     * children during the transition animations. For example, layouts with wrap_content will
     * adjust their bounds according to the dimensions of their children.
     *
     * <p>The default changing transitions animate the bounds and scroll positions of the
     * target views. These are the animations that will run on the parent hierarchy, not
     * the custom animations that happen to be set on the transition. This allows custom
     * behavior for the children of the transitioning container, but uses standard behavior
     * of resizing/rescrolling on any changing parents.
     *
     * @param animateParentHierarchy A Boolean value indicating whether the parents of
     * transitioning views should also be animated during the transition. Default value is true.
     */
    CARAPI SetAnimateParentHierarchy(
        /* [in] */ Boolean animateParentHierarchy);

    /**
     * Starts the animations set up for a CHANGING transition. We separate the setup of these
     * animations from actually starting them, to avoid side-effects that starting the animations
     * may have on the properties of the affected objects. After setup, we tell the affected parent
     * that this transition should be started. The parent informs its ViewAncestor, which then
     * starts the transition after the current layout/measurement phase, just prior to drawing
     * the view hierarchy.
     *
     * @hide
     */
    CARAPI StartChangingAnimations();

    /**
     * Ends the animations that are set up for a CHANGING transition. This is a variant of
     * startChangingAnimations() which is called when the window the transition is playing in
     * is not visible. We need to make sure the animations put their targets in their end states
     * and that the transition finishes to remove any mid-process state (such as isRunning()).
     *
     * @hide
     */
    CARAPI EndChangingAnimations();

    /**
     * Returns true if animations are running which animate layout-related properties. This
     * essentially means that either CHANGE_APPEARING or CHANGE_DISAPPEARING animations
     * are running, since these animations operate on layout-related properties.
     *
     * @return true if CHANGE_APPEARING or CHANGE_DISAPPEARING animations are currently
     * running.
     */
    CARAPI IsChangingLayout(
        /* [out] */ Boolean* changing);

    /**
     * Returns true if any of the animations in this transition are currently running.
     *
     * @return true if any animations in the transition are running.
     */
    CARAPI IsRunning(
        /* [out] */ Boolean* running);

    /**
     * Cancels the currently running transition. Note that we cancel() the changing animations
     * but end() the visibility animations. This is because this method is currently called
     * in the context of starting a new transition, so we want to move things from their mid-
     * transition positions, but we want them to have their end-transition visibility.
     *
     * @hide
     */
    CARAPI Cancel();

    /**
     * Cancels the specified type of transition. Note that we cancel() the changing animations
     * but end() the visibility animations. This is because this method is currently called
     * in the context of starting a new transition, so we want to move things from their mid-
     * transition positions, but we want them to have their end-transition visibility.
     *
     * @hide
     */
    CARAPI Cancel(
        /* [in] */ Int32 transitionType);

    /**
     * This method is called by ViewGroup when there is a call to layout() on the container
     * with this LayoutTransition. If the CHANGING transition is enabled and if there is no other
     * transition currently running on the container, then this call runs a CHANGING transition.
     * The transition does not start immediately; it just sets up the mechanism to run if any
     * of the children of the container change their layout parameters (similar to
     * the CHANGE_APPEARING and CHANGE_DISAPPEARING transitions).
     *
     * @param parent The ViewGroup whose layout() method has been called.
     *
     * @hide
     */
    CARAPI LayoutChange(
        /* [in] */ IViewGroup* parent);

    /**
     * This method is called by ViewGroup when a child view is about to be added to the
     * container. This callback starts the process of a transition; we grab the starting
     * values, listen for changes to all of the children of the container, and start appropriate
     * animations.
     *
     * @param parent The ViewGroup to which the View is being added.
     * @param child The View being added to the ViewGroup.
     */
    CARAPI AddChild(
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* child);

    /**
     * @deprecated Use {@link #showChild(android.view.ViewGroup, android.view.View, Int32)}.
     */
    //@Deprecated
    CARAPI ShowChild(
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* child);

    /**
     * This method is called by ViewGroup when a child view is about to be made visible in the
     * container. This callback starts the process of a transition; we grab the starting
     * values, listen for changes to all of the children of the container, and start appropriate
     * animations.
     *
     * @param parent The ViewGroup in which the View is being made visible.
     * @param child The View being made visible.
     * @param oldVisibility The previous visibility value of the child View, either
     * {@link View#GONE} or {@link View#INVISIBLE}.
     */
    CARAPI ShowChild(
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* child,
        /* [in] */ Int32 oldVisibility);

    /**
     * This method is called by ViewGroup when a child view is about to be removed from the
     * container. This callback starts the process of a transition; we grab the starting
     * values, listen for changes to all of the children of the container, and start appropriate
     * animations.
     *
     * @param parent The ViewGroup from which the View is being removed.
     * @param child The View being removed from the ViewGroup.
     */
    CARAPI RemoveChild(
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* child);

    /**
     * @deprecated Use {@link #hideChild(android.view.ViewGroup, android.view.View, Int32)}.
     */
    //@Deprecated
    CARAPI HideChild(
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* child);

    /**
     * This method is called by ViewGroup when a child view is about to be hidden in
     * container. This callback starts the process of a transition; we grab the starting
     * values, listen for changes to all of the children of the container, and start appropriate
     * animations.
     *
     * @param parent The parent ViewGroup of the View being hidden.
     * @param child The View being hidden.
     * @param newVisibility The new visibility value of the child View, either
     * {@link View#GONE} or {@link View#INVISIBLE}.
     */
    CARAPI HideChild(
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* child,
        /* [in] */ Int32 newVisibility);

    /**
     * Add a listener that will be called when the bounds of the view change due to
     * layout processing.
     *
     * @param listener The listener that will be called when layout bounds change.
     */
    CARAPI AddTransitionListener(
        /* [in] */ ITransitionListener* listener);

    /**
     * Remove a listener for layout changes.
     *
     * @param listener The listener for layout bounds change.
     */
    CARAPI RemoveTransitionListener(
        /* [in] */ ITransitionListener* listener);

    /**
     * Gets the current list of listeners for layout changes.
     * @return
     */
    CARAPI GetTransitionListeners(
        /* [out, callee] */ ArrayOf<ITransitionListener*>** listeners);

    CARAPI GetTransitionListeners(
        /* [out] */ IList** listeners);

private:
    /**
     * This function sets up animations on all of the views that change during layout.
     * For every child in the parent, we create a change animation of the appropriate
     * type (appearing, disappearing, or changing) and ask it to populate its start values from its
     * target view. We add layout listeners to all child views and listen for changes. For
     * those views that change, we populate the end values for those animations and start them.
     * Animations are not run on unchanging views.
     *
     * @param parent The container which is undergoing a change.
     * @param newView The view being added to or removed from the parent. May be null if the
     * changeReason is CHANGING.
     * @param changeReason A value of APPEARING, DISAPPEARING, or CHANGING, indicating whether the
     * transition is occurring because an item is being added to or removed from the parent, or
     * if it is running in response to a layout operation (that is, if the value is CHANGING).
     */
    CARAPI_(void) RunChangeTransition(
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* newView,
        /* [in] */ Int32 changeReason);

    /**
     * Utility function called by runChangingTransition for both the children and the parent
     * hierarchy.
     */
    CARAPI_(void) SetupChangeAnimation(
        /* [in] */ IViewGroup* parent,
        /* [in] */ Int32 changeReason,
        /* [in] */ IAnimator* baseAnimator,
        /* [in] */ Int64 duration,
        /* [in] */ IView* child);

    /**
     * This method runs the animation that makes an added item appear.
     *
     * @param parent The ViewGroup to which the View is being added.
     * @param child The View being added to the ViewGroup.
     */
    CARAPI_(void) RunAppearingTransition(
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* child);

    /**
     * This method runs the animation that makes a removed item disappear.
     *
     * @param parent The ViewGroup from which the View is being removed.
     * @param child The View being removed from the ViewGroup.
     */
    CARAPI_(void) RunDisappearingTransition(
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* child);

    /**
     * This method is called by ViewGroup when a child view is about to be added to the
     * container. This callback starts the process of a transition; we grab the starting
     * values, listen for changes to all of the children of the container, and start appropriate
     * animations.
     *
     * @param parent The ViewGroup to which the View is being added.
     * @param child The View being added to the ViewGroup.
     * @param changesLayout Whether the removal will cause changes in the layout of other views
     * in the container. INVISIBLE views becoming VISIBLE will not cause changes and thus will not
     * affect CHANGE_APPEARING or CHANGE_DISAPPEARING animations.
     */
    CARAPI_(void) AddChild(
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* child,
        /* [in] */ Boolean changesLayout);

    CARAPI_(Boolean) HasListeners();

    /**
     * This method is called by ViewGroup when a child view is about to be removed from the
     * container. This callback starts the process of a transition; we grab the starting
     * values, listen for changes to all of the children of the container, and start appropriate
     * animations.
     *
     * @param parent The ViewGroup from which the View is being removed.
     * @param child The View being removed from the ViewGroup.
     * @param changesLayout Whether the removal will cause changes in the layout of other views
     * in the container. Views becoming INVISIBLE will not cause changes and thus will not
     * affect CHANGE_APPEARING or CHANGE_DISAPPEARING animations.
     */
    CARAPI_(void) RemoveChild(
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* child,
        /* [in] */ Boolean changesLayout);

    static CARAPI_(Boolean) InitStatics();

private:
    /**
     * Private bit fields used to set the collection of enabled transition types for
     * mTransitionTypes.
     */
    static const Int32 FLAG_APPEARING;
    static const Int32 FLAG_DISAPPEARING;
    static const Int32 FLAG_CHANGE_APPEARING;
    static const Int32 FLAG_CHANGE_DISAPPEARING;
    static const Int32 FLAG_CHANGING;

    /**
     * These variables hold the animations that are currently used to run the transition effects.
     * These animations are set to defaults, but can be changed to custom animations by
     * calls to setAnimator().
     */
    AutoPtr<IAnimator> mDisappearingAnim;
    AutoPtr<IAnimator> mAppearingAnim;
    AutoPtr<IAnimator> mChangingAppearingAnim;
    AutoPtr<IAnimator> mChangingDisappearingAnim;
    AutoPtr<IAnimator> mChangingAnim;

    /**
     * These are the default animations, defined in the constructor, that will be used
     * unless the user specifies custom animations.
     */
    static AutoPtr<IObjectAnimator> sDefaultChange;
    static AutoPtr<IObjectAnimator> sDefaultChangeIn;
    static AutoPtr<IObjectAnimator> sDefaultChangeOut;
    static AutoPtr<IObjectAnimator> sDefaultFadeIn;
    static AutoPtr<IObjectAnimator> sDefaultFadeOut;

    /**
     * The default duration used by all animations.
     */
    static Int64 DEFAULT_DURATION;

    /**
     * The durations of the different animations
     */
    Int64 mChangingAppearingDuration;
    Int64 mChangingDisappearingDuration;
    Int64 mChangingDuration;
    Int64 mAppearingDuration;
    Int64 mDisappearingDuration;

    /**
     * The start delays of the different animations. Note that the default behavior of
     * the appearing item is the default duration, since it should wait for the items to move
     * before fading it. Same for the changing animation when disappearing; it waits for the item
     * to fade out before moving the other items.
     */
    Int64 mAppearingDelay;
    Int64 mDisappearingDelay;
    Int64 mChangingAppearingDelay;
    Int64 mChangingDisappearingDelay;
    Int64 mChangingDelay;

    /**
     * The inter-animation delays used on the changing animations
     */
    Int64 mChangingAppearingStagger;
    Int64 mChangingDisappearingStagger;
    Int64 mChangingStagger;

    /**
     * Static interpolators - these are stateless and can be shared across the instances
     */
    static AutoPtr<ITimeInterpolator> ACCEL_DECEL_INTERPOLATOR;
    static AutoPtr<ITimeInterpolator> DECEL_INTERPOLATOR;
    static AutoPtr<ITimeInterpolator> sAppearingInterpolator;
    static AutoPtr<ITimeInterpolator> sDisappearingInterpolator;
    static AutoPtr<ITimeInterpolator> sChangingAppearingInterpolator;
    static AutoPtr<ITimeInterpolator> sChangingDisappearingInterpolator;
    static AutoPtr<ITimeInterpolator> sChangingInterpolator;
    static Boolean sInit;

    /**
     * The default interpolators used for the animations
     */
    AutoPtr<ITimeInterpolator> mAppearingInterpolator;
    AutoPtr<ITimeInterpolator> mDisappearingInterpolator;
    AutoPtr<ITimeInterpolator> mChangingAppearingInterpolator;
    AutoPtr<ITimeInterpolator> mChangingDisappearingInterpolator;
    AutoPtr<ITimeInterpolator> mChangingInterpolator;

    /**
     * These hashmaps are used to store the animations that are currently running as part of
     * the transition. The reason for this is that a further layout event should cause
     * existing animations to stop where they are prior to starting new animations. So
     * we cache all of the current animations in this map for possible cancellation on
     * another layout event. LinkedHashMaps are used to preserve the order in which animations
     * are inserted, so that we process events (such as setting up start values) in the same order.
     */
    HashMap<AutoPtr<IView>, AutoPtr<IAnimator> > mPendingAnimations;

    //TODO LinkedHashMap is same to HashMap?
    //LinkedHashMap<AutoPtr<IView>, AutoPtr<IAnimator> > mCurrentChangingAnimations;
    HashMap<AutoPtr<IView>, AutoPtr<IAnimator> > mCurrentChangingAnimations;

    //TODO LinkedHashMap is same to HashMap?
    // LinkedHashMap<AutoPtr<IView>, AutoPtr<IAnimator> > mCurrentAppearingAnimations;
    HashMap<AutoPtr<IView>, AutoPtr<IAnimator> > mCurrentAppearingAnimations;

    //TODO LinkedHashMap is same to HashMap?
    // LinkedHashMap<AutoPtr<IView>, AutoPtr<IAnimator> > mCurrentDisappearingAnimations;
    HashMap<AutoPtr<IView>, AutoPtr<IAnimator> > mCurrentDisappearingAnimations;

    /**
     * This hashmap is used to track the listeners that have been added to the children of
     * a container. When a layout change occurs, an animation is created for each View, so that
     * the pre-layout values can be cached in that animation. Then a listener is added to the
     * view to see whether the layout changes the bounds of that view. If so, the animation
     * is set with the final values and then run. If not, the animation is not started. When
     * the process of setting up and running all appropriate animations is done, we need to
     * remove these listeners and clear out the map.
     */
    HashMap<AutoPtr<IView>, AutoPtr<IViewOnLayoutChangeListener> > mLayoutChangeListenerMap;

    /**
     * Used to track the current delay being assigned to successive animations as they are
     * started. This value is incremented for each new animation, then zeroed before the next
     * transition begins.
     */
    Int64 mStaggerDelay;

    /**
     * These are the types of transition animations that the LayoutTransition is reacting
     * to. By default, appearing/disappearing and the change animations related to them are
     * enabled (not CHANGING).
     */
    Int32 mTransitionTypes;
    /**
     * The set of listeners that should be notified when APPEARING/DISAPPEARING transitions
     * start and end.
     */
    List<AutoPtr<ITransitionListener> > mListeners;

    /**
     * Controls whether changing animations automatically animate the parent hierarchy as well.
     * This behavior prevents artifacts when wrap_content layouts snap to the end state as the
     * transition begins, causing visual glitches and clipping.
     * Default value is true.
     */
    Boolean mAnimateParentHierarchy;
    typedef HashMap<AutoPtr<IView>, AutoPtr<IAnimator> >::Iterator VAIterator;
    typedef HashMap<AutoPtr<IView>, AutoPtr<IAnimator> > VAHashMap;

};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_CLAYOUTTRANSITION_H__

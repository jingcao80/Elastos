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

#ifndef __ELASTOS_DROID_TRANSITION_TRANSITION_H__
#define __ELASTOS_DROID_TRANSITION_TRANSITION_H__

#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/transition/PathMotion.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IWindowId;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::IInt64SparseArray;

using Elastos::Core::ICloneable;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Transition {

class Transition
    : public Object
    , public ITransition
    , public ICloneable
{
public:
    /**
     * Utility adapter class to avoid having to override all three methods
     * whenever someone just wants to listen for a single event.
     *
     * @hide
     * */
    class TransitionListenerAdapter
        : public Object
        , public ITransitionListener
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI OnTransitionStart(
            /* [in] */ ITransition* transition);

        CARAPI OnTransitionEnd(
            /* [in] */ ITransition* transition);

        CARAPI OnTransitionCancel(
            /* [in] */ ITransition* transition);

        CARAPI OnTransitionPause(
            /* [in] */ ITransition* transition);

        CARAPI OnTransitionResume(
            /* [in] */ ITransition* transition);
    };

    /**
     * Holds information about each animator used when a new transition starts
     * while other transitions are still running to determine whether a running
     * animation should be canceled or a new animation noop'd. The structure holds
     * information about the state that an animation is going to, to be compared to
     * end state of a new animation.
     * @hide
     */
    class AnimationInfo
        : public Object
    {
    public:
        AnimationInfo(
            /* [in] */ IView* view,
            /* [in] */ const String& name,
            /* [in] */ ITransition* transition,
            /* [in] */ IWindowId* windowId,
            /* [in] */ ITransitionValues* values);

    public:
        AutoPtr<IView> mView;
        String mName;
        AutoPtr<ITransitionValues> mValues;
        AutoPtr<IWindowId> mWindowId;
        AutoPtr<ITransition> mTransition;
    };

private:
    /**
     * Utility class for managing typed ArrayLists efficiently. In particular, this
     * can be useful for lists that we don't expect to be used often (eg, the exclude
     * lists), so we'd like to keep them nulled out by default. This causes the code to
     * become tedious, with constant null checks, code to allocate when necessary,
     * and code to null out the reference when the list is empty. This class encapsulates
     * all of that functionality into simple add()/remove() methods which perform the
     * necessary checks, allocation/null-out as appropriate, and return the
     * resulting list.
     */
    class ArrayListManager
        : public Object
    {
    public:
        /**
         * Add the specified item to the list, returning the resulting list.
         * The returned list can either the be same list passed in or, if that
         * list was null, the new list that was created.
         *
         * Note that the list holds unique items; if the item already exists in the
         * list, the list is not modified.
         */
        static CARAPI_(AutoPtr<IArrayList>) Add(
            /* [in] */ IArrayList* list,
            /* [in] */ IInterface* item);

        /**
         * Remove the specified item from the list, returning the resulting list.
         * The returned list can either the be same list passed in or, if that
         * list becomes empty as a result of the remove(), the new list was created.
         */
        static CARAPI_(AutoPtr<IArrayList>) Remove(
            /* [in] */ IArrayList* list,
            /* [in] */ IInterface* item);
    };

    class AnimatorListenerAdapter_1
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter_1();

        AnimatorListenerAdapter_1(
            /* [in] */ IArrayList* curAnimators,
            /* [in] */ IArrayMap* runningAnimators);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    public:
        AutoPtr<IArrayList> mCurAnimators;
        AutoPtr<IArrayMap> mRunningAnimators;
    };

    class AnimatorListenerAdapter_2
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter_2(
            /* [in] */ Transition* owner);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    public:
        Transition* mOwner;
    };

    class PathMotionOverride
        : public PathMotion
    {
    public:
        CARAPI GetPath(
            /* [in] */ Float startX,
            /* [in] */ Float startY,
            /* [in] */ Float endX,
            /* [in] */ Float endY,
            /* [out] */ IPath** result);
    };

public:
    CAR_INTERFACE_DECL()

    Transition();

    /**
     * Constructs a Transition object with no target objects. A transition with
     * no targets defaults to running on all target objects in the scene hierarchy
     * (if the transition is not contained in a TransitionSet), or all target
     * objects passed down from its parent (if it is in a TransitionSet).
     */
    CARAPI constructor();

    /**
     * Perform inflation from XML and apply a class-specific base style from a
     * theme attribute or style resource. This constructor of Transition allows
     * subclasses to use their own base style when they are inflating.
     *
     * @param context The Context the transition is running in, through which it can
     *        access the current theme, resources, etc.
     * @param attrs The attributes of the XML tag that is inflating the transition.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetDuration(
        /* [in] */ Int64 duration);

    CARAPI GetDuration(
        /* [out] */ Int64* dura);

    CARAPI SetStartDelay(
        /* [in] */ Int64 startDelay);

    CARAPI GetStartDelay(
        /* [out] */ Int64* result);

    CARAPI SetInterpolator(
        /* [in] */ ITimeInterpolator* interpolator);

    CARAPI GetInterpolator(
        /* [out] */ ITimeInterpolator** result);

    CARAPI GetTransitionProperties(
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI CreateAnimator(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransitionValues* startValues,
        /* [in] */ ITransitionValues* endValues,
        /* [out] */ IAnimator** result);

    CARAPI SetMatchOrder(
        /* [in] */ ArrayOf<Int32>* matches);

    CARAPI IsValidTarget(
        /* [in] */ IView* target,
        /* [out] */ Boolean* result);

    CARAPI AddTarget(
        /* [in] */ Int32 targetId);

    CARAPI AddTarget(
        /* [in] */ const String& targetName);

    /**
     * Adds the Class of a target view that this Transition is interested in
     * animating. By default, there are no targetTypes, and a Transition will
     * listen for changes on every view in the hierarchy below the sceneRoot
     * of the Scene being transitioned into. Setting targetTypes constrains
     * the Transition to only listen for, and act on, views with these classes.
     * Views with different classes will be ignored.
     *
     * <p>Note that any View that can be cast to targetType will be included, so
     * if targetType is <code>View.class</code>, all Views will be included.</p>
     *
     * @see #addTarget(int)
     * @see #addTarget(android.view.View)
     * @see #excludeTarget(Class, boolean)
     * @see #excludeChildren(Class, boolean)
     *
     * @param targetType The type to include when running this transition.
     * @return The Transition to which the target class was added.
     * Returning the same object makes it easier to chain calls during
     * construction, such as
     * <code>transitionSet.addTransitions(new Fade()).addTarget(ImageView.class);</code>
     */
//    public Transition addTarget(Class targetType);

    CARAPI RemoveTarget(
        /* [in] */ Int32 targetId);

    CARAPI RemoveTarget(
        /* [in] */ const String& targetName);

    CARAPI ExcludeTarget(
        /* [in] */ Int32 targetId,
        /* [in] */ Boolean exclude);

    CARAPI ExcludeTarget(
        /* [in] */ const String& targetName,
        /* [in] */ Boolean exclude);

    CARAPI ExcludeChildren(
        /* [in] */ Int32 targetId,
        /* [in] */ Boolean exclude);

    CARAPI ExcludeTarget(
        /* [in] */ IView* target,
        /* [in] */ Boolean exclude);

    CARAPI ExcludeChildren(
        /* [in] */ IView* target,
        /* [in] */ Boolean exclude);

    /**
     * Whether to add the given type to the list of types to exclude from this
     * transition. The <code>exclude</code> parameter specifies whether the target
     * type should be added to or removed from the excluded list.
     *
     * <p>Excluding targets is a general mechanism for allowing transitions to run on
     * a view hierarchy while skipping target views that should not be part of
     * the transition. For example, you may want to avoid animating children
     * of a specific ListView or Spinner. Views can be excluded either by their
     * id, or by their instance reference, or by the Class of that view
     * (eg, {@link Spinner}).</p>
     *
     * @see #excludeChildren(Class, boolean)
     * @see #excludeTarget(int, boolean)
     * @see #excludeTarget(View, boolean)
     *
     * @param type The type to ignore when running this transition.
     * @param exclude Whether to add the target type to or remove it from the
     * current list of excluded target types.
     * @return This transition object.
     */
//    public Transition excludeTarget(Class type, boolean exclude);

    /**
     * Whether to add the given type to the list of types whose children should
     * be excluded from this transition. The <code>exclude</code> parameter
     * specifies whether the target type should be added to or removed from
     * the excluded list.
     *
     * <p>Excluding targets is a general mechanism for allowing transitions to run on
     * a view hierarchy while skipping target views that should not be part of
     * the transition. For example, you may want to avoid animating children
     * of a specific ListView or Spinner. Views can be excluded either by their
     * id, or by their instance reference, or by the Class of that view
     * (eg, {@link Spinner}).</p>
     *
     * @see #excludeTarget(Class, boolean)
     * @see #excludeChildren(int, boolean)
     * @see #excludeChildren(View, boolean)
     *
     * @param type The type to ignore when running this transition.
     * @param exclude Whether to add the target type to or remove it from the
     * current list of excluded target types.
     * @return This transition object.
     */
//    public Transition excludeChildren(Class type, boolean exclude);

    CARAPI AddTarget(
        /* [in] */ IView* target);

    CARAPI RemoveTarget(
        /* [in] */ IView* target);

    /**
     * Removes the given target from the list of targets that this Transition
     * is interested in animating.
     *
     * @param target The type of the target view, must be non-null.
     * @return Transition The Transition from which the target is removed.
     * Returning the same object makes it easier to chain calls during
     * construction, such as
     * <code>transitionSet.addTransitions(new Fade()).removeTarget(someType);</code>
     */
//    public Transition removeTarget(Class target);

    CARAPI GetTargetIds(
        /* [out] */ IList** result);

    CARAPI GetTargets(
        /* [out] */ IList** result);

    CARAPI GetTargetNames(
        /* [out] */ IList** result);

    CARAPI GetTargetViewNames(
        /* [out] */ IList** result);

    CARAPI GetTargetTypes(
        /* [out] */ IList** result);

    CARAPI CaptureValues(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ Boolean start);

    static CARAPI_(void) AddViewValues(
        /* [in] */ ITransitionValuesMaps* transitionValuesMaps,
        /* [in] */ IView* view,
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI ClearValues(
        /* [in] */ Boolean start);

    CARAPI GetTransitionValues(
        /* [in] */ IView* view,
        /* [in] */ Boolean start,
        /* [out] */ ITransitionValues** result);

    CARAPI GetMatchedTransitionValues(
        /* [in] */ IView* view,
        /* [in] */ Boolean viewInStart,
        /* [out] */ ITransitionValues** result);

    CARAPI Pause(
        /* [in] */ IView* sceneRoot);

    CARAPI Resume(
        /* [in] */ IView* sceneRoot);

    CARAPI PlayTransition(
        /* [in] */ IViewGroup* sceneRoot);

    CARAPI AreValuesChanged(
        /* [in] */ ITransitionValues* oldValues,
        /* [in] */ ITransitionValues* newValues,
        /* [out] */ Boolean* result);

    CARAPI AddListener(
        /* [in] */ ITransitionListener* listener);

    CARAPI RemoveListener(
        /* [in] */ ITransitionListener* listener);

    CARAPI SetEpicenterCallback(
        /* [in] */ IEpicenterCallback* epicenterCallback);

    CARAPI GetEpicenterCallback(
        /* [out] */ IEpicenterCallback** result);

    CARAPI GetEpicenter(
        /* [out] */ IRect** result);

    CARAPI SetPathMotion(
        /* [in] */ IPathMotion* pathMotion);

    CARAPI GetPathMotion(
        /* [out] */ IPathMotion** result);

    CARAPI SetPropagation(
        /* [in] */ ITransitionPropagation* transitionPropagation);

    CARAPI GetPropagation(
        /* [out] */ ITransitionPropagation** result);

    CARAPI CapturePropagationValues(
        /* [in] */ ITransitionValues* transitionValues);

    CARAPI SetSceneRoot(
        /* [in] */ IViewGroup* sceneRoot);

    CARAPI SetCanRemoveViews(
        /* [in] */ Boolean canRemoveViews);

    CARAPI CanRemoveViews(
        /* [out] */ Boolean* result);

    CARAPI SetNameOverrides(
        /* [in] */ IArrayMap* overrides);

    CARAPI GetNameOverrides(
        /* [out] */ IArrayMap** result);

    CARAPI ForceVisibility(
        /* [in] */ Int32 visibility,
        /* [in] */ Boolean isStartValue);

    CARAPI ToString(
        /* [out] */ String* result);

    virtual CARAPI Clone(
        /* [out] */ IInterface** result) = 0;

    CARAPI CloneImpl(
        /* [in] */ ITransition* obj);

    CARAPI GetName(
        /* [out] */ String* result);

    CARAPI ToString(
        /* [in] */ const String& indent,
        /* [out] */ String* res);

    /**
     * This is a utility method used by subclasses to handle standard parts of
     * setting up and running an Animator: it sets the {@link #getDuration()
     * duration} and the {@link #getStartDelay() startDelay}, starts the
     * animation, and, when the animator ends, calls {@link #end()}.
     *
     * @param animator The Animator to be run during this transition.
     *
     * @hide
     */
    CARAPI Animate(
        /* [in] */ IAnimator* animator);

    /**
     * This method is called automatically by the transition and
     * TransitionSet classes prior to a Transition subclass starting;
     * subclasses should not need to call it directly.
     *
     * @hide
     */
    CARAPI Start();

    /**
     * This method is called automatically by the Transition and
     * TransitionSet classes when a transition finishes, either because
     * a transition did nothing (returned a null Animator from
     * {@link Transition#createAnimator(ViewGroup, TransitionValues,
     * TransitionValues)}) or because the transition returned a valid
     * Animator and end() was called in the onAnimationEnd()
     * callback of the AnimatorListener.
     *
     * @hide
     */
    CARAPI End();

    /**
     * This method cancels a transition that is currently running.
     *
     * @hide
     */
    CARAPI Cancel();

    /**
     * This method, essentially a wrapper around all calls to createAnimator for all
     * possible target views, is called with the entire set of start/end
     * values. The implementation in Transition iterates through these lists
     * and calls {@link #createAnimator(ViewGroup, TransitionValues, TransitionValues)}
     * with each set of start/end values on this transition. The
     * TransitionSet subclass overrides this method and delegates it to
     * each of its children in succession.
     *
     * @hide
     */
    CARAPI CreateAnimators(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransitionValuesMaps* startValues,
        /* [in] */ ITransitionValuesMaps* endValues,
        /* [in] */ IArrayList* startValuesList,
        /* [in] */ IArrayList* endValuesList);

    /**
     * This is called internally once all animations have been set up by the
     * transition hierarchy.
     *
     * @hide
     */
    CARAPI RunAnimators();

private:
    static CARAPI_(AutoPtr<ArrayOf<Int32> >) ParseMatchOrder(
        /* [in] */ const String& matchOrderString);

    static CARAPI_(Boolean) IsValidMatch(
        /* [in] */ Int32 match);

    static CARAPI_(Boolean) AlreadyContains(
        /* [in] */ ArrayOf<Int32>* array,
        /* [in] */ Int32 searchIndex);

    /**
     * Match start/end values by View instance. Adds matched values to mStartValuesList
     * and mEndValuesList and removes them from unmatchedStart and unmatchedEnd.
     */
    CARAPI_(void) MatchInstances(
        /* [in] */ IArrayMap* unmatchedStart,
        /* [in] */ IArrayMap* unmatchedEnd);

    /**
     * Match start/end values by Adapter item ID. Adds matched values to mStartValuesList
     * and mEndValuesList and removes them from unmatchedStart and unmatchedEnd, using
     * startItemIds and endItemIds as a guide for which Views have unique item IDs.
     */
    CARAPI_(void) MatchItemIds(
        /* [in] */ IArrayMap* unmatchedStart,
        /* [in] */ IArrayMap* unmatchedEnd,
        /* [in] */ IInt64SparseArray* startItemIds,
        /* [in] */ IInt64SparseArray* endItemIds);

    /**
     * Match start/end values by Adapter view ID. Adds matched values to mStartValuesList
     * and mEndValuesList and removes them from unmatchedStart and unmatchedEnd, using
     * startIds and endIds as a guide for which Views have unique IDs.
     */
    CARAPI_(void) MatchIds(
        /* [in] */ IArrayMap* unmatchedStart,
        /* [in] */ IArrayMap* unmatchedEnd,
        /* [in] */ ISparseArray* startIds,
        /* [in] */ ISparseArray* endIds);

    /**
     * Match start/end values by Adapter transitionName. Adds matched values to mStartValuesList
     * and mEndValuesList and removes them from unmatchedStart and unmatchedEnd, using
     * startNames and endNames as a guide for which Views have unique transitionNames.
     */
    CARAPI_(void) MatchNames(
        /* [in] */ IArrayMap* unmatchedStart,
        /* [in] */ IArrayMap* unmatchedEnd,
        /* [in] */ IArrayMap* startNames,
        /* [in] */ IArrayMap* endNames);

    /**
     * Adds all values from unmatchedStart and unmatchedEnd to mStartValuesList and mEndValuesList,
     * assuming that there is no match between values in the list.
     */
    CARAPI_(void) AddUnmatched(
        /* [in] */ IArrayMap* unmatchedStart,
        /* [in] */ IArrayMap* unmatchedEnd);

    CARAPI_(void) MatchStartAndEnd(
        /* [in] */ ITransitionValuesMaps* startValues,
        /* [in] */ ITransitionValuesMaps* endValues);


    static CARAPI_(AutoPtr<IArrayMap>) GetRunningAnimators();

    CARAPI_(void) RunAnimator(
        /* [in] */ IAnimator* animator,
        /* [in] */ IArrayMap* runningAnimators);

    /**
     * Utility method to manage the boilerplate code that is the same whether we
     * are excluding targets or their children.
     */
    static CARAPI_(AutoPtr<IArrayList>) ExcludeObject(
        /* [in] */ IArrayList* list,
        /* [in] */ IInterface* target,
        /* [in] */ Boolean exclude);

    /**
     * Recursive method which captures values for an entire view hierarchy,
     * starting at some root view. Transitions without targetIDs will use this
     * method to capture values for all possible views.
     *
     * @param view The view for which to capture values. Children of this View
     * will also be captured, recursively down to the leaf nodes.
     * @param start true if values are being captured in the start scene, false
     * otherwise.
     */
    CARAPI_(void) CaptureHierarchy(
        /* [in] */ IView* view,
        /* [in] */ Boolean start);

    static CARAPI_(Boolean) IsValueChanged(
        /* [in] */ ITransitionValues* oldValues,
        /* [in] */ ITransitionValues* newValues,
        /* [in] */ const String& key);

public:
    static Boolean DBG;

    /**
     * With {@link #setMatchOrder(int...)}, chooses to match by View instance.
     */
    const static Int32 MATCH_INSTANCE;

    /**
     * With {@link #setMatchOrder(int...)}, chooses to match by
     * {@link android.view.View#getTransitionName()}. Null names will not be matched.
     */
    const static Int32 MATCH_NAME;

    /**
     * With {@link #setMatchOrder(int...)}, chooses to match by
     * {@link android.view.View#getId()}. Negative IDs will not be matched.
     */
    const static Int32 MATCH_ID;

    /**
     * With {@link #setMatchOrder(int...)}, chooses to match by the {@link android.widget.Adapter}
     * item id. When {@link android.widget.Adapter#hasStableIds()} returns false, no match
     * will be made for items.
     */
    const static Int32 MATCH_ITEM_ID;

    Int64 mStartDelay;
    Int64 mDuration;
    AutoPtr<ITimeInterpolator> mInterpolator;
    AutoPtr<IArrayList> mTargetIds;
    AutoPtr<IArrayList> mTargets;
    AutoPtr<IArrayList> mTargetNames;
    AutoPtr<IArrayList> mTargetTypes;
    AutoPtr<IArrayList> mTargetIdExcludes;
    AutoPtr<IArrayList> mTargetExcludes;
    AutoPtr<IArrayList> mTargetTypeExcludes;
    AutoPtr<IArrayList> mTargetNameExcludes;
    AutoPtr<IArrayList> mTargetIdChildExcludes;
    AutoPtr<IArrayList> mTargetChildExcludes;
    AutoPtr<IArrayList> mTargetTypeChildExcludes;

    AutoPtr<ITransitionSet> mParent;

    AutoPtr<IArrayList> mStartValuesList; // only valid after playTransition starts
    AutoPtr<IArrayList> mEndValuesList; // only valid after playTransitions starts

    // Scene Root is set at createAnimator() time in the cloned Transition
    AutoPtr<IViewGroup> mSceneRoot;

    // Whether removing views from their parent is possible. This is only for views
    // in the start scene, which are no longer in the view hierarchy. This property
    // is determined by whether the previous Scene was created from a layout
    // resource, and thus the views from the exited scene are going away anyway
    // and can be removed as necessary to achieve a particular effect, such as
    // removing them from parents to add them to overlays.
    Boolean mCanRemoveViews;

    // Number of per-target instances of this Transition currently running. This count is
    // determined by calls to start() and end()
    Int32 mNumInstances;

    // Whether this transition is currently paused, due to a call to pause()
    Boolean mPaused;

    // The set of listeners to be sent transition lifecycle events.
    AutoPtr<IArrayList> mListeners;

    // The set of animators collected from calls to createAnimator(),
    // to be run in runAnimators()
    AutoPtr<IArrayList> mAnimators;

    // The function for calculating the Animation start delay.
    AutoPtr<ITransitionPropagation> mPropagation;

    // The rectangular region for Transitions like Explode and TransitionPropagations
    // like CircularPropagation
    AutoPtr<IEpicenterCallback> mEpicenterCallback;

    // For Fragment shared element transitions, linking views explicitly by mismatching
    // transitionNames.
    AutoPtr<IArrayMap> mNameOverrides;

private:
    static const String TAG;

    static const Int32 MATCH_FIRST;
    static const Int32 MATCH_LAST;

    static const String MATCH_INSTANCE_STR;
    static const String MATCH_NAME_STR;
    /** To be removed before L release */
    static const String MATCH_VIEW_NAME_STR;
    static const String MATCH_ID_STR;
    static const String MATCH_ITEM_ID_STR;

    static AutoPtr<ArrayOf<Int32> > DEFAULT_MATCH_ORDER;

    static AutoPtr<IPathMotion> STRAIGHT_PATH_MOTION;

    String mName;
    AutoPtr<ITransitionValuesMaps> mStartValues;
    AutoPtr<ITransitionValuesMaps> mEndValues;
    AutoPtr<ArrayOf<Int32> > mMatchOrder;

    // Per-animator information used for later canceling when future transitions overlap
//    static AutoPtr<IThreadLocal> sRunningAnimators;

    // Track all animators in use in case the transition gets canceled and needs to
    // cancel running animators
    AutoPtr<IArrayList> mCurrentAnimators;

    // Whether this transition has ended. Used to avoid pause/resume on transitions
    // that have completed
    Boolean mEnded;

    // The function used to interpolate along two-dimensional points. Typically used
    // for adding curves to x/y View motion.
    AutoPtr<IPathMotion> mPathMotion;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_TRANSITION_H__

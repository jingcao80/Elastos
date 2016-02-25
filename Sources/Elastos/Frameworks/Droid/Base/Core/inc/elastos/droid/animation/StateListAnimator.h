
#ifndef __ELASTOS_DROID_ANIMATION_STATELISTANIMATOR_H__
#define __ELASTOS_DROID_ANIMATION_STATELISTANIMATOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::View::IView;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Animation {

/**
 * Lets you define a number of Animators that will run on the attached View depending on the View's
 * drawable state.
 * <p>
 * It can be defined in an XML file with the <code>&lt;selector></code> element.
 * Each State Animator is defined in a nested <code>&lt;item></code> element.
 *
 * @attr ref android.R.styleable#DrawableStates_state_focused
 * @attr ref android.R.styleable#DrawableStates_state_window_focused
 * @attr ref android.R.styleable#DrawableStates_state_enabled
 * @attr ref android.R.styleable#DrawableStates_state_checkable
 * @attr ref android.R.styleable#DrawableStates_state_checked
 * @attr ref android.R.styleable#DrawableStates_state_selected
 * @attr ref android.R.styleable#DrawableStates_state_activated
 * @attr ref android.R.styleable#DrawableStates_state_active
 * @attr ref android.R.styleable#DrawableStates_state_single
 * @attr ref android.R.styleable#DrawableStates_state_first
 * @attr ref android.R.styleable#DrawableStates_state_middle
 * @attr ref android.R.styleable#DrawableStates_state_last
 * @attr ref android.R.styleable#DrawableStates_state_pressed
 * @attr ref android.R.styleable#StateListAnimatorItem_animation
 */
class StateListAnimator
    : public Object
    , public IStateListAnimator
{
public:
    /**
     * @hide
     */
    class Tuple: public Object
    {
    public:
        Tuple(
            /* [in] */ ArrayOf<Int32>* specs,
            /* [in] */ IAnimator* animator);

        /**
         * @hide
         */
        CARAPI_(AutoPtr<ArrayOf<Int32> >) GetSpecs();

        /**
         * @hide
         */
        CARAPI_(AutoPtr<IAnimator>) GetAnimator();

    public:
        AutoPtr<ArrayOf<Int32> > mSpecs;

        AutoPtr<IAnimator> mAnimator;
    };


private:
    class AnimatorListener
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListener(
            /* [in] */ StateListAnimator* host);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        StateListAnimator* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    StateListAnimator();

    /**
     * Associates the given animator with the provided drawable state specs so that it will be run
     * when the View's drawable state matches the specs.
     *
     * @param specs The drawable state specs to match against
     * @param animator The animator to run when the specs match
     */
    virtual CARAPI AddState(
        /* [in] */ ArrayOf<Int32>* specs,
        /* [in] */ IAnimator* animator);

    /**
     * Returns the current {@link android.animation.Animator} which is started because of a state
     * change.
     *
     * @return The currently running Animator or null if no Animator is running
     * @hide
     */
    virtual CARAPI GetRunningAnimator(
        /* [out] */ IAnimator** animator);

    /**
     * @hide
     */
    virtual CARAPI GetTarget(
        /* [out] */ IView** view);

    /**
     * Called by View
     * @hide
     */
    virtual CARAPI SetTarget(
        /* [in] */ IView* view);

    /**
     * Called by View
     * @hide
     */
    virtual CARAPI SetState(
        /* [in] */ ArrayOf<Int32>* state);

    /**
     * @hide
     */
    virtual CARAPI GetTuples(
        /* [out] */ IArrayList** list);

    /**
     * If there is an animation running for a recent state change, ends it.
     * <p>
     * This causes the animation to assign the end value(s) to the View.
     */
    virtual CARAPI JumpToCurrentState();

private:
    CARAPI_(void) ClearTarget();

    CARAPI_(void) Start(
        /* [in] */ Tuple* match);

    CARAPI_(void) Cancel();

private:
    List<AutoPtr<Tuple> > mTuples;

    AutoPtr<Tuple> mLastMatch;

    AutoPtr<IAnimator> mRunningAnimator;

    AutoPtr<IWeakReference> mViewRef;
    AutoPtr<AnimatorListener> mAnimatorListener;
};

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_ANIMATION_STATELISTANIMATOR_H__


#ifndef __ELASTOS_DROID_TRANSITION_CTRANSITIONMANAGERHELPER_H__
#define __ELASTOS_DROID_TRANSITION_CTRANSITIONMANAGERHELPER_H__

#include "_Elastos_Droid_Transition_CTransitionManagerHelper.h"
#include "elastos/droid/transition/CTransitionManager.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass(CTransitionManagerHelper)
    , public Singleton
    , public ITransitionManagerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Gets the current default transition. The initial value is an {@link
     * AutoTransition} instance.
     *
     * @return The current default transition.
     * @see #setDefaultTransition(Transition)
     *
     * @hide pending later changes
     */
    CARAPI GetDefaultTransition(
        /* [out] */ ITransition** result);

    /**
     * Convenience method to simply change to the given scene using
     * the default transition for TransitionManager.
     *
     * @param scene The Scene to change to
     */
    CARAPI Go(
        /* [in] */ IScene* scene);

    /**
     * Convenience method to simply change to the given scene using
     * the given transition.
     *
     * <p>Passing in <code>null</code> for the transition parameter will
     * result in the scene changing without any transition running, and is
     * equivalent to calling {@link Scene#exit()} on the scene root's
     * current scene, followed by {@link Scene#enter()} on the scene
     * specified by the <code>scene</code> parameter.</p>
     *
     * @param scene The Scene to change to
     * @param transition The transition to use for this scene change. A
     * value of null causes the scene change to happen with no transition.
     */
    CARAPI Go(
        /* [in] */ IScene* scene,
        /* [in] */ ITransition* transition);

    /**
     * Convenience method to animate, using the default transition,
     * to a new scene defined by all changes within the given scene root between
     * calling this method and the next rendering frame.
     * Equivalent to calling {@link #beginDelayedTransition(ViewGroup, Transition)}
     * with a value of <code>null</code> for the <code>transition</code> parameter.
     *
     * @param sceneRoot The root of the View hierarchy to run the transition on.
     */
    CARAPI BeginDelayedTransition(
        /* [in] */ IViewGroup* sceneRoot);

    /**
     * Convenience method to animate to a new scene defined by all changes within
     * the given scene root between calling this method and the next rendering frame.
     * Calling this method causes TransitionManager to capture current values in the
     * scene root and then post a request to run a transition on the next frame.
     * At that time, the new values in the scene root will be captured and changes
     * will be animated. There is no need to create a Scene; it is implied by
     * changes which take place between calling this method and the next frame when
     * the transition begins.
     *
     * <p>Calling this method several times before the next frame (for example, if
     * unrelated code also wants to make dynamic changes and run a transition on
     * the same scene root), only the first call will trigger capturing values
     * and exiting the current scene. Subsequent calls to the method with the
     * same scene root during the same frame will be ignored.</p>
     *
     * <p>Passing in <code>null</code> for the transition parameter will
     * cause the TransitionManager to use its default transition.</p>
     *
     * @param sceneRoot The root of the View hierarchy to run the transition on.
     * @param transition The transition to use for this change. A
     * value of null causes the TransitionManager to use the default transition.
     */
    CARAPI BeginDelayedTransition(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransition* transition);
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CTRANSITIONMANAGERHELPER_H__

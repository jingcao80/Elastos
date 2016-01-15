
#ifndef __ELASTOS_DROID_TRANSITION_CTRANSITIONMANAGER_H__
#define __ELASTOS_DROID_TRANSITION_CTRANSITIONMANAGER_H__

#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_Transition_CTransitionManager.h"
#include "elastos/droid/transition/Transition.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::View::IViewOnAttachStateChangeListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IView;

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * This class manages the set of transitions that fire when there is a
 * change of {@link Scene}. To use the manager, add scenes along with
 * transition objects with calls to {@link #setTransition(Scene, Transition)}
 * or {@link #setTransition(Scene, Scene, Transition)}. Setting specific
 * transitions for scene changes is not required; by default, a Scene change
 * will use {@link AutoTransition} to do something reasonable for most
 * situations. Specifying other transitions for particular scene changes is
 * only necessary if the application wants different transition behavior
 * in these situations.
 *
 * <p>TransitionManagers can be declared in XML resource files inside the
 * <code>res/transition</code> directory. TransitionManager resources consist of
 * the <code>transitionManager</code>tag name, containing one or more
 * <code>transition</code> tags, each of which describe the relationship of
 * that transition to the from/to scene information in that tag.
 * For example, here is a resource file that declares several scene
 * transitions:</p>
 *
 * {@sample development/samples/ApiDemos/res/transition/transitions_mgr.xml TransitionManager}
 *
 * <p>For each of the <code>fromScene</code> and <code>toScene</code> attributes,
 * there is a reference to a standard XML layout file. This is equivalent to
 * creating a scene from a layout in code by calling
 * {@link Scene#getSceneForLayout(ViewGroup, int, Context)}. For the
 * <code>transition</code> attribute, there is a reference to a resource
 * file in the <code>res/transition</code> directory which describes that
 * transition.</p>
 *
 * Information on XML resource descriptions for transitions can be found for
 * {@link android.R.styleable#Transition}, {@link android.R.styleable#TransitionSet},
 * {@link android.R.styleable#TransitionTarget}, {@link android.R.styleable#Fade},
 * and {@link android.R.styleable#TransitionManager}.
 */
CarClass(CTransitionManager)
    , public Object
    , public ITransitionManager
{
private:
    /**
     * This private utility class is used to listen for both OnPreDraw and
     * OnAttachStateChange events. OnPreDraw events are the main ones we care
     * about since that's what triggers the transition to take place.
     * OnAttachStateChange events are also important in case the view is removed
     * from the hierarchy before the OnPreDraw event takes place; it's used to
     * clean up things since the OnPreDraw listener didn't get called in time.
     */
    class MultiListener
        : public Object
        , public IOnPreDrawListener
        , public IViewOnAttachStateChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        MultiListener(
            /* [in] */ ITransition* transition,
            /* [in] */ IViewGroup* sceneRoot);

        CARAPI OnViewAttachedToWindow(
            /* [in] */ IView* v);

        CARAPI OnViewDetachedFromWindow(
            /* [in] */ IView* v);

        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

    private:
        CARAPI_(void) RemoveListeners();

    public:
        AutoPtr<ITransition> mTransition;
        AutoPtr<IViewGroup> mSceneRoot;
    };

    class TransitionListenerAdapterOverride
        : public Transition::TransitionListenerAdapter
    {
    public:
        TransitionListenerAdapterOverride(
            /* [in] */ IViewGroup* sceneRoot,
            /* [in] */ IArrayMap* runningTransitions);

        CARAPI OnTransitionEnd(
            /* [in] */ ITransition* transition);

    public:
        AutoPtr<IViewGroup> mSceneRoot;
        AutoPtr<IArrayMap> mRunningTransitions;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CTransitionManager();

    CARAPI constructor();

    CARAPI SetDefaultTransition(
        /* [in] */ ITransition* transition);

    static CARAPI_(AutoPtr<ITransition>) GetDefaultTransition();

    CARAPI SetTransition(
        /* [in] */ IScene* scene,
        /* [in] */ ITransition* transition);

    CARAPI SetTransition(
        /* [in] */ IScene* fromScene,
        /* [in] */ IScene* toScene,
        /* [in] */ ITransition* transition);

    CARAPI TransitionTo(
        /* [in] */ IScene* scene);

    static CARAPI_(void) Go(
        /* [in] */ IScene* scene);

    static CARAPI_(void) Go(
        /* [in] */ IScene* scene,
        /* [in] */ ITransition* transition);

    static CARAPI_(void) BeginDelayedTransition(
        /* [in] */ IViewGroup* sceneRoot);

    static CARAPI_(void) BeginDelayedTransition(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransition* transition);

private:
    /**
     * Returns the Transition for the given scene being entered. The result
     * depends not only on the given scene, but also the scene which the
     * {@link Scene#getSceneRoot() sceneRoot} of the Scene is currently in.
     *
     * @param scene The scene being entered
     * @return The Transition to be used for the given scene change. If no
     * Transition was specified for this scene change, the default transition
     * will be used instead.
     */
    CARAPI_(AutoPtr<ITransition>) GetTransition(
        /* [in] */ IScene* scene);

    /**
     * This is where all of the work of a transition/scene-change is
     * orchestrated. This method captures the start values for the given
     * transition, exits the current Scene, enters the new scene, captures
     * the end values for the transition, and finally plays the
     * resulting values-populated transition.
     *
     * @param scene The scene being entered
     * @param transition The transition to play for this scene change
     */
    static CARAPI_(void) ChangeScene(
        /* [in] */ IScene* scene,
        /* [in] */ ITransition* transition);

    static CARAPI_(AutoPtr<IArrayMap>) GetRunningTransitions();

    static CARAPI_(void) SceneChangeRunTransition(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransition* transition);

    static CARAPI_(void) SceneChangeSetup(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ ITransition* transition);

public:
    AutoPtr<IArrayMap> mSceneTransitions;
    AutoPtr<IArrayMap> mScenePairTransitions;

private:
    static const String TAG;

    static AutoPtr<ITransition> sDefaultTransition;

    static AutoPtr<ArrayOf<String> > EMPTY_STRINGS;

//    static AutoPtr<IThreadLocal> sRunningTransitions;
    static AutoPtr<IArrayList> sPendingTransitions;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CTRANSITIONMANAGER_H__

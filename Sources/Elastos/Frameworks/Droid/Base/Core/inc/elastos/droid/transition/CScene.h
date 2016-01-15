
#ifndef __ELASTOS_DROID_TRANSITION_CSCENE_H__
#define __ELASTOS_DROID_TRANSITION_CSCENE_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_Transition_CScene.h"

#include <elastos/core/Object.h>

using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IView;
using Elastos::Droid::Content::IContext;

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * A scene represents the collection of values that various properties in the
 * View hierarchy will have when the scene is applied. A Scene can be
 * configured to automatically run a Transition when it is applied, which will
 * animate the various property changes that take place during the
 * scene change.
 */
CarClass(CScene)
    , public Object
    , public IScene
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CScene();

    /**
     * Constructs a Scene with no information about how values will change
     * when this scene is applied. This constructor might be used when
     * a Scene is created with the intention of being dynamically configured,
     * through setting {@link #setEnterAction(Runnable)} and possibly
     * {@link #setExitAction(Runnable)}.
     *
     * @param sceneRoot The root of the hierarchy in which scene changes
     * and transitions will take place.
     */
    CARAPI constructor(
        /* [in] */ IViewGroup* sceneRoot);

    /**
     * Constructs a Scene which, when entered, will remove any
     * children from the sceneRoot container and add the layout
     * object as a new child of that container.
     *
     * @param sceneRoot The root of the hierarchy in which scene changes
     * and transitions will take place.
     * @param layout The view hierarchy of this scene, added as a child
     * of sceneRoot when this scene is entered.
     */
    CARAPI constructor(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ IView* layout);

    /**
     * @deprecated use {@link #Scene(ViewGroup, View)}.
     */
    CARAPI constructor(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ IViewGroup* layout);

    CARAPI GetSceneRoot(
        /* [out] */ IViewGroup** result);

    CARAPI Exit();

    CARAPI Enter();

    /**
     * Returns a Scene described by the resource file associated with the given
     * <code>layoutId</code> parameter. If such a Scene has already been created for
     * the given <code>sceneRoot</code>, that same Scene will be returned.
     * This caching of layoutId-based scenes enables sharing of common scenes
     * between those created in code and those referenced by {@link TransitionManager}
     * XML resource files.
     *
     * @param sceneRoot The root of the hierarchy in which scene changes
     * and transitions will take place.
     * @param layoutId The id of a standard layout resource file.
     * @param context The context used in the process of inflating
     * the layout resource.
     * @return The scene for the given root and layout id
     */
    static CARAPI_(AutoPtr<IScene>) GetSceneForLayout(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ Int32 layoutId,
        /* [in] */ IContext* context);

    /**
     * Set the scene that the given view is in. The current scene is set only
     * on the root view of a scene, not for every view in that hierarchy. This
     * information is used by Scene to determine whether there is a previous
     * scene which should be exited before the new scene is entered.
     *
     * @param view The view on which the current scene is being set
     */
    static CARAPI_(void) SetCurrentScene(
        /* [in] */ IView* view,
        /* [in] */ IScene* scene);

    /**
     * Gets the current {@link Scene} set on the given view. A scene is set on a view
     * only if that view is the scene root.
     *
     * @return The current Scene set on this view. A value of null indicates that
     * no Scene is currently set.
     */
    static CARAPI_(AutoPtr<IScene>) GetCurrentScene(
        /* [in] */ IView* view);

    CARAPI SetEnterAction(
        /* [in] */ IRunnable* action);

    CARAPI SetExitAction(
        /* [in] */ IRunnable* action);

    CARAPI IsCreatedFromLayoutResource(
        /* [out] */ Boolean* result);

private:
    /**
     * Constructs a Scene which, when entered, will remove any
     * children from the sceneRoot container and will inflate and add
     * the hierarchy specified by the layoutId resource file.
     *
     * <p>This method is hidden because layoutId-based scenes should be
     * created by the caching factory method {@link Scene#getCurrentScene(View)}.</p>
     *
     * @param sceneRoot The root of the hierarchy in which scene changes
     * and transitions will take place.
     * @param layoutId The id of a resource file that defines the view
     * hierarchy of this scene.
     * @param context The context used in the process of inflating
     * the layout resource.
     */
    CARAPI constructor(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ Int32 layoutId,
        /* [in] */ IContext* context);

public:
    AutoPtr<IRunnable> mEnterAction;
    AutoPtr<IRunnable> mExitAction;

private:
    AutoPtr<IContext> mContext;
    Int32 mLayoutId;
    AutoPtr<IViewGroup> mSceneRoot;
    AutoPtr<IView> mLayout; // alternative to layoutId
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CSCENE_H__

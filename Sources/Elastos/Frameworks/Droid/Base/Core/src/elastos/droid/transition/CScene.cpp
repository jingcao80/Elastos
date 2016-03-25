
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/transition/CScene.h"
#include "elastos/droid/utility/SparseArray.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::SparseArray;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::LayoutInflater;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CScene::
//===============================================================
CAR_OBJECT_IMPL(CScene)

CAR_INTERFACE_IMPL(CScene, Object, IScene)

CScene::CScene()
    : mLayoutId(0)
{
}

AutoPtr<IScene> CScene::GetSceneForLayout(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ Int32 layoutId,
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> pScenes;
    IView::Probe(sceneRoot)->GetTag(R::id::scene_layoutid_cache, (IInterface**)&pScenes);
    AutoPtr<ISparseArray> scenes = ISparseArray::Probe(pScenes);
    if (scenes == NULL) {
        scenes = new SparseArray();
        IView::Probe(sceneRoot)->SetTagInternal(R::id::scene_layoutid_cache, scenes);
    }
    AutoPtr<IInterface> p;
    scenes->Get(layoutId, (IInterface**)&p);
    AutoPtr<IScene> scene = IScene::Probe(p);
    if (scene != NULL) {
        return scene;
    }
    else {
        AutoPtr<CScene> s = new CScene();
        s->constructor(sceneRoot, layoutId, context);
        scenes->Put(layoutId, IScene::Probe(s));
        return s;
    }
}

ECode CScene::constructor(
    /* [in] */ IViewGroup* sceneRoot)
{
    mSceneRoot = sceneRoot;
    return NOERROR;
}

ECode CScene::constructor(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ Int32 layoutId,
    /* [in] */ IContext* context)
{
    mContext = context;
    mSceneRoot = sceneRoot;
    mLayoutId = layoutId;
    return NOERROR;
}

ECode CScene::constructor(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IView* layout)
{
    mSceneRoot = sceneRoot;
    mLayout = layout;
    return NOERROR;
}

ECode CScene::constructor(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IViewGroup* layout)
{
    mSceneRoot = sceneRoot;
    mLayout = IView::Probe(layout);
    return NOERROR;
}

ECode CScene::GetSceneRoot(
    /* [out] */ IViewGroup** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSceneRoot;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CScene::Exit()
{
    AutoPtr<IScene> s = GetCurrentScene(IView::Probe(mSceneRoot));
    if (Object::Equals(s->Probe(EIID_IInterface), TO_IINTERFACE(this))) {
        if (mExitAction != NULL) {
            mExitAction->Run();
        }
    }
    return NOERROR;
}

ECode CScene::Enter()
{
    // Apply layout change, if any
    if (mLayoutId > 0 || mLayout != NULL) {
        // empty out parent container before adding to it
        AutoPtr<IViewGroup> sr;
        GetSceneRoot((IViewGroup**)&sr);
        sr->RemoveAllViews();

        if (mLayoutId > 0) {
            AutoPtr<ILayoutInflater> inf;
            LayoutInflater::From(mContext, (ILayoutInflater**)&inf);
            AutoPtr<IView> v;
            inf->Inflate(mLayoutId, mSceneRoot, (IView**)&v);
        }
        else {
            mSceneRoot->AddView(mLayout);
        }
    }

    // Notify next scene that it is entering. Subclasses may override to configure scene.
    if (mEnterAction != NULL) {
        mEnterAction->Run();
    }

    SetCurrentScene(IView::Probe(mSceneRoot), this);
    return NOERROR;
}

void CScene::SetCurrentScene(
    /* [in] */ IView* view,
    /* [in] */ IScene* scene)
{
    view->SetTagInternal(R::id::current_scene, scene);
}

AutoPtr<IScene> CScene::GetCurrentScene(
    /* [in] */ IView* view)
{
    AutoPtr<IInterface> p;
    view->GetTag(R::id::current_scene, (IInterface**)&p);
    return IScene::Probe(p);
}

ECode CScene::SetEnterAction(
    /* [in] */ IRunnable* action)
{
    mEnterAction = action;
    return NOERROR;
}

ECode CScene::SetExitAction(
    /* [in] */ IRunnable* action)
{
    mExitAction = action;
    return NOERROR;
}

ECode CScene::IsCreatedFromLayoutResource(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mLayoutId > 0);
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos

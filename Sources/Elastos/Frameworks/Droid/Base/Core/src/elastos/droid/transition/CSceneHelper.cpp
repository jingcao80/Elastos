
#include "elastos/droid/transition/CSceneHelper.h"
#include "elastos/droid/transition/CScene.h"

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CSceneHelper::
//===============================================================

CAR_SINGLETON_IMPL(CSceneHelper)

CAR_INTERFACE_IMPL(CSceneHelper, Singleton, ISceneHelper)

ECode CSceneHelper::GetSceneForLayout(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ Int32 layoutId,
    /* [in] */ IContext* context,
    /* [out] */ IScene** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IScene> p = CScene::GetSceneForLayout(sceneRoot, layoutId, context);
    *result = p.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSceneHelper::SetCurrentScene(
    /* [in] */ IView* vw,
    /* [in] */ IScene* scene)
{
    CScene::SetCurrentScene(vw, scene);
    return NOERROR;
}

ECode CSceneHelper::GetCurrentScene(
    /* [in] */ IView* view,
    /* [out] */ IScene** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IScene> p = CScene::GetCurrentScene(view);
    *result = p.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos

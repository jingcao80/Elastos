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

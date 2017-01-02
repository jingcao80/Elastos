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

#include "elastos/droid/transition/CTransitionManagerHelper.h"
#include "elastos/droid/transition/CTransitionManager.h"

namespace Elastos {
namespace Droid {
namespace Transition {

CAR_SINGLETON_IMPL(CTransitionManagerHelper)

CAR_INTERFACE_IMPL(CTransitionManagerHelper, Singleton, ITransitionManagerHelper)

ECode CTransitionManagerHelper::GetDefaultTransition(
    /* [out] */ ITransition** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ITransition> t = CTransitionManager::GetDefaultTransition();
    *result = t;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTransitionManagerHelper::Go(
    /* [in] */ IScene* scene)
{
    CTransitionManager::Go(scene);
    return NOERROR;
}

ECode CTransitionManagerHelper::Go(
    /* [in] */ IScene* scene,
    /* [in] */ ITransition* transition)
{
    CTransitionManager::Go(scene, transition);
    return NOERROR;
}

ECode CTransitionManagerHelper::BeginDelayedTransition(
    /* [in] */ IViewGroup* sceneRoot)
{
    CTransitionManager::BeginDelayedTransition(sceneRoot);
    return NOERROR;
}

ECode CTransitionManagerHelper::BeginDelayedTransition(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransition* transition)
{
    CTransitionManager::BeginDelayedTransition(sceneRoot, transition);
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos

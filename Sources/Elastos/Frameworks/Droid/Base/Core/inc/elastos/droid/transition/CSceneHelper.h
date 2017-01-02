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

#ifndef __ELASTOS_DROID_TRANSITION_CSCENEHELPER_H__
#define __ELASTOS_DROID_TRANSITION_CSCENEHELPER_H__

#include "_Elastos_Droid_Transition_CSceneHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Content::IContext;

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass (CSceneHelper)
    , public Singleton
    , public ISceneHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetSceneForLayout(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ Int32 layoutId,
        /* [in] */ IContext* context,
        /* [out] */ IScene** result);

    CARAPI SetCurrentScene(
        /* [in] */ IView* vw,
        /* [in] */ IScene* scene);

    CARAPI GetCurrentScene(
        /* [in] */ IView* view,
        /* [out] */ IScene** result);
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CSCENEHELPER_H__
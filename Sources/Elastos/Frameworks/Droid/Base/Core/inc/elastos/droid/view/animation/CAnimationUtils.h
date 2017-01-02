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

#ifndef __ELASTOS_DROID_VIEW_ANIMATION_CANIMATIONUTILS_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_CANIMATIONUTILS_H__

#include "_Elastos_Droid_View_Animation_CAnimationUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CarClass(CAnimationUtils)
    , public Singleton
    , public IAnimationUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CurrentAnimationTimeMillis(
        /* [out] */ Int64* time);

    CARAPI LoadAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Int32 id,
        /* [out] */ IAnimation** animation);

    CARAPI LoadLayoutAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Int32 id,
        /* [out] */ ILayoutAnimationController** controller);

    CARAPI MakeInAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Boolean fromLeft,
        /* [out] */ IAnimation** animation);

    CARAPI MakeOutAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Boolean toRight,
        /* [out] */ IAnimation** animation);

    CARAPI MakeInChildBottomAnimation(
        /* [in] */ IContext* context,
        /* [out] */ IAnimation** animation);

    CARAPI LoadInterpolator(
        /* [in] */ IContext* context,
        /* [in] */ Int32 id,
        /* [out] */ IInterpolator** interpolator);

    CARAPI LoadInterpolator(
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ Int32 id,
        /* [out] */ IInterpolator** interpolator);
};

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_ANIMATION_CANIMATIONUTILS_H__

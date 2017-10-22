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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/CAnimationUtils.h"
#include "elastos/droid/view/animation/AnimationUtils.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CAR_INTERFACE_IMPL(CAnimationUtils, Singleton, IAnimationUtils)

CAR_SINGLETON_IMPL(CAnimationUtils)

ECode CAnimationUtils::CurrentAnimationTimeMillis(
    /* [out] */ Int64* time)
{
    return AnimationUtils::CurrentAnimationTimeMillis(time);
}

ECode CAnimationUtils::LoadAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Int32 id,
    /* [out] */ IAnimation** animation)
{
    return AnimationUtils::LoadAnimation(context, id, animation);
}

ECode CAnimationUtils::LoadLayoutAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Int32 id,
    /* [out] */ ILayoutAnimationController** controller)
{
    return AnimationUtils::LoadLayoutAnimation(context, id, controller);
}

ECode CAnimationUtils::MakeInAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Boolean fromLeft,
    /* [out] */ IAnimation** animation)
{
    return AnimationUtils::MakeInAnimation(context, fromLeft, animation);
}

ECode CAnimationUtils::MakeOutAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Boolean toRight,
    /* [out] */ IAnimation** animation)
{
    return AnimationUtils::MakeOutAnimation(context, toRight, animation);
}

ECode CAnimationUtils::MakeInChildBottomAnimation(
    /* [in] */ IContext* context,
    /* [out] */ IAnimation** animation)
{
    return AnimationUtils::MakeInChildBottomAnimation(context, animation);
}

ECode CAnimationUtils::LoadInterpolator(
    /* [in] */ IContext* context,
    /* [in] */ Int32 id,
    /* [out] */ IInterpolator** interpolator)
{
    return AnimationUtils::LoadInterpolator(context, id, interpolator);
}

ECode CAnimationUtils::LoadInterpolator(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ Int32 id,
    /* [out] */ IInterpolator** interpolator)
{
    return AnimationUtils::LoadInterpolator(res, theme, id, interpolator);
}

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

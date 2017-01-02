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

#include "elastos/droid/animation/RevealAnimator.h"
#include "elastos/droid/view/CViewAnimationUtilsHelper.h"

using Elastos::Droid::Animation::RevealAnimator;

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CViewAnimationUtilsHelper, Singleton, IViewAnimationUtilsHelper)

CAR_SINGLETON_IMPL(CViewAnimationUtilsHelper)

ECode CViewAnimationUtilsHelper::CreateCircularReveal(
    /* [in] */ IView* view,
    /* [in] */ Int32 centerX,
    /* [in] */ Int32 centerY,
    /* [in] */ Float startRadius,
    /* [in] */ Float endRadius,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)
    VALIDATE_NOT_NULL(view)

    AutoPtr<RevealAnimator> ra = new RevealAnimator(view, centerX, centerY, startRadius, endRadius);
    *result = (IAnimator*)ra.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos

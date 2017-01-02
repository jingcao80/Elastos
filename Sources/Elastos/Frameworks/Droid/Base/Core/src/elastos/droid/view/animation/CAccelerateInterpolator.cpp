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
#include "elastos/droid/view/animation/CAccelerateInterpolator.h"
#include "elastos/droid/internal/view/animation/NativeInterpolatorFactoryHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Internal::View::Animation::EIID_INativeInterpolatorFactory;
using Elastos::Droid::Internal::View::Animation::NativeInterpolatorFactoryHelper;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CAR_OBJECT_IMPL(CAccelerateInterpolator);

CAR_INTERFACE_IMPL_4(CAccelerateInterpolator, Object, IAccelerateInterpolator,
        INativeInterpolatorFactory, IInterpolator, ITimeInterpolator);

CAccelerateInterpolator::CAccelerateInterpolator()
    : mFactor(0.0f)
    , mDoubleFactor(0ll)
{}

CAccelerateInterpolator::~CAccelerateInterpolator()
{}

ECode CAccelerateInterpolator::constructor()
{
    mFactor = 1.0f;
    mDoubleFactor = 2.0;

    return NOERROR;
}

ECode CAccelerateInterpolator::constructor(
    /* [in] */ Float factor)
{
    mFactor = factor;
    mDoubleFactor = 2 * mFactor;

    return NOERROR;
}

ECode CAccelerateInterpolator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IResourcesTheme> theme;
    context->GetTheme((IResourcesTheme**)&theme);
    return constructor(res, theme, attrs);
}

ECode CAccelerateInterpolator::constructor(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::AccelerateInterpolator);
    AutoPtr<ITypedArray> sa;

    if (theme != NULL) {
        theme->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&sa);
    }
    else {
        res->ObtainAttributes(attrs, attrIds, (ITypedArray**)&sa);
    }

    sa->GetFloat(R::styleable::AccelerateInterpolator_factor, 1.0f, &mFactor);
    mDoubleFactor = 2 * mFactor;

    return sa->Recycle();
}

ECode CAccelerateInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output);

    if (mFactor == 1.0f) {
        *output = input * input;
    }
    else {
        *output = (Float)Elastos::Core::Math::Pow(input, mDoubleFactor);
    }

    return NOERROR;
}

ECode CAccelerateInterpolator::CreateNativeInterpolator(
    /* [out] */ Int64* interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    *interpolator = NativeInterpolatorFactoryHelper::CreateAccelerateInterpolator(mFactor);
    return NOERROR;
}

ECode CAccelerateInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = TRUE;
    return NOERROR;
}

}// namespace Animation
}// namespace View
}// namespace Droid
}// namespace Elastos

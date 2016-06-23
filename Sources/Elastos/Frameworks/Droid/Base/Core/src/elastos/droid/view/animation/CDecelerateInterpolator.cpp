
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/CDecelerateInterpolator.h"
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

CAR_OBJECT_IMPL(CDecelerateInterpolator);

CAR_INTERFACE_IMPL_4(CDecelerateInterpolator, Object, IDecelerateInterpolator, \
    INativeInterpolatorFactory, IInterpolator, ITimeInterpolator);

CDecelerateInterpolator::CDecelerateInterpolator()
    : mFactor(1.0f)
{}

CDecelerateInterpolator::~CDecelerateInterpolator()
{}

ECode CDecelerateInterpolator::constructor()
{
    return NOERROR;
}

ECode CDecelerateInterpolator::constructor(
    /* [in] */ Float factor)
{
    mFactor = factor;
    return NOERROR;
}

ECode CDecelerateInterpolator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IResourcesTheme> theme;
    context->GetTheme((IResourcesTheme**)&theme);
    return constructor(res, theme, attrs);
}

ECode CDecelerateInterpolator::constructor(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::DecelerateInterpolator);
    AutoPtr<ITypedArray> a;

    if (theme != NULL) {
        theme->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);
    }
    else {
        res->ObtainAttributes(attrs, attrIds, (ITypedArray**)&a);
    }

    a->GetFloat(R::styleable::DecelerateInterpolator_factor, 1.0f, &mFactor);

    a->Recycle();
    return NOERROR;
}

ECode CDecelerateInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output);

    if (mFactor == 1.0f) {
        *output = 1.0f - (1.0f - input) * (1.0f - input);
    }
    else {
        *output = 1.0f - Elastos::Core::Math::Pow((1.0f - input), 2 * mFactor);
    }

    return NOERROR;
}

ECode CDecelerateInterpolator::CreateNativeInterpolator(
    /* [out] */ Int64* interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    *interpolator = NativeInterpolatorFactoryHelper::CreateDecelerateInterpolator(mFactor);
    return NOERROR;
}

ECode CDecelerateInterpolator::HasNativeInterpolator(
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

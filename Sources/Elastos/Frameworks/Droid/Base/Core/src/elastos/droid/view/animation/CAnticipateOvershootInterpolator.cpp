
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/animation/CAnticipateOvershootInterpolator.h"
#include "elastos/droid/internal/view/animation/NativeInterpolatorFactoryHelper.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Internal::View::Animation::EIID_INativeInterpolatorFactory;
using Elastos::Droid::Internal::View::Animation::NativeInterpolatorFactoryHelper;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CAR_OBJECT_IMPL(CAnticipateOvershootInterpolator);

CAR_INTERFACE_IMPL_4(CAnticipateOvershootInterpolator, Object,
        IAnticipateOvershootInterpolator, INativeInterpolatorFactory,
        IInterpolator, ITimeInterpolator);

CAnticipateOvershootInterpolator::CAnticipateOvershootInterpolator()
    : mTension(0.0f)
{}

CAnticipateOvershootInterpolator::~CAnticipateOvershootInterpolator()
{}

ECode CAnticipateOvershootInterpolator::constructor()
{
    mTension = 2.0f * 1.5f;
    return NOERROR;
}

ECode CAnticipateOvershootInterpolator::constructor(
    /* [in] */ Float tension)
{
    mTension = tension * 1.5f;
    return NOERROR;
}

ECode CAnticipateOvershootInterpolator::constructor(
    /* [in] */ Float tension,
    /* [in] */ Float extraTension)
{
    mTension = tension * extraTension;
    return NOERROR;
}

ECode CAnticipateOvershootInterpolator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IResourcesTheme> theme;
    context->GetTheme((IResourcesTheme**)&theme);
    return constructor(res, theme, attrs);
}

ECode CAnticipateOvershootInterpolator::constructor(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ IAttributeSet* attrs)
{
    Int32 size = ArraySize(R::styleable::AnticipateOvershootInterpolator);
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(size);
    attrIds->Copy(R::styleable::AnticipateOvershootInterpolator, size);
    AutoPtr<ITypedArray> a;

    if (theme != NULL) {
        theme->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);
    } else {
        res->ObtainAttributes(attrs, attrIds, (ITypedArray**)&a);
    }

    Float tension, extraTension;
    a->GetFloat(R::styleable::AnticipateOvershootInterpolator_tension, 2.0f, &tension);
    a->GetFloat(R::styleable::AnticipateOvershootInterpolator_extraTension, 1.5f, &extraTension);

    mTension = tension * extraTension;

    return a->Recycle();
}

static Float A(
    /* [in] */ Float t,
    /* [in] */ Float s)
{
    return t * t * ((s + 1) * t - s);
}

static Float O(
    /* [in] */ Float t,
    /* [in] */ Float s)
{
    return t * t * ((s + 1) * t + s);
}

ECode CAnticipateOvershootInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output);

    // a(t, s) = t * t * ((s + 1) * t - s)
    // o(t, s) = t * t * ((s + 1) * t + s)
    // f(t) = 0.5 * a(t * 2, tension * extraTension), when t < 0.5
    // f(t) = 0.5 * (o(t * 2 - 2, tension * extraTension) + 2), when t <= 1.0
    if (input < 0.5f) {
        *output = 0.5f * A(input * 2.0f, mTension);
    }
    else {
        *output = 0.5f * (O(input * 2.0f - 2.0f, mTension) + 2.0f);
    }

    return NOERROR;
}

ECode CAnticipateOvershootInterpolator::CreateNativeInterpolator(
    /* [out] */ Int64* interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    *interpolator = NativeInterpolatorFactoryHelper::CreateAnticipateOvershootInterpolator(mTension);
    return NOERROR;
}

ECode CAnticipateOvershootInterpolator::HasNativeInterpolator(
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

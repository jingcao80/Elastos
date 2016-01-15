
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/CAnticipateInterpolator.h"
#include "elastos/droid/internal/view/animation/NativeInterpolatorFactoryHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Internal::View::Animation::EIID_INativeInterpolatorFactory;
using Elastos::Droid::Internal::View::Animation::NativeInterpolatorFactoryHelper;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CAR_OBJECT_IMPL(CAnticipateInterpolator);

CAR_INTERFACE_IMPL_3(CAnticipateInterpolator, Object, IAnticipateInterpolator, INativeInterpolatorFactory, IInterpolator);

CAnticipateInterpolator::CAnticipateInterpolator()
    : mTension(0.0f)
{}

CAnticipateInterpolator::~CAnticipateInterpolator()
{}

ECode CAnticipateInterpolator::constructor()
{
    mTension = 2.0f;
    return NOERROR;
}

ECode CAnticipateInterpolator::constructor(
    /* [in] */ Float tension)
{
    mTension = tension;
    return NOERROR;
}

ECode CAnticipateInterpolator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IResourcesTheme> theme;
    context->GetTheme((IResourcesTheme**)&theme);
    return constructor(res, theme, attrs);
}

ECode CAnticipateInterpolator::constructor(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::AnticipateInterpolator),
            ArraySize(R::styleable::AnticipateInterpolator));

    AutoPtr<ITypedArray> a;
    if (theme != NULL) {
        theme->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);
    }
    else {
        res->ObtainAttributes(attrs, attrIds, (ITypedArray**)&a);
    }

    a->GetFloat(R::styleable::AnticipateInterpolator_tension, 2.0f, &mTension);

    return a->Recycle();
}

ECode CAnticipateInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output);
    *output = input * input * ((mTension + 1) * input - mTension);
    return NOERROR;
}

ECode CAnticipateInterpolator::CreateNativeInterpolator(
    /* [out] */ Int64* interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    *interpolator = NativeInterpolatorFactoryHelper::CreateAnticipateInterpolator(mTension);
    return NOERROR;
}

ECode CAnticipateInterpolator::HasNativeInterpolator(
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

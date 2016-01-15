
#include "elastos/droid/animation/RectEvaluator.h"
#include "elastos/droid/graphics/CRect.h"

using Elastos::Droid::Graphics::CRect;

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL(RectEvaluator, Object, ITypeEvaluator);
RectEvaluator::RectEvaluator()
{
}

RectEvaluator::RectEvaluator(
    /* [in] */ IRect* reuseRect)
    : mRect(reuseRect)
{
}

ECode RectEvaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* startValue,
    /* [in] */ IInterface* endValue,
    /* [out] */ IInterface** rect)
{
    VALIDATE_NOT_NULL(rect);
    Int32 v1 = 0, v2 = 0;
    AutoPtr<IRect> realSV = IRect::Probe(startValue);
    AutoPtr<IRect> realEV = IRect::Probe(endValue);

    realSV->GetLeft(&v1);
    realEV->GetLeft(&v2);
    Int32 left = v1 + (Int32) ((v2 - v1) * fraction);

    realSV->GetTop(&v1);
    realEV->GetTop(&v2);
    Int32 top = v1 + (Int32) ((v2 - v1) * fraction);

    realSV->GetRight(&v1);
    realEV->GetRight(&v2);
    Int32 right = v1 + (Int32) ((v2 - v1) * fraction);

    realSV->GetBottom(&v1);
    realEV->GetBottom(&v2);
    Int32 bottom = v1 + (Int32) ((v2 - v1) * fraction);
    if (mRect == NULL) {
        return CRect::New(left, top, right, bottom, (IRect**)rect);
    }

    mRect->Set(left, top, right, bottom);
    *rect = mRect;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

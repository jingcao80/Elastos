
#include "elastos/droid/animation/PointFEvaluator.h"
#include "elastos/droid/graphics/CPointF.h"

using Elastos::Droid::Graphics::CPointF;

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL(PointFEvaluator, Object, ITypeEvaluator);
PointFEvaluator::PointFEvaluator()
{
}

PointFEvaluator::PointFEvaluator(
    /* [in] */ IPointF* reuse)
    : mPoint(reuse)
{
}

ECode PointFEvaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IPointF* startValue,
    /* [in] */ IPointF* endValue,
    /* [out] */ IInterface** pf)
{
    VALIDATE_NOT_NULL(pf);
    Float v1 = 0.f, v2 = 0.f;
    startValue->GetX(&v1);
    endValue->GetX(&v2);
    Float x = v1 + (fraction * (v2 - v1));

    startValue->GetY(&v1);
    endValue->GetY(&v2);
    Float y = v1 + (fraction * (v2 - v1));

    if (mPoint != NULL) {
        mPoint->Set(x, y);
        *pf = mPoint;
        REFCOUNT_ADD(*pf);
        return NOERROR;
    }

    return CPointF::New(x, y, (IPointF**)pf);
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos


#include "elastos/droid/animation/CFloatEvaluator.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Core::IFloat;
using Elastos::Core::CFloat;

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_OBJECT_IMPL(CFloatEvaluator)
CAR_INTERFACE_IMPL_2(CFloatEvaluator, Object, IFloatEvaluator, ITypeEvaluator)

CFloatEvaluator::CFloatEvaluator()
{}

CFloatEvaluator::~CFloatEvaluator()
{}

ECode CFloatEvaluator::constructor()
{
    return NOERROR;
}

ECode CFloatEvaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* startValue,
    /* [in] */ IInterface* endValue,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    assert(startValue != NULL && INumber::Probe(startValue) != NULL);
    assert(endValue != NULL && INumber::Probe(endValue) != NULL);

    Float startFloat = 0.0f, endFloat = 0.0f;
    INumber::Probe(startValue)->FloatValue(&startFloat);
    INumber::Probe(endValue)->FloatValue(&endFloat);

    AutoPtr<IFloat> r;
    CFloat::New(startFloat + fraction * (endFloat - startFloat),
            (IFloat**)&r);
    *result = r;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos


#include "elastos/droid/animation/CFloatEvaluator.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::INumber;

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

    Float startFloat = 0.0f, endFloat = 0.0f;
    INumber::Probe(startValue)->FloatValue(&startFloat);
    INumber::Probe(endValue)->FloatValue(&endFloat);
    Float retValue = startFloat + fraction * (endFloat - startFloat);

    AutoPtr<IFloat> r;
    CFloat::New(retValue, (IFloat**)&r);
    *result = r;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

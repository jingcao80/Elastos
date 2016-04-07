
#include "elastos/droid/animation/FloatArrayEvaluator.h"

using Elastos::Core::CFloat;
using Elastos::Core::IFloat;
using Elastos::Core::EIID_IFloat;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL(FloatArrayEvaluator, Object, ITypeEvaluator);
FloatArrayEvaluator::FloatArrayEvaluator()
{
}

FloatArrayEvaluator::FloatArrayEvaluator(
    /* [in] */ ArrayOf<Float>* reuseArray)
    : mArray(reuseArray)
{
}

ECode FloatArrayEvaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* startValue,
    /* [in] */ IInterface* endValue,
    /* [out] */ IInterface** result)
{
    assert(IArrayOf::Probe(startValue) != NULL && IArrayOf::Probe(endValue) != NULL);

    Int32 length = 0;
    if (mArray == NULL) {
        // array = new int[startValue.length];
        IArrayOf::Probe(startValue)->GetLength(&length);
    } else {
        length = mArray->GetLength();
    }

    AutoPtr<IArrayOf> array;
    CArrayOf::New(EIID_IFloat, length, (IArrayOf**)&array);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> istart, iend;
        IArrayOf::Probe(startValue)->Get(i, (IInterface**)&istart);
        IArrayOf::Probe(endValue)->Get(i, (IInterface**)&iend);
        Float start = 0;
        IFloat::Probe(istart)->GetValue(&start);
        Float end = 0;
        IFloat::Probe(iend)->GetValue(&end);
        Float iv = start + (fraction * (end - start));
        AutoPtr<IFloat> value;
        CFloat::New(iv, (IFloat**)&value);
        array->Set(i, value);
    }

    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

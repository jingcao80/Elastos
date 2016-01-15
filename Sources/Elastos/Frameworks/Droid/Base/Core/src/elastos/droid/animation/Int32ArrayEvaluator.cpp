
#include "elastos/droid/animation/Int32ArrayEvaluator.h"

using Elastos::Core::CArrayOf;
using Elastos::Core::CInteger32;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::IArrayOf;

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL(Int32ArrayEvaluator, Object, ITypeEvaluator);
Int32ArrayEvaluator::Int32ArrayEvaluator()
{
}

Int32ArrayEvaluator::Int32ArrayEvaluator(
    /* [in] */ ArrayOf<Int32>* reuseArray)
    : mArray(reuseArray)
{
}

ECode Int32ArrayEvaluator::Evaluate(
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
    CArrayOf::New(EIID_IInteger32, length, (IArrayOf**)&array);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInteger32> istart, iend;
        IArrayOf::Probe(startValue)->Get(i, (IInterface**)&istart);
        IArrayOf::Probe(endValue)->Get(i, (IInterface**)&iend);
        Int32 start = 0;
        istart->GetValue(&start);
        Int32 end = 0;
        iend->GetValue(&end);
        Int32 iv = (Int32) (start + (fraction * (end - start)));
        AutoPtr<IInteger32> value;
        CInteger32::New(iv, (IInteger32**)&value);
        array->Set(i, value);
    }

    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

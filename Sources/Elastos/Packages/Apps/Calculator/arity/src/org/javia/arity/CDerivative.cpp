
#include "org/javia/arity/CDerivative.h"
#include "org/javia/arity/CComplex.h"

using Org::Javia::Arity::EIID_IDerivative;

namespace Org {
namespace Javia {
namespace Arity {

const Double CDerivative::H;
const Double CDerivative::INVH;

CDerivative::CDerivative()
{
    CComplex::New(o, (IComplex**)&mC);
}

CAR_OBJECT_IMPL(CDerivative)

CAR_INTERFACE_IMPL(CDerivative, Function, IDerivative)

ECode CDerivative::constructor(
    /* [in] */ IFunction* f)
{
    mF = f;
    return f->CheckArity(1);
}

ECode CDerivative::Eval(
    /* [in] */ Double x,
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    mC->Set(x, H);
    Double d;
    mF->Eval(mC, &d);
    AutoPtr<IComplex> c;
    FAIL_RETURN(f->Eval(mC, (IComplex**)&c))
    *result = ((CComplex*)c.Get())->mIm * INVH;
    return NOERROR;
}

ECode CDerivative::Arity(
    /* [out] */ Int32* arity)
{
    VALIDATE_NOT_NULL(arity)
    *arity = 1;
    return NOERROR;
}

} // namespace Arity
} // namespace Javia
} // namespace Org
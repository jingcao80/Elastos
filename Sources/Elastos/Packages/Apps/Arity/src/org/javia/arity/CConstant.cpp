
#include "org/javia/arity/CConstant.h"
#include "org/javia/arity/CComplex.h"

using Org::Javia::Arity::EIID_IConstant;

namespace Org {
namespace Javia {
namespace Arity {

CAR_OBJECT_IMPL(CConstant)

CAR_INTERFACE_IMPL(CConstant, Function, IConstant)

ECode CConstant::constructor(
    /* [in] */ IComplex* o)
{
    CComplex::New(o, (IComplex**)&mValue);
    return NOERROR;
}

ECode CConstant::EvalComplex(
    /* [out] */ IComplex** complex)
{
    VALIDATE_NOT_NULL(complex)
    *complex = mValue;
    REFCOUNT_ADD(*complex)
    return NOERROR;
}

ECode CConstant::Eval(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    return mValue->AsReal(result);
}

ECode CConstant::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return IObject::Probe(mValue)->ToString(str);
}

ECode CConstant::Arity(
    /* [out] */ Int32* arity)
{
    VALIDATE_NOT_NULL(arity)
    *arity = 0;
    return NOERROR;
}

} // namespace Arity
} // namespace Javia
} // namespace Org
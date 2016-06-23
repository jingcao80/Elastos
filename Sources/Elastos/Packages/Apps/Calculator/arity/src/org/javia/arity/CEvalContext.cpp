
#include "org/javia/arity/CEvalContext.h"
#include "org/javia/arity/CComplex.h"

using Org::Javia::Arity::EIID_IEvalContext;

namespace Org {
namespace Javia {
namespace Arity {

const Int32 CEvalContext::MAX_STACK_SIZE;

CEvalContext::CEvalContext()
    : mStackBase(0)
{
    mStackRe = ArrayOf<Double>::Alloc(MAX_STACK_SIZE);
    AutoPtr<ArrayOf<IComplex*> > mStackComplex = ArrayOf<IComplex*>::Alloc(MAX_STACK_SIZE);
    Int32 mStackBase;

    AutoPtr<ArrayOf<Double> > mArgs1 = ArrayOf<Double>::Alloc(1);
    AutoPtr<ArrayOf<Double> > mArgs2 = ArrayOf<Double>::Alloc(2);
}

CAR_OBJECT_IMPL(CEvalContext)

CAR_INTERFACE_IMPL(CEvalContext, Object, IEvalContext)

ECode CEvalContext::constructor()
{
    for (Int32 i = 0; i < MAX_STACK_SIZE; ++i) {
        AutoPtr<IComplex> c;
        CComplex::New((IComplex**)&c);
        mStackComplex->Set(i, c);
    }
    mArgs1c = ArrayOf<IComplex*>::Alloc(1);
    AutoPtr<IComplex> c1, c2, c3;
    CComplex::New((IComplex**)&c1);
    mArgs1c->Set(0, c1);
    mArgs2c = ArrayOf<IComplex*>::Alloc(2);
    AutoPtr<IComplex> c1, c2, c3;
    CComplex::New((IComplex**)&c2);
    CComplex::New((IComplex**)&c3);
    mArgs2c->Set(0, c2);
    mArgs2c->Set(1, c3);
    return NOERROR;
}

} // namespace Arity
} // namespace Javia
} // namespace Org
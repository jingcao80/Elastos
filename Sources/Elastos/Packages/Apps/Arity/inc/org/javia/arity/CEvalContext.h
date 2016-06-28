
#ifndef _ORG_JAVIA_ARITY_CEVALCONTEXT_H__
#define _ORG_JAVIA_ARITY_CEVALCONTEXT_H__

#include "_Org_Javia_Arity_CEvalContext.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

namespace Org {
namespace Javia {
namespace Arity {

/**
   To evaluate CompiledFunctions from multiple threads in parallel,
   you need to create one EvalContext instance per thread,
   and pass it to the eval() methods of CompiledFunction.
 */
CarClass(CEvalContext)
    , public Object
    , public IEvalContext
{
public:
    CEvalContext();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

public:
    static const Int32 MAX_STACK_SIZE = 128; //if stack ever grows above this likely something is wrong
    AutoPtr<ArrayOf<Double> > mStackRe;
    AutoPtr<ArrayOf<IComplex*> > mStackComplex;
    Int32 mStackBase;

    AutoPtr<ArrayOf<Double> > mArgs1;
    AutoPtr<ArrayOf<Double> > mArgs2;
    AutoPtr<ArrayOf<IComplex*> > mArgs1c;
    AutoPtr<ArrayOf<IComplex*> > mArgs2c;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_CEVALCONTEXT_H__
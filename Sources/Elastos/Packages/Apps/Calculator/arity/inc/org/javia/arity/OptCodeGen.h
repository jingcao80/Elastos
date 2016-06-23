
#ifndef _ORG_JAVIA_ARITY_OPTCODEGEN_H__
#define _ORG_JAVIA_ARITY_OPTCODEGEN_H__

#include "org/javia/arity/SimpleCodeGen.h"
#include "org/javia/arity/EvalContext.h"
#include "org/javia/arity/CCompiledFunction.h"

namespace Org {
namespace Javia {
namespace Arity {

/* Optimizing Code Generator
   Reads tokens in RPN (Reverse Polish Notation) order,
   and generates VM opcodes,
   doing constant-folding optimization.
 */
class OptCodeGen : public SimpleCodeGen
{
public:
    OptCodeGen();

    CARAPI_(void) Start();

    CARAPI Push(
        /* [in] */ Token* token);

    CARAPI_(AutoPtr<CCompiledFunction>) GetFun(
        /* [in] */ Int32 arity);

public:
    AutoPtr<EvalContext> mContext;
    Int32 mSp;
    AutoPtr<ArrayOf<IComplex*> > mStack;

    AutoPtr<ArrayOf<Double> > mTraceConstsRe;
    AutoPtr<ArrayOf<Double> > mTraceConstsIm;
    AutoPtr<ArrayOf<IFunction*> > mTraceFuncs;
    AutoPtr<ArrayOf<Byte> > mTraceCode;
    AutoPtr<CCompiledFunction> mTracer;

    Int32 mIntrinsicArity;

private:
    Boolean mIsPercent;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_OPTCODEGEN_H__

#include "org/javia/arity/Compiler.h"
#include "org/javia/arity/Lexer.h"
#include "org/javia/arity/CConstant.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Org {
namespace Javia {
namespace Arity {

Compiler::Compiler()
{
    mLexer = new Lexer();
    mRpn = new RPN();
    mDeclParser = new DeclarationParser();
    mCodeGen = new OptCodeGen();
    mSimpleCodeGen = new SimpleCodeGen();
    mDecl   = new Declaration();
}

ECode Compiler::CompileSimple(
    /* [in] */ CSymbols* symbols,
    /* [in] */ const String& expression,
    /* [out] */ IFunction** func)
{
    VALIDATE_NOT_NULL(func)
    *func = NULL;
    mSimpleCodeGen->SetSymbols(symbols)
    mRpn->SetConsumer((TokenConsumer*)mSimpleCodeGen.Get());
    FAIL_RETURN(mLexer->Scan(expression, (TokenConsumer*)mRpn.Get()))
    *func = IFunction::Probe(mSimpleCodeGen->GetFun());
    REFCOUNT_ADD(*func)
    return NOERROR;
}

ECode Compiler::Compile(
    /* [in] */ CSymbols* symbols,
    /* [in] */ const String& source,
    /* [out] */ IFunction** func)
{
    VALIDATE_NOT_NULL(func)
    *func = NULL;
    AutoPtr<IFunction> fun;
    mDecl->Parse(source, mLexer, mDeclParser);
    if (decl->mArity == DeclarationParser::UNKNOWN_ARITY) {
        // try {
        AutoPtr<IFunction> f;
        FAIL_RETURN(CompileSimple(symbols, mDecl->mExpression, (IFunction**)&f))
        AutoPtr<IComplex> complex;
        FAIL_RETURN(f->EvalComplex((IComplex**)&complex))
        CConstant::New(complex, (IFunction**)&fun);
        // } catch (SyntaxException e) {
        //     if (e != SimpleCodeGen.HAS_ARGUMENTS) {
        //         throw e;
        //     }
        //     // fall-through (see below)
        // }
    }

    if (fun == NULL) {
        // either decl.arity was set, or an HAS_ARGUMENTS exception ocurred above
        symbols->PushFrame();
        symbols->AddArguments(mDecl->mArgs);
        // try {
        mCodeGen->SetSymbols(symbols);
        mRpn->SetConsumer(mCodeGen);
        mLexer->Scan(mDecl->Expression, mRpn);
        // } finally {
        //     symbols.popFrame();
        // }
        symbols->PopFrame();
        Int32 arity = mDecl->mArity;
        if (arity == DeclarationParser::UNKNOWN_ARITY) {
            arity = mCodeGen->mIntrinsicArity;
        }
        fun = IFunction::Probe(mCodeGen->GetFun(arity));
    }
    ((Function*)fun.Get())->mComment = source;
    *func = fun;
    REFCOUNT_ADD(*func)
    return NOERROR;
}

ECode Compiler::CompileWithName(
    /* [in] */ CSymbols* symbols,
    /* [in] */ const String& source,
    /* [out] */ IFunctionAndName** func)
{
    VALIDATE_NOT_NULL(func)
    *func = NULL;
    AutoPtr<IFunction> f;
    FAIL_RETURN(Compile(symbols, source, (IFunction**)&f))
    return CFunctionAndName::New(f, mDecl->mName, func);
}

} // namespace Arity
} // namespace Javia
} // namespace Org
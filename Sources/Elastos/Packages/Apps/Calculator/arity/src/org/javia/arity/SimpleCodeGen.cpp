
#include "org/javia/arity/SimpleCodeGen.h"
#include "org/javia/arity/VM.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Org {
namespace Javia {
namespace Arity {

SimpleCodeGen::SimpleCodeGen()
{
    mCode = new ByteStack();
    mConsts  = new DoubleStack();
    mFuncs = new FunctionStack();
}

void SimpleCodeGen::SetSymbols(
    /* [in] */ CSymbols* symbols)
{
    mSymbols = symbols;
}

void SimpleCodeGen::Start()
{
    mCode->Clear();
    mConsts->Clear();
    mFuncs->Clear();
}

ECode SimpleCodeGen::GetSymbol(
    /* [in] */ Token* token,
    /* [out] */ Symbol** _symbol)
{
    VALIDATE_NOT_NULL(_symbol)
    *_symbol = NULL;
    String name = token->mName;
    Boolean isDerivative = token->IsDerivative();
    if (isDerivative) {
        if (token->mArity == 1) {
            name = name.Substring(0, name.GetLength() - 1);
        }
        else {
            Slogger::E("SimpleCodeGen", "Derivative expects arity 1 but found %d %d", token->mArity, token->mPosition);
            return E_SYNTAX_EXCEPTION;
        }
    }
    AutoPtr<Symbol> symbol = (Symbol*)mSymbols->Lookup(name, token->mArity).Get();
    if (symbol == NULL) {
        Slogger::E("SimpleCodeGen", "undefined '%s' with arity %d %d", name.string(), token->mArity, token->mPosition);
        return E_SYNTAX_EXCEPTION;
    }
    if (isDerivative && symbol->mOp > 0 && symbol->mFun == NULL) {
        symbol->mFun = NULL;
        CompiledFunction::MakeOpFunction(symbol->mOp, (IFunction**)&(symbol->mFun));
    }
    if (isDerivative && symbol->mFun == NULL) {
        Slogger::E("SimpleCodeGen", "Invalid derivative %s %d", name.string(), token->mPosition);
        return E_SYNTAX_EXCEPTION;
    }
    *_symbol = symbol;
    REFCOUNT_ADD(*_symbol)
    return NOERROR;
}

ECode SimpleCodeGen::Push(
    /* [in] */ Token* token)
{
    Byte op;
    switch (token->mId) {
        case Lexer::NUMBER:
            op = VM::_CONST;
            mConsts->Push(token->mValue, 0);
            break;

        case Lexer::_CONST:
        case Lexer::CALL: {
            AutoPtr<Symbol> symbol;
            FAIL_RETURN(GetSymbol(token, (Symbol**)&symbol))
            if (token->IsDerivative()) {
                op = VM::CALL;
                AutoPtr<IFunction> fun;
                symbol->mFun->GetDerivative((IFunction**)&fun);
                mFuncs->Push(fun);
            }
            else if (symbol->mOp > 0) { // built-in
                op = symbol->mOp;
                if (op >= VM::LOAD0 && op <= VM::LOAD4) {
                    Slogger::E("SimpleCodeGen", "eval() on implicit function");
                    return E_SYNTAX_EXCEPTION;
                }
            }
            else if (symbol->mFun != NULL) { // function call
                op = VM::CALL;
                mFuncs->Push(symbol->mFun);
            }
            else { // variable reference
                op = VM::_CONST;
                mConsts->Push(symbol->mValueRe, symbol->mValueIm);
            }
            break;
        }

        default:
            op = token->mVmop;
            if (op <= 0) {
                Slogger::E("SimpleCodeGen", "wrong vmop: %d, id %d", op, token->mId);
                return E_SYNTAX_EXCEPTION;
            }
    }
    mCode->Push(op);
    return NOERROR;
}

AutoPtr<CCompiledFunction> SimpleCodeGen::GetFun()
{
    AutoPtr<CCompiledFunction> func;
    CCompiledFunction::NewByFriend(0, mCode->ToArray(),
            mConsts->GetRe(), mConsts->GetIm(), mFuncs->ToArray(), (CCompiledFunction**)&func);
    return func;
}

} // namespace Arity
} // namespace Javia
} // namespace Org
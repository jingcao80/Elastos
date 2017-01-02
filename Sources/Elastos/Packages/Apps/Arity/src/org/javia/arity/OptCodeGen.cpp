//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "org/javia/arity/OptCodeGen.h"
#include "org/javia/arity/CEvalContext.h"
#include "org/javia/arity/Lexer.h"
#include "org/javia/arity/VM.h"
#include "org/javia/arity/CComplex.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Org {
namespace Javia {
namespace Arity {

OptCodeGen::OptCodeGen()
    : SimpleCodeGen()
    , mSp(0)
    , mIntrinsicArity(0)
    , mIsPercent(FALSE)
{
    CEvalContext::NewByFriend((CEvalContext**)&mContext);
    mStack = mContext->mStackComplex;
    mTraceConstsRe = ArrayOf<Double>::Alloc(1);
    mTraceConstsIm = ArrayOf<Double>::Alloc(1);
    mTraceFuncs = ArrayOf<IFunction*>::Alloc(1);
    mTraceCode = ArrayOf<Byte>::Alloc(1);
    CCompiledFunction::NewByFriend(0, mTraceCode, mTraceConstsRe, mTraceConstsIm, mTraceFuncs,
            (CCompiledFunction**)&mTracer);
}

void OptCodeGen::Start()
{
    SimpleCodeGen::Start();
    mSp = -1;
    mIntrinsicArity = 0;
    mIsPercent = FALSE;
}

ECode OptCodeGen::Push(
    /* [in] */ Token* token)
{
    // System.err.println("state " + getFun(0) + "; token " + token);
    Boolean prevWasPercent = mIsPercent;
    mIsPercent = FALSE;
    Byte op;
    switch (token->mId) {
    case Lexer::NUMBER:
        op = VM::_CONST;
        (*mTraceConstsRe)[0] = token->mValue;
        (*mTraceConstsIm)[0] = 0;
        break;

    case Lexer::_CONST:
    case Lexer::CALL: {
        AutoPtr<Symbol> symbol;
        FAIL_RETURN(GetSymbol(token, (Symbol**)&symbol))
        if (token->IsDerivative()) {
            op = VM::CALL;
            AutoPtr<IFunction> temp;
            symbol->mFun->GetDerivative((IFunction**)&temp);
            mTraceFuncs->Set(0, temp);
        }
        else if (symbol->mOp > 0) { // built-in
            op = symbol->mOp;
            if (op >= VM::LOAD0 && op <= VM::LOAD4) {
                Int32 arg = op - VM::LOAD0;
                if (arg + 1 > mIntrinsicArity) {
                    mIntrinsicArity = arg + 1;
                }
                ((CComplex*)(*mStack)[++mSp])->mRe = Elastos::Core::Math::DOUBLE_NAN;
                ((CComplex*)(*mStack)[mSp])->mIm = 0;
                mCode->Push(op);
                //System.out.println("op " + VM.opcodeName[op] + "; sp " + sp + "; top " + stack[sp]);
                return NOERROR;
            }
        }
        else if (symbol->mFun != NULL) { // function call
            op = VM::CALL;
            mTraceFuncs->Set(0, symbol->mFun);
        }
        else { // variable reference
            op = VM::_CONST;
            (*mTraceConstsRe)[0] = symbol->mValueRe;
            (*mTraceConstsIm)[0] = symbol->mValueIm;
        }
        break;
    }

    default:
        op = token->mVmop;
        if (op <= 0) {
            Slogger::E("OptCodeGen", "wrong vmop: %d", op);
            return E_SYNTAX_EXCEPTION;
        }
        if (op == VM::PERCENT) {
            mIsPercent = TRUE;
        }
    }
    (*mTraceCode)[0] = op;
    if (op != VM::RND) {
        mTracer->ExecWithoutCheckComplex(mContext, mSp, prevWasPercent ? -1 : -2, &mSp);
    }
    else {
        ((CComplex*)(*mStack)[++mSp])->mRe = Elastos::Core::Math::DOUBLE_NAN;
        ((CComplex*)(*mStack)[mSp])->mIm = 0;
    }

    //System.out.println("op " + VM.opcodeName[op] + "; old " + oldSP + "; sp " + sp + "; top " + stack[sp] + " " + stack[0]);

    //constant folding
    Boolean isNaN;
    if (((*mStack)[mSp]->IsNaN(&isNaN), !isNaN) || op == VM::_CONST) {
        Int32 nPopCode;
        if (op == VM::CALL) {
            (*mTraceFuncs)[0]->Arity(&nPopCode);
        }
        else {
            nPopCode = (*VM::ARITY)[op];
        }
        while (nPopCode > 0) {
            Byte pop = mCode->Pop();
            if (pop == VM::_CONST) {
                mConsts->Pop();
            }
            else if (pop == VM::CALL) {
                AutoPtr<IFunction> f = mFuncs->Pop();
                Int32 arity;
                f->Arity(&arity);
                nPopCode += arity - 1;
            }
            else {
                nPopCode += (*VM::ARITY)[pop];
            }
            --nPopCode;
        }
        mConsts->Push(((CComplex*)(*mStack)[mSp])->mRe, ((CComplex*)(*mStack)[mSp])->mIm);
        op = VM::_CONST;
    }
    else if (op == VM::CALL) {
        mFuncs->Push((*mTraceFuncs)[0]);
    }
    mCode->Push(op);
    return NOERROR;
}

AutoPtr<CCompiledFunction> OptCodeGen::GetFun(
    /* [in] */ Int32 arity)
{
    AutoPtr<CCompiledFunction> func;
    CCompiledFunction::NewByFriend(arity, mCode->ToArray(), mConsts->GetRe(),
            mConsts->GetIm(), mFuncs->ToArray(), (CCompiledFunction**)&func);
    return func;
}

} // namespace Arity
} // namespace Javia
} // namespace Org
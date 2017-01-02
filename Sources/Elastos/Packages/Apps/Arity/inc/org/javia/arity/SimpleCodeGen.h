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

#ifndef _ORG_JAVIA_ARITY_SIMPLECODEGEN_H__
#define _ORG_JAVIA_ARITY_SIMPLECODEGEN_H__

#include "org/javia/arity/TokenConsumer.h"
#include "org/javia/arity/Token.h"
#include "org/javia/arity/ByteStack.h"
#include "org/javia/arity/DoubleStack.h"
#include "org/javia/arity/FunctionStack.h"
#include "org/javia/arity/CSymbols.h"
#include "org/javia/arity/Symbol.h"
#include "org/javia/arity/CCompiledFunction.h"

namespace Org {
namespace Javia {
namespace Arity {

/* Non-optimizing Code Generator
   Reads tokens in RPN (Reverse Polish Notation) order,
   and generates VM opcodes,
   without any optimization.
 */
class SimpleCodeGen : public TokenConsumer
{
public:
    SimpleCodeGen();

    virtual ~SimpleCodeGen() {}

    CARAPI_(void) SetSymbols(
        /* [in] */ CSymbols* symbols);

    CARAPI_(void) Start();

    CARAPI GetSymbol(
        /* [in] */ Token* token,
        /* [out] */ Symbol** symbol);

    CARAPI Push(
        /* [in] */ Token* token);

    CARAPI_(AutoPtr<CCompiledFunction>) GetFun();

public:
    AutoPtr<ByteStack> mCode;
    AutoPtr<DoubleStack> mConsts;
    AutoPtr<FunctionStack> mFuncs;

    //String argNames[];
    AutoPtr<CSymbols> mSymbols;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_SIMPLECODEGEN_H__
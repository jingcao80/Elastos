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

#ifndef _ORG_JAVIA_ARITY_COMPILER_H__
#define _ORG_JAVIA_ARITY_COMPILER_H__

#include "org/javia/arity/Lexer.h"
#include "org/javia/arity/RPN.h"
#include "org/javia/arity/DeclarationParser.h"
#include "org/javia/arity/Declaration.h"

using Elastos::Utility::IStack;

namespace Org {
namespace Javia {
namespace Arity {

class SimpleCodeGen;
class OptCodeGen;
class CSymbols;

/**
   Compiles a textual arithmetic expression to a {@link Function}.<p>
*/
class Compiler : public Object
{
public:
    Compiler();

    CARAPI CompileSimple(
        /* [in] */ CSymbols* symbols,
        /* [in] */ const String& expression,
        /* [out] */ IFunction** func);

    CARAPI Compile(
        /* [in] */ CSymbols* symbols,
        /* [in] */ const String& source,
        /* [out] */ IFunction** func);

    CARAPI CompileWithName(
        /* [in] */ CSymbols* symbols,
        /* [in] */ const String& source,
        /* [out] */ IFunctionAndName** func);

private:
    AutoPtr<Lexer> mLexer;
    AutoPtr<RPN> mRpn;
    AutoPtr<DeclarationParser> mDeclParser;
    AutoPtr<OptCodeGen> mCodeGen;
    AutoPtr<SimpleCodeGen> mSimpleCodeGen;
    AutoPtr<Declaration> mDecl;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_COMPILER_H__
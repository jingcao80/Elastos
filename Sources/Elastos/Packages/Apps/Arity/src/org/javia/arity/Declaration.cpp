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

#include "org/javia/arity/Declaration.h"

namespace Org {
namespace Javia {
namespace Arity {

static AutoPtr<ArrayOf<String> > InitArgs()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(0);
    return args;
}
const AutoPtr<ArrayOf<String> > Declaration::NO_ARGS = InitArgs();

ECode Declaration::Parse(
    /* [in] */ const String& source,
    /* [in] */ Lexer* lexer,
    /* [in] */ DeclarationParser* declParser)
{
    Int32 equalPos = source.IndexOf('=');
    String decl;

    if (equalPos == -1) {
        decl = String(NULL);
        mExpression = source;
        mName = String(NULL);
        mArgs = NO_ARGS;
        mArity = DeclarationParser::UNKNOWN_ARITY;
    }
    else {
        decl = source.Substring(0, equalPos);
        mExpression = source.Substring(equalPos + 1);
        FAIL_RETURN(lexer->Scan(decl, declParser))
        mName = declParser->mName;
        mArgs = declParser->ArgNames();
        mArity = declParser->mArity;
    }
    return NOERROR;
    /*
    if (arity == DeclarationParser.UNKNOWN_ARITY) {
        args = IMPLICIT_ARGS;
    }
    */
}

} // namespace Arity
} // namespace Javia
} // namespace Org
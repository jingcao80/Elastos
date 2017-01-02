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

#ifndef _ORG_JAVIA_ARITY_DECLARATION_H__
#define _ORG_JAVIA_ARITY_DECLARATION_H__

#include "org/javia/arity/Lexer.h"
#include "org/javia/arity/DeclarationParser.h"

namespace Org {
namespace Javia {
namespace Arity {

class Declaration : public Object
{
public:
    Declaration()
        : mArity(0)
    {}

    CARAPI Parse(
        /* [in] */ const String& source,
        /* [in] */ Lexer* lexer,
        /* [in] */ DeclarationParser* declParser);

public:
    String mName;
    AutoPtr<ArrayOf<String> > mArgs;
    Int32 mArity;
    String mExpression;

private:
    static const AutoPtr<ArrayOf<String> > NO_ARGS;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_DECLARATION_H__
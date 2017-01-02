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

#ifndef _ORG_JAVIA_ARITY_DECLARATIONPARSER_H__
#define _ORG_JAVIA_ARITY_DECLARATIONPARSER_H__

#include "org/javia/arity/TokenConsumer.h"

using Elastos::Utility::IVector;

namespace Org {
namespace Javia {
namespace Arity {

class DeclarationParser : public TokenConsumer
{
public:
    DeclarationParser();

    CARAPI_(void) Start();

    CARAPI Push(
        /* [in] */ Token* token);

    CARAPI_(AutoPtr<ArrayOf<String> >) ArgNames();

public:
    static const AutoPtr<ArrayOf<String> > NO_ARGS;
    static const Int32 UNKNOWN_ARITY = -2;
    static const Int32 MAX_ARITY = 5;

    String mName;
    Int32 mArity;
    AutoPtr<IVector> mArgs;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_DECLARATIONPARSER_H__
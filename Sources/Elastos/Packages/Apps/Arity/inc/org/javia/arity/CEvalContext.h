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
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

#ifndef _ORG_JAVIA_ARITY_CConstant_H__
#define _ORG_JAVIA_ARITY_CConstant_H__

#include "_Org_Javia_Arity_CConstant.h"
#include "org/javia/arity/Function.h"

namespace Org {
namespace Javia {
namespace Arity {

/**
   A constant presented as a function, always evaluates to the same value.
 */
CarClass(CConstant)
    , public Function
    , public IConstant
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IComplex* o);

    //@Override
    /** Returns the complex constant. */
    CARAPI EvalComplex(
        /* [out] */ IComplex** complex);

    //@Override
    /** Returns the complex constant as a real value.
        @see Complex.asReas()
     */
    CARAPI Eval(
        /* [out] */ Double* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI Arity(
        /* [out] */ Int32* arity);

private:
    AutoPtr<IComplex> mValue;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_CConstant_H__
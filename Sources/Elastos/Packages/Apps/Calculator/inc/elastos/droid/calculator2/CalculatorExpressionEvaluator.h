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

#ifndef _ELASTOS_DROID_CALCULATOR2_CALCULATOREXPRESSIONEVALUATOR_H__
#define _ELASTOS_DROID_CALCULATOR2_CALCULATOREXPRESSIONEVALUATOR_H__

#include "_Elastos.Droid.Calculator2.h"
#include <_Org.Javia.Arity.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Core::ICharSequence;
using Org::Javia::Arity::ISymbols;

namespace Elastos {
namespace Droid {
namespace Calculator2 {

class CalculatorExpressionEvaluator
    : public Object
    , public ICalculatorExpressionEvaluator
{
public:
    CalculatorExpressionEvaluator();

    ~CalculatorExpressionEvaluator();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ICalculatorExpressionTokenizer* tokenizer);

    CARAPI Evaluate(
        /* [in] */ ICharSequence* expr,
        /* [in] */ IEvaluateCallback* callback);

    CARAPI Evaluate(
        /* [in] */ const String& expr,
        /* [in] */ IEvaluateCallback* callback);

private:
    static const Int32 MAX_DIGITS;
    static const Int32 ROUNDING_DIGITS;

    AutoPtr<ISymbols> mSymbols;
    AutoPtr<ICalculatorExpressionTokenizer> mTokenizer;
};

} // namespace Calculator2
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_CALCULATOR2_CALCULATOREXPRESSIONEVALUATOR_H__
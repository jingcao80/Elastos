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

#ifndef _ELASTOS_DROID_CALCULATOR3_CALCULATOREXPRESSIONBUILDER_H__
#define _ELASTOS_DROID_CALCULATOR3_CALCULATOREXPRESSIONBUILDER_H__

#include "_Elastos.Droid.Calculator3.h"
#include <elastos/droid/text/SpannableStringBuilder.h>
#include "elastos/droid/calculator3/CalculatorExpressionTokenizer.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Text::SpannableStringBuilder;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Calculator3 {

class CalculatorExpressionBuilder
    : public SpannableStringBuilder
    , public ICalculatorExpressionBuilder
{
public:
    CalculatorExpressionBuilder();

    ~CalculatorExpressionBuilder();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ICharSequence* text,
        /* [in] */ ICalculatorExpressionTokenizer* tokenizer,
        /* [in] */ Boolean isEdited);

    CARAPI Replace(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ICharSequence* tb,
        /* [in] */ Int32 tbstart,
        /* [in] */ Int32 tbend
        /*SpannableStringBuilder** result*/);

private:
    AutoPtr<CalculatorExpressionTokenizer> mTokenizer;
    Boolean mIsEdited;
};

} // namespace Calculator3
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_CALCULATOR3_CALCULATOREXPRESSIONBUILDER_H__
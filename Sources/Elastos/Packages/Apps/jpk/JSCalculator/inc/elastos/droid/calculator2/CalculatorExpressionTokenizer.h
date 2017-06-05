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

#ifndef _ELASTOS_DROID_CALCULATOR2_CALCULATOREXPRESSIONTOKENIZER_H__
#define _ELASTOS_DROID_CALCULATOR2_CALCULATOREXPRESSIONTOKENIZER_H__

#include "_Elastos.Droid.Calculator2.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Calculator2 {

class CalculatorExpressionTokenizer
    : public Object
    , public ICalculatorExpressionTokenizer
{
public:
    CalculatorExpressionTokenizer();

    ~CalculatorExpressionTokenizer();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI GetNormalizedExpression(
        /* [in] */ const String& expr,
        /* [out] */ String* result);

    CARAPI GetLocalizedExpression(
        /* [in] */ const String& expr,
        /* [out] */ String* result);

private:
    HashMap<String, String> mReplacementMap;
};

} // namespace Calculator2
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_CALCULATOR2_CALCULATOREXPRESSIONTOKENIZER_H__
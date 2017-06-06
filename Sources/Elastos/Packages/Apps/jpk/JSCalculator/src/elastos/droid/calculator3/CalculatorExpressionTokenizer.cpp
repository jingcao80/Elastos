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

#include "elastos/droid/calculator3/CalculatorExpressionTokenizer.h"
#include "R.h"
// #include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Content.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Calculator3 {

CalculatorExpressionTokenizer::CalculatorExpressionTokenizer()
{}

CalculatorExpressionTokenizer::~CalculatorExpressionTokenizer()
{}

CAR_INTERFACE_IMPL(CalculatorExpressionTokenizer, Object, ICalculatorExpressionTokenizer)

ECode CalculatorExpressionTokenizer::constructor(
    /* [in] */ IContext* context)
{
    String str;
    context->GetString(R::string::dec_point, &str);
    mReplacementMap[String(".")] = str;
    context->GetString(R::string::digit_0, &str);
    mReplacementMap[String("0")] = str;
    context->GetString(R::string::digit_1, &str);
    mReplacementMap[String("1")] = str;
    context->GetString(R::string::digit_2, &str);
    mReplacementMap[String("2")] = str;
    context->GetString(R::string::digit_3, &str);
    mReplacementMap[String("3")] = str;
    context->GetString(R::string::digit_4, &str);
    mReplacementMap[String("4")] = str;
    context->GetString(R::string::digit_5, &str);
    mReplacementMap[String("5")] = str;
    context->GetString(R::string::digit_6, &str);
    mReplacementMap[String("6")] = str;
    context->GetString(R::string::digit_7, &str);
    mReplacementMap[String("7")] = str;
    context->GetString(R::string::digit_8, &str);
    mReplacementMap[String("8")] = str;
    context->GetString(R::string::digit_9, &str);
    mReplacementMap[String("9")] = str;

    context->GetString(R::string::op_div, &str);
    mReplacementMap[String("/")] = str;
    context->GetString(R::string::op_mul, &str);
    mReplacementMap[String("*")] = str;
    context->GetString(R::string::op_sub, &str);
    mReplacementMap[String("-")] = str;

    context->GetString(R::string::fun_cos, &str);
    mReplacementMap[String("cos")] = str;
    context->GetString(R::string::fun_ln, &str);
    mReplacementMap[String("ln")] = str;
    context->GetString(R::string::fun_log, &str);
    mReplacementMap[String("log")] = str;
    context->GetString(R::string::fun_sin, &str);
    mReplacementMap[String("sin")] = str;
    context->GetString(R::string::fun_tan, &str);
    mReplacementMap[String("tan")] = str;

    context->GetString(R::string::inf, &str);
    mReplacementMap[String("Infinity")] = str;

    return NOERROR;
}

ECode CalculatorExpressionTokenizer::GetNormalizedExpression(
    /* [in] */ const String& _expr,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String expr = _expr;
    HashMap<String, String>::Iterator it = mReplacementMap.Begin();
    for (; it != mReplacementMap.End(); ++it) {
        String temp;
        StringUtils::Replace(expr, it->mSecond, it->mFirst, &temp);
        expr = temp;
    }
    *result = expr;
    return NOERROR;
}

ECode CalculatorExpressionTokenizer::GetLocalizedExpression(
    /* [in] */ const String& _expr,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String expr = _expr;
    HashMap<String, String>::Iterator it = mReplacementMap.Begin();
    for (; it != mReplacementMap.End(); ++it) {
        String temp;
        StringUtils::Replace(expr, it->mFirst, it->mSecond, &temp);
        expr = temp;
    }
    *result = expr;
    return NOERROR;
}

} // namespace Calculator3
} // namespace Droid
} // namespace Elastos
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

#include "elastos/droid/calculator3/CalculatorExpressionBuilder.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/droid/text/SpannableStringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/text/TextUtils.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Calculator3 {

CalculatorExpressionBuilder::CalculatorExpressionBuilder()
{}

CalculatorExpressionBuilder::~CalculatorExpressionBuilder()
{}

CAR_INTERFACE_IMPL(CalculatorExpressionBuilder, SpannableStringBuilder, ICalculatorExpressionBuilder)\

ECode CalculatorExpressionBuilder::constructor(
    /* [in] */ ICharSequence* text,
    /* [in] */ ICalculatorExpressionTokenizer* tokenizer,
    /* [in] */ Boolean isEdited)
{
    SpannableStringBuilder::constructor(text);
    mTokenizer = (CalculatorExpressionTokenizer*)tokenizer;
    mIsEdited = isEdited;
    return NOERROR;
}

ECode CalculatorExpressionBuilder::Replace(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ICharSequence* tb,
    /* [in] */ Int32 tbstart,
    /* [in] */ Int32 tbend)
{
    Int32 length;
    GetLength(&length);
    if (start != length || end != length) {
        mIsEdited = TRUE;
        return SpannableStringBuilder::Replace(start, end, tb, tbstart, tbend);
    }

    AutoPtr<ICharSequence> cs;
    tb->SubSequence(tbstart, tbend, (ICharSequence**)&cs);
    String str;
    cs->ToString(&str);
    String appendExpr;
    mTokenizer->GetNormalizedExpression(str, &appendExpr);
    if (appendExpr.GetLength() == 1) {
        String tmpStr;
        ToString(&tmpStr);
        String expr;
        mTokenizer->GetNormalizedExpression(tmpStr, &expr);
        switch (appendExpr.GetChar(0)) {
            case '.':
                {
                    // don't allow two decimals in the same number
                    Int32 index = expr.LastIndexOf('.');
                    String subStr = expr.Substring(index + 1, start);

                    if (index != -1 && TextUtils::IsDigitsOnly(StringUtils::ParseCharSequence(subStr))) {
                        appendExpr = String("");
                    }
                    break;
                }

            case '+':
            case '*':
            case '/':
                {
                    // don't allow leading operator
                    if (start == 0) {
                        appendExpr = String("");
                        break;
                    }

                    // don't allow multiple successive operators
                    while (start > 0 && String("+-*/").IndexOf(expr.GetChar(start - 1)) != -1) {
                        --start;
                    }
                    // fall through
                }

            case '-':
                {
                    // don't allow -- or +-
                    if (start > 0 && String("+-").IndexOf(expr.GetChar(start - 1)) != -1) {
                        --start;
                    }

                    // mark as edited since operators can always be appended
                    mIsEdited = TRUE;
                    break;
                }

            default:
                break;
        }
    }

    // since this is the first edit replace the entire string
    if (!mIsEdited && appendExpr.GetLength() > 0) {
        start = 0;
        mIsEdited = TRUE;
    }

    String tmp;
    mTokenizer->GetLocalizedExpression(appendExpr, &tmp);
    appendExpr = tmp;
    return SpannableStringBuilder::Replace(start, end, StringUtils::ParseCharSequence(appendExpr), 0, appendExpr.GetLength());
}

} // namespace Calculator3
} // namespace Droid
} // namespace Elastos
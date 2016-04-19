#include "elastos/droid/calculator2/CalculatorExpressionBuilder.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/droid/text/SpannableStringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/text/TextUtils.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Calculator2 {

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
    /* [in] */ Int32 tbend
    /*SpannableStringBuilder** result*/)
{
    Int32 length;
    GetLength(&length);
    if (start != length || end != length) {
        mIsEdited = TRUE;
        return SpannableStringBuilder::Replace(start, end, tb, tbstart, tbend);
    }

    String appendExpr;
    AutoPtr<ICharSequence> cs;
    tb->SubSequence(tbstart, tbend, (ICharSequence**)&cs);
    cs->ToString(&appendExpr);
    String str;
    mTokenizer->GetNormalizedExpression(appendExpr, &str);

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

                    if (index != -1 && TextUtils::IsDigitsOnly(StringUtils::ParseCharSequence(subStr).Get())) {
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
    return Replace(start, end, StringUtils::ParseCharSequence(appendExpr).Get(), 0, appendExpr.GetLength());
}

} // namespace Calculator2
} // namespace Droid
} // namespace Elastos
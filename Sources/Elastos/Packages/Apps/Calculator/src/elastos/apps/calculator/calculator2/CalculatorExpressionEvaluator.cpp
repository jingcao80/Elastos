#include "elastos/apps/calculator/calculator2/CalculatorExpressionEvaluator.h"
#include "R.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::IDouble;

namespace Elastos{
namespace Apps {
namespace Calculator {
namespace Calculator2 {

const Int32 CalculatorExpressionEvaluator::MAX_DIGITS = 12;
const Int32 CalculatorExpressionEvaluator::ROUNDING_DIGITS = 2;

CalculatorExpressionEvaluator::CalculatorExpressionEvaluator()
{}

CalculatorExpressionEvaluator::~CalculatorExpressionEvaluator()
{}

CAR_INTERFACE_IMPL(CalculatorExpressionEvaluator, Object, ICalculatorExpressionEvaluator)

ECode CalculatorExpressionEvaluator::constructor(
    /* [in] */ ICalculatorExpressionTokenizer* tokenizer)
{
    assert(0 && "TODO");
    // mSymbols = new Symbols();
    mTokenizer = tokenizer;
    return NOERROR;
}

ECode CalculatorExpressionEvaluator::Evaluate(
    /* [in] */ ICharSequence* expr,
    /* [in] */ IEvaluateCallback* callback)
{
    String str;
    expr->ToString(&str);
    return Evaluate(str, callback);
}

ECode CalculatorExpressionEvaluator::Evaluate(
    /* [in] */ const String& expr,
    /* [in] */ IEvaluateCallback* callback)
{
    String strexpr;
    mTokenizer->GetNormalizedExpression(expr, &strexpr);

    // remove any trailing operators
    while (strexpr.GetLength() > 0 && String("+-/*").IndexOf(strexpr.GetChar(strexpr.GetLength() - 1)) != -1) {
        strexpr = strexpr.Substring(0, strexpr.GetLength() - 1);
    }

    // try {
        if (strexpr.GetLength() == 0 || CoreUtils::Convert(StringUtils::ParseDouble(strexpr)) != NULL) {
            callback->OnEvaluate(strexpr, String(NULL), ICalculator::INVALID_RES_ID);
            return NOERROR;
        }
    // } catch (NumberFormatException e) {
        // strexpr is not a simple number
    // }

    // try {
        Double result;
        assert(0 && "TODO"); // Symbols
        // result =  mSymbols.eval(strexpr);
        Boolean flag = FALSE;
        if ((CoreUtils::Convert(result)->IsNaN(&flag), flag)) {
            callback->OnEvaluate(strexpr, String(NULL), R::string::error_nan);
        } else {
            // The arity library uses floating point arithmetic when evaluating the expression
            // leading to precision errors in the result. The method doubleToString hides these
            // errors; rounding the result by dropping N digits of precision.
            String resultString;
            assert(0 && "TODO"); // arity.jar
            // mTokenizer->GetLocalizedExpression(
            //         Util.doubleToString(result, MAX_DIGITS, ROUNDING_DIGITS), &resultString);
            callback->OnEvaluate(strexpr, resultString, ICalculator::INVALID_RES_ID);
        }
    // } catch (SyntaxException e) {
        callback->OnEvaluate(strexpr, String(NULL), R::string::error_syntax);
    // }

}

} // namespace Calculator2
} // namespace Calculator
} // namespace Apps
} // namespace Elastos
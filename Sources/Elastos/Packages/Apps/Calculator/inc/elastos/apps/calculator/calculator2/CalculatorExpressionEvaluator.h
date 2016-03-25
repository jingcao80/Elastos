#ifndef _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CALCULATOREXPRESSIONEVALUATOR_H__
#define _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CALCULATOREXPRESSIONEVALUATOR_H__

#include "_Elastos.Apps.Calculator.Calculator2.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Core::ICharSequence;

namespace Elastos{
namespace Apps {
namespace Calculator {
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

    // assert(0 && "TODO");
    // Symbols mSymbols;
    AutoPtr<ICalculatorExpressionTokenizer> mTokenizer;
};

} // namespace Calculator2
} // namespace Calculator
} // namespace Apps
} // namespace Elastos

#endif // _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CALCULATOREXPRESSIONEVALUATOR_H__
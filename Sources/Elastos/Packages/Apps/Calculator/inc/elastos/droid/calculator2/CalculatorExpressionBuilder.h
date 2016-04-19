#ifndef _ELASTOS_DROID_CALCULATOR2_CALCULATOREXPRESSIONBUILDER_H__
#define _ELASTOS_DROID_CALCULATOR2_CALCULATOREXPRESSIONBUILDER_H__

#include "_Elastos.Droid.Calculator2.h"
#include <elastos/droid/text/SpannableStringBuilder.h>
#include "elastos/droid/calculator2/CalculatorExpressionTokenizer.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Text::SpannableStringBuilder;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Calculator2 {

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

} // namespace Calculator2
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_CALCULATOR2_CALCULATOREXPRESSIONBUILDER_H__
#ifndef _ELASTOS_DROID_CALCULATOR2_CALCULATOREXPRESSIONTOKENIZER_H__
#define _ELASTOS_DROID_CALCULATOR2_CALCULATOREXPRESSIONTOKENIZER_H__

#include "_Elastos.Droid.Calculator2.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::IHashMap;

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
    AutoPtr<IHashMap> mReplacementMap;
};

} // namespace Calculator2
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_CALCULATOR2_CALCULATOREXPRESSIONTOKENIZER_H__
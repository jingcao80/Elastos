#ifndef _ELASTOS_DROID_CALCULATOR2_CCALCULATOR_H__
#define _ELASTOS_DROID_CALCULATOR2_CCALCULATOR_H__

#include "_Elastos_Droid_Calculator2_CCalculator.h"
#include "elastos/droid/calculator2/Calculator.h"

namespace Elastos {
namespace Droid {
namespace Calculator2 {

CarClass(CCalculator), public Calculator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Calculator2
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_CALCULATOR2_CCALCULATOR_H__
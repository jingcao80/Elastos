#ifndef _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CCALCULATOR_H__
#define _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CCALCULATOR_H__

#include "_Elastos_Apps_Calculator_Calculator2_CCalculator.h"
#include "elastos/apps/calculator/calculator2/Calculator.h"

namespace Elastos{
namespace Apps {
namespace Calculator {
namespace Calculator2 {

CarClass(CCalculator), public Calculator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Calculator2
} // namespace Calculator
} // namespace Apps
} // namespace Elastos

#endif // _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CCALCULATOR_H__
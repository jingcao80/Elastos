
#ifndef __ELASTOS_UTILITY_CTIMER_H__
#define __ELASTOS_UTILITY_CTIMER_H__

#include "_Elastos_Utility_CTimer.h"
#include "Timer.h"

namespace Elastos {
namespace Utility {

CarClass(CTimer)
    , public Timer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CTIMER_H__
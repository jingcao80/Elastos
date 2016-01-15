
#ifndef __ELASTOS_UTILITY_CRANDOM_H__
#define __ELASTOS_UTILITY_CRANDOM_H__

#include "_Elastos_Utility_CRandom.h"
#include "Random.h"

namespace Elastos {
namespace Utility {

CarClass(CRandom)
    , public Random
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_CORE_CRANDOM_H__

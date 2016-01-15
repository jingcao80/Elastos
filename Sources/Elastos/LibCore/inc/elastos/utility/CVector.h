#ifndef __ELASTOS_UTILITY_CVECTOR_H__
#define __ELASTOS_UTILITY_CVECTOR_H__

#include "_Elastos_Utility_CVector.h"
#include "Vector.h"


namespace Elastos {
namespace Utility {

CarClass(CVector)
    , public Vector
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__CVECTOR_H__
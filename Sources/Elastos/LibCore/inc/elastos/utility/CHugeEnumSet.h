#ifndef __ELASTOS_UTILITY_CHUGEENUMSET_H__
#define __ELASTOS_UTILITY_CHUGEENUMSET_H__

#include "_Elastos_Utility_CHugeEnumSet.h"
#include "HugeEnumSet.h"

namespace Elastos {
namespace Utility {

CarClass(CHugeEnumSet)
    , public HugeEnumSet
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__CHUGEENUMSET_H__

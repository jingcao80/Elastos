#ifndef __ELASTOS_UTILITY_CSUBABSTRACT_LIST_H__
#define __ELASTOS_UTILITY_CSUBABSTRACT_LIST_H__

#include "_Elastos_Utility_CSubAbstractList.h"
#include "elastos/utility/AbstractList.h"

namespace Elastos {
namespace Utility {

CarClass(CSubAbstractList)
    , public SubAbstractList
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CSUBABSTRACT_LIST_H__

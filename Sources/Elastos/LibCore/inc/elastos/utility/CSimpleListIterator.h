#ifndef __ELASTOS_UTILITY_CSIMPLELISTITERATOR_H__
#define __ELASTOS_UTILITY_CSIMPLELISTITERATOR_H__

#include "_Elastos_Utility_CSimpleListIterator.h"
#include "elastos/utility/AbstractList.h"

using Elastos::Utility::AbstractList;

namespace Elastos {
namespace Utility {

CarClass(CSimpleListIterator)
    , public AbstractList::SimpleListIterator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CSIMPLELISTITERATOR_H__

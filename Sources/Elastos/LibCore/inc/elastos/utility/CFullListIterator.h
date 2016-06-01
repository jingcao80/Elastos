#ifndef __ELASTOS_UTILITY_CFULLLIST_ITERATOR_H__
#define __ELASTOS_UTILITY_CFULLLIST_ITERATOR_H__

#include "_Elastos_Utility_CFullListIterator.h"
#include "elastos/utility/AbstractList.h"

using Elastos::Utility::AbstractList;

namespace Elastos {
namespace Utility {

CarClass(CFullListIterator)
    , public AbstractList::FullListIterator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CFULLLIST_ITERATOR_H__

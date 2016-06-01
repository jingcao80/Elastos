#ifndef __ELASTOS_UTILITY_CFULLLISTCSUBABSTRACTLIST_ITERATOR_H__
#define __ELASTOS_UTILITY_CFULLLISTCSUBABSTRACTLIST_ITERATOR_H__

#include "_Elastos_Utility_CSubAbstractListIterator.h"
#include "elastos/utility/AbstractList.h"

namespace Elastos {
namespace Utility {

CarClass(CSubAbstractListIterator)
    , public SubAbstractList::SubAbstractListIterator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CFULLLISTCSUBABSTRACTLIST_ITERATOR_H__

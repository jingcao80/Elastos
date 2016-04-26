#ifndef __ELASTOS_UTILITY_CARRAYLIST_ITERATOR_H__
#define __ELASTOS_UTILITY_CARRAYLIST_ITERATOR_H__

#include "_Elastos_Utility_CArrayListIterator.h"
#include "elastos/utility/ArrayList.h"

using Elastos::Utility::ArrayList;

namespace Elastos {
namespace Utility {

CarClass(CArrayListIterator)
    , public ArrayList::ArrayListIterator
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CARRAYLIST_ITERATOR_H__

#ifndef __ELASTOS_UTILITY_CHASHTABLE_H__
#define __ELASTOS_UTILITY_CHASHTABLE_H__

#include "_Elastos_Utility_CHashTable.h"
#include "HashTable.h"

namespace Elastos {
namespace Utility {

CarClass(CHashTable)
    , public HashTable
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CHASHTABLE_H__

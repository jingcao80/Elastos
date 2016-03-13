#ifndef __ELASTOS_UTILITY_CARRAYLIST_H__
#define __ELASTOS_UTILITY_CARRAYLIST_H__

#include "_Elastos_Utility_CArrayList.h"
#include "elastos/utility/ArrayList.h"

using Elastos::Utility::ArrayList;

namespace Elastos {
namespace Utility {

CarClass(CArrayList)
    , public ArrayList
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__CARRAYLIST_H__


#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CTYPEDPROPERTIES_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CTYPEDPROPERTIES_H__

#include "_Elastos_Droid_Internal_Utility_CTypedProperties.h"
#include "elastos/droid/internal/utility/TypedProperties.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CTypedProperties)
    , public TypedProperties
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CTYPEDPROPERTIES_H__

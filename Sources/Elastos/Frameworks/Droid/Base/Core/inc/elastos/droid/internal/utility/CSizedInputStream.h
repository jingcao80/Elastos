
#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CSIZEDINPUTSTREAM_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CSIZEDINPUTSTREAM_H__

#include "_Elastos_Droid_Internal_Utility_CSizedInputStream.h"
#include "elastos/droid/internal/utility/SizedInputStream.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CSizedInputStream)
    , public SizedInputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CSIZEDINPUTSTREAM_H__

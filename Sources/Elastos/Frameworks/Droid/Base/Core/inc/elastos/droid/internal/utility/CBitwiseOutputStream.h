
#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CBITWISEOUTPUTSTREAM_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CBITWISEOUTPUTSTREAM_H__

#include "_Elastos_Droid_Internal_Utility_CBitwiseOutputStream.h"
#include "elastos/droid/internal/utility/BitwiseOutputStream.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CBitwiseOutputStream)
    , public BitwiseOutputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CBITWISEOUTPUTSTREAM_H__

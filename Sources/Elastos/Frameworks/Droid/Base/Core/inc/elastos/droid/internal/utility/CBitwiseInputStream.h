
#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CBITWISEINPUTSTREAM_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CBITWISEINPUTSTREAM_H__

#include "_Elastos_Droid_Internal_Utility_CBitwiseInputStream.h"
#include "elastos/droid/internal/utility/BitwiseInputStream.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CBitwiseInputStream)
    , public BitwiseInputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CBITWISEINPUTSTREAM_H__

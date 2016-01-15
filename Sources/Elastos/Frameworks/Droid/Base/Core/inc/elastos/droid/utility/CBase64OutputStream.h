
#ifndef __ELASTOS_DROID_UTILITY_CBASE64OUTPUTSTREAM_H__
#define __ELASTOS_DROID_UTILITY_CBASE64OUTPUTSTREAM_H__

#include "_Elastos_Droid_Utility_CBase64OutputStream.h"
#include "elastos/droid/utility/Base64OutputStream.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CBase64OutputStream)
    , public Base64OutputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CBASE64OUTPUTSTREAM_H__

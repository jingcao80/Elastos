
#ifndef __ELASTOS_DROID_UTILITY_CBASE64INPUTSTREAM_H__
#define __ELASTOS_DROID_UTILITY_CBASE64INPUTSTREAM_H__

#include "_Elastos_Droid_Utility_CBase64InputStream.h"
#include "elastos/droid/utility/Base64InputStream.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CBase64InputStream)
    , public Base64InputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CBASE64INPUTSTREAM_H__

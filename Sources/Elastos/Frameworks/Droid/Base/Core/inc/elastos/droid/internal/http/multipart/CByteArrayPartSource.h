
#ifndef __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_CBYTEARRAYPARTSOURCE_H__
#define __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_CBYTEARRAYPARTSOURCE_H__

#include "_Elastos_Droid_Internal_Http_Multipart_CByteArrayPartSource.h"
#include "elastos/droid/internal/http/multipart/ByteArrayPartSource.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

CarClass(CByteArrayPartSource)
    , public ByteArrayPartSource
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_CBYTEARRAYPARTSOURCE_H__

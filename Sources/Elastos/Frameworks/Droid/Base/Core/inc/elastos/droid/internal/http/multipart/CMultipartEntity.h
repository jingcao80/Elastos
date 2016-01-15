
#ifndef __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_CMULTIPARTENTITY_H__
#define __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_CMULTIPARTENTITY_H__

#include "_Elastos_Droid_Internal_Http_Multipart_CMultipartEntity.h"
#include "elastos/droid/internal/http/multipart/MultipartEntity.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Http {
namespace Multipart {

CarClass(CMultipartEntity)
    , public MultipartEntity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Multipart
} // namespace Http
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_HTTP_MULTIPART_CMULTIPARTENTITY_H__

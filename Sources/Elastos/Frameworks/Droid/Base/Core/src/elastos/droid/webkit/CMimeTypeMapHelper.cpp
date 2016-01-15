
#include "elastos/droid/webkit/CMimeTypeMapHelper.h"
#include "elastos/droid/webkit/MimeTypeMap.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CMimeTypeMapHelper, Object, IMimeTypeMapHelper);

CAR_SINGLETON_IMPL(CMimeTypeMapHelper);

ECode CMimeTypeMapHelper::GetFileExtensionFromUrl(
    /* [in] */ const String& url,
    /* [out] */ String* fileExtension)
{
    VALIDATE_NOT_NULL(fileExtension);
    *fileExtension = MimeTypeMap::GetFileExtensionFromUrl(url);
    return NOERROR;
}

ECode CMimeTypeMapHelper::GetSingleton(
    /* [out] */ IMimeTypeMap** mimeTypeMap)
{
    VALIDATE_NOT_NULL(mimeTypeMap);
    *mimeTypeMap = MimeTypeMap::sMimeTypeMap;
    REFCOUNT_ADD(*mimeTypeMap);
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
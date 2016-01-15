
#include "elastos/droid/media/CThumbnailUtils.h"
#include "elastos/droid/media/ThumbnailUtils.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CThumbnailUtils)

CAR_INTERFACE_IMPL(CThumbnailUtils, Singleton, IThumbnailUtils)

ECode CThumbnailUtils::CreateImageThumbnail(
    /* [in] */ const String& filePath,
    /* [in] */ Int32 kind,
    /* [out] */ IBitmap** result)
{
    return ThumbnailUtils::CreateImageThumbnail(filePath, kind, result);
}

ECode CThumbnailUtils::CreateVideoThumbnail(
    /* [in] */ const String& filePath,
    /* [in] */ Int32 kind,
    /* [out] */ IBitmap** result)
{
    return ThumbnailUtils::CreateVideoThumbnail(filePath, kind, result);
}

ECode CThumbnailUtils::ExtractThumbnail(
    /* [in] */ IBitmap* source,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IBitmap** result)
{
    return ThumbnailUtils::ExtractThumbnail(source, width, height, result);
}

ECode CThumbnailUtils::ExtractThumbnail(
    /* [in] */ IBitmap* source,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 options,
    /* [out] */ IBitmap** result)
{
    return ThumbnailUtils::ExtractThumbnail(source, width, height, options, result);
}

} // namespace Media
} // namespace Droid
} // namespace Elastos

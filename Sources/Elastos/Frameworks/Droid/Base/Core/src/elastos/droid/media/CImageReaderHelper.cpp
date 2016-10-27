#include "elastos/droid/media/CImageReader.h"
#include "elastos/droid/media/CImageReaderHelper.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CImageReaderHelper, Singleton, IImageReaderHelper)

CAR_SINGLETON_IMPL(CImageReaderHelper)

ECode CImageReaderHelper::NewInstance(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ Int32 maxImages,
    /* [out] */ IImageReader** result)
{
    return CImageReader::NewInstance(width, height, format, maxImages, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos

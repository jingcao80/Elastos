
#include "elastos/droid/opengl/CETC1Texture.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.CoreLibrary.IO.h"

namespace Elastos {
namespace Droid {
namespace Opengl {
CAR_INTERFACE_IMPL(CETC1Texture, Object, IETC1Texture)

CAR_OBJECT_IMPL(CETC1Texture)

ECode CETC1Texture::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)

    *width = mWidth;
    return NOERROR;
}

ECode CETC1Texture::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)

    *height = mHeight;
    return NOERROR;
}

ECode CETC1Texture::GetData(
    /* [out] */ Elastos::IO::IByteBuffer** data)
{
    VALIDATE_NOT_NULL(data)

    *data = mData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode CETC1Texture::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Elastos::IO::IByteBuffer* data)
{
    mWidth = width;
    mHeight = height;
    mData = data;
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos


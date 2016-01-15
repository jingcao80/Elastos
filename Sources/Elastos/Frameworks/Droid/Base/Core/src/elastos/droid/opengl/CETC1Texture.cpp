
#include "CETC1Texture.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

ECode CETC1Texture::GetWidth(
    /* [out] */ Int32* width)
{
    *width = mWidth;
    return NOERROR;
}

ECode CETC1Texture::GetHeight(
    /* [out] */ Int32* height)
{
    *height = mHeight;
    return NOERROR;
}

ECode CETC1Texture::GetData(
    /* [out] */ Elastos::IO::IByteBuffer** data)
{
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


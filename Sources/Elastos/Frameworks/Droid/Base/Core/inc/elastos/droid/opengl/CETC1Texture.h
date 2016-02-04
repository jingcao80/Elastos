
#ifndef __ELASTOS_DROID_OPENGL_CETC1TEXTURE_H__
#define __ELASTOS_DROID_OPENGL_CETC1TEXTURE_H__

#include "Elastos.Droid.Opengl.h"
#include "Elastos.CoreLibrary.IO.h"
#include "_Elastos_Droid_Opengl_CETC1Texture.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::IO::IByteBuffer;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CETC1Texture)
    , public Object
    , public IETC1Texture
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetWidth(
        /* [out] */ Int32* width);

    CARAPI GetHeight(
        /* [out] */ Int32* height);

    CARAPI GetData(
        /* [out] */ Elastos::IO::IByteBuffer** data);

    CARAPI constructor(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Elastos::IO::IByteBuffer* data);

private:
    Int32 mWidth;
    Int32 mHeight;
    AutoPtr<IByteBuffer> mData;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CETC1TEXTURE_H__

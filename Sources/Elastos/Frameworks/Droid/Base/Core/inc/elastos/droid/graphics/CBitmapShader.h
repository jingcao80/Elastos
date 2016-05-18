
#ifndef __ELASTOS_DROID_GRAPHICS_CBITMAPSHADER_H__
#define __ELASTOS_DROID_GRAPHICS_CBITMAPSHADER_H__

#include "_Elastos_Droid_Graphics_CBitmapShader.h"
#include "elastos/droid/graphics/Shader.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CBitmapShader)
    , public Shader
    , public IBitmapShader
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ ShaderTileMode tileX,
        /* [in] */ ShaderTileMode tileY);

protected:
    /**
     * @hide
     */
    // @Override
    virtual CARAPI Copy(
        /* [out] */ IShader** shader);

private:
    static CARAPI_(Int64) NativeCreate(
        /* [in] */ Int64 nBitmap,
        /* [in] */ Int32 tileModeX,
        /* [in] */ Int32 tileModeY);

public:
    /**
     * Prevent garbage collection.
     * @hide
     */
    AutoPtr<IBitmap> mBitmap;

    ShaderTileMode mTileX;
    ShaderTileMode mTileY;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CBITMAPSHADER_H__

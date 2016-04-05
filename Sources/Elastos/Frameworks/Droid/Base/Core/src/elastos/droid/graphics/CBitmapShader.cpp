
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CBitmapShader.h"
#include "elastos/droid/graphics/CBitmap.h"
#include <skia/core/SkShader.h>
#include <skia/core/SkBitmap.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CBitmapShader);

CAR_INTERFACE_IMPL(CBitmapShader, Shader, IBitmapShader);

ECode CBitmapShader::constructor(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ShaderTileMode tileX,
    /* [in] */ ShaderTileMode tileY)
{
    mBitmap = bitmap;
    mTileX = tileX;
    mTileY = tileY;
    Handle64 nativeBitmatp = ((CBitmap*)bitmap)->mNativeBitmap;
    Init(NativeCreate(nativeBitmatp, (Int32)tileX, (Int32)tileY));
    return NOERROR;
}

ECode CBitmapShader::Copy(
    /* [out] */ IShader** shader)
{
    VALIDATE_NOT_NULL(shader);
    AutoPtr<IShader> copy;
    CBitmapShader::New(mBitmap, mTileX, mTileY, (IShader**)&copy);
    CopyLocalMatrix(copy);
    *shader = copy;
    REFCOUNT_ADD(*shader);
    return NOERROR;
}

Int64 CBitmapShader::NativeCreate(
    /* [in] */ Int64 bitmapHandle,
    /* [in] */ Int32 tileModeX,
    /* [in] */ Int32 tileModeY)
{
    const SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(bitmapHandle);
    SkShader* s = SkShader::CreateBitmapShader(*bitmap,
            (SkShader::TileMode)tileModeX,
            (SkShader::TileMode)tileModeY);
    return reinterpret_cast<Int64>(s);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

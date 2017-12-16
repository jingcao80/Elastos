//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CBitmapShader.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/graphics/Matrix.h"
#include <skia/core/SkShader.h>
#include <skia/core/SkBitmap.h>
#include <skia/core/SkImagePriv.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CBitmapShader);

CAR_INTERFACE_IMPL(CBitmapShader, Shader, IBitmapShader);

CBitmapShader::CBitmapShader()
    : mTileX(0)
    , mTileY(0)
{
}

ECode CBitmapShader::constructor(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ShaderTileMode tileX,
    /* [in] */ ShaderTileMode tileY)
{
    mBitmap = bitmap;
    mTileX = tileX;
    mTileY = tileY;
    Init(CreateNativeInstance(mLocalMatrix));
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

Int64 CBitmapShader::CreateNativeInstance(
    /* [in] */ IMatrix* matrix)
{
    Int64 nativeMatrix = matrix == NULL ? 0 :
            ((Matrix*)matrix)->mNativeMatrix;
    Int64 nativeBitmatp = ((CBitmap*)mBitmap.Get())->mNativeBitmap;
    return NativeCreate(nativeMatrix, nativeBitmatp, mTileX, mTileY);
}

Int64 CBitmapShader::NativeCreate(
    /* [in] */ Int64 matrixHandle,
    /* [in] */ Int64 bitmapHandle,
    /* [in] */ ShaderTileMode tileModeX,
    /* [in] */ ShaderTileMode tileModeY)
{
    const SkMatrix* matrix = reinterpret_cast<const SkMatrix*>(matrixHandle);
    SkBitmap bitmap;
    if (bitmapHandle) {
        // Only pass a valid SkBitmap object to the constructor if the Bitmap exists. Otherwise,
        // we'll pass an empty SkBitmap to avoid crashing/excepting for compatibility.
        reinterpret_cast<BitmapWrapper*>(bitmapHandle)->bitmap().getSkBitmapForShaders(&bitmap);
    }

    sk_sp<SkImage> image = SkMakeImageFromRasterBitmap(bitmap, kNever_SkCopyPixelsMode);
    sk_sp<SkShader> baseShader = image->makeShader(
            (SkShader::TileMode)tileModeX, (SkShader::TileMode)tileModeY);

    SkShader* shader;
    if (matrix) {
        shader = baseShader->makeWithLocalMatrix(*matrix).release();
    }
    else {
        shader = baseShader.release();
    }

    return reinterpret_cast<Int64>(shader);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

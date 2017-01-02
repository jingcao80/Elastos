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
#include <skia/core/SkShader.h>
#include <skia/core/SkBitmap.h>

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
    Int64 nativeBitmatp = ((CBitmap*)bitmap)->mNativeBitmap;
    Init(NativeCreate(nativeBitmatp, tileX, tileY));
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
    /* [in] */ ShaderTileMode tileModeX,
    /* [in] */ ShaderTileMode tileModeY)
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

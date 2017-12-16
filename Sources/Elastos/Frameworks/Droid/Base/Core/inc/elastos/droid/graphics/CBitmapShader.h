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

    CBitmapShader();

    CARAPI constructor(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ ShaderTileMode tileX,
        /* [in] */ ShaderTileMode tileY);

protected:
    /**
     * @hide
     */
    // @Override
    CARAPI Copy(
        /* [out] */ IShader** shader);

    // @Override
    CARAPI_(Int64) CreateNativeInstance(
        /* [in] */ IMatrix* matrix);

private:
    static CARAPI_(Int64) NativeCreate(
        /* [in] */ Int64 nMatrix,
        /* [in] */ Int64 nBitmap,
        /* [in] */ ShaderTileMode tileModeX,
        /* [in] */ ShaderTileMode tileModeY);

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

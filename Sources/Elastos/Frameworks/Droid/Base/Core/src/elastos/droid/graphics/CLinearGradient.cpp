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
#include "elastos/droid/graphics/CLinearGradient.h"
#include "elastos/droid/graphics/Matrix.h"
#include <skia/effects/SkGradientShader.h>
#include <skia/private/SkTemplates.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

const Int32 CLinearGradient::TYPE_COLORS_AND_POSITIONS = 1;
const Int32 CLinearGradient::TYPE_COLOR_START_AND_COLOR_END = 2;

CAR_OBJECT_IMPL(CLinearGradient);

CAR_INTERFACE_IMPL(CLinearGradient, Shader, ILinearGradient);

CLinearGradient::CLinearGradient()
    : mType(0)
    , mX0(0)
    , mY0(0)
    , mX1(0)
    , mY1(0)
    , mColor0(0)
    , mColor1(0)
    , mTileMode(-1)
{}

ECode CLinearGradient::constructor(
    /* [in] */ Float x0,
    /* [in] */ Float y0,
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ ArrayOf<Float>* positions,
    /* [in] */ ShaderTileMode tile)
{
    if (colors->GetLength() < 2) {
//        throw new IllegalArgumentException("needs >= 2 number of colors");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (positions != NULL && colors->GetLength() != positions->GetLength()) {
//        throw new IllegalArgumentException("color and position arrays must be of equal length");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mType = TYPE_COLORS_AND_POSITIONS;
    mX0 = x0;
    mY0 = y0;
    mX1 = x1;
    mY1 = y1;
    mColors = colors;
    mPositions = positions;
    mTileMode = tile;
    Init(CreateNativeInstance(mLocalMatrix));
    return NOERROR;
}

ECode CLinearGradient::constructor(
    /* [in] */ Float x0,
    /* [in] */ Float y0,
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Int32 color0,
    /* [in] */ Int32 color1,
    /* [in] */ ShaderTileMode tile)
{
    mType = TYPE_COLOR_START_AND_COLOR_END;
    mX0 = x0;
    mY0 = y0;
    mX1 = x1;
    mY1 = y1;
    mColor0 = color0;
    mColor1 = color1;
    mTileMode = tile;
    Init(CreateNativeInstance(mLocalMatrix));
    return NOERROR;
}

/**
 * By default Skia gradients will interpolate their colors in unpremul space
 * and then premultiply each of the results. We must set this flag to preserve
 * backwards compatiblity by premultiplying the colors of the gradient first,
 * and then interpolating between them.
 */
static const uint32_t sGradientShaderFlags = SkGradientShader::kInterpolateColorsInPremul_Flag;

Int64 CLinearGradient::NativeCreate1(
    /* [in] */ Int64 matrixHandle,
    /* [in] */ Float x0,
    /* [in] */ Float y0,
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ ArrayOf<Float>* positions,
    /* [in] */ ShaderTileMode tileMode)
{
    const SkMatrix* matrix = reinterpret_cast<const SkMatrix*>(matrixHandle);
    SkPoint pts[2];
    pts[0].set(x0, y0);
    pts[1].set(x1, y1);

    size_t count = colors->GetLength();

#ifdef SK_SCALAR_IS_FLOAT
    SkScalar* pos = positions ? positions->GetPayload() : NULL;
#else
    #error Need to convert float array to SkScalar array before calling the following function.
#endif

    sk_sp<SkShader> baseShader(SkGradientShader::MakeLinear(pts,
            reinterpret_cast<const SkColor*>(colors->GetPayload()), pos, count,
            static_cast<SkShader::TileMode>(tileMode), sGradientShaderFlags, NULL));

    SkShader* shader;
    if (matrix) {
        shader = baseShader->makeWithLocalMatrix(*matrix).release();
    } else {
        shader = baseShader.release();
    }

    assert(shader != NULL);
    return reinterpret_cast<Int64>(shader);
}

Int64 CLinearGradient::NativeCreate2(
    /* [in] */ Int64 matrixHandle,
    /* [in] */ Float x0,
    /* [in] */ Float y0,
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Int32 color0,
    /* [in] */ Int32 color1,
    /* [in] */ ShaderTileMode tileMode)
{
    const SkMatrix* matrix = reinterpret_cast<const SkMatrix*>(matrixHandle);
    SkPoint pts[2];
    pts[0].set(x0, y0);
    pts[1].set(x1, y1);

    SkColor colors[2];
    colors[0] = color0;
    colors[1] = color1;

    sk_sp<SkShader> baseShader(SkGradientShader::MakeLinear(pts, colors, NULL, 2,
            static_cast<SkShader::TileMode>(tileMode), sGradientShaderFlags, NULL));

    SkShader* s;
    if (matrix) {
        s = baseShader->makeWithLocalMatrix(*matrix).release();
    } else {
        s = baseShader.release();
    }

    assert(s != NULL);
    return reinterpret_cast<Int64>(s);
}

ECode CLinearGradient::Copy(
    /* [out] */ IShader** shader)
{
    AutoPtr<IShader> copy;
    switch (mType) {
        case TYPE_COLORS_AND_POSITIONS:
            CLinearGradient::New(mX0, mY0, mX1, mY1, mColors->Clone(),
                    mPositions != NULL ? mPositions->Clone() : NULL, mTileMode, (IShader**)&copy);
            break;
        case TYPE_COLOR_START_AND_COLOR_END:
            CLinearGradient::New(mX0, mY0, mX1, mY1, mColor0, mColor1, mTileMode, (IShader**)&copy);
            break;
        default:
            *shader = NULL;
            // throw new IllegalArgumentException("LinearGradient should be created with either " +
            //         "colors and positions or start color and end color");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    CopyLocalMatrix(copy);
    *shader = copy;
    REFCOUNT_ADD(*shader);
    return NOERROR;
}

Int64 CLinearGradient::CreateNativeInstance(
    /* [in] */ IMatrix* matrix)
{
    Int64 nativeMatrix = matrix == NULL ? 0 :
            ((Matrix*)matrix)->mNativeMatrix;
    if (mType == TYPE_COLORS_AND_POSITIONS) {
        return NativeCreate1(nativeMatrix, mX0, mY0, mX1, mY1,
                mColors, mPositions, mTileMode);
    }
    else { // TYPE_COLOR_START_AND_COLOR_END
        return NativeCreate2(nativeMatrix, mX0, mY0, mX1, mY1,
                mColor0, mColor1, mTileMode);
    }
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

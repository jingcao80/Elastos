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
#include "elastos/droid/graphics/CSweepGradient.h"
#include "elastos/droid/graphics/Matrix.h"
#include <skia/effects/SkGradientShader.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

const Int32 CSweepGradient::TYPE_COLORS_AND_POSITIONS = 1;
const Int32 CSweepGradient::TYPE_COLOR_START_AND_COLOR_END = 2;

CAR_OBJECT_IMPL(CSweepGradient);
CAR_INTERFACE_IMPL(CSweepGradient, Shader, ISweepGradient);

CSweepGradient::CSweepGradient()
    : mType(0)
    , mCx(0)
    , mCy(0)
    , mColor0(0)
    , mColor1(0)
{}

ECode CSweepGradient::constructor(
    /* [in] */ Float cx,
    /* [in] */ Float cy,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ ArrayOf<Float>* positions)
{
    if (colors == NULL || colors->GetLength() < 2) {
//        throw new IllegalArgumentException("needs >= 2 number of colors");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (positions != NULL && colors->GetLength() != positions->GetLength()) {
//        throw new IllegalArgumentException(
//                    "color and position arrays must be of equal length");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mType = TYPE_COLORS_AND_POSITIONS;
    mCx = cx;
    mCy = cy;
    mColors = colors;
    mPositions = positions;
    Init(CreateNativeInstance(mLocalMatrix));
    return NOERROR;
}

ECode CSweepGradient::constructor(
    /* [in] */ Float cx,
    /* [in] */ Float cy,
    /* [in] */ Int32 color0,
    /* [in] */ Int32 color1)
{
    mType = TYPE_COLOR_START_AND_COLOR_END;
    mCx = cx;
    mCy = cy;
    mColor0 = color0;
    mColor1 = color1;
    Init(CreateNativeInstance(mLocalMatrix));
    return NOERROR;
}

ECode CSweepGradient::Copy(
    /* [out] */ IShader** shader)
{
    VALIDATE_NOT_NULL(shader);
    AutoPtr<IShader> copy;
    switch (mType) {
        case TYPE_COLORS_AND_POSITIONS: {
            AutoPtr< ArrayOf<Int32> > colors = mColors->Clone();
            AutoPtr<ArrayOf<Float> > positions;
            if (mPositions) {
                positions = mPositions->Clone();
            }
            CSweepGradient::New(mCx, mCy, colors, positions, (IShader**)&copy);
            break;
        }
        case TYPE_COLOR_START_AND_COLOR_END:
            CSweepGradient::New(mCx, mCy, mColor0, mColor1, (IShader**)&copy);
            break;
        default:
            *shader = NULL;
            // throw new IllegalArgumentException("SweepGradient should be created with either " +
            //         "colors and positions or start color and end color");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    CopyLocalMatrix(copy);
    *shader = copy;
    REFCOUNT_ADD(*shader);
    return NOERROR;
}

Int64 CSweepGradient::CreateNativeInstance(
    /* [in] */ IMatrix* matrix)
{
    Int64 nativeMatrix = matrix == NULL ? 0 :
            ((Matrix*)matrix)->mNativeMatrix;
    if (mType == TYPE_COLORS_AND_POSITIONS) {
        return NativeCreate1(nativeMatrix, mCx, mCy, mColors, mPositions);
    } else { // TYPE_COLOR_START_AND_COLOR_END
        return NativeCreate2(nativeMatrix, mCx, mCy, mColor0, mColor1);
    }
}

static const uint32_t sGradientShaderFlags = SkGradientShader::kInterpolateColorsInPremul_Flag;

Int64 CSweepGradient::NativeCreate1(
    /* [in] */ Int64 matrixHandle,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ ArrayOf<Float>* positions)
{
    const SkMatrix* matrix = reinterpret_cast<const SkMatrix*>(matrixHandle);
    size_t count = (size_t)colors->GetLength();
    const Int32* colValues = colors->GetPayload();

#ifdef SK_SCALAR_IS_FLOAT
    SkScalar* pos = positions ? positions->GetPayload() : NULL;
#else
    #error Need to convert float array to SkScalar array before calling the following function.
#endif

    sk_sp<SkShader> baseShader = SkGradientShader::MakeSweep(x, y,
            reinterpret_cast<const SkColor*>(colors), pos, count,
            sGradientShaderFlags, NULL);

    SkShader* shader;
    if (matrix) {
        shader = baseShader->makeWithLocalMatrix(*matrix).release();
    } else {
        shader = baseShader.release();
    }

    return reinterpret_cast<Int64>(shader);
}

Int64 CSweepGradient::NativeCreate2(
    /* [in] */ Int64 matrixHandle,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int32 color0,
    /* [in] */ Int32 color1)
{
    const SkMatrix* matrix = reinterpret_cast<const SkMatrix*>(matrixHandle);
    SkColor colors[2];
    colors[0] = color0;
    colors[1] = color1;

    sk_sp<SkShader> baseShader = SkGradientShader::MakeSweep(x, y, colors,
            NULL, 2, sGradientShaderFlags, NULL);

    SkShader* shader;
    if (matrix) {
        shader = baseShader->makeWithLocalMatrix(*matrix).release();
    } else {
        shader = baseShader.release();
    }
    return reinterpret_cast<Int64>(shader);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

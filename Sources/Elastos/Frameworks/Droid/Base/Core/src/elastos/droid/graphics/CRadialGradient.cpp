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
#include "elastos/droid/graphics/CRadialGradient.h"
#include <skia/effects/SkGradientShader.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

const Int32 CRadialGradient::TYPE_COLORS_AND_POSITIONS = 1;
const Int32 CRadialGradient::TYPE_COLOR_CENTER_AND_COLOR_EDGE = 2;

CAR_OBJECT_IMPL(CRadialGradient);
CAR_INTERFACE_IMPL(CRadialGradient, Shader, IRadialGradient);

CRadialGradient::CRadialGradient()
    : mType(0)
    , mX(0)
    , mY(0)
    , mRadius(0)
    , mCenterColor(0)
    , mEdgeColor(0)
    , mTileMode(-1)
{}

ECode CRadialGradient::constructor(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float radius,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ ArrayOf<Float>* positions,
    /* [in] */ ShaderTileMode tile)
{
    if (radius <= 0) {
//        throw new IllegalArgumentException("radius must be > 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (colors == NULL || colors->GetLength() < 2) {
//        throw new IllegalArgumentException("needs >= 2 number of colors");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (positions != NULL && colors->GetLength() != positions->GetLength()) {
//        throw new IllegalArgumentException("color and position arrays must be of equal length");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mType = TYPE_COLORS_AND_POSITIONS;
    mX = x;
    mY = y;
    mRadius = radius;
    mColors = colors;
    mPositions = positions;
    mTileMode = tile;
    Init(NativeCreate1(x, y, radius, colors, positions, tile));
    return NOERROR;
}

ECode CRadialGradient::constructor(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float radius,
    /* [in] */ Int32 centerColor,
    /* [in] */ Int32 edgeColor,
    /* [in] */ ShaderTileMode tile)
{
    if (radius <= 0) {
//        throw new IllegalArgumentException("radius must be > 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mType = TYPE_COLOR_CENTER_AND_COLOR_EDGE;
    mX = x;
    mY = y;
    mRadius = radius;
    mCenterColor = centerColor;
    mEdgeColor = edgeColor;
    mTileMode = tile;
    Init(NativeCreate2(x, y, radius, centerColor, edgeColor, tile));
    return NOERROR;
}

ECode CRadialGradient::Copy(
    /* [out] */ IShader** shader)
{
    VALIDATE_NOT_NULL(shader);
    AutoPtr<IShader> copy;
    switch (mType) {
        case TYPE_COLORS_AND_POSITIONS: {
            AutoPtr<ArrayOf<Int32> > colors = mColors->Clone();
            AutoPtr<ArrayOf<Float> > positions;
            if (mPositions) {
                positions = mPositions->Clone();
            }
            CRadialGradient::New(mX, mY, mRadius, colors,
                positions, mTileMode, (IShader**)&copy);
            break;
        }

        case TYPE_COLOR_CENTER_AND_COLOR_EDGE:
            CRadialGradient::New(mX, mY, mRadius, mCenterColor, mEdgeColor, mTileMode, (IShader**)&copy);
            break;
        default:
            *shader = NULL;
            // throw new IllegalArgumentException("RadialGradient should be created with either " +
            //         "colors and positions or center color and edge color");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    CopyLocalMatrix(copy);
    *shader = copy;
    REFCOUNT_ADD(*shader);
    return NOERROR;
}

Int64 CRadialGradient::NativeCreate1(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float radius,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ ArrayOf<Float>* positions,
    /* [in] */ ShaderTileMode tile)
{
    SkPoint center;
    center.set(x, y);

    size_t count = (size_t)colors->GetLength();
    const Int32* colorValues = colors->GetPayload();

#ifdef SK_SCALAR_IS_FLOAT
    SkScalar* pos = positions ? positions->GetPayload() : NULL;
#else
    #error Need to convert float array to SkScalar array before calling the following function.
#endif

    SkShader* shader = SkGradientShader::CreateRadial(center, radius,
            reinterpret_cast<const SkColor*>(colorValues), pos, count,
            static_cast<SkShader::TileMode>(tile));
    // env->ReleaseIntArrayElements(colorArray, const_cast<jint*>(colorValues),
    //                              JNI_ABORT);

    // ThrowIAE_IfNull(env, shader);
    assert(shader != NULL);
    return reinterpret_cast<Int64>(shader);
}

Int64 CRadialGradient::NativeCreate2(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float radius,
    /* [in] */ Int32 color0,
    /* [in] */ Int32 color1,
    /* [in] */ ShaderTileMode tile)
{
    SkPoint center;
    center.set(x, y);

    SkColor colors[2];
    colors[0] = color0;
    colors[1] = color1;

    SkShader* s = SkGradientShader::CreateRadial(center, radius, colors, NULL,
            2, (SkShader::TileMode)tile);
    assert(s != NULL);
    return reinterpret_cast<Int64>(s);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

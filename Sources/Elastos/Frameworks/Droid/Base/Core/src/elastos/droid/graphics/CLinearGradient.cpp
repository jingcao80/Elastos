
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CLinearGradient.h"
#include <skia/effects/SkGradientShader.h>
#include <skia/core/SkTemplates.h>

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
    /* [in] */ const ArrayOf<Int32>* colors,
    /* [in] */ const ArrayOf<Float>* positions,
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
    mColors = const_cast<ArrayOf<Int32>*>(colors);
    mPositions = const_cast<ArrayOf<Float>*>(positions);
    mTileMode = tile;
    Init(NativeCreate1(x0, y0, x1, y1, colors, positions, tile));
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
    Init(NativeCreate2(x0, y0, x1, y1, color0, color1, tile));
    return NOERROR;
}

Int64 CLinearGradient::NativeCreate1(
    /* [in] */ Float x0,
    /* [in] */ Float y0,
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ const ArrayOf<Int32>* colors,
    /* [in] */ const ArrayOf<Float>* positions,
    /* [in] */ ShaderTileMode tileMode)
{
    SkPoint pts[2];
    pts[0].set(x0, y0);
    pts[1].set(x1, y1);

    size_t count = colors->GetLength();

    // AutoJavaFloatArray autoPos(env, posArray, count);
#ifdef SK_SCALAR_IS_FLOAT
    SkScalar* pos = positions->GetPayload();
#else
    #error Need to convert float array to SkScalar array before calling the following function.
#endif

    SkShader* shader = SkGradientShader::CreateLinear(pts,
            reinterpret_cast<const SkColor*>(colors->GetPayload()), pos, count,
            static_cast<SkShader::TileMode>(tileMode));

    // env->ReleaseIntArrayElements(colorArray, const_cast<jint*>(colorValues), JNI_ABORT);
    // ThrowIAE_IfNull(env, shader);
    assert(shader != NULL);
    return reinterpret_cast<Int64>(shader);
}

Int64 CLinearGradient::NativeCreate2(
    /* [in] */ Float x0,
    /* [in] */ Float y0,
    /* [in] */ Float x1,
    /* [in] */ Float y1,
    /* [in] */ Int32 color0,
    /* [in] */ Int32 color1,
    /* [in] */ ShaderTileMode tileMode)
{
    SkPoint pts[2];
    pts[0].set(x0, y0);
    pts[1].set(x1, y1);

    SkColor colors[2];
    colors[0] = color0;
    colors[1] = color1;

    SkShader* s = SkGradientShader::CreateLinear(pts, colors, NULL, 2, (SkShader::TileMode)tileMode);

    // ThrowIAE_IfNull(env, s);
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

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

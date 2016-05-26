
#ifndef __ELASTOS_DROID_GRAPHICS_CLINEARGRADIENT_H__
#define __ELASTOS_DROID_GRAPHICS_CLINEARGRADIENT_H__

#include "_Elastos_Droid_Graphics_CLinearGradient.h"
#include "elastos/droid/graphics/Shader.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CLinearGradient)
    , public Shader
    , public ILinearGradient
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CLinearGradient();

    /** Create a shader that draws a linear gradient along a line.
     *  @param x0           The x-coordinate for the start of the gradient line
     *  @param y0           The y-coordinate for the start of the gradient line
     *  @param x1           The x-coordinate for the end of the gradient line
     *  @param y1           The y-coordinate for the end of the gradient line
     *  @param  colors      The colors to be distributed along the gradient line
     *  @param  positions   May be null. The relative positions [0..1] of
     *                      each corresponding color in the colors array. If this is null,
     *                      the the colors are distributed evenly along the gradient line.
     *  @param  tile        The Shader tiling mode
     */
    CARAPI constructor(
        /* [in] */ Float x0,
        /* [in] */ Float y0,
        /* [in] */ Float x1,
        /* [in] */ Float y1,
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ ArrayOf<Float>* positions,
        /* [in] */ ShaderTileMode tile);

    /** Create a shader that draws a linear gradient along a line.
     *  @param x0       The x-coordinate for the start of the gradient line
     *  @param y0       The y-coordinate for the start of the gradient line
     *  @param x1       The x-coordinate for the end of the gradient line
     *  @param y1       The y-coordinate for the end of the gradient line
     *  @param  color0  The color at the start of the gradient line.
     *  @param  color1  The color at the end of the gradient line.
     *  @param  tile    The Shader tiling mode
     */
    CARAPI constructor(
        /* [in] */ Float x0,
        /* [in] */ Float y0,
        /* [in] */ Float x1,
        /* [in] */ Float y1,
        /* [in] */ Int32 color0,
        /* [in] */ Int32 color1,
        /* [in] */ ShaderTileMode tile);

private:
    CARAPI_(Int64) NativeCreate1(
        /* [in] */ Float x0,
        /* [in] */ Float y0,
        /* [in] */ Float x1,
        /* [in] */ Float y1,
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ ArrayOf<Float>* positions,
        /* [in] */ ShaderTileMode tileMode);

    CARAPI_(Int64) NativeCreate2(
        /* [in] */ Float x0,
        /* [in] */ Float y0,
        /* [in] */ Float x1,
        /* [in] */ Float y1,
        /* [in] */ Int32 color0,
        /* [in] */ Int32 color1,
        /* [in] */ ShaderTileMode tileMode);

protected:
    /**
     * @hide
     */
    // @Override
    CARAPI Copy(
        /* [out] */ IShader** shader);

private:
    static const Int32 TYPE_COLORS_AND_POSITIONS;
    static const Int32 TYPE_COLOR_START_AND_COLOR_END;

    /**
     * Type of the LinearGradient: can be either TYPE_COLORS_AND_POSITIONS or
     * TYPE_COLOR_START_AND_COLOR_END.
     */
    Int32 mType;

    Float mX0;
    Float mY0;
    Float mX1;
    Float mY1;
    AutoPtr<ArrayOf<Int32> > mColors;
    AutoPtr<ArrayOf<Float> > mPositions;
    Int32 mColor0;
    Int32 mColor1;

    ShaderTileMode mTileMode;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CLINEARGRADIENT_H__

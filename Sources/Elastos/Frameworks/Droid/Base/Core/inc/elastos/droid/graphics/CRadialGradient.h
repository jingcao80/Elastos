
#ifndef __ELASTOS_DROID_GRAPHICS_CRADIALGRADIENT_H__
#define __ELASTOS_DROID_GRAPHICS_CRADIALGRADIENT_H__

#include "_Elastos_Droid_Graphics_CRadialGradient.h"
#include "elastos/droid/graphics/Shader.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CRadialGradient)
    , public Shader
    , public IRadialGradient
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CRadialGradient();

    /** Create a shader that draws a radial gradient given the center and radius.
        @param x        The x-coordinate of the center of the radius
        @param y        The y-coordinate of the center of the radius
        @param radius   Must be positive. The radius of the circle for this gradient
        @param colors   The colors to be distributed between the center and edge of the circle
        @param positions May be NULL. The relative position of
                        each corresponding color in the colors array. If this is NULL,
                        the the colors are distributed evenly between the center and edge of the circle.
        @param  tile    The Shader tiling mode
    */
    CARAPI constructor(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float radius,
        /* [in] */ const ArrayOf<Int32>& colors,
        /* [in] */ ArrayOf<Float>* positions,
        /* [in] */ ShaderTileMode tile);

    /** Create a shader that draws a radial gradient given the center and radius.
        @param x        The x-coordinate of the center of the radius
        @param y        The y-coordinate of the center of the radius
        @param radius   Must be positive. The radius of the circle for this gradient
        @param color0   The color at the center of the circle.
        @param color1   The color at the edge of the circle.
        @param tile     The Shader tiling mode
    */
    CARAPI constructor(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float radius,
        /* [in] */ Int32 color0,
        /* [in] */ Int32 color1,
        /* [in] */ ShaderTileMode tile);

protected:
    /**
     * @hide
     */
    // @Override
    CARAPI Copy(
        /* [out] */ IShader** shader);

private:
    static CARAPI_(Int64) NativeCreate1(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float radius,
        /* [in] */ const ArrayOf<Int32>& colors,
        /* [in] */ ArrayOf<Float>* positions,
        /* [in] */ ShaderTileMode tile);

    static CARAPI_(Int64) NativeCreate2(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float radius,
        /* [in] */ Int32 color0,
        /* [in] */ Int32 color1,
        /* [in] */ ShaderTileMode tile);

private:
    static const Int32 TYPE_COLORS_AND_POSITIONS;
    static const Int32 TYPE_COLOR_CENTER_AND_COLOR_EDGE;

    /**
     * Type of the RadialGradient: can be either TYPE_COLORS_AND_POSITIONS or
     * TYPE_COLOR_CENTER_AND_COLOR_EDGE.
     */
    Int32 mType;

    Float mX;
    Float mY;
    Float mRadius;
    AutoPtr<ArrayOf<Int32> > mColors;
    AutoPtr<ArrayOf<Float> > mPositions;
    Int32 mCenterColor;
    Int32 mEdgeColor;

    ShaderTileMode mTileMode;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CRADIALGRADIENT_H__

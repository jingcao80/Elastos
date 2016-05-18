
#ifndef __ELASTOS_DROID_GRAPHICS_CLIGHTINGCOLORFILTER_H__
#define __ELASTOS_DROID_GRAPHICS_CLIGHTINGCOLORFILTER_H__

#include "_Elastos_Droid_Graphics_CLightingColorFilter.h"
#include "elastos/droid/graphics/ColorFilter.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * A color filter that can be used to simulate simple lighting effects.
 * A <code>LightingColorFilter</code> is defined by two parameters, one
 * used to multiply the source color (called <code>colorMultiply</code>)
 * and one used to add to the source color (called <code>colorAdd</code>).
 * The alpha channel is left untouched by this color filter.
 *
 * Given a source color RGB, the resulting R'G'B' color is computed thusly:
 * <pre>
 * R' = R * colorMultiply.R + colorAdd.R
 * G' = G * colorMultiply.G + colorAdd.G
 * B' = B * colorMultiply.B + colorAdd.B
 * </pre>
 * The result is pinned to the <code>[0..255]</code> range for each channel.
 */
CarClass(CLightingColorFilter)
    , public ColorFilter
    , public ILightingColorFilter
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CLightingColorFilter();

    /**
     * Create a colorfilter that multiplies the RGB channels by one color, and then adds a second color,
     * pinning the result for each component to [0..255]. The alpha components of the mul and add arguments
     * are ignored.
     */
    CARAPI constructor(
        /* [in] */ Int32 mul,
        /* [in] */ Int32 add);

    /**
     * Returns the RGB color used to multiply the source color when the
     * color filter is applied.
     *
     * @see #setColorMultiply(int)
     *
     * @hide
     */
    CARAPI GetColorMultiply(
        /* [out] */ Int32* colorMultiply);

    /**
     * Specifies the RGB color used to multiply the source color when the
     * color filter is applied.
     * The alpha channel of this color is ignored.
     *
     * @see #getColorMultiply()
     *
     * @hide
     */
    CARAPI SetColorMultiply(
        /* [in] */ Int32 mul);

    /**
     * Returns the RGB color that will be added to the source color
     * when the color filter is applied.
     *
     * @see #setColorAdd(int)
     *
     * @hide
     */
    CARAPI GetColorAdd(
        /* [out] */ Int32* color);

    /**
     * Specifies the RGB that will be added to the source color when
     * the color filter is applied.
     * The alpha channel of this color is ignored.
     *
     * @see #getColorAdd()
     *
     * @hide
     */
    CARAPI SetColorAdd(
        /* [in] */ Int32 add);

private:
    CARAPI_(void) Update();

    static CARAPI_(Int64) NativeCreateLightingFilter(
        /* [in] */ Int32 mul,
        /* [in] */ Int32 add);

private:
    Int32 mMul;
    Int32 mAdd;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CLIGHTINGCOLORFILTER_H__


#ifndef __ELASTOS_DROID_GRAPHICS_CPorterDuffColorFilter_H__
#define __ELASTOS_DROID_GRAPHICS_CPorterDuffColorFilter_H__

#include "_Elastos_Droid_Graphics_CPorterDuffColorFilter.h"
#include "elastos/droid/graphics/ColorFilter.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CPorterDuffColorFilter)
    , public ColorFilter
    , public IPorterDuffColorFilter
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CPorterDuffColorFilter();

    /**
     * Create a colorfilter that uses the specified color and porter-duff mode.
     *
     * @param srcColor       The source color used with the specified
     *                       porter-duff mode
     * @param mode           The porter-duff mode that is applied
     */
    CARAPI constructor(
        /* [in] */ Int32 srcColor,
        /* [in] */ PorterDuffMode mode);

    /**
     * Returns the ARGB color used to tint the source pixels when this filter
     * is applied.
     *
     * @see Color
     * @see #setColor(int)
     *
     * @hide
     */
    CARAPI GetColor(
        /* [out] */ Int32* color);

    /**
     * Specifies the color to tint the source pixels with when this color
     * filter is applied.
     *
     * @param color An ARGB {@link Color color}
     *
     * @see Color
     * @see #getColor()
     * @see #getMode()
     *
     * @hide
     */
    CARAPI SetColor(
        /* [in] */ Int32 color);

    /**
     * Returns the Porter-Duff mode used to composite this color filter's
     * color with the source pixel when this filter is applied.
     *
     * @see PorterDuff
     * @see #setMode(android.graphics.PorterDuff.Mode)
     *
     * @hide
     */
    CARAPI GetMode(
        /* [out] */ PorterDuffMode* mode);

    /**
     * Specifies the Porter-Duff mode to use when compositing this color
     * filter's color with the source pixel at draw time.
     *
     * @see PorterDuff
     * @see #getMode()
     * @see #getColor()
     *
     * @hide
     */
    CARAPI SetMode(
        /* [in] */ PorterDuffMode mode);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* equals);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* code);

private:
    CARAPI_(void) Update();

    static CARAPI_(Int64) NativeCreatePorterDuffFilter(
        /* [in] */ Int32 srcColor,
        /* [in] */ Int32 porterDuffMode);

private:
    Int32 mColor;
    PorterDuffMode mMode;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CPorterDuffColorFilter_H__

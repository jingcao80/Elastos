
#ifndef __ELASTOS_DROID_GRAPHICS_CCOLORMATRIXCOLORFILTER_H__
#define __ELASTOS_DROID_GRAPHICS_CCOLORMATRIXCOLORFILTER_H__

#include "_Elastos_Droid_Graphics_CColorMatrixColorFilter.h"
#include "elastos/droid/graphics/ColorFilter.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CColorMatrixColorFilter)
    , public ColorFilter
    , public IColorMatrixColorFilter
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CColorMatrixColorFilter();

    /**
     * Create a colorfilter that transforms colors through a 4x5 color matrix.
     *
     * @param matrix 4x5 matrix used to transform colors. It is copied into
     *               the filter, so changes made to the matrix after the filter
     *               is constructed will not be reflected in the filter.
     */
    CARAPI constructor(
        /* [in] */ IColorMatrix* matrix);

    /**
     * Create a colorfilter that transforms colors through a 4x5 color matrix.
     *
     * @param array array of floats used to transform colors, treated as a 4x5
     *              matrix. The first 20 entries of the array are copied into
     *              the filter. See ColorMatrix.
     */
    CARAPI constructor(
        /* [in] */ const ArrayOf<Float>& array);

    /**
     * Returns the {@link ColorMatrix} used by this filter. The returned
     * value is never null. Modifying the returned matrix does not have
     * any effect until you call {@link #setColorMatrix(ColorMatrix)}.
     *
     * @see #setColorMatrix(ColorMatrix)
     *
     * @hide
     */
    CARAPI GetColorMatrix(
        /* [out] */ IColorMatrix** matrix);

    /**
     * Specifies the color matrix used by this filter. If the specified
     * color matrix is null, this filter's color matrix will be reset to
     * the identity matrix.
     *
     * @param matrix A {@link ColorMatrix} or null
     *
     * @see #getColorMatrix()
     * @see android.graphics.ColorMatrix#reset()
     * @see #setColorMatrix(float[])
     *
     * @hide
     */
    CARAPI SetColorMatrix(
        /* [in] */ IColorMatrix* matrix);

    /**
     * Specifies the color matrix used by this filter. If the specified
     * color matrix is null, this filter's color matrix will be reset to
     * the identity matrix.
     *
     * @param array Array of floats used to transform colors, treated as a 4x5
     *              matrix. The first 20 entries of the array are copied into
     *              the filter. See {@link ColorMatrix}.
     *
     * @see #getColorMatrix()
     * @see android.graphics.ColorMatrix#reset()
     * @see #setColorMatrix(ColorMatrix)
     *
     * @throws ArrayIndexOutOfBoundsException if the specified array's
     *         length is < 20
     *
     * @hide
     */
    CARAPI SetColorMatrix(
        /* [in] */ ArrayOf<Float>* array);

private:
    CARAPI_(void) Update();

    static CARAPI_(Int64) NativeColorMatrixFilter(
        /* [in] */ ArrayOf<Float>* array);

private:
    AutoPtr<IColorMatrix> mMatrix;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CCOLORMATRIXCOLORFILTER_H__

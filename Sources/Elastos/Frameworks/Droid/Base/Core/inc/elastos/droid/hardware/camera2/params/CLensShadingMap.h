
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CLENSSHADINGMAP_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CLENSSHADINGMAP_H__

#include "_Elastos_Droid_Hardware_Camera2_Params_CLensShadingMap.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CarClass(CLensShadingMap)
    , public Object
    , public ILensShadingMap
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CLensShadingMap();

    virtual ~CLensShadingMap();

    CARAPI constructor();

    /**
     * Create a new immutable LensShadingMap instance.
     *
     * <p>The elements must be stored in a row-major order (fully packed).</p>
     *
     * <p>This constructor takes over the array; do not write to the array afterwards.</p>
     *
     * @param elements
     *          An array of elements whose length is
     *          {@code RggbChannelVector.COUNT * rows * columns}
     *
     * @throws IllegalArgumentException
     *            if the {@code elements} array length is invalid,
     *            if any of the subelems are not finite or less than {@value #MINIMUM_GAIN_FACTOR},
     *            or if rows or columns is not positive
     * @throws NullPointerException
     *            if {@code elements} is {@code null}
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Float>* elements,
        /* [in] */ Int32 rows,
        /* [in] */ Int32 columns);

    /**
     * Get the number of rows in this map.
     */
    CARAPI GetRowCount(
        /* [out] */ Int32* value);

    /**
     * Get the number of columns in this map.
     */
    CARAPI GetColumnCount(
        /* [out] */ Int32* value);

    /**
     * Get the total number of gain factors in this map.
     *
     * <p>A single gain factor contains exactly one color channel.
     * Use with {@link #copyGainFactors} to allocate a large-enough array.</p>
     */
    CARAPI GetGainFactorCount(
        /* [out] */ Int32* value);

    /**
     * Get a single color channel gain factor from this lens shading map by its row and column.
     *
     * <p>The rows must be within the range [0, {@link #getRowCount}),
     * the column must be within the range [0, {@link #getColumnCount}),
     * and the color channel must be within the range [0, {@value RggbChannelVector#COUNT}).</p>
     *
     * <p>The channel order is {@code [R, Geven, Godd, B]}, where
     * {@code Geven} is the green channel for the even rows of a Bayer pattern, and
     * {@code Godd} is the odd rows.
     * </p>
     *
     * @param colorChannel color channel from {@code [R, Geven, Godd, B]}
     * @param column within the range [0, {@link #getColumnCount})
     * @param row within the range [0, {@link #getRowCount})
     *
     * @return a gain factor >= {@value #MINIMUM_GAIN_FACTOR}
     *
     * @throws IllegalArgumentException if any of the parameters was out of range
     *
     * @see #RED
     * @see #GREEN_EVEN
     * @see #GREEN_ODD
     * @see #BLUE
     * @see #getRowCount
     * @see #getColumnCount
     */
    CARAPI GetGainFactor(
        /* [in] */ Int32 colorChannel,
        /* [in] */ Int32 column,
        /* [in] */ Int32 row,
        /* [out] */ Float* value);

    /**
     * Get a gain factor vector from this lens shading map by its row and column.
     *
     * <p>The rows must be within the range [0, {@link #getRowCount}),
     * the column must be within the range [0, {@link #getColumnCount}).</p>
     *
     * @param column within the range [0, {@link #getColumnCount})
     * @param row within the range [0, {@link #getRowCount})
     *
     * @return an {@link RggbChannelVector} where each gain factor >= {@value #MINIMUM_GAIN_FACTOR}
     *
     * @throws IllegalArgumentException if any of the parameters was out of range
     *
     * @see #getRowCount
     * @see #getColumnCount
     */
    CARAPI GetGainFactorVector(
        /* [in] */ Int32 column,
        /* [in] */ Int32 row,
        /* [out] */ IRggbChannelVector** outrggb);

    /**
     * Copy all gain factors in row-major order from this lens shading map into the destination.
     *
     * <p>Each gain factor will be >= {@link #MINIMUM_GAIN_FACTOR}.</p>
     *
     * @param destination
     *          an array big enough to hold at least {@link RggbChannelVector#COUNT}
     *          elements after the {@code offset}
     * @param offset
     *          a non-negative offset into the array
     * @throws NullPointerException
     *          If {@code destination} was {@code null}
     * @throws IllegalArgumentException
     *          If offset was negative
     * @throws ArrayIndexOutOfBoundsException
     *          If there's not enough room to write the elements at the specified destination and
     *          offset.
     *
     * @see CaptureResult#STATISTICS_LENS_SHADING_MAP
     */
    CARAPI CopyGainFactors(
        /* [in] */ ArrayOf<Float>* destination,
        /* [in] */ Int32 offset);

    /**
     * Check if this LensShadingMap is equal to another LensShadingMap.
     *
     * <p>Two lens shading maps are equal if and only if they have the same rows/columns,
     * and all of their elements are {@link Object#equals equal}.</p>
     *
     * @return {@code true} if the objects were equal, {@code false} otherwise
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* value);

    /**
     * {@inheritDoc}
     */
    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* value);

private:
    Int32 mRows;
    Int32 mColumns;
    AutoPtr<ArrayOf<Float> > mElements;
};

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CLENSSHADINGMAP_H__

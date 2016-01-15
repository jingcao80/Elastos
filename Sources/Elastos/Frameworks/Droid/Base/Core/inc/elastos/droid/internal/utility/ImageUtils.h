#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_IMAGEUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_IMAGEUTILS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IMatrix;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

/**
 * Utility class for image analysis and processing.
 *
 * @hide
 */
class ImageUtils
    : public Object
{
public:
    ImageUtils();

    /**
     * Checks whether a bitmap is grayscale. Grayscale here means "very close to a perfect
     * gray".
     *
     * Instead of scanning every pixel in the bitmap, we first resize the bitmap to no more than
     * COMPACT_BITMAP_SIZE^2 pixels using filtering. The hope is that any non-gray color elements
     * will survive the squeezing process, contaminating the result with color.
     */
    CARAPI_(Boolean) IsGrayscale(
        /* [in] */ IBitmap* bitmap);

    /**
     * Classifies a color as grayscale or not. Grayscale here means "very close to a perfect
     * gray"; if all three channels are approximately equal, this will return true.
     *
     * Note that really transparent colors are always grayscale.
     */
    static CARAPI_(Boolean) IsGrayscale(
        /* [in] */ Int32 color);

private:
    /**
     * Makes sure that {@code mTempBuffer} has at least length {@code size}.
     */
    CARAPI_(void) EnsureBufferSize(
        /* [in] */ Int32 size);

private:
    // Amount (max is 255) that two channels can differ before the color is no longer "gray".
    static const Int32 TOLERANCE;

    // Alpha amount for which values below are considered transparent.
    static const Int32 ALPHA_TOLERANCE;

    // Size of the smaller bitmap we're actually going to scan.
    static const Int32 COMPACT_BITMAP_SIZE; // pixels

    AutoPtr<ArrayOf<Int32> > mTempBuffer;
    AutoPtr<IBitmap> mTempCompactBitmap;
    AutoPtr<ICanvas> mTempCompactBitmapCanvas;
    AutoPtr<IPaint> mTempCompactBitmapPaint;
    AutoPtr<IMatrix> mTempMatrix;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_IMAGEUTILS_H__

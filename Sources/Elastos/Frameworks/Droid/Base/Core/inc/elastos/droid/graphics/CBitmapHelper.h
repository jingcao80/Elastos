
#ifndef __ELASTOS_DROID_GRAPHICS_CBITMAPHELPER_H__
#define __ELASTOS_DROID_GRAPHICS_CBITMAPHELPER_H__

#include "_Elastos_Droid_Graphics_CBitmapHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Core::Singleton;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CBitmapHelper)
    , public Singleton
    , public IBitmapHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI SetDefaultDensity(
        /* [in] */ Int32 density);

    CARAPI GetDefaultDensity(
        /* [out] */ Int32* density);

    CARAPI CreateScaledBitmap(
        /* [in] */ IBitmap* src,
        /* [in] */ Int32 dstWidth,
        /* [in] */ Int32 dstHeight,
        /* [in] */ Boolean filter,
        /* [out] */ IBitmap** bitmap);

    /**
     * Returns an immutable bitmap from the source bitmap. The new bitmap may
     * be the same object as source, or a copy may have been made.  It is
     * initialized with the same density as the original bitmap.
     */
    CARAPI CreateBitmap(
        /* [in] */ IBitmap* source,
        /* [out] */ IBitmap** bitmap);

    /**
     * Returns an immutable bitmap from the specified subset of the source
     * bitmap. The new bitmap may be the same object as source, or a copy may
     * have been made. It is initialized with the same density as the original
     * bitmap.
     *
     * @param source   The bitmap we are subsetting
     * @param x        The x coordinate of the first pixel in source
     * @param y        The y coordinate of the first pixel in source
     * @param width    The number of pixels in each row
     * @param height   The number of rows
     * @return A copy of a subset of the source bitmap or the source bitmap itself.
     */
    CARAPI CreateBitmap(
        /* [in] */ IBitmap* source,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ IBitmap** bitmap);

    /**
     * Returns an immutable bitmap from subset of the source bitmap,
     * transformed by the optional matrix. The new bitmap may be the
     * same object as source, or a copy may have been made. It is
     * initialized with the same density as the original bitmap.
     *
     * If the source bitmap is immutable and the requested subset is the
     * same as the source bitmap itself, then the source bitmap is
     * returned and no new bitmap is created.
     *
     * @param source   The bitmap we are subsetting
     * @param x        The x coordinate of the first pixel in source
     * @param y        The y coordinate of the first pixel in source
     * @param width    The number of pixels in each row
     * @param height   The number of rows
     * @param m        Optional matrix to be applied to the pixels
     * @param filter   true if the source should be filtered.
     *                   Only applies if the matrix contains more than just
     *                   translation.
     * @return A bitmap that represents the specified subset of source
     * @throws IllegalArgumentException if the x, y, width, height values are
     *         outside of the dimensions of the source bitmap.
     */
    CARAPI CreateBitmap(
        /* [in] */ IBitmap* source,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ IMatrix* m,
        /* [in] */ Boolean filter,
        /* [out] */ IBitmap** bitmap);

    /**
     * Returns a mutable bitmap with the specified width and height.  Its
     * initial density is as per {@link #getDensity}.
     *
     * @param width    The width of the bitmap
     * @param height   The height of the bitmap
     * @param config   The bitmap config to create.
     * @throws IllegalArgumentException if the width or height are <= 0
     */
    CARAPI CreateBitmap(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ BitmapConfig config,
        /* [out] */ IBitmap** bitmap);

    /**
     * Returns a immutable bitmap with the specified width and height, with each
     * pixel value set to the corresponding value in the colors array.  Its
     * initial density is as per {@link #getDensity}.
     *
     * @param colors   Array of {@link Color} used to initialize the pixels.
     * @param offset   Number of values to skip before the first color in the
     *                 array of colors.
     * @param stride   Number of colors in the array between rows (must be >=
     *                 width or <= -width).
     * @param width    The width of the bitmap
     * @param height   The height of the bitmap
     * @param config   The bitmap config to create. If the config does not
     *                 support per-pixel alpha (e.g. RGB_565), then the alpha
     *                 bytes in the colors[] will be ignored (assumed to be FF)
     * @throws IllegalArgumentException if the width or height are <= 0, or if
     *         the color array's length is less than the number of pixels.
     */
    CARAPI CreateBitmap(
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ BitmapConfig config,
        /* [out] */ IBitmap** bitmap);

    /**
     * Returns a immutable bitmap with the specified width and height, with each
     * pixel value set to the corresponding value in the colors array.  Its
     * initial density is as per {@link #getDensity}.
     *
     * @param colors   Array of {@link Color} used to initialize the pixels.
     *                 This array must be at least as large as width * height.
     * @param width    The width of the bitmap
     * @param height   The height of the bitmap
     * @param config   The bitmap config to create. If the config does not
     *                 support per-pixel alpha (e.g. RGB_565), then the alpha
     *                 bytes in the colors[] will be ignored (assumed to be FF)
     * @throws IllegalArgumentException if the width or height are <= 0, or if
     *         the color array's length is less than the number of pixels.
     */
    CARAPI CreateBitmap(
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ BitmapConfig config,
        /* [out] */ IBitmap** bitmap);

    /**
     * Returns a mutable bitmap with the specified width and height.  Its
     * initial density is determined from the given {@link DisplayMetrics}.
     *
     * @param display  Display metrics for the display this bitmap will be
     *                 drawn on.
     * @param width    The width of the bitmap
     * @param height   The height of the bitmap
     * @param config   The bitmap config to create.
     * @throws IllegalArgumentException if the width or height are <= 0
     */
    CARAPI CreateBitmap(
        /* [in] */ IDisplayMetrics* display,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ BitmapConfig config,
        /* [out] */ IBitmap** bitmap);

    /**
     * Returns a immutable bitmap with the specified width and height, with each
     * pixel value set to the corresponding value in the colors array.  Its
     * initial density is determined from the given {@link DisplayMetrics}.
     *
     * @param display  Display metrics for the display this bitmap will be
     *                 drawn on.
     * @param colors   Array of {@link Color} used to initialize the pixels.
     * @param offset   Number of values to skip before the first color in the
     *                 array of colors.
     * @param stride   Number of colors in the array between rows (must be >=
     *                 width or <= -width).
     * @param width    The width of the bitmap
     * @param height   The height of the bitmap
     * @param config   The bitmap config to create. If the config does not
     *                 support per-pixel alpha (e.g. RGB_565), then the alpha
     *                 bytes in the colors[] will be ignored (assumed to be FF)
     * @throws IllegalArgumentException if the width or height are <= 0, or if
     *         the color array's length is less than the number of pixels.
     */
    CARAPI CreateBitmap(
        /* [in] */ IDisplayMetrics* display,
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 stride,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ BitmapConfig config,
        /* [out] */ IBitmap** bitmap);

    /**
     * Returns a immutable bitmap with the specified width and height, with each
     * pixel value set to the corresponding value in the colors array.  Its
     * initial density is determined from the given {@link DisplayMetrics}.
     *
     * @param display  Display metrics for the display this bitmap will be
     *                 drawn on.
     * @param colors   Array of {@link Color} used to initialize the pixels.
     *                 This array must be at least as large as width * height.
     * @param width    The width of the bitmap
     * @param height   The height of the bitmap
     * @param config   The bitmap config to create. If the config does not
     *                 support per-pixel alpha (e.g. RGB_565), then the alpha
     *                 bytes in the colors[] will be ignored (assumed to be FF)
     * @throws IllegalArgumentException if the width or height are <= 0, or if
     *         the color array's length is less than the number of pixels.
     */
    CARAPI CreateBitmap(
        /* [in] */ IDisplayMetrics* display,
        /* [in] */ ArrayOf<Int32>* colors,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ BitmapConfig config,
        /* [out] */ IBitmap** bitmap);

    CARAPI ScaleFromDensity(
        /* [in] */ Int32 size,
        /* [in] */ Int32 sdensity,
        /* [in] */ Int32 tdensity,
        /* [out] */ Int32* retSize);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CBITMAPHELPER_H__

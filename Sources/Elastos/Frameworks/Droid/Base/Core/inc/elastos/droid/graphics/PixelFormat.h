#ifndef __ELASTOS_DROID_GRAPHICS_PIXELFORMAT_H__
#define __ELASTOS_DROID_GRAPHICS_PIXELFORMAT_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

class PixelFormat
    : public Object
    , public IPixelFormat
{
public:
    CAR_INTERFACE_DECL();

    static CARAPI_(Boolean) FormatHasAlpha(
        /* [in] */ Int32 format);

    CARAPI GetPixelFormatInfo(
        /* [in] */ Int32 format,
        /* [in] */ IPixelFormat* info);

    CARAPI GetBitsPerPixel(
        /* [out] */ Int32* result);

    /**
     * Determine whether or not this is a public-visible and non-deprecated {@code format}.
     *
     * <p>In particular, {@code @hide} formats will return {@code false}.</p>
     *
     * <p>Any other indirect formats (such as {@code TRANSPARENT} or {@code TRANSLUCENT})
     * will return {@code false}.</p>
     *
     * @param format an integer format
     * @return a boolean
     *
     * @hide
     */
    static CARAPI_(Boolean) IsPublicFormat(
        /* [in] */ Int32 format);

public:
    Int32  mBytesPerPixel;
    Int32  mBitsPerPixel;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_PIXELFORMAT_H__

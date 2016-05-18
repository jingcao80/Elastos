
#ifndef __ELASTOS_DROID_GRAPHICS_CIMAGEFORMAT_H__
#define __ELASTOS_DROID_GRAPHICS_CIMAGEFORMAT_H__

#include "_Elastos_Droid_Graphics_CImageFormat.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CImageFormat)
    , public Singleton
    , public IImageFormat
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    /**
     * Use this function to retrieve the number of bits per pixel of an
     * ImageFormat.
     *
     * @param format
     * @return the number of bits per pixel of the given format or -1 if the
     *         format doesn't exist or is not supported.
     */
    CARAPI GetBitsPerPixel(
        /* [in] */ Int32 format,
        /* [out] */ Int32* bits);

    /**
     * Determine whether or not this is a public-visible {@code format}.
     *
     * <p>In particular, {@code @hide} formats will return {@code false}.</p>
     *
     * <p>Any other formats (including UNKNOWN) will return {@code false}.</p>
     *
     * @param format an integer format
     * @return a boolean
     *
     * @hide
     */
    CARAPI IsPublicFormat(
        /* [in] */ Int32 format,
        /* [out] */ Boolean* is);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CIMAGEFORMAT_H__

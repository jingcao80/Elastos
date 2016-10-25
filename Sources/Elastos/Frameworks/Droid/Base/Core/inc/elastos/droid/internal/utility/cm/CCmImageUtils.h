#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_CCMIMAGEUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_CCMIMAGEUTILS_H__

#include "_Elastos_Droid_Internal_Utility_Cm_CCmImageUtils.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IPoint;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CarClass(CCmImageUtils)
    , public Singleton
    , public ICmImageUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Gets the Width and Height of the image
     *
     * @param inputStream The input stream of the image
     *
     * @return A point structure that holds the Width and Height (x and y)
     */
    CARAPI GetImageDimension(
        /* [in] */ IInputStream* inputStream,
        /* [out] */ IPoint** result);

    /**
     * Crops the input image and returns a new InputStream of the cropped area
     *
     * @param inputStream The input stream of the image
     * @param imageWidth Width of the input image
     * @param imageHeight Height of the input image
     * @param inputStream Desired Width
     * @param inputStream Desired Width
     *
     * @return a new InputStream of the cropped area
     */
    CARAPI CropImage(
        /* [in] */ IInputStream* inputStream,
        /* [in] */ Int32 imageWidth,
        /* [in] */ Int32 imageHeight,
        /* [in] */ Int32 outWidth,
        /* [in] */ Int32 outHeight,
        /* [out] */ IInputStream** result);

    /**
     * Crops the lock screen image and returns a new InputStream of the cropped area
     *
     * @param pkgName Name of the theme package
     * @param context The context
     *
     * @return a new InputStream of the cropped image
     */
    CARAPI GetCroppedKeyguardStream(
        /* [in] */ const String& pkgName,
        /* [in] */ IContext* context,
        /* [out] */ IInputStream** result);

    /**
     * Crops the wallpaper image and returns a new InputStream of the cropped area
     *
     * @param pkgName Name of the theme package
     * @param context The context
     *
     * @return a new InputStream of the cropped image
     */
    CARAPI GetCroppedWallpaperStream(
        /* [in] */ const String& pkgName,
        /* [in] */ IContext* context,
        /* [out] */ IInputStream** result);
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_CCMIMAGEUTILS_H__

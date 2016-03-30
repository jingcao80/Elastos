#include "elastos/droid/internal/utility/cm/CCmImageUtils.h"
#include "elastos/droid/internal/utility/cm/CmImageUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CAR_INTERFACE_IMPL(CCmImageUtils, Singleton, ICmImageUtils)

CAR_SINGLETON_IMPL(CCmImageUtils)

ECode CCmImageUtils::GetImageDimension(
    /* [in] */ IInputStream* inputStream,
    /* [out] */ IPoint** result)
{
    return CmImageUtils::GetImageDimension(inputStream, result);
}

ECode CCmImageUtils::CropImage(
    /* [in] */ IInputStream* inputStream,
    /* [in] */ Int32 imageWidth,
    /* [in] */ Int32 imageHeight,
    /* [in] */ Int32 outWidth,
    /* [in] */ Int32 outHeight,
    /* [out] */ IInputStream** result)
{
    return CmImageUtils::CropImage(inputStream, imageWidth,
            imageHeight, outWidth, outHeight, result);
}

ECode CCmImageUtils::GetCroppedKeyguardStream(
    /* [in] */ const String& pkgName,
    /* [in] */ IContext* context,
    /* [out] */ IInputStream** result)
{
    return CmImageUtils::GetCroppedKeyguardStream(pkgName, context, result);
}

ECode CCmImageUtils::GetCroppedWallpaperStream(
    /* [in] */ const String& pkgName,
    /* [in] */ IContext* context,
    /* [out] */ IInputStream** result)
{
    return CmImageUtils::GetCroppedWallpaperStream(pkgName, context, result);
}

} //namespace Cm
} //namespace Utility
} //namespace Internal
} //namespace Droid
} //namespace Elastos

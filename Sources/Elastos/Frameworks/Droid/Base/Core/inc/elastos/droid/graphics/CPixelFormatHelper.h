
#ifndef __ELASTOS_DROID_GRAPHICS_CPIXELFORMAT_HELPER_H__
#define __ELASTOS_DROID_GRAPHICS_CPIXELFORMAT_HELPER_H__

#include "_Elastos_Droid_Graphics_CPixelFormatHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CPixelFormatHelper)
    , public Singleton
    , public IPixelFormatHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI FormatHasAlpha(
        /* [in] */ Int32 format,
        /* [out] */ Boolean* result);

    CARAPI GetPixelFormatInfo(
        /* [in] */ Int32 format,
        /* [in] */ IPixelFormat* info);

    CARAPI IsPublicFormat(
        /* [in] */ Int32 format,
        /* [out] */ Boolean* result);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CPIXELFORMAT_HELPER_H__

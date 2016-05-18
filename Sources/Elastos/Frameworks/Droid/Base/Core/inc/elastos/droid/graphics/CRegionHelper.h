
#ifndef __ELASTOS_DROID_GRAPHICS_CREGIONHELPER_H__
#define __ELASTOS_DROID_GRAPHICS_CREGIONHELPER_H__

#include "_Elastos_Droid_Graphics_CRegionHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CRegionHelper)
    , public Singleton
    , public IRegionHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI Obtain(
        /* [out] */ IRegion** region);

    CARAPI Obtain(
        /* [in] */ IRegion* other,
        /* [out] */ IRegion** region);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CREGIONHELPER_H__

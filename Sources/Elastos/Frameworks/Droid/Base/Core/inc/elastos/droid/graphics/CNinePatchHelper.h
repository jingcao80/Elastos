
#ifndef __ELASTOS_DROID_GRAPHICS_CNINEPATCHHELPER_H__
#define __ELASTOS_DROID_GRAPHICS_CNINEPATCHHELPER_H__

#include "_Elastos_Droid_Graphics_CNinePatchHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CNinePatchHelper)
    , public Singleton
    , public INinePatchHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI IsNinePatchChunk(
        /* [in] */ ArrayOf<Byte>* chunk,
        /* [out] */ Boolean* result);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CNINEPATCHHELPER_H__



#ifndef __ELASTOS_DROID_GRAPHICS_CPOINTFHELPER_H__
#define __ELASTOS_DROID_GRAPHICS_CPOINTFHELPER_H__

#include "_Elastos_Droid_Graphics_CPointFHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CPointFHelper)
    , public Singleton
    , public IPointFHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI Length(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [out] */ Float* length);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CPOINTFHELPER_H__

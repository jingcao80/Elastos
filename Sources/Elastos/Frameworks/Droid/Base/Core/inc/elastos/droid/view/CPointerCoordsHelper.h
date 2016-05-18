#ifndef __ELASTOS_DROID_VIEW_CPOINTERCOORDSHELPER_H__
#define __ELASTOS_DROID_VIEW_CPOINTERCOORDSHELPER_H__

#include "_Elastos_Droid_View_CPointerCoordsHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CPointerCoordsHelper)
    , public Singleton
    , public IPointerCoordsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI CreateArray(
        /* [in] */ Int32 size,
        /* [out, callee] */ ArrayOf<IPointerCoords*>** array);
};

} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_CPOINTERCOORDSHELPER_H__

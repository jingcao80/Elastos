#ifndef __ELASTOS_DROID_VIEW_CPOINTERPROPERTIESHELPER_H__
#define __ELASTOS_DROID_VIEW_CPOINTERPROPERTIESHELPER_H__

#include "_Elastos_Droid_View_CPointerPropertiesHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CPointerPropertiesHelper)
    , public Singleton
    , public IPointerPropertiesHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI CreateArray(
        /* [in] */ Int32 size,
        /* [out, callee] */ ArrayOf<IPointerProperties*>** array);
};

} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_CPOINTERPROPERTIESHELPER_H__

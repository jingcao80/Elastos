
#ifndef __ELASTOS_DROID_VIEW_CFALLBACKACTIONHELPER_H__
#define __ELASTOS_DROID_VIEW_CFALLBACKACTIONHELPER_H__

#include "_Elastos_Droid_View_CFallbackActionHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CFallbackActionHelper)
    , public Singleton
    , public IFallbackActionHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI Obtain(
        /* [out] */ IFallbackAction** action);
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CFALLBACKACTIONHELPER_H__


#ifndef __ELASTOS_DROID_LOCATION_CLOCATIONPROVIDERHELPER_H__
#define __ELASTOS_DROID_LOCATION_CLOCATIONPROVIDERHELPER_H__

#include "_Elastos_Droid_Location_CLocationProviderHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Internal::Location::IProviderProperties;

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CLocationProviderHelper)
    , public Singleton
    , public ILocationProviderHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI PropertiesMeetCriteria(
        /* [in] */ const String& name,
        /* [in] */ IProviderProperties* properties,
        /* [in] */ ICriteria* criteria,
        /* [out] */ Boolean* rst);
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif

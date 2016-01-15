
#ifndef __ELASTOS_DROID_LOCATION_CLOCATIONREQUESTHELPER_H__
#define __ELASTOS_DROID_LOCATION_CLOCATIONREQUESTHELPER_H__

#include "_Elastos_Droid_Location_CLocationRequestHelper.h"
#include "elastos/droid/location/LocationRequest.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Location {

CarClass(CLocationRequestHelper)
    , public Singleton
    , public ILocationRequestHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Create a location request with default parameters.
     *
     * <p>Default parameters are for a low power, slowly updated location.
     * It can then be adjusted as required by the applications before passing
     * to the {@link LocationManager}
     *
     * @return a new location request
     */
    CARAPI Create(
        /* [out] */ ILocationRequest** lr);

    /** @hide */
    CARAPI CreateFromDeprecatedProvider(
        /* [in] */ const String& provider,
        /* [in] */ Int64 minTime,
        /* [in] */ Float minDistance,
        /* [in] */ Boolean singleShot,
        /* [out] */ ILocationRequest** lr);

    /** @hide */
    CARAPI CreateFromDeprecatedCriteria(
        /* [in] */ ICriteria* criteria,
        /* [in] */ Int64 minTime,
        /* [in] */ Float minDistance,
        /* [in] */ Boolean singleShot,
        /* [out] */ ILocationRequest** lr);

    /** @hide */
    CARAPI QualityToString(
        /* [in] */ Int32 quality,
        /* [out] */ String* str);
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_CLOCATIONREQUESTHELPER_H__
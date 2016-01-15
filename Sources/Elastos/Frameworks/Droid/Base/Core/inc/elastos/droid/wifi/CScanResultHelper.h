
#ifndef __ELASTOS_DROID_NET_WIFI_CSCANRESULTHELPER_H__
#define __ELASTOS_DROID_NET_WIFI_CSCANRESULTHELPER_H__

#include "_Elastos_Droid_Wifi_CScanResultHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CScanResultHelper)
    , public Singleton
    , public IScanResultHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * @hide
     * TODO: makes real freq boundaries
     */
    CARAPI Is24GHz(
        /* [in] */ Int32 freq,
        /* [out] */ Boolean* result);

    /**
     * @hide
     * TODO: makes real freq boundaries
     */
    CARAPI Is5GHz(
        /* [in] */ Int32 freq,
        /* [out] */ Boolean* result);
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CSCANRESULTHELPER_H__

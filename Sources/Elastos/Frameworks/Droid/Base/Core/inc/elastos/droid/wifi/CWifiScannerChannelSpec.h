
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFISCANNERCHANNELSPEC_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFISCANNERCHANNELSPEC_H__

#include "_Elastos_Droid_Wifi_CWifiScannerChannelSpec.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiScannerChannelSpec)
    , public Object
    , public IWifiScannerChannelSpec
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int32 frequency);

    /**
     * channel frequency in MHz; for example channel 1 is specified as 2412
     */
    CARAPI GetFrequency(
        /* [out] */ Int32* result);

    CARAPI SetFrequency(
        /* [in] */ Int32 frequency);

    /**
     * if true, scan this channel in passive fashion.
     * This flag is ignored on DFS channel specification.
     * @hide
     */
    CARAPI GetPassive(
        /* [out] */ Boolean* result);

    CARAPI SetPassive(
        /* [in] */ Boolean passive);

    /**
     * how long to dwell on this channel
     * @hide
     */
    CARAPI GetDwellTimeMS(
        /* [out] */ Int32* result);

    CARAPI SetDwellTimeMS(
        /* [in] */ Int32 dwellTimeMS);

private:
    /**
     * channel frequency in MHz; for example channel 1 is specified as 2412
     */
    Int32 mFrequency;
    /**
     * if true, scan this channel in passive fashion.
     * This flag is ignored on DFS channel specification.
     * @hide
     */
    Boolean mPassive;                                    /* ignored on DFS channels */
    /**
     * how long to dwell on this channel
     * @hide
     */
    Int32 mDwellTimeMS;                                    /* not supported for now */
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFISCANNERCHANNELSPEC_H__

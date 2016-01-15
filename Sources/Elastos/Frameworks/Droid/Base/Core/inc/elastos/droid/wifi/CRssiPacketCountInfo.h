
#ifndef __ELASTOS_DROID_NET_WIFI_CRSSIPACKETCOUNTINFO_H__
#define __ELASTOS_DROID_NET_WIFI_CRSSIPACKETCOUNTINFO_H__

#include "_Elastos_Droid_Wifi_CRssiPacketCountInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CRssiPacketCountInfo)
    , public Object
    , public IRssiPacketCountInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcel* parcel);

    CARAPI GetRssi(
        /* [out] */ Int32* rssi);

    CARAPI SetRssi(
        /* [in] */ Int32 rssi);

    CARAPI GetTxgood(
        /* [out] */ Int32* txgood);

    CARAPI SetTxgood(
        /* [in] */ Int32 txgood);

    CARAPI GetTxbad(
        /* [out] */ Int32* txbad);

    CARAPI SetTxbad(
        /* [in] */ Int32 txbad);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetRxgood(
        /* [out] */ Int32* result);

    CARAPI SetRxgood(
        /* [in] */ Int32 rxgood);

private:
    Int32 mRssi;
    Int32 mTxgood;
    Int32 mTxbad;
    Int32 mRxgood;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_CRSSIPACKETCOUNTINFO_H__

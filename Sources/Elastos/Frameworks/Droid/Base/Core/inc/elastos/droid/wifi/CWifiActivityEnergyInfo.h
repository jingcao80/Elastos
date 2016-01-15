
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFIACTIVITYENERGYINFO_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFIACTIVITYENERGYINFO_H__

#include "_Elastos_Droid_Wifi_CWifiActivityEnergyInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiActivityEnergyInfo)
    , public Object
    , public IParcelable
    , public IWifiActivityEnergyInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int32 stackState,
        /* [in] */ Int32 txTime,
        /* [in] */ Int32 rxTime,
        /* [in] */ Int32 idleTime,
        /* [in] */ Int32 energyUsed);

    /**
     * @return bt stack reported state
     */
    CARAPI GetStackState(
        /* [out] */ Int32* result);

    /**
     * @return tx time in ms
     */
    CARAPI GetControllerTxTimeMillis(
        /* [out] */ Int32* result);

    /**
     * @return rx time in ms
     */
    CARAPI GetControllerRxTimeMillis(
        /* [out] */ Int32* result);

    /**
     * @return idle time in ms
     */
    CARAPI GetControllerIdleTimeMillis(
        /* [out] */ Int32* result);

    /**
     * product of current(mA), voltage(V) and time(ms)
     * @return energy used
     */
    CARAPI GetControllerEnergyUsed(
        /* [out] */ Int32* result);

    /**
     * @return timestamp(wall clock) of record creation
     */
    CARAPI GetTimeStamp(
        /* [out] */ Int64* result);

    /**
     * @return if the record is valid
     */
    CARAPI IsValid(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out,
        /* [in] */ Int32 flags);

    CARAPI DescribeContents(
        /* [out] */ Int32* contents);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    // public static final Parcelable.Creator<WifiActivityEnergyInfo> CREATOR =
    //         new Parcelable.Creator<WifiActivityEnergyInfo>() {
    //     public WifiActivityEnergyInfo createFromParcel(Parcel in) {
    //         int stackState = in.readInt();
    //         int txTime = in.readInt();
    //         int rxTime = in.readInt();
    //         int idleTime = in.readInt();
    //         int energyUsed = in.readInt();
    //         return new WifiActivityEnergyInfo(stackState, txTime, rxTime,
    //                 idleTime, energyUsed);
    //     }
    //     public WifiActivityEnergyInfo[] newArray(int size) {
    //         return new WifiActivityEnergyInfo[size];
    //     }
    // };

private:
    Int32 mStackState;
    Int32 mControllerTxTimeMs;
    Int32 mControllerRxTimeMs;
    Int32 mControllerIdleTimeMs;
    Int32 mControllerEnergyUsed;
    Int64 mTimestamp;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFIACTIVITYENERGYINFO_H__

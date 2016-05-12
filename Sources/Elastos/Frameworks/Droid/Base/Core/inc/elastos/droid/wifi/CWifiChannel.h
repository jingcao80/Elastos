
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFICHANNEL_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFICHANNEL_H__

#include "_Elastos_Droid_Wifi_CWifiChannel.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiChannel)
    , public Object
    , public IParcelable
    , public IWifiChannel
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetFreqMHz(
        /* [out] */ Int32* result);

    CARAPI SetFreqMHz(
        /* [in] */ Int32 freqMHz);

    CARAPI GetChannelNum(
        /* [out] */ Int32* result);

    CARAPI SetChannelNum(
        /* [in] */ Int32 channelNum);

    CARAPI GetIsDFS(
        /* [out] */ Boolean* result);

    CARAPI SetIsDFS(
        /* [in] */ Boolean isDFS);

    /** is IBSS allowed? */
    CARAPI GetIbssAllowed(
        /* [out] */ Boolean* result);

    CARAPI SetIbssAllowed(
        /* [in] */ Boolean ibssAllowed);

    /** check for validity */
    CARAPI IsValid(
        /* [out] */ Boolean* result);

    /** implement Parcelable interface */
    //@Override
    CARAPI DescribeContents(
        /* [out] */ Int32* contents);

    /** implement Parcelable interface */
    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

public:
    /** implement Parcelable interface */
    // public static final Parcelable.Creator<WifiChannel> CREATOR =
    //         new Parcelable.Creator<WifiChannel>() {
    //     @Override
    //     public WifiChannel createFromParcel(Parcel in) {
    //         WifiChannel channel = new WifiChannel();
    //         channel.freqMHz = in.readInt();
    //         channel.channelNum = in.readInt();
    //         channel.isDFS = in.readInt() != 0;
    //         return channel;
    //     }

    //     @Override
    //     public WifiChannel[] newArray(int size) {
    //         return new WifiChannel[size];
    //     }
    // };

private:
    static const Int32 MIN_FREQ_MHZ = 2412;
    static const Int32 MAX_FREQ_MHZ = 5825;

    static const Int32 MIN_CHANNEL_NUM = 1;
    static const Int32 MAX_CHANNEL_NUM = 196;

    /** frequency */
    Int32 mFreqMHz;

    /** channel number */
    Int32 mChannelNum;

    /** is it a DFS channel? */
    Boolean mIsDFS;

    /** is IBSS allowed? */
    Boolean mIbssAllowed;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFICHANNEL_H__

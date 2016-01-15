
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFINETWORKCONNECTIONSTATISTICS_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFINETWORKCONNECTIONSTATISTICS_H__

#include "_Elastos_Droid_Wifi_CWifiNetworkConnectionStatistics.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiNetworkConnectionStatistics)
    , public Object
    , public IWifiNetworkConnectionStatistics
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWifiNetworkConnectionStatistics();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 connection,
        /* [in] */ Int32 usage);

    CARAPI constructor(
        /* [in] */ IWifiNetworkConnectionStatistics* source);

    CARAPI GetNumConnection(
        /* [out] */ Int32* result);

    CARAPI SetNumConnection(
        /* [in] */ Int32 numConnection);

    CARAPI GetNumUsage(
        /* [out] */ Int32* result);

    CARAPI SetNumUsage(
        /* [in] */ Int32 numUsage);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    /** Implement the Parcelable interface */
    CARAPI DescribeContents(
        /* [out] */ Int32* contents);

    /** Implement the Parcelable interface */
    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest,
        /* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    /** Implement the Parcelable interface */
    // public static final Creator<WifiNetworkConnectionStatistics> CREATOR =
    //     new Creator<WifiNetworkConnectionStatistics>() {
    //         public WifiNetworkConnectionStatistics createFromParcel(Parcel in) {
    //             int numConnection = in.readInt();
    //             int numUsage = in.readInt();
    //             WifiNetworkConnectionStatistics stats =
    //                     new WifiNetworkConnectionStatistics(numConnection, numUsage);
    //             return stats;
    //         }

    //         public WifiNetworkConnectionStatistics[] newArray(int size) {
    //             return new WifiNetworkConnectionStatistics[size];
    //         }
    //     };

private:
    static const String TAG;

    Int32 mNumConnection;
    Int32 mNumUsage;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFINETWORKCONNECTIONSTATISTICS_H__

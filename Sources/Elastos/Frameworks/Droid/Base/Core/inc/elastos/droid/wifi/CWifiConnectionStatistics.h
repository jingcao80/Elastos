
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFICONNECTIONSTATISTICS_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFICONNECTIONSTATISTICS_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Wifi_CWifiConnectionStatistics.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiConnectionStatistics)
    , public Object
    , public IParcelable
    , public IWifiConnectionStatistics
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWifiConnectionStatistics();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IWifiConnectionStatistics* source);

    CARAPI GetUntrustedNetworkHistory(
        /* [out] */ IHashMap** result);

    CARAPI SetUntrustedNetworkHistory(
        /* [in] */ IHashMap* untrustedNetworkHistory);

    CARAPI GetNum5GhzConnected(
        /* [out] */ Int32* result);

    CARAPI SetNum5GhzConnected(
        /* [in] */ Int32 num5GhzConnected);

    CARAPI GetNum24GhzConnected(
        /* [out] */ Int32* result);

    CARAPI SetNum24GhzConnected(
        /* [in] */ Int32 num24GhzConnected);

    CARAPI GetNumAutoJoinAttempt(
        /* [out] */ Int32* result);

    CARAPI SetNumAutoJoinAttempt(
        /* [in] */ Int32 numAutoJoinAttempt);

    CARAPI GetNumAutoRoamAttempt(
        /* [out] */ Int32* result);

    CARAPI SetNumAutoRoamAttempt(
        /* [in] */ Int32 numAutoRoamAttempt);

    CARAPI GetNumWifiManagerJoinAttempt(
        /* [out] */ Int32* result);

    CARAPI SetNumWifiManagerJoinAttempt(
        /* [in] */ Int32 numWifiManagerJoinAttempt);

    CARAPI IncrementOrAddUntrusted(
        /* [in] */ const String& SSID,
        /* [in] */ Int32 connection,
        /* [in] */ Int32 usage);

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
    // public static final Creator<WifiConnectionStatistics> CREATOR =
    //     new Creator<WifiConnectionStatistics>() {
    //         public WifiConnectionStatistics createFromParcel(Parcel in) {
    //             WifiConnectionStatistics stats = new WifiConnectionStatistics();
    //             stats.num24GhzConnected = in.readInt();
    //             stats.num5GhzConnected = in.readInt();
    //             stats.numAutoJoinAttempt = in.readInt();
    //             stats.numAutoRoamAttempt = in.readInt();
    //             stats.numWifiManagerJoinAttempt = in.readInt();
    //             int n = in.readInt();
    //             while (n-- > 0) {
    //                 String Key = in.readString();
    //                 int numConnection = in.readInt();
    //                 int numUsage = in.readInt();
    //                 WifiNetworkConnectionStatistics st =
    //                         new WifiNetworkConnectionStatistics(numConnection, numUsage);
    //                 stats.untrustedNetworkHistory.put(Key, st);
    //             }
    //             return stats;
    //         }

    //         public WifiConnectionStatistics[] newArray(int size) {
    //             return new WifiConnectionStatistics[size];
    //         }
    //     };

private:
    static const String TAG;

    /**
     *  history of past connection to untrusted SSID
     *  Key = SSID
     *  Value = num connection
     */
    AutoPtr<IHashMap> mUntrustedNetworkHistory;// String, WifiNetworkConnectionStatistics>

    // Number of time we polled the chip and were on 5GHz
    Int32 mNum5GhzConnected;

    // Number of time we polled the chip and were on 2.4GHz
    Int32 mNum24GhzConnected;

    // Number autojoin attempts
    Int32 mNumAutoJoinAttempt;

    // Number auto-roam attempts
    Int32 mNumAutoRoamAttempt;

    // Number wifimanager join attempts
    Int32 mNumWifiManagerJoinAttempt;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFICONNECTIONSTATISTICS_H__

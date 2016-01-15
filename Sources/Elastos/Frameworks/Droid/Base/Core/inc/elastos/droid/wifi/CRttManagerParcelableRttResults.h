
#ifndef __ELASTOS_DROID_NET_WIFI_CRTTMANAGERPARCELABLERTTRESULTS_H__
#define __ELASTOS_DROID_NET_WIFI_CRTTMANAGERPARCELABLERTTRESULTS_H__

#include "_Elastos_Droid_Wifi_CRttManagerParcelableRttResults.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CRttManagerParcelableRttResults)
    , public Object
    , public IParcelable
    , public IRttManagerParcelableRttResults
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ ArrayOf<IRttManagerRttResult*>* result);

    CARAPI GetResults(
        /* [out, callee] */ ArrayOf<IRttManagerRttResult*>** result);

    CARAPI SetResults(
        /* [in] */ ArrayOf<IRttManagerRttResult*>* results);

    CARAPI DescribeContents(
        /* [out] */ Int32* contents);

    /** Implement the Parcelable interface {@hide} */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest,
        /* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    // /** Implement the Parcelable interface {@hide} */
    // public static final Creator<ParcelableRttResults> CREATOR =
    //         new Creator<ParcelableRttResults>() {
    //             public ParcelableRttResults createFromParcel(Parcel in) {

    //                 int num = in.readInt();

    //                 if (num == 0) {
    //                     return new ParcelableRttResults(null);
    //                 }

    //                 RttResult results[] = new RttResult[num];
    //                 for (int i = 0; i < num; i++) {
    //                     results[i] = new RttResult();
    //                     results[i].bssid = in.readString();
    //                     results[i].status = in.readInt();
    //                     results[i].requestType = in.readInt();
    //                     results[i].ts = in.readLong();
    //                     results[i].rssi = in.readInt();
    //                     results[i].rssi_spread = in.readInt();
    //                     results[i].tx_rate = in.readInt();
    //                     results[i].rtt_ns = in.readLong();
    //                     results[i].rtt_sd_ns = in.readLong();
    //                     results[i].rtt_spread_ns = in.readLong();
    //                     results[i].distance_cm = in.readInt();
    //                     results[i].distance_sd_cm = in.readInt();
    //                     results[i].distance_spread_cm = in.readInt();
    //                 }

    //                 ParcelableRttResults parcelableResults = new ParcelableRttResults(results);
    //                 return parcelableResults;
    //             }

    //             public ParcelableRttResults[] newArray(int size) {
    //                 return new ParcelableRttResults[size];
    //             }
    //         };

private:
    AutoPtr< ArrayOf<IRttManagerRttResult*> > mResults;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CRTTMANAGERPARCELABLERTTRESULTS_H__

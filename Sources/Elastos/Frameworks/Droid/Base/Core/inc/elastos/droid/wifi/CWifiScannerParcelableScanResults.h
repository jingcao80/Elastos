
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFISCANNERPARCELABLESCANRESULTS_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFISCANNERPARCELABLESCANRESULTS_H__

#include "_Elastos_Droid_Wifi_CWifiScannerParcelableScanResults.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiScannerParcelableScanResults)
    , public Object
    , public IParcelable
    , public IWifiScannerParcelableScanResults
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<IScanResult*>* results);

    CARAPI SetResults(
        /* [in] */ ArrayOf<IScanResult*>* results);

    CARAPI GetResults(
        /* [out, callee] */ ArrayOf<IScanResult*>** result);

    /** Implement the Parcelable interface {@hide} */
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
    /** Implement the Parcelable interface {@hide} */
    // public static final Creator<ParcelableScanResults> CREATOR =
    //         new Creator<ParcelableScanResults>() {
    //             public ParcelableScanResults createFromParcel(Parcel in) {
    //                 int n = in.readInt();
    //                 ScanResult results[] = new ScanResult[n];
    //                 for (int i = 0; i < n; i++) {
    //                     results[i] = ScanResult.CREATOR.createFromParcel(in);
    //                 }
    //                 return new ParcelableScanResults(results);
    //             }

    //             public ParcelableScanResults[] newArray(int size) {
    //                 return new ParcelableScanResults[size];
    //             }
    //         };

private:
    AutoPtr< ArrayOf<IScanResult*> > mResults;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFISCANNERPARCELABLESCANRESULTS_H__

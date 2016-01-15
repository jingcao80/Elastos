
#ifndef __ELASTOS_DROID_NET_WIFI_BATCHEDSCANRESULT_H__
#define __ELASTOS_DROID_NET_WIFI_BATCHEDSCANRESULT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Wifi {

/**
 * Describes the Results of a batched set of wifi scans where the firmware performs many
 * scans and stores the timestamped results without waking the main processor each time.
 * @hide pending review
 */
class BatchedScanResult
    : public Object
    //, public IParcelable
{

public:
    BatchedScanResult();

    BatchedScanResult(
        /* [in] */ const BatchedScanResult& source);

    CARAPI ToString(
        /* [out] */ String* str);

    /** Implement the Parcelable interface {@hide} */
    CARAPI DescribeContents(
        /* [out] */ Int32* contents);

    /** Implement the Parcelable interface {@hide} */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest,
        /* [in] */ Int32 flags);

public:
    /** Inidcates this scan was interrupted and may only have partial results. */
    Boolean truncated;

    /** The result of this particular scan. */
    AutoPtr<IList> scanResults;

    /** Implement the Parcelable interface {@hide} */
    // public static final Creator<BatchedScanResult> CREATOR =
    //     new Creator<BatchedScanResult>() {
    //         public BatchedScanResult createFromParcel(Parcel in) {
    //             BatchedScanResult result = new BatchedScanResult();
    //             result.truncated = (in.readInt() == 1);
    //             int count = in.readInt();
    //             while (count-- > 0) {
    //                 result.scanResults.add(ScanResult.CREATOR.createFromParcel(in));
    //             }
    //             return result;
    //         }

    //         public BatchedScanResult[] newArray(int size) {
    //             return new BatchedScanResult[size];
    //         }
    //     };

private:
    static const String TAG;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_BATCHEDSCANRESULT_H__

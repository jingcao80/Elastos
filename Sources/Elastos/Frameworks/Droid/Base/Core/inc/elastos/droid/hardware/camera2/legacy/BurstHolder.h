
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_BURSTHOLDER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_BURSTHOLDER_H__

#include "Elastos.Droid.Hardware.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class BurstHolder
    : public Object
    , public IBurstHolder
{
public:
    CAR_INTERFACE_DECL()

    BurstHolder();

    virtual ~BurstHolder() {}

    CARAPI constructor();

    /**
     * Immutable container for a burst of capture results.
     *
     * @param requestId id of the burst request.
     * @param repeating true if this burst is repeating.
     * @param requests a {@link List} of {@link CaptureRequest}s in this burst.
     * @param jpegSurfaceIds a {@link Collection} of IDs for the surfaces that have jpeg outputs.
     */
    CARAPI constructor(
        /* [in] */ Int32 requestId,
        /* [in] */ Boolean repeating,
        /* [in] */ IList* requests,
        /* [in] */ ICollection* jpegSurfaceIds);

    /**
     * Get the id of this request.
     */
    CARAPI GetRequestId(
        /* [out] */ Int32* value);

    /**
     * Return true if this repeating.
     */
    CARAPI IsRepeating(
        /* [out] */ Boolean* value);

    /**
     * Return the number of requests in this burst sequence.
     */
    CARAPI GetNumberOfRequests(
        /* [out] */ Int32* value);

    /**
     * Create a list of {@link RequestHolder} objects encapsulating the requests in this burst.
     *
     * @param frameNumber the starting framenumber for this burst.
     * @return the list of {@link RequestHolder} objects.
     */
    CARAPI ProduceRequestHolders(
        /* [in] */ Int64 frameNumber,
        /* [out] */ IList** outlist);

private:
    static const String TAG;
    AutoPtr<IArrayList> mRequestBuilders;
    Boolean mRepeating;
    Int32 mRequestId;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_BURSTHOLDER_H__

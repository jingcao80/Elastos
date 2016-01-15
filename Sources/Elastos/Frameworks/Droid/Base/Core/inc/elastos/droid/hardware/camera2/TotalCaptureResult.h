
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_TOTALCAPTURERESULT_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_TOTALCAPTURERESULT_H__

#include "Elastos.Droid.Hardware.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/camera2/CaptureResult.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

class TotalCaptureResult
    : public CaptureResult
    , public ITotalCaptureResult
{
public:
    CAR_INTERFACE_DECL()

    TotalCaptureResult();

    virtual ~TotalCaptureResult() {}

    CARAPI constructor();

    /**
     * Takes ownership of the passed-in camera metadata and the partial results
     *
     * @param partials a list of partial results; {@code null} will be substituted for an empty list
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ICameraMetadataNative* results,
        /* [in] */ ICaptureRequest* parent,
        /* [in] */ ICaptureResultExtras* extras,
        /* [in] */ IList* partials);

    /**
     * Creates a request-less result.
     *
     * <p><strong>For testing only.</strong></p>
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ICameraMetadataNative* results,
        /* [in] */ Int32 sequenceId);

    /**
     * Get the read-only list of partial results that compose this total result.
     *
     * <p>The list is returned is unmodifiable; attempting to modify it will result in a
     * {@code UnsupportedOperationException} being thrown.</p>
     *
     * <p>The list size will be inclusive between {@code 0} and
     * {@link CameraCharacteristics#REQUEST_PARTIAL_RESULT_COUNT}, with elements in ascending order
     * of when {@link CameraCaptureSession.CaptureCallback#onCaptureProgressed} was invoked.</p>
     *
     * @return unmodifiable list of partial results
     */
    CARAPI GetPartialResults(
        /* [out] */ IList** outlist);

private:
    AutoPtr<IList> mPartialResults;
};

} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_TOTALCAPTURERESULT_H__

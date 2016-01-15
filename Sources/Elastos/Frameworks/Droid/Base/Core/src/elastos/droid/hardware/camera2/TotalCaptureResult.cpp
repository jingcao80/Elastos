
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/camera2/TotalCaptureResult.h"

using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CAR_INTERFACE_IMPL(TotalCaptureResult, CaptureResult, ITotalCaptureResult)

TotalCaptureResult::TotalCaptureResult()
{
}

ECode TotalCaptureResult::constructor()
{
    return NOERROR;
}

ECode TotalCaptureResult::constructor(
    /* [in] */ ICameraMetadataNative* results,
    /* [in] */ ICaptureRequest* parent,
    /* [in] */ ICaptureResultExtras* extras,
    /* [in] */ IList* partials)
{
    CaptureResult::constructor(results, parent, extras);

    if (partials == NULL) {
        CArrayList::New((IList**)&mPartialResults);
    }
    else {
        mPartialResults = partials;
    }
    return NOERROR;
}

ECode TotalCaptureResult::constructor(
    /* [in] */ ICameraMetadataNative* results,
    /* [in] */ Int32 sequenceId)
{
    CaptureResult::constructor(results, sequenceId);

    return CArrayList::New((IList**)&mPartialResults);
}

ECode TotalCaptureResult::GetPartialResults(
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist)

    AutoPtr<ICollections> c;
    CCollections::AcquireSingleton((ICollections**)&c);
    return c->UnmodifiableList(mPartialResults, outlist);
}

} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
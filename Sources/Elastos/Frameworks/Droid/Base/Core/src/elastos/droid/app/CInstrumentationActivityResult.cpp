
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/CInstrumentationActivityResult.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CInstrumentationActivityResult, Object, IInstrumentationActivityResult)

CAR_OBJECT_IMPL(CInstrumentationActivityResult)

CInstrumentationActivityResult::CInstrumentationActivityResult()
    : mResultCode(0)
{
}

ECode CInstrumentationActivityResult::constructor(
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* resultData)
{
    mResultCode = resultCode;
    mResultData = resultData;
    return NOERROR;
}

ECode CInstrumentationActivityResult::GetResultCode(
    /* [out] */ Int32* resultCode)
{
    VALIDATE_NOT_NULL(resultCode);
    *resultCode = mResultCode;
    return NOERROR;
}

ECode CInstrumentationActivityResult::GetResultData(
    /* [out] */ IIntent** resultData)
{
    VALIDATE_NOT_NULL(resultData);
    *resultData = mResultData;
    REFCOUNT_ADD(*resultData);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos

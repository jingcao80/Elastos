
#include "RecursiveAction.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

//===============================================================================
// RecursiveAction::
//===============================================================================
Int64 RecursiveAction::mSerialVersionUID = 5232453952276485070L;

CAR_INTERFACE_IMPL(RecursiveAction, ForkJoinTask, IRecursiveAction)

ECode RecursiveAction::GetRawResult(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    *outface = NULL;
    return NOERROR;
}

ECode RecursiveAction::SetRawResult(
    /* [in] */ IInterface* value)
{
    return NOERROR;
}

ECode RecursiveAction::Exec(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    Compute();
    *res = TRUE;
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

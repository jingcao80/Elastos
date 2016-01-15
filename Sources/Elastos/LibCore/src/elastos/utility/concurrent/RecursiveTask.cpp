
#include "RecursiveTask.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

//===============================================================================
// RecursiveTask::
//===============================================================================
Int64 RecursiveTask::mSerialVersionUID = 5232453952276485270L;

CAR_INTERFACE_IMPL(RecursiveTask, ForkJoinTask, IRecursiveTask)

ECode RecursiveTask::GetRawResult(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    *outface = mResult;
    REFCOUNT_ADD(*outface)
    return NOERROR;
}

ECode RecursiveTask::SetRawResult(
    /* [in] */ IInterface* value)
{
    mResult = value;
    return NOERROR;
}

ECode RecursiveTask::Exec(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    Compute((IInterface**)&mResult);
    *res = TRUE;
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

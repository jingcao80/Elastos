
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CProcessStartResult.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CProcessStartResult, Object, IProcessStartResult)

CAR_OBJECT_IMPL(CProcessStartResult)

CProcessStartResult::CProcessStartResult()
    : mPid(0)
    , mUsingWrapper(FALSE)
{}

ECode CProcessStartResult::GetPid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid);
    *pid = mPid;
    return NOERROR;
}

ECode CProcessStartResult::SetPid(
    /* [in] */ Int32 pid)
{
    mPid = pid;
    return NOERROR;
}

ECode CProcessStartResult::GetUsingWrapper(
    /* [out] */ Boolean* usingWrapper)
{
    VALIDATE_NOT_NULL(usingWrapper);
    *usingWrapper = mUsingWrapper;
    return NOERROR;
}

ECode CProcessStartResult::SetUsingWrapper(
    /* [in] */ Boolean usingWrapper)
{
    mUsingWrapper = usingWrapper;
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos

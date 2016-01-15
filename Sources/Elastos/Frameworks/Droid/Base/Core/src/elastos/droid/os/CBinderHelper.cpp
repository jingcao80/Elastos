
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/os/CBinderHelper.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CBinderHelper, Singleton, IBinderHelper)

CAR_SINGLETON_IMPL(CBinderHelper)

ECode CBinderHelper::GetCallingPid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid);
    *pid = CBinder::GetCallingPid();
    return NOERROR;
}

ECode CBinderHelper::GetCallingUid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid);
    *pid = CBinder::GetCallingUid();
    return NOERROR;
}

ECode CBinderHelper::GetCallingUserHandle(
    /* [out] */ IUserHandle** h)
{
    VALIDATE_NOT_NULL(h);
    AutoPtr<IUserHandle> userHandle = CBinder::GetCallingUserHandle();
    *h = userHandle;
    REFCOUNT_ADD(*h);
    return NOERROR;
}

ECode CBinderHelper::ClearCallingIdentity(
    /* [out] */ Int64* token)
{
    VALIDATE_NOT_NULL(token);
    *token = CBinder::ClearCallingIdentity();
    return NOERROR;
}

ECode CBinderHelper::RestoreCallingIdentity(
    /* [in] */ Int64 token)
{
    return CBinder::RestoreCallingIdentity(token);
}

ECode CBinderHelper::SetThreadStrictModePolicy(
    /* [in] */ Int32 policyMask)
{
    return CBinder::SetThreadStrictModePolicy(policyMask);
}

ECode CBinderHelper::GetThreadStrictModePolicy(
    /* [out] */ Int32* policy)
{
    VALIDATE_NOT_NULL(policy);
    *policy = CBinder::GetThreadStrictModePolicy();
    return NOERROR;
}

ECode CBinderHelper::FlushPendingCommands()
{
    return CBinder::FlushPendingCommands();
}

ECode CBinderHelper::JoinThreadPool()
{
    return CBinder::JoinThreadPool();
}

} // namespace Os
} // namespace Droid
} // namespace Elastos

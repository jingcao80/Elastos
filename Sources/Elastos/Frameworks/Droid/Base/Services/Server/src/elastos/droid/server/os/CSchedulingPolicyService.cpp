
#include "elastos/droid/server/os/CSchedulingPolicyService.h"
#include <Elastos.Droid.Content.h>
#include <elastos/droid/os/Process.h>

using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IISchedulingPolicyService;
using Elastos::Droid::Content::Pm::IPackageManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Os {

const String CSchedulingPolicyService::TAG("CSchedulingPolicyService");

// Minimum and maximum values allowed for requestPriority parameter prio
const Int32 CSchedulingPolicyService::PRIORITY_MIN = 1;
const Int32 CSchedulingPolicyService::PRIORITY_MAX = 3;

CAR_INTERFACE_IMPL_2(CSchedulingPolicyService, Object, IISchedulingPolicyService, IBinder)

CAR_OBJECT_IMPL(CSchedulingPolicyService)

CSchedulingPolicyService::CSchedulingPolicyService()
{}

ECode CSchedulingPolicyService::constructor()
{
    return NOERROR;
}

ECode CSchedulingPolicyService::RequestPriority(
    /* [in] */ Int32 pid,
    /* [in] */  Int32 tid,
    /* [in] */  Int32 prio,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    //Log.i(TAG, "requestPriority(pid=" + pid + ", tid=" + tid + ", prio=" + prio + ")");

    // Verify that caller is mediaserver, priority is in range, and that the
    // callback thread specified by app belongs to the app that called mediaserver.
    // Once we've verified that the caller is mediaserver, we can trust the pid but
    // we can't trust the tid.  No need to explicitly check for pid == 0 || tid == 0,
    // since if not the case then the getThreadGroupLeader() test will also fail.
    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int32 uid;
    binder->GetCallingUid(&uid);
    if (uid != IProcess::MEDIA_UID || prio < PRIORITY_MIN ||
            prio > PRIORITY_MAX || Process::GetThreadGroupLeader(tid) != pid) {
        *result = IPackageManager::PERMISSION_DENIED;
    }
    // try {
        // make good use of our CAP_SYS_NICE capability
    ECode ec = Process::SetThreadGroup(tid, uid == pid ?
        IProcess::THREAD_GROUP_AUDIO_SYS : IProcess::THREAD_GROUP_AUDIO_APP);
    FAIL_GOTO(ec, _EXIT_)

    // must be in this order or it fails the schedulability constraint
    ec = Process::SetThreadScheduler(tid, IProcess::PROC_SCHED_FIFO, prio);

_EXIT_:
    // } catch (RuntimeException e) {
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        *result = IPackageManager::PERMISSION_DENIED;
        return NOERROR;
    }
    // }

    *result = IPackageManager::PERMISSION_GRANTED;
    return NOERROR;
}

ECode CSchedulingPolicyService::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}


} // namespace Os
} // namespace Server
} // namespace Droid
} // namespace Elastos


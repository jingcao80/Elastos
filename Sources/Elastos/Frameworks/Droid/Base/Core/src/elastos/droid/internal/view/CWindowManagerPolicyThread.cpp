
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/view/CWindowManagerPolicyThread.h"
#include "elastos/droid/internal/view/WindowManagerPolicyThread.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CAR_INTERFACE_IMPL(CWindowManagerPolicyThread, Singleton, IWindowManagerPolicyThread)

CAR_SINGLETON_IMPL(CWindowManagerPolicyThread)

ECode CWindowManagerPolicyThread::Set(
    /* [in] */ IThread* thread,
    /* [in] */ ILooper* looper)
{
    return WindowManagerPolicyThread::Set(thread, looper);
}

ECode CWindowManagerPolicyThread::GetThread(
    /* [out] */ IThread** thread)
{
    VALIDATE_NOT_NULL(thread);
    AutoPtr<IThread> temp = WindowManagerPolicyThread::GetThread();
    *thread = temp;
    REFCOUNT_ADD(*thread);
    return NOERROR;
}

ECode CWindowManagerPolicyThread::GetLooper(
    /* [out] */ ILooper** looper)
{
    VALIDATE_NOT_NULL(looper);
    AutoPtr<ILooper> temp = WindowManagerPolicyThread::GetLooper();
    *looper = temp;
    REFCOUNT_ADD(*looper);
    return NOERROR;
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#include <Elastos.CoreLibrary.Core.h>
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/view/WindowManagerPolicyThread.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

AutoPtr<IThread> WindowManagerPolicyThread::sThread;
AutoPtr<ILooper> WindowManagerPolicyThread::sLooper;

ECode WindowManagerPolicyThread::Set(
    /* [in] */ IThread* thread,
    /* [in] */ ILooper* looper)
{
    sThread = thread;
    sLooper = looper;
    return NOERROR;
}

AutoPtr<IThread> WindowManagerPolicyThread::GetThread()
{
    return sThread;
}

AutoPtr<ILooper> WindowManagerPolicyThread::GetLooper()
{
    return sLooper;
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

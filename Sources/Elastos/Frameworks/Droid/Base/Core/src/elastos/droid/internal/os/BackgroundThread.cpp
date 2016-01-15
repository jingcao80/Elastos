
#include "elastos/droid/internal/os/BackgroundThread.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ILooper;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

AutoPtr<BackgroundThread> BackgroundThread::sInstance;
AutoPtr<IHandler> BackgroundThread::sHandler;
Object BackgroundThread::sLock;

CAR_INTERFACE_IMPL(BackgroundThread, HandlerThread, IBackgroundThread)

BackgroundThread::BackgroundThread()
{
     HandlerThread::constructor(String("android.bg"), IProcess::THREAD_PRIORITY_BACKGROUND);
}

void BackgroundThread::EnsureThreadLocked()
{
    if (sInstance == NULL) {
        sInstance = new BackgroundThread();
        sInstance->Start();
        AutoPtr<ILooper> looper;
        sInstance->GetLooper((ILooper**)&looper);
        CHandler::New(looper, (IHandler**)&sHandler);
    }
}

AutoPtr<IBackgroundThread> BackgroundThread::GetInstance()
{
    AutoLock lock(sLock);
    EnsureThreadLocked();
    return sInstance;
}

AutoPtr<IHandler> BackgroundThread::GetHandler()
{
    AutoLock lock(sLock);
    EnsureThreadLocked();
    return sHandler;
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

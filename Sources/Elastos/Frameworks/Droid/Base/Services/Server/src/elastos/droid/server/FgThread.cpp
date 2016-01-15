#include "elastos/droid/server/FgThread.h"
#include "elastos/droid/server/UiThread.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::CHandler;

namespace Elastos {
namespace Droid {
namespace Server {

AutoPtr<FgThread> FgThread::sInstance;
AutoPtr<IHandler> FgThread::sHandler;

FgThread::FgThread()
    : ServiceThread(String("android.fg"), IProcess::THREAD_PRIORITY_DEFAULT, TRUE /*allowIo*/)
{
}

void FgThread::EnsureThreadLocked()
{
    if (sInstance == NULL) {
        sInstance = new FgThread();
        sInstance->Start();
        AutoPtr<ILooper> looper;
        sInstance->GetLooper((ILooper**)&looper);
        CHandler::New((IHandler**)&sHandler);
    }
}

AutoPtr<FgThread> FgThread::Get()
{
    Object& lock = UiThread::sLock;
    synchronized(lock) {
        EnsureThreadLocked();
    }
    return sInstance;
}

AutoPtr<IHandler> FgThread::GetHandler()
{
    Object& lock = UiThread::sLock;
    synchronized(lock) {
        EnsureThreadLocked();
    }
    return sHandler;
}


} // namespace Server
} // namepsace Droid
} // namespace Elastos

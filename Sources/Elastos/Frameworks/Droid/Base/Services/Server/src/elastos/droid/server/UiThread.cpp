#include "elastos/droid/server/UiThread.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::CHandler;

namespace Elastos {
namespace Droid {
namespace Server {

Object UiThread::sLock;
AutoPtr<UiThread> UiThread::sInstance;
AutoPtr<IHandler> UiThread::sHandler;

UiThread::UiThread()
    : ServiceThread(String("android.ui"), IProcess::THREAD_PRIORITY_FOREGROUND, TRUE /*allowIo*/)
{
}

void UiThread::EnsureThreadLocked()
{
    if (sInstance == NULL) {
        sInstance = new UiThread();
        sInstance->Start();
        AutoPtr<ILooper> looper;
        sInstance->GetLooper((ILooper**)&looper);
        CHandler::New(looper, (IHandler**)&sHandler);
    }
}

AutoPtr<UiThread> UiThread::Get()
{
    synchronized(sLock) {
        EnsureThreadLocked();
    }
    return sInstance;
}

AutoPtr<IHandler> UiThread::GetHandler()
{
    synchronized(sLock) {
        EnsureThreadLocked();
    }
    return sHandler;
}


} // namespace Server
} // namepsace Droid
} // namespace Elastos


#include "MyThread.h"
#include "stdio.h"

UInt32 MyThread::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 MyThread::Release()
{
    return ElRefBase::Release();
}

PInterface MyThread::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IThread) {
        return (IThread*)this;
    }

    return NULL;
}

ECode MyThread::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    if (pIID == NULL) {
        return E_INVALID_ARGUMENT;
    }

    if (pObject == (IInterface*)(IThread*)this) {
        *pIID = EIID_IThread;
    }
    else {
        return E_INVALID_ARGUMENT;
    }

    return NOERROR;
}

MyThread::MyThread()
    : mStop(FALSE)
{
    Thread::Init();
}

MyThread::MyThread(
    /* [in] */ IRunnable* runnable)
{
    Thread::Init(runnable);
}

MyThread::MyThread(
    /* [in] */ IRunnable* runnable,
    /* [in] */ const String& threadName)
{
    Thread::Init(runnable, threadName);
}

MyThread::MyThread(
    /* [in] */ const String& threadName)
{
    Thread::Init(threadName);
}

MyThread::MyThread(
    /* [in] */ IThreadGroup* group,
    /* [in] */ IRunnable* runnable)
{
    Thread::Init(group, runnable);
}

MyThread::MyThread(
    /* [in] */ IThreadGroup* group,
    /* [in] */ IRunnable* runnable,
    /* [in] */ const String& threadName)
{
    Thread::Init(group, runnable, threadName);
}

MyThread::MyThread(
    /* [in] */ IThreadGroup* group,
    /* [in] */ const String& threadName)
{
    Thread::Init(group, threadName);
}

MyThread::MyThread(
    /* [in] */ IThreadGroup* group,
    /* [in] */ IRunnable* runnable,
    /* [in] */ const String& threadName,
    /* [in] */ Int64 stackSize)
{
    Thread::Init(group, runnable, threadName, stackSize);
}

ECode MyThread::CheckAccess()
{
    return Thread::CheckAccess();
}

ECode MyThread::CountStackFrames(
    /* [out] */ Int32* number)
{
    return Thread::CountStackFrames(number);
}

ECode MyThread::Destroy()
{
    return Thread::Destroy();
}

ECode MyThread::GetId(
    /* [out] */ Int64* id)
{
    return Thread::GetId(id);
}

ECode MyThread::GetName(
    /* [out] */ String* name)
{
    return Thread::GetName(name);
}

ECode MyThread::GetPriority(
    /* [out] */ Int32* priority)
{
    return Thread::GetPriority(priority);
}

ECode MyThread::GetState(
    /* [out] */ ThreadState* state)
{
    return Thread::GetState(state);
}

ECode MyThread::GetThreadGroup(
    /* [out] */ IThreadGroup** group)
{
    return Thread::GetThreadGroup(group);
}

ECode MyThread::Interrupt()
{
    return Thread::Interrupt();
}

ECode MyThread::IsAlive(
    /* [out] */ Boolean* isAlive)
{
    return Thread::IsAlive(isAlive);
}

ECode MyThread::IsDaemon(
    /* [out] */ Boolean* isDaemon)
{
    return Thread::IsDaemon(isDaemon);
}

ECode MyThread::IsInterrupted(
    /* [out] */ Boolean* isInterrupted)
{
    return Thread::IsInterrupted(isInterrupted);
}

ECode MyThread::Join()
{
    return Thread::Join();
}

ECode MyThread::JoinEx(
    /* [in] */ Int64 millis)
{
    return Thread::JoinEx(millis);
}

ECode MyThread::JoinEx2(
    /* [in] */ Int64 millis,
    /* [in] */ Int32 nanos)
{
    return Thread::JoinEx2(millis, nanos);
}

ECode MyThread::Resume()
{
    return Thread::Resume();
}

ECode MyThread::SetDaemon(
    /* [in] */ Boolean isDaemon)
{
    return Thread::SetDaemon(isDaemon);
}

ECode MyThread::SetName(
    /* [in] */ const String& threadName)
{
    return Thread::SetName(threadName);
}

ECode MyThread::SetPriority(
    /* [in] */ Int32 priority)
{
    return Thread::SetPriority(priority);
}

ECode MyThread::Start()
{
    return Thread::Start();
}

ECode MyThread::Stop()
{
    return Thread::Stop();
}

ECode MyThread::Suspend()
{
    return Thread::Suspend();
}

ECode MyThread::Detach()
{
    return Thread::Detach();
}

ECode MyThread::Lock()
{
    return Thread::Lock();
}

ECode MyThread::Unlock()
{
    return Thread::Unlock();
}

ECode MyThread::Notify()
{
    return Thread::Notify();
}

ECode MyThread::NotifyAll()
{
    return Thread::NotifyAll();
}

ECode MyThread::Wait()
{
    return Thread::Wait();
}

ECode MyThread::WaitEx(
    /* [in] */ Int64 millis)
{
    return Thread::Wait(millis);
}

ECode MyThread::WaitEx2(
    /* [in] */ Int64 millis,
    /* [in] */ Int32 nanos)
{
    return Thread::Wait(millis, nanos);
}

ECode MyThread::Unpark()
{
    return Thread::Unpark();
}

ECode MyThread::ParkFor(
    /* [in] */ Int64 nanos)
{
    return Thread::ParkFor(nanos);
}

ECode MyThread::ParkUntil(
    /* [in] */ Int64 time)
{
    return Thread::ParkUntil(time);
}

ECode MyThread::Run()
{
    while ( !mStop ) {
        ECode ec = Thread::Sleep(1000);
        if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
            printf("======Thread interrupted...======\n");
        }

    }

    printf("======Thread exiting under request...======\n");
    return NOERROR;
}

Mutex* MyThread::GetSelfLock()
{
    return &mLock;
}

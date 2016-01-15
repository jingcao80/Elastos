
#ifndef __MYTHREAD_H__
#define __MYTHREAD_H__

#include <elastos/Thread.h>
#include <elrefbase.h>
#include <elastos/Mutex.h>


class MyThread
    : public ElRefBase
    , public Thread
    , public IThread
{
public:
    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    MyThread();

    MyThread(
        /* [in] */ IRunnable* runnable);

    MyThread(
        /* [in] */ IRunnable* runnable,
        /* [in] */ const String& threadName);

    MyThread(
        /* [in] */ const String& threadName);

    MyThread(
        /* [in] */ IThreadGroup* group,
        /* [in] */ IRunnable* runnable);

    MyThread(
        /* [in] */ IThreadGroup* group,
        /* [in] */ IRunnable* runnable,
        /* [in] */ const String& threadName);

    MyThread(
        /* [in] */ IThreadGroup* group,
        /* [in] */ const String& threadName);

    MyThread(
        /* [in] */ IThreadGroup* group,
        /* [in] */ IRunnable* runnable,
        /* [in] */ const String& threadName,
        /* [in] */ Int64 stackSize);

    CARAPI CheckAccess();

    CARAPI CountStackFrames(
        /* [out] */ Int32* number);

    CARAPI Destroy();

    CARAPI GetId(
        /* [out] */ Int64* id);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetPriority(
        /* [out] */ Int32* priority);

    CARAPI GetState(
        /* [out] */ ThreadState* state);

    CARAPI GetThreadGroup(
        /* [out] */ IThreadGroup** group);

    CARAPI Interrupt();

    CARAPI IsAlive(
        /* [out] */ Boolean* isAlive);

    CARAPI IsDaemon(
        /* [out] */ Boolean* isDaemon);

    CARAPI IsInterrupted(
        /* [out] */ Boolean* isInterrupted);

    CARAPI Join();

    CARAPI JoinEx(
        /* [in] */ Int64 millis);

    CARAPI JoinEx2(
        /* [in] */ Int64 millis,
        /* [in] */ Int32 nanos);

    CARAPI Resume();

    CARAPI SetDaemon(
        /* [in] */ Boolean isDaemon);

    CARAPI SetName(
        /* [in] */ const String& threadName);

    CARAPI SetPriority(
        /* [in] */ Int32 priority);

    CARAPI Start();

    CARAPI Stop();

    CARAPI Suspend();

    CARAPI Wait(
        /* [in] */ Int64 time,
        /* [in] */ Int32 frac);

    CARAPI Run();

private:
    CARAPI_(Mutex*) GetSelfLock();

public:
    Boolean mStop;

private:
    Mutex mLock;
};

#endif //__MYTHREAD_H__

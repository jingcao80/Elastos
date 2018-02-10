
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
    else if (riid == EIID_IObject) {
        return (IObject*)this;
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
    else if (pObject == (IInterface *)(IObject *)this) {
        *pIID = EIID_IObject;
        return NOERROR;
    }
    else {
        return E_INVALID_ARGUMENT;
    }

    return NOERROR;
}

MyThread::MyThread()
    : mHost(NULL)
    , mScanResult(NULL)
{
    Thread::constructor();
}

MyThread::MyThread(
    /* [in] */ IRunnable* runnable)
{
    Thread::constructor(runnable);
}

MyThread::MyThread(
    /* [in] */ IRunnable* runnable,
    /* [in] */ const String& threadName)
{
    Thread::constructor(runnable, threadName);
}

MyThread::MyThread(
    /* [in] */ const String& threadName)
{
    Thread::constructor(threadName);
}

MyThread::MyThread(
    /* [in] */ IThreadGroup* group,
    /* [in] */ IRunnable* runnable)
{
    Thread::constructor(group, runnable);
}

MyThread::MyThread(
    /* [in] */ IThreadGroup* group,
    /* [in] */ IRunnable* runnable,
    /* [in] */ const String& threadName)
{
    Thread::constructor(group, runnable, threadName);
}

MyThread::MyThread(
    /* [in] */ IThreadGroup* group,
    /* [in] */ const String& threadName)
{
    Thread::constructor(group, threadName);
}

MyThread::MyThread(
    /* [in] */ IThreadGroup* group,
    /* [in] */ IRunnable* runnable,
    /* [in] */ const String& threadName,
    /* [in] */ Int64 stackSize)
{
    Thread::constructor(group, runnable, threadName, stackSize);
}

MyThread::MyThread(
    /* [in] */ CWifiSetting* host,
    /* [in]  */ ScanResult* res,
    /* [in]  */ ICharSequence* password)
    : mHost(host)
    , mScanResult(res)
    , mPassword(password)
{
    Thread::constructor();
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

ECode MyThread::Join(
    /* [in] */ Int64 millis)
{
    return Thread::Join(millis);
}

ECode MyThread::Join(
    /* [in] */ Int64 millis,
    /* [in] */ Int32 nanos)
{
    return Thread::Join(millis, nanos);
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
printf("File=[%s], Line = [%d], FUNC = [%s], this=[%p]\n", __FILE__, __LINE__, __FUNCTION__, this);
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

ECode MyThread::Wait(
    /* [in] */ Int64 millis)
{
    return Thread::Wait(millis);
}

ECode MyThread::Wait(
    /* [in] */ Int64 millis,
    /* [in] */ Int32 nanos)
{
    return Thread::Wait(millis, nanos);
}

ECode MyThread::Run()
{
printf("File=[%s], Line = [%d], FUNC = [%s], mScanResult=[%p], mPassword=[%p]\n",
    __FILE__, __LINE__, __FUNCTION__, mScanResult, mPassword.Get());
    if (mScanResult != NULL && mPassword != NULL) {
        String strPwd;
        mPassword->ToString(&strPwd);

        int flag = ElWifi_AddNetwork(
            mScanResult->ssid,
            mScanResult->security,
            (const char*)strPwd);

        void (STDCALL CWifiSetting::*pHandlerFunc)(Int32, ICharSequence*);
        pHandlerFunc = &CWifiSetting::WifiConnectedStatus;
        AutoPtr<IParcel> params;
        CCallbackParcel::New((IParcel**)&params);
        params->WriteInt32(flag);

        AutoPtr<ICharSequence> str;
        CStringWrapper::New(String(mScanResult->ssid),
                (ICharSequence**)&str);
        params->WriteInterfacePtr(str.Get());

        mHost->mApartment->PostCppCallbackDelayed(
            (HANDLE)mHost, *(HANDLE*)&pHandlerFunc,
            params, 0, 0);
    }
    return NOERROR;
}

Object* MyThread::GetSelfLock()
{
    return &mLock;
}


#include "MyRunnable.h"
#include "MyThread.h"
#include <elautoptr.h>
#include <unistd.h>
#include <stdio.h>

void printArgs(
    /* [in] */ IThread* t)
{
    Int64 id;
    Int32 priority;
    t->GetId(&id);
    t->GetPriority(&priority);
    String name;
    t->GetName(&name);
    printf("=======thread id = %d, thread priority = %d, thread name = %s\n",
            id, priority, (const char*)name);
}

ECode test1()
{
    printf("Start main() at Line:%d\n", __LINE__);

    MyThread* myThread = new MyThread();
    AutoPtr<IThread> thr2 = (IThread*)myThread->Probe(EIID_IThread);
    thr2->Start();
    Thread::Sleep(3000);
    printArgs(thr2);
    thr2->SetPriority(ELASTOS_PRIORITY_FOREGROUND);
    printArgs(thr2);
    myThread->mStop = TRUE;
    thr2->Interrupt();
    Thread::Sleep(3000);
    return NOERROR;
}

void* EntryRoutine(void *arg)
{
    ECode ec;

    AutoPtr<IThread> thread;
    Thread::Attach((IThread**)&thread);
    printf("==== Thread attached ====\n");
    ISynchronize* syncObj = ISynchronize::Probe(thread);
    assert(syncObj != NULL);
    syncObj->Lock();
    printf("==== Thread locked ====\n");
    syncObj->Unlock();
    printf("==== Thread unlocked ====\n");
    thread->Detach();
    printf("==== Thread detached ====\n");

    return (void*)NOERROR;
}

ECode test2()
{
    pthread_t pthread;

    if (pthread_create(&pthread, NULL, EntryRoutine, NULL)) {
        return E_THREAD_ABORTED;
    }
    Thread::Sleep(2000);
    return NOERROR;
}

int main(int argc, char *argv[])
{
    int ret;

    ret = test1();
    ret = test2();
    return ret;
}

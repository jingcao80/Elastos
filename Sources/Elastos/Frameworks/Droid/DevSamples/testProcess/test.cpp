
#include "test.h"
#include <elautoptr.h>
#include <elastos/core/Thread.h>
#include <tstcom.h>
#include <os/Process.h>

using namespace Elastos::Droid::Os;
using Elastos::Core::Thread;
using namespace Elastos::Core;


/* test MyPid  MyTid MyUid */                                              //  OK
int CTest::Test1(int argc, char *argv[])
{
    Int32 pid = Process::MyPid();
    printf("==CTest::Test1==[pid]=%d\n",pid);

    Int32 tid = Process::MyTid();
    printf("==CTest::Test1==[tid]=%d\n",tid);

    Int32 uid = Process::MyUid();
    printf("==CTest::Test1==[uid]=%d\n",uid);

    //sleep(10);

    return NOERROR;
}


/* test GetFreeMemory  GetTotalMemory GetElapsedCpuTime*/                 //OK
int CTest::Test2(int argc, char *argv[])
{
    Int64 freeMem = Process::GetFreeMemory();
    printf("==CTest::Test2==[freeMem]=%lld\n",freeMem);

    Int64 totalMem = Process::GetTotalMemory();
    printf("==CTest::Test2==[totalMem]=%lld\n",totalMem);

    Int64 ectime = Process::GetElapsedCpuTime();
    printf("==CTest::Test2==[ectime]=%lld\n",ectime);

    return NOERROR;
}


/* test GetPss  GetUidForPid GetParentPid GetThreadGroupLeader GetPids*/    //OK
int CTest::Test3(int argc, char *argv[])
{
    Int32 pid = Process::MyPid();
    printf("==CTest::Test3==[pid]=%d\n",pid);

    Int64 pss = Process::GetPss(pid);
    printf("==CTest::Test3==[pss]=%lld\n",pss);

    Int32 ufPid = Process::GetUidForPid(pid);
    printf("==CTest::Test3==[ufPid]=%d\n",ufPid);

    Int32 pPid = Process::GetParentPid(pid);
    printf("==CTest::Test3==[pPid]=%d\n",pPid);

    Int32 gleader = Process::GetThreadGroupLeader(pid);
    printf("==CTest::Test3==[gleader]=%d\n",gleader);

    AutoPtr<ArrayOf<Int32> > pids;
    Process::GetPids(String("/proc"), NULL, (ArrayOf<Int32>**)&pids);
    for(Int32 i = 0; i < pids->GetLength(); i++)
    {
        printf("==CTest::Test3==[pids %d]=%d\n",i,(*pids)[i]);
    }

    //sleep(10);

    return NOERROR;
}


/* test GetUidForName GetGidForName*/                                        //OK
int CTest::Test4(int argc, char *argv[])
{
    Int32 uidForName1 = Process::GetUidForName(String("root"));
    printf("==CTest::Test4==[uidForName1]=%d\n",uidForName1);

    Int32 uidForName2 = Process::GetUidForName(String("system"));
    printf("==CTest::Test4==[uidForName2]=%d\n",uidForName2);

    Int32 uidForName3 = Process::GetUidForName(String("graphics"));
    printf("==CTest::Test4==[uidForName3]=%d\n",uidForName3);



    Int32 gidForName1 = Process::GetGidForName(String("root"));
    printf("==CTest::Test4==[gidForName1]=%d\n",gidForName1);

    Int32 gidForName2 = Process::GetGidForName(String("system"));
    printf("==CTest::Test4==[gidForName2]=%d\n",gidForName2);

    Int32 gidForName3 = Process::GetUidForName(String("graphics"));
    printf("==CTest::Test4==[gidForName3]=%d\n",gidForName3);

    return NOERROR;
}


/* test SetThreadPriority */   // Priority:[-20,19]                          // OK
int CTest::Test5(int argc, char *argv[])
{
    Int32 pid = Process::MyPid();
    printf("==CTest::Test3==[pid]=%d\n",pid);

    Int32 prio;
    Process::GetThreadPriority(pid, &prio);
    printf("==CTest::Test5==[priority before]=%d\n",prio);

    Process::SetThreadPriority(prio+1<=19 ? prio+1 : prio-1);

    Process::GetThreadPriority(pid, &prio);
    printf("==CTest::Test5==[priority  after]=%d\n",prio);

    return NOERROR;
}

/* test SetThreadGroup */                                                  //  OK
int CTest::Test6(int argc, char *argv[])
{
    Int32 pid = Process::MyPid();
    printf("==CTest::Test6==[pid]=%d\n",pid);

    ECode ec = Process::SetThreadGroup(pid, IProcess::THREAD_GROUP_BG_NONINTERACTIVE);
    printf("==CTest::Test6==[ec]=%d\n",ec);

    return NOERROR;
}

/* test SetProcessGroup */                                                 //  OK
int CTest::Test7(int argc, char *argv[])
{
    Int32 pid = Process::MyPid();
    printf("==CTest::Test7==[pid]=%d\n",pid);

    ECode ec = Process::SetProcessGroup(pid, IProcess::THREAD_GROUP_DEFAULT);
    printf("==CTest::Test7==[ec]=%d\n",ec);

    return NOERROR;
}

/* test SetUid */
int CTest::Test8(int argc, char *argv[])                                  // OK
{
    Int32 uidb = Process::MyUid();
    printf("==CTest::Test8==[uid change before]=%d\n",uidb);

    Int32 uidForSystem = Process::GetUidForName(String("system"));
    printf("==CTest::Test8==[uidForSystem]=%d\n",uidForSystem);

    ECode ec = Process::SetUid(uidForSystem);
    printf("==CTest::Test8==[ec]=%d\n",ec);

    Int32 uida = Process::MyUid();
    printf("==CTest::Test8==[uid change after]=%d\n",uida);

    return NOERROR;
}

/* test SetGid */                                                        // OK
int CTest::Test9(int argc, char *argv[])
{
    Int32 gid1 = getgid();
    printf("==CTest::Test9==[gid change before]=%d\n",gid1);

    Int32 gidForSystem = Process::GetGidForName(String("system"));
    printf("==CTest::Test9==[gidForSystem]=%d\n",gidForSystem);

    ECode ec = Process::SetGid(gidForSystem);
    printf("==CTest::Test9==[ec]=%d\n",ec);

    Int32 gid2 = getgid();
    printf("==CTest::Test9==[gid change after]=%d\n",gid2);

    return NOERROR;
}

/* test IsIsolated    SetCanSelfBackground */             // OK
int CTest::Test10(int argc, char *argv[])
{
    Boolean issola = Process::IsIsolated();
    printf("==CTest::Test10==[issola]=%d\n",issola);

    Process::SetCanSelfBackground(FALSE);

    return NOERROR;
}

/* test ReadProcFile */                                     //do not have dir : /sys/kernel/mm/ksm
int CTest::Test11(int argc, char *argv[])
{
    AutoPtr<ArrayOf<Int32> > SINGLE_LONG_FORMAT = ArrayOf<Int32>::Alloc(1);
    (*SINGLE_LONG_FORMAT)[0] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;

    AutoPtr<ArrayOf<Int64> > int64Out = ArrayOf<Int64>::Alloc(1);

    Boolean result;

    Int32 page_size = 4*1024;

    (*int64Out)[0] = 0;
    Process::ReadProcFile(String("/sys/kernel/mm/ksm/pages_shared"), *SINGLE_LONG_FORMAT,
        NULL, (ArrayOf<Int64>*)int64Out, NULL, &result);
    Int64 shared = (*int64Out)[0] * page_size / 1024;
    printf("==CTest::Test11==[shared]=%lld\n",shared);

    (*int64Out)[0] = 0;
    Process::ReadProcFile(String("/sys/kernel/mm/ksm/pages_sharing"), *SINGLE_LONG_FORMAT,
        NULL, (ArrayOf<Int64>*)int64Out, NULL, &result);
    Int64 sharing = (*int64Out)[0] * page_size / 1024;
    printf("==CTest::Test11==[sharing]=%lld\n",sharing);

    (*int64Out)[0] = 0;
    Process::ReadProcFile(String("/sys/kernel/mm/ksm/pages_unshared"), *SINGLE_LONG_FORMAT,
        NULL, (ArrayOf<Int64>*)int64Out, NULL, &result);
    Int64 unshared = (*int64Out)[0] * page_size / 1024;
    printf("==CTest::Test11==[unshared]=%lld\n",unshared);

    (*int64Out)[0] = 0;
    Process::ReadProcFile(String("/sys/kernel/mm/ksm/pages_volatile"), *SINGLE_LONG_FORMAT,
        NULL, (ArrayOf<Int64>*)int64Out, NULL, &result);
    Int64 voltile = (*int64Out)[0] * page_size / 1024;
    printf("==CTest::Test11==[voltile]=%lld\n",voltile);


    return NOERROR;
}

/* test KillProcess */                                            // OK
int CTest::Test12(int argc, char *argv[])
{
    Int32 pid = Process::MyPid();
    printf("==CTest::Test12==[pid]=%d\n",pid);

    printf("==CTest::Test12==[call KillProcess]======\n");
    Process::KillProcess(pid);
    printf("==CTest::Test12==[if be killed will not print this]======\n");

    return NOERROR;
}

/* test  KillProcessQuiet*/                                      // OK
int CTest::Test13(int argc, char *argv[])
{
    Int32 pid = Process::MyPid();
    printf("==CTest::Test13==[pid]=%d\n",pid);

    printf("==CTest::Test13==[call KillProcessQuiet]======\n");
    Process::KillProcessQuiet(pid);
    printf("==CTest::Test13==[if be killed will not print this]======\n");

    return NOERROR;
}
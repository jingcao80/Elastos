
#include "MyTimerTask.h"
#include <elastos/Thread.h>
#include <elautoptr.h>
#include <stdio.h>


void ScheduleAtFixRate()
{
    AutoPtr<ITimerTask> r = new MyTimerTask();
    AutoPtr<ITimer> timer;
    CTimer::New((ITimer**)&timer);
    timer->ScheduleAtFixedRate(r, 1000, 1000);

    Thread::Sleep(8000);
    timer->Cancel();
    return;
}

int main(int argc, char *argv[])
{
    ScheduleAtFixRate();
    printf("exit main\n");
    return 0;
}

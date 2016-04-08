
#include "MyTimerTask.h"
#include "stdio.h"


Int32 MyTimerTask::sCount = 0;

ECode MyTimerTask::Run()
{
    printf("======MyTimerTask Run======\n");
    printf("         count is %d\n", ++sCount);
    printf("=============================\n");
    return NOERROR;
}

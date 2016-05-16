#include <e_time.h>
#include <windows.h>
#define UTC1970  0x19db1ded53e8000LL
/*
 * DIVISOR representing the number of 100-nanosecond intervals
 * in one second.
 */
#define DIVISOR  (1000 * 1000 * 10)

#define FILETIME_TO_Timeval(filetime, tv) \
                do { \
                    long long *pInt64 = (long long *)&(fileTime); \
                    (tv).tv_sec = (long)((*pInt64 - UTC1970) / DIVISOR); \
                    (tv).tv_usec =  (long)(((*pInt64 - UTC1970) % DIVISOR) / 10); \
                    } while(0)


int gettimeofday(struct timeval *ptimeval, void *ptimezone)
{
    SYSTEMTIME sysTime;
    FILETIME fileTime;

    GetSystemTime(&sysTime);
    SystemTimeToFileTime(&sysTime, &fileTime);
    FILETIME_TO_Timeval(fileTime, *ptimeval);
    return 0;
}

int Elastos_QueryPerformanceCounter(long long *pPerformanceCount)
{
    BOOL ret = FALSE;
    ret = QueryPerformanceCounter((LARGE_INTEGER *)pPerformanceCount);
    if (!ret) {
        return -1;
    }

    return 0;
}

int Elastos_QueryPerformanceFrequency(long long *pFrequency )
{
    BOOL ret = FALSE;

    ret = QueryPerformanceFrequency((LARGE_INTEGER*)pFrequency);
    if (!ret) {
        return -1;
    }

    return 0;
}

//int clock_gettime(unsigned long clock_id, struct timespec *tp)
//{
// return -1;
//}

//unsigned long clock_getres(unsigned long clock_id, struct timespec *res)
//{
//    return 0;
//}
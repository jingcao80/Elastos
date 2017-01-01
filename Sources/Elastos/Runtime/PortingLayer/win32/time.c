//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
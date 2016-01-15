//#include <sys/types.h>

#ifndef _WINSOCK_H
#define _TIMEVAL_DEFINED
#include <time.h>
#include <pthread.h>
typedef long time_t;
typedef long suseconds_t;
struct timeval {
  time_t      tv_sec;
  suseconds_t tv_usec;
};
#endif /* _WINSOCK_H */
#define CLOCK_MONOTONIC 4
//typedef unsigned long clock_t;
extern "C" {
int gettimeofday(struct timeval *__p, void *__tz);
int Elastos_QueryPerformanceCounter(long long *pPerformanceCount);
int Elastos_QueryPerformanceFrequency(long long *pFrequency );
//int _EXFUN(settimeofday, (const struct timeval *, const struct timezone *));
//int _EXFUN(utimes, (const char *__path, const struct timeval *__tvp));
//int _EXFUN(getitimer, (int __which, struct itimerval *__value));
//int _EXFUN(setitimer, (int __which, const struct itimerval *__value,
//					struct itimerval *__ovalue));

//unsigned long	 clock(void);


//int clock_gettime(unsigned long clock_id, struct timespec *tp);
//unsigned long clock_getres(unsigned long clock_id, struct timespec *res);
}

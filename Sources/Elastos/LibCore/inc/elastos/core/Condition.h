
#ifndef __ELASTOS_CORE_CONDITION_H__
#define __ELASTOS_CORE_CONDITION_H__

#include <pthread.h>
#include <time.h>
#include <elastos.h>

namespace Elastos {
namespace Core {

/*
 * Condition variable class.  The implementation is system-dependent.
 *
 * Condition variables are paired up with mutexes.  Lock the mutex,
 * call wait(), then either re-wait() if things aren't quite what you want,
 * or unlock the mutex and continue.  All threads calling wait() must
 * use the same mutex for a given Condition.
 */
class Condition
{
public:
    enum {
        PRIVATE = 0,
        SHARED = 1
    };

    Condition();

    Condition(Int32 type);

    ~Condition();

    // Wait on the condition variable.  Lock the mutex before calling.
    CARAPI_(Int32) Wait(Mutex& mutex);

    // same with relative timeout
    CARAPI_(Int32) WaitNanos(Mutex& mutex, Int64 reltime, Int64* leftTime = NULL);

    // Signal the condition variable, allowing one thread to continue.
    CARAPI_(void) Signal();

    // Signal the condition variable, allowing all threads to continue.
    CARAPI_(void) Broadcast();

private:
    pthread_cond_t mCond;
};

// ---------------------------------------------------------------------------
inline Condition::Condition()
{
    pthread_cond_init(&mCond, NULL);
}

inline Condition::Condition(Int32 type)
{
    if (type == SHARED) {
        pthread_condattr_t attr;
        pthread_condattr_init(&attr);
        pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_cond_init(&mCond, &attr);
        pthread_condattr_destroy(&attr);
    }
    else {
        pthread_cond_init(&mCond, NULL);
    }
}

inline Condition::~Condition()
{
    pthread_cond_destroy(&mCond);
}

inline Int32 Condition::Wait(Mutex& mutex)
{
    return pthread_cond_wait(&mCond, &mutex.mMutex);
}

inline Int32 Condition::WaitNanos(Mutex& mutex, Int64 reltime, Int64* leftTime)
{
    struct timespec ts;
#if defined(HAVE_POSIX_CLOCKS)
    clock_gettime(CLOCK_REALTIME, &ts);
#else // HAVE_POSIX_CLOCKS
    // we don't support the clocks here.
    struct timeval t;
    gettimeofday(&t, NULL);
    ts.tv_sec = t.tv_sec;
    ts.tv_nsec = t.tv_usec * 1000;
#endif // HAVE_POSIX_CLOCKS
    ts.tv_sec += reltime / 1000000000;
    ts.tv_nsec += reltime % 1000000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec += 1;
    }
    Int32 status = pthread_cond_timedwait(&mCond, &mutex.mMutex, &ts);
    if (leftTime == NULL) return status;
    struct timespec now_ts;
#if defined(HAVE_POSIX_CLOCKS)
    clock_gettime(CLOCK_REALTIME, &now_ts);
#else // HAVE_POSIX_CLOCKS
    // we don't support the clocks here.
    struct timeval now_t;
    gettimeofday(&now_t, NULL);
    now_ts.tv_sec = now_t.tv_sec;
    now_ts.tv_nsec = now_t.tv_usec * 1000;
#endif // HAVE_POSIX_CLOCKS
    *leftTime = (ts.tv_sec - now_ts.tv_sec) * 1000000000 + (ts.tv_nsec - now_ts.tv_nsec);
    return status;
}

inline void Condition::Signal()
{
    pthread_cond_signal(&mCond);
}

inline void Condition::Broadcast()
{
    pthread_cond_broadcast(&mCond);
}

} // namespace Core
} // namespace Elastos

#endif // __ELASTOS_CORE_CONDITION_H__

#ifndef __ELASTOS_CORE_RWLOCK_H__
#define __ELASTOS_CORE_RWLOCK_H__

#include <pthread.h>
#ifdef _win32
#include <sys/types.h>
#endif
#include <elastos.h>

namespace Elastos {
namespace Core {

/*
 * Simple mutex class.  The implementation is system-dependent.
 *
 * The mutex must be unlocked by the thread that locked it.  They are not
 * recursive, i.e. the same thread can't lock it multiple times.
 */
class RWLock
{
public:
    enum {
        PRIVATE = 0,
        SHARED = 1
    };

    RWLock();

    RWLock(const char* name);

    RWLock(Int32 type, const char* name = NULL);

    ~RWLock();

    CARAPI_(Int32) ReadLock();

    CARAPI_(Int32) TryReadLock();

    CARAPI_(Int32) WriteLock();

    CARAPI_(Int32) TryWriteLock();

    CARAPI_(void) Unlock();

    class AutoRLock
    {
    public:
        inline AutoRLock(RWLock& rwlock) : mLock(rwlock) { mLock.ReadLock(); }

        inline ~AutoRLock() { mLock.Unlock(); }

    private:
        RWLock& mLock;
    };

    class AutoWLock
    {
    public:
        inline AutoWLock(RWLock& rwlock) : mLock(rwlock) { mLock.WriteLock(); }

        inline ~AutoWLock() { mLock.Unlock(); }

    private:
        RWLock& mLock;
    };

private:
    // A RWLock cannot be copied
    RWLock(const RWLock&);
    RWLock& operator = (const RWLock&);

    pthread_rwlock_t mRWLock;
};

inline RWLock::RWLock()
{
    pthread_rwlock_init(&mRWLock, NULL);
}

inline RWLock::RWLock(const char* name)
{
    pthread_rwlock_init(&mRWLock, NULL);
}

inline RWLock::RWLock(Int32 type, const char* name)
{
    if (type == SHARED) {
        pthread_rwlockattr_t attr;
        pthread_rwlockattr_init(&attr);
        pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_rwlock_init(&mRWLock, &attr);
        pthread_rwlockattr_destroy(&attr);
    }
    else {
        pthread_rwlock_init(&mRWLock, NULL);
    }
}

inline RWLock::~RWLock()
{
    pthread_rwlock_destroy(&mRWLock);
}

inline Int32 RWLock::ReadLock()
{
    return -pthread_rwlock_rdlock(&mRWLock);
}

inline Int32 RWLock::TryReadLock()
{
    return -pthread_rwlock_tryrdlock(&mRWLock);
}

inline Int32 RWLock::WriteLock()
{
    return -pthread_rwlock_wrlock(&mRWLock);
}

inline Int32 RWLock::TryWriteLock()
{
    return -pthread_rwlock_trywrlock(&mRWLock);
}

inline void RWLock::Unlock()
{
    pthread_rwlock_unlock(&mRWLock);
}

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_RWLOCK_H__

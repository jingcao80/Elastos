//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __ELASTOS_SPINLOCK_H__
#define __ELASTOS_SPINLOCK_H__

#include <elatypes.h>
#include <elaatomics.h>
#include <unistd.h>

class SpinLock
{
public:
    SpinLock() : m_bLocked(FALSE) {}

    void Lock();

    void Unlock();

    _ELASTOS Boolean TryLock();

private:
    _ELASTOS Int32 m_bLocked;
};

CAR_INLINE void SpinLock::Lock()
{
    while (atomic_swap(TRUE, &m_bLocked)) {
        while (m_bLocked) usleep(1);
    }
}

CAR_INLINE void SpinLock::Unlock()
{
    atomic_swap(FALSE, &m_bLocked);
}

CAR_INLINE _ELASTOS Boolean SpinLock::TryLock()
{
    return !atomic_swap(TRUE, &m_bLocked);
}

#endif //__ELASTOS_SPINLOCK_H__

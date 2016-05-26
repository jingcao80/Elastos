
#ifndef __ELASTOS_UTILITY_CONCURRENT_LOCKS_ELASTOS_UTILITY_ABSTRACTOWNABLESYNCHRONIZER_H__
#define __ELASTOS_UTILITY_CONCURRENT_LOCKS_ELASTOS_UTILITY_ABSTRACTOWNABLESYNCHRONIZER_H__

#include <elastos/core/Object.h>

using Elastos::Core::IThread;
using Elastos::Core::Object;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Locks {

/**
 * A synchronizer that may be exclusively owned by a thread.  This
 * class provides a basis for creating locks and related synchronizers
 * that may entail a notion of ownership.  The
 * <tt>AbstractOwnableSynchronizer</tt> class itself does not manage or
 * use this information. However, subclasses and tools may use
 * appropriately maintained values to help control and monitor access
 * and provide diagnostics.
 *
 * @since 1.6
 * @author Doug Lea
 */
class AbstractOwnableSynchronizer
    : public Object
{
protected:
    /**
     * Empty constructor for use by subclasses.
     */
    AbstractOwnableSynchronizer() {}

    /**
     * Sets the thread that currently owns exclusive access. A
     * <tt>null</tt> argument indicates that no thread owns access.
     * This method does not otherwise impose any synchronization or
     * <tt>volatile</tt> field accesses.
     */
    CARAPI_(void) SetExclusiveOwnerThread(
        /* [in] */ IThread* t)
    {
        mExclusiveOwnerThread = t;
    }

    /**
     * Returns the thread last set by
     * <tt>setExclusiveOwnerThread</tt>, or <tt>null</tt> if never
     * set.  This method does not otherwise impose any synchronization
     * or <tt>volatile</tt> field accesses.
     * @return the owner thread
     */
    CARAPI_(AutoPtr<IThread>) GetExclusiveOwnerThread()
    {
        return mExclusiveOwnerThread;
    }

private:
    /**
     * The current owner of exclusive mode synchronization.
     */
    AutoPtr<IThread> mExclusiveOwnerThread;
};

} // namespace Locks
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_LOCKS_ELASTOS_UTILITY_ABSTRACTOWNABLESYNCHRONIZER_H__

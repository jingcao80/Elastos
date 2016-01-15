
#ifndef __ELASTOS_CORE_THREADGROUP_H__
#define __ELASTOS_CORE_THREADGROUP_H__

#include "Mutex.h"
#include "Object.h"

namespace Elastos {
namespace Core {

class ThreadGroup
    : public Object
    , public IThreadGroup
    , public IThreadUncaughtExceptionHandler
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Initialize the special "system" ThreadGroup. Was "main" in Harmony,
     * but we have an additional group above that in Android.
     */
    ThreadGroup();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ IThreadGroup* parent,
        /* [in] */ const String& name);

    /**
     * The thread is being terminated by an uncaught exception. Further
     * exceptions thrown in this method are prevent the remainder of the
     * method from executing, but are otherwise ignored.
     *
     * @param thread the thread that has an uncaught exception
     * @param ex the exception that was thrown
     */
    CARAPI UncaughtException(
        /* [in] */ IThread* thread,
        /* [in] */ ECode ec);

    virtual CARAPI ActiveCount(
        /* [out] */ Int32* number);

    virtual CARAPI ActiveGroupCount(
        /* [out] */ Int32* number);

    CARAPI AddThreadImpl(
        /* [in] */ IThread* thread);

    virtual CARAPI AllowThreadSuspension(
        /* [in] */ Boolean b,
        /* [out] */ Boolean* result);

    virtual CARAPI CheckAccess();

    virtual CARAPI Destroy();

    virtual CARAPI Enumerate(
        /* [out] */ ArrayOf<IThread*>* threads,
        /* [out] */ Int32* number);

    virtual CARAPI Enumerate(
        /* [out] */ ArrayOf<IThread*>* threads,
        /* [in] */ Boolean recurse,
        /* [out] */ Int32* number);

    virtual CARAPI Enumerate(
        /* [out] */ ArrayOf<IThreadGroup*>* groups,
        /* [out] */ Int32* number);

    virtual CARAPI Enumerate(
        /* [out] */ ArrayOf<IThreadGroup*>* groups,
        /* [in] */ Boolean recurse,
        /* [out] */ Int32* number);

    virtual CARAPI GetMaxPriority(
        /* [out] */ Int32* maxPriority);

    virtual CARAPI GetName(
        /* [out] */ String* name);

    virtual CARAPI GetParent(
        /* [out] */ IThreadGroup** parent);

    virtual CARAPI Interrupt();

    virtual CARAPI IsDaemon(
        /* [out] */ Boolean* isDaemon);

    virtual CARAPI IsDestroyed(
        /* [out] */ Boolean* isDestroyed);

    virtual CARAPI List();

    virtual CARAPI IsParentOf(
        /* [in] */ IThreadGroup* g,
        /* [out] */ Boolean* result);

    /**
     * Removes a {@code Thread} from this group. This should only be visible to class
     * java.lang.Thread, and should only be called when a Thread dies.
     *
     * @param thread Thread to remove
     *
     * @see #add(Thread)
     */
    CARAPI RemoveThreadImpl(
        /* [in] */ IThread* thread);

    virtual CARAPI Resume();

    virtual CARAPI SetDaemon(
        /* [in] */ Boolean isDaemon);

    virtual CARAPI SetMaxPriority(
        /* [in] */ Int32 newMax);

    virtual CARAPI Stop();

    virtual CARAPI Suspend();

    // BEGIN android-added
    /**
     * Non-standard method for adding a thread to a group, required by Dalvik.
     *
     * @param thread Thread to add
     *
     * @throws IllegalThreadStateException if the thread has been destroyed
     *         already
     *
     * @see #add(java.lang.Thread)
     * @see #removeThread(java.lang.Thread)
     */
    CARAPI AddThread(
        /* [in] */ IThread* thread);

    /**
     * Non-standard method for adding a thread to a group, required by Dalvik.
     *
     * @param thread Thread to add
     *
     * @throws IllegalThreadStateException if the thread has been destroyed
     *         already
     *
     * @see #remove(java.lang.Thread)
     * @see #addThread(java.lang.Thread)
     */
    CARAPI RemoveThread(
        /* [in] */ IThread* thread);
    // END android-added

private:
    /**
     * Adds a {@code ThreadGroup} to this thread group.
     *
     * @param g ThreadGroup to add
     * @throws IllegalThreadStateException if this group has been destroyed already
     */
    CARAPI AddThreadGroup(
        /* [in] */ IThreadGroup* g);

    /*
     * Auxiliary method that destroys this thread group and recursively all its
     * subgroups if this is a daemon ThreadGroup.
     *
     * @see #destroy
     * @see #setDaemon
     * @see #isDaemon
     */
    CARAPI_(void) DestroyIfEmptyDaemon();

    /**
     * Copies into <param>enumeration</param> starting at
     * <param>enumerationIndex</param> all Threads or ThreadGroups in the
     * receiver. If <param>recurse</param> is true, recursively enumerate the
     * elements in subgroups.
     *
     * If the array passed as parameter is too small no exception is thrown -
     * the extra elements are simply not copied.
     *
     * @param enumeration array into which the elements will be copied
     * @param recurse Indicates whether subgroups should be enumerated or not
     * @param enumerationIndex Indicates in which position of the enumeration
     *        array we are
     * @param enumeratingThreads Indicates whether we are enumerating Threads or
     *        ThreadGroups
     * @return How many elements were enumerated/copied over
     */
    CARAPI_(Int32) EnumerateThreadGeneric(
        /* [in] */ Boolean recurse,
        /* [in] */ Int32 enumerationIndex,
        /* [out] */ ArrayOf<IThread*>* enumeration);

    CARAPI_(Int32) EnumerateThreadGroupGeneric(
        /* [in] */ Boolean recurse,
        /* [in] */ Int32 enumerationIndex,
        /* [out] */ ArrayOf<IThreadGroup*>* enumeration);

    /*
     * Outputs to {@code System.out}a text representation of the
     * hierarchy of Threads and ThreadGroups in this thread group (and recursively).
     * The indentation will be four spaces per level of nesting.
     *
     * @param levels How many levels of nesting, so that proper indentation can
     * be output.
     */
    CARAPI_(void) ListImpl(
        /* [in] */ Int32 levels);

    /**
     * Removes an immediate subgroup.
     *
     * @param g ThreadGroup to remove
     *
     * @see #add(Thread)
     * @see #add(ThreadGroup)
     */
    CARAPI_(void) RemoveThreadGroup(
        /* [in] */ IThreadGroup* g);

    /**
         * Sets the parent {@code ThreadGroup} of this thread group, and adds this
     * thread group to the parent's collection of immediate children (if {@code parent} is
     * not {@code null}).
     *
     * @param parent The parent ThreadGroup, or null to make this thread group
     *        the root ThreadGroup
     *
     * @see #getParent
     * @see #parentOf
     */
    CARAPI_(void) SetParent(
        /* [in] */ IThreadGroup* parent);

    /**
     * @deprecated Requires deprecated method Thread.suspend().
     */
    //@SuppressWarnings("deprecation")
    //@Deprecated
    CARAPI_(Boolean) StopHelper();

    /**
     * @deprecated Requires deprecated method Thread.suspend().
     */
    //@SuppressWarnings("deprecation")
    //@Deprecated
    CARAPI_(Boolean) SuspendHelper();

public:
    // The ThreadGroup to which this ThreadGroup belongs
    AutoPtr<IThreadGroup> mParent;

    // BEGIN android-note
    // VM needs this field name for debugging.
    // END android-note
    Int32 mNumThreads;

    // The number of children groups
    Int32 mNumGroups;

private:
    // Name of this ThreadGroup
    String mName;
    // BEGIN android-note
    // VM needs this field name for debugging.
    // END android-note

    // Maximum priority for Threads inside this ThreadGroup
    Int32 mMaxPriority;

    // The Threads this ThreadGroup contains
    AutoPtr< ArrayOf<IThread*> > mChildrenThreads;
    // Locked when using the childrenThreads field
    Mutex mChildrenThreadsLock;

    // The ThreadGroups this ThreadGroup contains
    AutoPtr< ArrayOf<IThreadGroup*> > mChildrenGroups;
    // Locked when using the childrenGroups field
    Mutex mChildrenGroupsLock;

    // Whether this ThreadGroup is a daemon ThreadGroup or not
    Boolean mIsDaemon;

    // Whether this ThreadGroup has already been destroyed or not
    Boolean mIsDestroyed;
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_THREADGROUP_H__

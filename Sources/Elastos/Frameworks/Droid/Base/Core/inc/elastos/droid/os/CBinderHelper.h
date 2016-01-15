#ifndef __ELASTOS_DROID_OS_CBINDERHELPER_H__
#define __ELASTOS_DROID_OS_CBINDERHELPER_H__

#include "_Elastos_Droid_Os_CBinderHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CBinderHelper)
    , public Singleton
    , public IBinderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Return the ID of the process that sent you the current transaction
     * that is being processed.  This pid can be used with higher-level
     * system services to determine its identity and check permissions.
     * If the current thread is not currently executing an incoming transaction,
     * then its own pid is returned.
     */
    CARAPI GetCallingPid(
        /* [out] */ Int32* pid);

    /**
     * Return the Linux uid assigned to the process that sent you the
     * current transaction that is being processed.  This uid can be used with
     * higher-level system services to determine its identity and check
     * permissions.  If the current thread is not currently executing an
     * incoming transaction, then its own uid is returned.
     */
    CARAPI GetCallingUid(
        /* [out] */ Int32* pid);

    /**
     * Return the UserHandle assigned to the process that sent you the
     * current transaction that is being processed.  This is the user
     * of the caller.  It is distinct from {@link #getCallingUid()} in that a
     * particular user will have multiple distinct apps running under it each
     * with their own uid.  If the current thread is not currently executing an
     * incoming transaction, then its own UserHandle is returned.
     */
    CARAPI GetCallingUserHandle(
        /* [out] */ IUserHandle** h);

    /**
     * Reset the identity of the incoming IPC on the current thread.  This can
     * be useful if, while handling an incoming call, you will be calling
     * on interfaces of other objects that may be local to your process and
     * need to do permission checks on the calls coming into them (so they
     * will check the permission of your own local process, and not whatever
     * process originally called you).
     *
     * @return Returns an opaque token that can be used to restore the
     * original calling identity by passing it to
     * {@link #restoreCallingIdentity(long)}.
     *
     * @see #getCallingPid()
     * @see #getCallingUid()
     * @see #restoreCallingIdentity(long)
     */
    CARAPI ClearCallingIdentity(
        /* [out] */ Int64* token);

    /**
     * Restore the identity of the incoming IPC on the current thread
     * back to a previously identity that was returned by {@link
     * #clearCallingIdentity}.
     *
     * @param token The opaque token that was previously returned by
     * {@link #clearCallingIdentity}.
     *
     * @see #clearCallingIdentity
     */
    CARAPI RestoreCallingIdentity(
        /* [in] */ Int64 token);

    /**
     * Sets the native thread-local StrictMode policy mask.
     *
     * <p>The StrictMode settings are kept in two places: a Java-level
     * threadlocal for libcore/Dalvik, and a native threadlocal (set
     * here) for propagation via Binder calls.  This is a little
     * unfortunate, but necessary to break otherwise more unfortunate
     * dependencies either of Dalvik on Android, or Android
     * native-only code on Dalvik.
     *
     * @see StrictMode
     * @hide
     */
    CARAPI SetThreadStrictModePolicy(
        /* [in] */ Int32 policyMask);

    /**
     * Gets the current native thread-local StrictMode policy mask.
     *
     * @see #setThreadStrictModePolicy
     * @hide
     */
    CARAPI GetThreadStrictModePolicy(
        /* [out] */ Int32* policy);

    /**
     * Flush any Binder commands pending in the current thread to the kernel
     * driver.  This can be
     * useful to call before performing an operation that may block for a long
     * time, to ensure that any pending object references have been released
     * in order to prevent the process from holding on to objects longer than
     * it needs to.
     */
    CARAPI FlushPendingCommands();

    /**
     * Add the calling thread to the IPC thread pool.  This function does
     * not return until the current process is exiting.
     */
    CARAPI JoinThreadPool();

};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CBINDERHELPER_H__

#ifndef __ELASTOS_DROID_INTERNAL_OS_BINDER_INTERNAL_H__
#define __ELASTOS_DROID_INTERNAL_OS_BINDER_INTERNAL_H__

#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Core::IRunnable;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

/**
 * Private and debugging Binder APIs.
 *
 * @see IBinder
 */
class ECO_PUBLIC BinderInternal
{
private:
    class ECO_LOCAL GcWatcher
        : public Object
    {
    public:
        ~GcWatcher();

        CARAPI Finalize();
    };

public:
    static CARAPI AddGcWatcher(
        /* [in] */ IRunnable* watcher);

    /**
     * Add the calling thread to the IPC thread pool.  This function does
     * not return until the current process is exiting.
     */
    static CARAPI JoinThreadPool();

    /**
     * Return the system time (as reported by {@link SystemClock#uptimeMillis
     * SystemClock.uptimeMillis()}) that the last garbage collection occurred
     * in this process.  This is not for general application use, and the
     * meaning of "when a garbage collection occurred" will change as the
     * garbage collector evolves.
     *
     * @return Returns the time as per {@link SystemClock#uptimeMillis
     * SystemClock.uptimeMillis()} of the last garbage collection.
     */
    static Int64 GetLastGcTime();

    /**
     * Return the global "context object" of the system.  This is usually
     * an implementation of IServiceManager, which you can use to find
     * other services.
     */
    static AutoPtr<IBinder> GetContextObject();

    /**
     * Special for system process to not allow incoming calls to run at
     * background scheduling priority.
     * @hide
     */
    static CARAPI DisableBackgroundScheduling(
        /* [in] */ Boolean disable);

    static CARAPI HandleGc();

    static CARAPI ForceGc(
        /* [in] */ const String& reason);

    static CARAPI ForceBinderGc();

private:
    ECO_LOCAL BinderInternal();
    ECO_LOCAL BinderInternal(const BinderInternal&);

private:
    // static AutoPtr<IWeakReference> sGcWatcher; //new WeakReference<GcWatcher>(new GcWatcher());

    // static AutoPtr<IArrayList> sGcWatchers;// = new ArrayList<>();
    // static AutoPtr<ArrayOf<IInterface*> > sTmpWatchers;// = new Runnable[1];
    // static Int64 sLastGcTime;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_BINDER_INTERNAL_H__

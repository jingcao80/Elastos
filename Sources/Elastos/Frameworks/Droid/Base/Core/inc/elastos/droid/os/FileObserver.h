
#ifndef __ELASTOS_DROID_OS_FILEOBSERVER_H__
#define __ELASTOS_DROID_OS_FILEOBSERVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Os::IFileObserver;
using Elastos::Core::IRunnable;
using Elastos::Core::Thread;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Os {

class ECO_PUBLIC FileObserver
    : public Object
    , public IFileObserver
{
private:
    class ObserverThread
        : public Thread
    {
        friend class FileObserver;
    public:
        ObserverThread();

        CARAPI Run();

        CARAPI_(Int32) StartWatching(
            /* [in] */ const String& path,
            /* [in] */ Int32 mask,
            /* [in] */ FileObserver* observer);

        CARAPI_(void) StopWatching(
            /* [in] */ Int32 descriptor);

        CARAPI OnEvent(
            /* [in] */ Int32 wfd,
            /* [in] */ Int32 mask,
            /* [in] */ const String& path);

    private:
        CARAPI_(Int32) NativeInit();

        CARAPI_(void) NativeObserve(
            /* [in] */ Int32 fd);

        CARAPI_(Int32) NativeStartWatching(
            /* [in] */ Int32 fd,
            /* [in] */ const String& pathString,
            /* [in] */ Int32 mask);

        CARAPI_(void) NativeStopWatching(
            /* [in] */ Int32 fd,
            /* [in] */ Int32 wfd);

    private:
        HashMap<Int32, AutoPtr<IWeakReference> > mObservers;
        Object mObserverslock;
        Int32 mFd;
    };

public:
    CAR_INTERFACE_DECL()

    FileObserver();

    /**
     * Create a new file observer for a certain file or directory.
     * Monitoring does not start on creation!  You must call
     * {@link #startWatching()} before you will receive events.
     *
     * @param path The file or directory to monitor
     * @param mask The event or events (added together) to watch for
     */
    FileObserver(
        /* [in] */ const String& path,
        /* [in] */ Int32 mask = IFileObserver::ALL_EVENTS);

    virtual ~FileObserver();

    CARAPI constructor(
        /* [in] */ const String& path);

    CARAPI constructor(
        /* [in] */ const String& path,
        /* [in] */ Int32 mask = IFileObserver::ALL_EVENTS);

    /**
     * Start watching for events.  The monitored file or directory must exist at
     * this time, or else no events will be reported (even if it appears later).
     * If monitoring is already started, this call has no effect.
     */
    CARAPI StartWatching();

    /**
     * Stop watching for events.  Some events may be in process, so events
     * may continue to be reported even after this method completes.  If
     * monitoring is already stopped, this call has no effect.
     */
    CARAPI StopWatching();

private:
    ECO_LOCAL static CARAPI_(Boolean) InitObserverThread();

private:
    ECO_LOCAL static AutoPtr<ObserverThread> sObserverThread;
    ECO_LOCAL static const Boolean sObserverThreadInited;

    // instance
    String mPath;
    Int32 mDescriptor;
    Int32 mMask;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_FILEOBSERVER_H__

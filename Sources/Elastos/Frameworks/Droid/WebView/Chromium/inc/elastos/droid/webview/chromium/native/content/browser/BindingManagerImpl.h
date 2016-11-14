
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_BINDINGMANAGERIMPL_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_BINDINGMANAGERIMPL_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/webkit/webview/chromium/native/content/browser/BindingManager.h"

using Elastos::Core::Runnable;
using Elastos::Droid::Utility::ISparseArray;

// import android.util.Log;
// import android.util.SparseArray;

// import com.google.common.annotations.VisibleForTesting;

// import org.chromium.base.SysUtils;
// import org.chromium.base.ThreadUtils;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
 * Manages oom bindings used to bound child services.
 */
class BindingManagerImpl : public BindingManager
{
private:
    /**
     * Wraps ChildProcessConnection keeping track of additional information needed to manage the
     * bindings of the connection. The reference to ChildProcessConnection is cleared when the
     * connection goes away, but ManagedConnection itself is kept (until overwritten by a new entry
     * for the same pid).
     */
    class ManagedConnection : public Object
    {
        friend class BindingManagerImpl;
    private:
        class DelayedRunnable
            : public Runnable
        {
        public:
            DelayedRunnable(
                /* [in] */ ManagedConnection* owner,
                /* [in] */ ChildProcessConnection* connection);

            CAR_INTERFACE_DECL()

            CARAPI Run();

        private:
            ManagedConnection* mOwner;
            ChildProcessConnection* mConnection;
        };

        class DoUnbindRunnable
            : public Runnable
        {
        public:
            DoUnbindRunnable(
                /* [in] */ ManagedConnection* owner,
                /* [in] */ ChildProcessConnection* connection);

            CAR_INTERFACE_DECL()

            CARAPI Run();

        private:
            ManagedConnection* mOwner;
            ChildProcessConnection* mConnection;
        };

    public:
        ManagedConnection(
            /* [in] */ BindingManagerImpl* owner,
            /* [in] */ ChildProcessConnection* connection);

        //CAR_INTERFACE_DECL()

        /**
         * Sets the visibility of the service, adding or removing the strong binding as needed. This
         * also removes the initial binding, as the service visibility is now known.
         */
        CARAPI_(void) SetInForeground(
            /* [in] */ Boolean nextInForeground);

        /**
         * Sets or removes additional binding when the service is main service during the embedder
         * background period.
         */
        CARAPI_(void) SetBoundForBackgroundPeriod(
            /* [in] */ Boolean nextBound);

        CARAPI_(Boolean) IsOomProtected();

        CARAPI_(void) ClearConnection();

        /** @return true iff the reference to the connection is no longer held */
        //@VisibleForTesting
        CARAPI_(Boolean) IsConnectionCleared();

    private:
        /** Removes the initial service binding. */
        CARAPI_(void) RemoveInitialBinding();

        /** Adds a strong service binding. */
        CARAPI_(void) AddStrongBinding();

        /** Removes a strong service binding. */
        CARAPI_(void) RemoveStrongBinding();

        /**
         * Drops the service bindings. This is used on low-end to drop bindings of the current
         * service when a new one is created.
         */
        CARAPI_(void) DropBindings();

    private:
        BindingManagerImpl* mOwner;

        // Set in constructor, cleared in clearConnection().
        AutoPtr<ChildProcessConnection> mConnection;

        // True iff there is a strong binding kept on the service because it is working in
        // foreground.
        Boolean mInForeground;

        // True iff there is a strong binding kept on the service because it was bound for the
        // application background period.
        Boolean mBoundForBackgroundPeriod;

        // When mConnection is cleared, oom binding status is stashed here.
        Boolean mWasOomProtected;
    };

public:
    static CARAPI_(AutoPtr<BindingManagerImpl>) CreateBindingManager();

    /**
     * Creates a testing instance of BindingManager. Testing instance will have the unbinding delays
     * set to 0, so that the tests don't need to deal with actual waiting.
     * @param isLowEndDevice true iff the created instance should apply low-end binding policies
     */
    static CARAPI_(AutoPtr<BindingManagerImpl>) CreateBindingManagerForTesting(
        /* [in] */ Boolean isLowEndDevice);

    //@Override
    CARAPI_(void) AddNewConnection(
        /* [in] */ Int32 pid,
        /* [in] */ ChildProcessConnection* connection);

    //@Override
    CARAPI_(void) SetInForeground(
        /* [in] */ Int32 pid,
        /* [in] */ Boolean inForeground);

    //@Override
    CARAPI_(void) OnSentToBackground();

    //@Override
    CARAPI_(void) OnBroughtToForeground();

    //@Override
    CARAPI_(Boolean) IsOomProtected(
        /* [in] */ Int32 pid);

    //@Override
    CARAPI_(void) ClearConnection(
        /* [in] */ Int32 pid);

    /** @return true iff the connection reference is no longer held */
    //@VisibleForTesting
    CARAPI_(Boolean) IsConnectionCleared(
        /* [in] */ Int32 pid);

private:
    /**
     * The constructor is private to hide parameters exposed for testing from the regular consumer.
     * Use factory methods to create an instance.
     */
    BindingManagerImpl(
        /* [in] */ Boolean isLowMemoryDevice,
        /* [in] */ Int64 removeInitialBindingDelay,
        /* [in] */ Int64 removeStrongBindingDelay);

private:
    static const String TAG;

    // Delay of 1 second used when removing the initial oom binding of a process.
    static const Int64 REMOVE_INITIAL_BINDING_DELAY_MILLIS = 1 * 1000;

    // Delay of 1 second used when removing temporary strong binding of a process (only on
    // non-low-memory devices).
    static const Int64 DETACH_AS_ACTIVE_HIGH_END_DELAY_MILLIS = 1 * 1000;

    // These fields allow to override the parameters for testing - see
    // createBindingManagerForTesting().
    const Int64 mRemoveInitialBindingDelay;
    const Int64 mRemoveStrongBindingDelay;
    const Boolean mIsLowMemoryDevice;

    // This can be manipulated on different threads, synchronize access on mManagedConnections.
    AutoPtr<ISparseArray> mManagedConnections;// ManagedConnection
    Object mManagedConnectionsLock;

    // The connection that was most recently set as foreground (using setInForeground()). This is
    // used to add additional binding on it when the embedder goes to background. On low-end, this
    // is also used to drop process bidnings when a new one is created, making sure that only one
    // renderer process at a time is protected from oom killing.
    AutoPtr<ManagedConnection> mLastInForeground;

    // Synchronizes operations that access mLastInForeground: setInForeground() and
    // addNewConnection().
    Object mLastInForegroundLock;

    // The connection bound with additional binding in onSentToBackground().
    AutoPtr<ManagedConnection> mBoundForBackgroundPeriod;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_BINDINGMANAGERIMPL_H__

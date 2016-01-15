
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_BROWSERSTARTUPCONTROLLER_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_BROWSERSTARTUPCONTROLLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content/browser/ChildProcessLauncher.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Utility::IList;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IHandler;
// import android.util.Log;

// import com.google.common.annotations.VisibleForTesting;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.base.ThreadUtils;
// import org.chromium.base.library_loader.LibraryLoader;
// import org.chromium.base.library_loader.LoaderErrors;
// import org.chromium.base.library_loader.ProcessInitException;
// import org.chromium.content.app.ContentMain;

// import java.util.ArrayList;
// import java.util.List;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

/**
 * This class controls how C++ browser main loop is started and ensures it happens only once.
 *
 * It supports kicking off the startup sequence in an asynchronous way. Startup can be called as
 * many times as needed (for instance, multiple activities for the same application), but the
 * browser process will still only be initialized once. All requests to start the browser will
 * always get their callback executed; if the browser process has already been started, the callback
 * is called immediately, else it is called when initialization is complete.
 *
 * All communication with this class must happen on the main thread.
 *
 * This is a singleton, and stores a reference to the application context.
 */
//@JNINamespace("content")
class BrowserStartupController
    : public Object
{
public:
    /**
     * This provides the interface to the callbacks for successful or failed startup
     */
    class StartupCallback
        : public Object
    {
    public:
        virtual CARAPI_(void) OnSuccess(
            /* [in] */ Boolean alreadyStarted) = 0;

        virtual CARAPI_(void) OnFailure() = 0;
    };

private:
    class EnqueueCallbackExecutionRunnable
        : public Runnable
    {
    public:
        EnqueueCallbackExecutionRunnable(
            /* [in] */ BrowserStartupController* owner,
            /* [in] */ const Int32 startupFailure,
            /* [in] */ const Boolean alreadyStarted);

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        BrowserStartupController* mOwner;
        Int32 mStartupFailure;
        Boolean mAlreadyStarted;
    };

    class PostStartupCompletedRunnable
        : public Runnable
    {
    public:
        PostStartupCompletedRunnable(
            /* [in] */ BrowserStartupController* owner,
            /* [in] */ StartupCallback* callback);

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        BrowserStartupController* mOwner;
        StartupCallback* mCallback;
    };

public:
    BrowserStartupController(
        /* [in] */ IContext* context);

    //@VisibleForTesting
    //@CalledByNative
    static CARAPI_(Boolean) BrowserMayStartAsynchonously();

    //@VisibleForTesting
    //@CalledByNative
    static CARAPI_(void) BrowserStartupComplete(
        /* [in] */ Int32 result);

    static CARAPI_(AutoPtr<BrowserStartupController>) Get(
        /* [in] */ IContext* context);

    //@VisibleForTesting
    static CARAPI_(AutoPtr<BrowserStartupController>) OverrideInstanceForTest(
        /* [in] */ BrowserStartupController* controller);

    /**
     * Start the browser process asynchronously. This will set up a queue of UI thread tasks to
     * initialize the browser process.
     * <p/>
     * Note that this can only be called on the UI thread.
     *
     * @param callback the callback to be called when browser startup is complete.
     */
    CARAPI_(void) StartBrowserProcessesAsync(
        /* [in] */ StartupCallback* callback);

    /**
     * Start the browser process synchronously. If the browser is already being started
     * asynchronously then complete startup synchronously
     *
     * <p/>
     * Note that this can only be called on the UI thread.
     *
     * @param maxRenderers The maximum number of renderer processes the browser may
     *                      create. Zero for single process mode.
     * @throws ProcessInitException
     */
    CARAPI_(void) StartBrowserProcessesSync(
        /* [in] */ Int32 maxRenderers);

    /**
     * Wrap ContentMain.start() for testing.
     */
    //@VisibleForTesting
    CARAPI_(Int32) ContentStart();

    CARAPI_(void) AddStartupCompletedObserver(
        /* [in] */ StartupCallback* callback);

    //@VisibleForTesting
    CARAPI_(void) PrepareToStartBrowserProcess(
        /* [in] */ Int32 maxRendererProcesses);

    /**
     * Initialization needed for tests. Mainly used by content browsertests.
     */
    CARAPI_(void) InitChromiumBrowserProcessForTests();

    static CARAPI_(void*) ElaBrowserStartupControllerCallback_Init();

private:
    static CARAPI_(void) SetAsynchronousStartup(
        /* [in] */ Boolean enable);

    CARAPI_(void) ExecuteEnqueuedCallbacks(
        /* [in] */ Int32 startupResult,
        /* [in] */ Boolean alreadyStarted);

    // Queue the callbacks to run. Since running the callbacks clears the list it is safe to call
    // this more than once.
    CARAPI_(void) EnqueueCallbackExecution(
        /* [in] */ const Int32 startupFailure,
        /* [in] */ const Boolean alreadyStarted);

    CARAPI_(void) PostStartupCompleted(
        /* [in] */ StartupCallback* callback);

    CARAPI_(String) GetPlugins();

    static CARAPI_(void) NativeSetCommandLineFlags(
        /* [in] */ Int32 maxRenderProcesses,
        /* [in] */ const String& pluginDescriptor);

    // Is this an official build of Chrome? Only native code knows for sure. Official build
    // knowledge is needed very early in process startup.
    static CARAPI_(Boolean) NativeIsOfficialBuild();

    static CARAPI_(Boolean) NativeIsPluginEnabled();

public:
    // Helper constants for {@link #executeEnqueuedCallbacks(int, boolean)}.
    //@VisibleForTesting
    static const Int32 STARTUP_SUCCESS = -1;
    //@VisibleForTesting
    static const Int32 STARTUP_FAILURE = 1;

    // Use single-process mode that runs the renderer on a separate thread in
    // the main application.
    static const Int32 MAX_RENDERERS_SINGLE_PROCESS = 0;

    // Cap on the maximum number of renderer processes that can be requested.
    // This is currently set to account for:
    //  13: The maximum number of sandboxed processes we have available
    // - 1: The regular New Tab Page
    // - 1: The incognito New Tab Page
    // - 1: A regular incognito tab
    // - 1: Safety buffer (http://crbug.com/251279)
    static const Int32 MAX_RENDERERS_LIMIT =
            ChildProcessLauncher::MAX_REGISTERED_SANDBOXED_SERVICES - 4;

private:
    static const String TAG;

    // Helper constants for {@link StartupCallback#onSuccess}.
    static const Boolean ALREADY_STARTED;
    static const Boolean NOT_ALREADY_STARTED;

    static AutoPtr<BrowserStartupController> sInstance;

    static Boolean sBrowserMayStartAsynchronously;

    // A list of callbacks that should be called when the async startup of the browser process is
    // complete.
    //const List<StartupCallback> mAsyncStartupCallbacks;
    AutoPtr<IList> mAsyncStartupCallbacks;

    // The context is set on creation, but the reference is cleared after the browser process
    // initialization has been started, since it is not needed anymore. This is to ensure the
    // context is not leaked.
    const AutoPtr<IContext> mContext;

    // Whether the async startup of the browser process has started.
    Boolean mHasStartedInitializingBrowserProcess;

    // Whether the async startup of the browser process is complete.
    Boolean mStartupDone;

    // This field is set after startup has been completed based on whether the startup was a success
    // or not. It is used when later requests to startup come in that happen after the initial set
    // of enqueued callbacks have been executed.
    Boolean mStartupSuccess;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_BROWSERSTARTUPCONTROLLER_H__

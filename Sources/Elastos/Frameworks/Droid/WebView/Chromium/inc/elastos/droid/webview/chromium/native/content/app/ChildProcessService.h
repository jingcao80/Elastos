
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_APP_CHILDPROCESSSERVICE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_APP_CHILDPROCESSSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/app/ChromiumLinkerParams.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Core::IThread;
using Elastos::Droid::App::IService;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::Runnable;
// import android.os.RemoteException;
// import android.util.Log;
using Elastos::Droid::View::ISurface;
using Elastos::Utility::IArrayList;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.base.library_loader.LibraryLoader;
// import org.chromium.base.library_loader.Linker;
// import org.chromium.base.library_loader.ProcessInitException;
// import org.chromium.content.browser.ChildProcessConnection;
// import org.chromium.content.browser.ChildProcessLauncher;
// import org.chromium.content.common.IChildProcessCallback;
// import org.chromium.content.common.IChildProcessService;

// import java.util.ArrayList;
// import java.util.concurrent.atomic.AtomicReference;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace App {

/**
 * This is the base class for child services; the [Non]SandboxedProcessService0, 1.. etc
 * subclasses provide the concrete service entry points, to enable the browser to connect
 * to more than one distinct process (i.e. one process per service number, up to limit of N).
 * The embedding application must declare these service instances in the application section
 * of its AndroidManifest.xml, for example with N entries of the form:-
 *     <service android:name="org.chromium.content.app.[Non]SandboxedProcessServiceX"
 *              android:process=":[non]sandboxed_processX" />
 * for X in 0...N-1 (where N is {@link ChildProcessLauncher#MAX_REGISTERED_SERVICES})
 */
//@JNINamespace("content")
class ChildProcessService
    : public Elastos::Droid::App::Service
{
private:
    class InnerRunnable
        : public Runnable
    {
    public:
        InnerRunnable(
            /* [in] */ ChildProcessService* owner);

        CARAPI Run();

    private:
        ChildProcessService* mOwner;
    };

public:
    ChildProcessService();

    //CAR_INTERFACE_DECL()

    /* package */
    static AutoPtr<IContext> GetContext();

    //@Override
    CARAPI OnCreate();

    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    static CARAPI_(void*) ElaChildProcessServiceCallback_Init();

private:
    static CARAPI_(void) EstablishSurfaceTexturePeer(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 pid,
        /* [in] */ IInterface* surfaceObject,
        /* [in] */ Int32 primaryID,
        /* [in] */ Int32 secondaryID);

    static CARAPI_(AutoPtr<IInterface>) GetViewSurface(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 surfaceId);

    static CARAPI_(AutoPtr<IInterface>) GetSurfaceTextureSurface(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 primaryId,
        /* [in] */ Int32 secondaryId);

    /**
     * Called from native code to share a surface texture with another child process.
     * Through using the callback object the browser is used as a proxy to route the
     * call to the correct process.
     *
     * @param pid Process handle of the child process to share the SurfaceTexture with.
     * @param surfaceObject The Surface or SurfaceTexture to share with the other child process.
     * @param primaryID Used to route the call to the correct client instance.
     * @param secondaryID Used to route the call to the correct client instance.
     */
    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) EstablishSurfaceTexturePeer(
        /* [in] */ Int32 pid,
        /* [in] */ IInterface* surfaceObject,
        /* [in] */ Int32 primaryID,
        /* [in] */ Int32 secondaryID);

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(AutoPtr<ISurface>) GetViewSurface(
        /* [in] */ Int32 surfaceId);

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(AutoPtr<ISurface>) GetSurfaceTextureSurface(
        /* [in] */ Int32 primaryId,
        /* [in] */ Int32 secondaryId);

    /**
     * The main entry point for a child process. This should be called from a new thread since
     * it will not return until the child process exits. See child_process_service.{h,cc}
     *
     * @param applicationContext The Application Context of the current process.
     * @param service The current ChildProcessService object.
     * @param fileIds A list of file IDs that should be registered for access by the renderer.
     * @param fileFds A list of file descriptors that should be registered for access by the
     * renderer.
     */
    static CARAPI_(void) NativeInitChildProcess(
        /* [in] */ IContext* applicationContext,
        /* [in] */ ChildProcessService* service,
        /* [in] */ ArrayOf<Int32>* extraFileIds,
        /* [in] */ ArrayOf<Int32>* extraFileFds,
        /* [in] */ Int32 cpuCount,
        /* [in] */ Int64 cpuFeatures);

    /**
     * Force the child process to exit.
     */
    static CARAPI_(void) NativeExitChildProcess();

    CARAPI_(void) NativeShutdownMainThread();

private:

    static const String MAIN_THREAD_NAME;
    static const String TAG;
    //TODO AutoPtr<IChildProcessCallback> mCallback;

    // This is the native "Main" thread for the renderer / utility process.
    AutoPtr<IThread> mMainThread;
    Object mMainThreadLock;

    // Parameters received via IPC, only accessed while holding the mMainThread monitor.
    AutoPtr< ArrayOf<String> > mCommandLineParams;
    Int32 mCpuCount;
    Int64 mCpuFeatures;
    // Pairs IDs and file descriptors that should be registered natively.
    AutoPtr<IArrayList> mFileIds;
    AutoPtr<IArrayList> mFileFds;
    // Linker-specific parameters for this child process service.
    AutoPtr<ChromiumLinkerParams> mLinkerParams;

// TODO   static AtomicReference<Context> sContext;
    Boolean mLibraryInitialized;
    // Becomes true once the service is bound. Access must synchronize around mMainThread.
    Boolean mIsBound;

    // Binder object used by clients for this service.
// TODO   const IChildProcessService.Stub mBinder;
};

} // namespace App
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_APP_CHILDPROCESSSERVICE_H__

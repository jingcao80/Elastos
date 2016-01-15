
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CHILDPROCESSLAUNCHER_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CHILDPROCESSLAUNCHER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content/app/ChildProcessService.h"
#include "elastos/droid/webkit/native/content/app/ChromiumLinkerParams.h"
#include "elastos/droid/webkit/native/content/app/SandboxedProcessService.h"
#include "elastos/droid/webkit/native/content/app/PrivilegedProcessService.h"
#include "elastos/droid/webkit/native/content/browser/ChildProcessConnection.h"
#include "elastos/droid/webkit/native/content/browser/BindingManager.h"
#include "elastos/droid/webkit/native/content/browser/FileDescriptorInfo.h"

using Elastos::Utility::IMap;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ISurfaceTexture;
// import android.util.Log;
// import android.util.Pair;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::Webkit::Content::App::ChildProcessService;
using Elastos::Droid::Webkit::Content::App::ChromiumLinkerParams;
using Elastos::Droid::Webkit::Content::App::SandboxedProcessService;
using Elastos::Droid::Webkit::Content::App::PrivilegedProcessService;

// import com.google.common.annotations.VisibleForTesting;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.base.ThreadUtils;
// import org.chromium.base.TraceEvent;
// import org.chromium.base.library_loader.Linker;
// import org.chromium.content.app.ChildProcessService;
// import org.chromium.content.app.ChromiumLinkerParams;
// import org.chromium.content.app.PrivilegedProcessService;
// import org.chromium.content.app.SandboxedProcessService;
// import org.chromium.content.common.IChildProcessCallback;
// import org.chromium.content.common.SurfaceWrapper;

// import java.util.ArrayList;
// import java.util.Map;
// import java.util.concurrent.ConcurrentHashMap;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

/**
 * This class provides the method to start/stop ChildProcess called by native.
 */
//@JNINamespace("content")
class ChildProcessLauncher
    : public Object
{
private:
    class ChildConnectionAllocator
        : public Object
    {
    public:
        ChildConnectionAllocator(
            /* [in] */ Boolean inSandbox);

        CARAPI_(void) SetServiceClass(
            /* [in] */ ChildProcessService* childClass);

        CARAPI_(AutoPtr<ChildProcessConnection>) Allocate(
            /* [in] */ IContext* context,
            /* [in] */ ChildProcessConnection::DeathCallback* deathCallback,
            /* [in] */ ChromiumLinkerParams* chromiumLinkerParams);

        CARAPI_(void) Free(
            /* [in] */ ChildProcessConnection* connection);

        /** @return the count of connections managed by the allocator */
        //@VisibleForTesting
        CARAPI_(Int32) AllocatedConnectionsCountForTesting();

    private:
        // Connections to services. Indices of the array correspond to the service numbers.
        const AutoPtr< ArrayOf<ChildProcessConnection*> > mChildProcessConnections;

        // The list of free (not bound) service indices. When looking for a free service, the first
        // index in that list should be used. When a service is unbound, its index is added to the
        // end of the list. This is so that we avoid immediately reusing the freed service (see
        // http://crbug.com/164069): the framework might keep a service process alive when it's been
        // unbound for a short time. If a new connection to the same service is bound at that point,
        // the process is reused and bad things happen (mostly static variables are set when we
        // don't expect them to).
        // SHOULD BE ACCESSED WITH mConnectionLock.
//        const ArrayList<Integer> mFreeConnectionIndices;
//        const Object mConnectionLock;

        AutoPtr<ChildProcessService> mChildClass;
        const Boolean mInSandbox;
    };

    class InnerDeathCallback
        : public ChildProcessConnection::DeathCallback
    {
    public:
        InnerDeathCallback();

        CARAPI_(void) OnChildProcessDied(
            /* [in] */ ChildProcessConnection* connection);

    private:
        ChildProcessLauncher* mOwner;
    };

    class InnerConnectionCallback
        : public ChildProcessConnection::ConnectionCallback
    {
    public:
        InnerConnectionCallback(
            /* [in] */ const ChildProcessConnection* connection,
            /* [in] */ const Int64 clientContext);

        CARAPI_(void) OnConnected(
            /* [in] */ Int32 pid);

    private:
        const ChildProcessConnection* mConnection;
        const Int64 mClientContext;
    };

public:
    /**
     * Sets service class for sandboxed service and privileged service.
     */
    static CARAPI_(void) SetChildProcessClass(
        /* [in] */ SandboxedProcessService* sandboxedServiceClass,
        /* [in] */ PrivilegedProcessService* privilegedServiceClass);

    //@VisibleForTesting
    static CARAPI_(void) SetBindingManagerForTesting(
        /* [in] */ BindingManager* manager);

    /**
     * Sets the visibility of the child process when it changes or when it is determined for the
     * first time.
     */
    //@CalledByNative
    static CARAPI_(void) SetInForeground(
        /* [in] */ Int32 pid,
        /* [in] */ Boolean inForeground);

    /**
     * Called when the embedding application is sent to background.
     */
    static CARAPI_(void) OnSentToBackground();

    /**
     * Called when the embedding application is brought to foreground.
     */
    static CARAPI_(void) OnBroughtToForeground();

    /**
     * Should be called early in startup so the work needed to spawn the child process can be done
     * in parallel to other startup work. Must not be called on the UI thread. Spare connection is
     * created in sandboxed child process.
     * @param context the application context used for the connection.
     */
    static CARAPI_(void) WarmUp(
        /* [in] */ IContext* context);

    /**
     * Spawns and connects to a child process. May be called on any thread. It will not block, but
     * will instead callback to {@link #nativeOnChildProcessStarted} when the connection is
     * established. Note this callback will not necessarily be from the same thread (currently it
     * always comes from the main thread).
     *
     * @param context Context used to obtain the application context.
     * @param commandLine The child process command line argv.
     * @param fileIds The ID that should be used when mapping files in the created process.
     * @param fileFds The file descriptors that should be mapped in the created process.
     * @param fileAutoClose Whether the file descriptors should be closed once they were passed to
     * the created process.
     * @param clientContext Arbitrary parameter used by the client to distinguish this connection.
     */
    //@CalledByNative
    static CARAPI_(void) Start(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<String>* commandLine,
        /* [in] */ Int32 childProcessId,
        /* [in] */ ArrayOf<Int32>* fileIds,
        /* [in] */ ArrayOf<Int32>* fileFds,
        /* [in] */ ArrayOf<Boolean>* fileAutoClose,
        /* [in] */ Int64 clientContext);

    //@VisibleForTesting
    static CARAPI_(void) TriggerConnectionSetup(
        /* [in] */ const ChildProcessConnection* connection,
        /* [in] */ ArrayOf<String>* commandLine,
        /* [in] */ Int32 childProcessId,
        /* [in] */ ArrayOf<FileDescriptorInfo*>* filesToBeMapped,
        /* [in] */ Int32 callbackType,
        /* [in] */ const Int64 clientContext);

    /**
     * Terminates a child process. This may be called from any thread.
     *
     * @param pid The pid (process handle) of the service connection obtained from {@link #start}.
     */
    //@CalledByNative
    static CARAPI_(void) Stop(
        /* [in] */ Int32 pid);

    static CARAPI_(void) LogPidWarning(
        /* [in] */ Int32 pid,
        /* [in] */ const String& message);

    //@VisibleForTesting
    static CARAPI_(AutoPtr<ChildProcessConnection>) AllocateBoundConnectionForTesting(
        /* [in] */ IContext* context);

    /** @return the count of sandboxed connections managed by the allocator */
    //@VisibleForTesting
    static CARAPI_(Int32) AllocatedConnectionsCountForTesting();

    /** @return the count of services set up and working */
    //@VisibleForTesting
    static CARAPI_(Int32) ConnectedServicesCountForTesting();

    static CARAPI_(void*) ElaChildProcessLauncherCallback_Init();

private:
    static CARAPI_(void) RegisterViewSurface(
        /* [in] */ Int32 surfaceId,
        /* [in] */ IInterface* surface);

    static CARAPI_(void) RegisterSurfaceTexture(
        /* [in] */ Int32 surfaceTextureId,
        /* [in] */ Int32 childProcessId,
        /* [in] */ IInterface* surfaceTexture);

    static CARAPI_(void) Start(
        /* [in] */ IInterface* context,
        /* [in] */ ArrayOf<String>* commandLine,
        /* [in] */ Int32 childProcessId,
        /* [in] */ ArrayOf<Int32>* fileIds,
        /* [in] */ ArrayOf<Int32>* fileFds,
        /* [in] */ ArrayOf<Boolean>* fileAutoClose,
        /* [in] */ Int64 clientContext);

    static CARAPI_(AutoPtr<ChildConnectionAllocator>) GetConnectionAllocator(
        /* [in] */ Boolean inSandbox);

    static CARAPI_(AutoPtr<ChildProcessConnection>) AllocateConnection(
        /* [in] */ IContext* context,
        /* [in] */ Boolean inSandbox,
        /* [in] */ ChromiumLinkerParams* chromiumLinkerParams);

    static CARAPI_(AutoPtr<ChromiumLinkerParams>) GetLinkerParamsForNewConnection();

    static CARAPI_(AutoPtr<ChildProcessConnection>) AllocateBoundConnection(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<String>* commandLine,
        /* [in] */ Boolean inSandbox);

    static CARAPI_(void) FreeConnection(
        /* [in] */ ChildProcessConnection* connection);

    /** @return true iff the child process is protected from out-of-memory killing */
    //@CalledByNative
    static CARAPI_(Boolean) IsOomProtected(
        /* [in] */ Int32 pid);

    //@CalledByNative
    static CARAPI_(void) RegisterViewSurface(
        /* [in] */ Int32 surfaceId,
        /* [in] */ ISurface* surface);

    //@CalledByNative
    static CARAPI_(void) UnregisterViewSurface(
        /* [in] */ Int32 surfaceId);

    //@CalledByNative
    static CARAPI_(void) RegisterSurfaceTexture(
        /* [in] */ Int32 surfaceTextureId,
        /* [in] */ Int32 childProcessId,
        /* [in] */ ISurfaceTexture* surfaceTexture);

    //@CalledByNative
    static CARAPI_(void) UnregisterSurfaceTexture(
        /* [in] */ Int32 surfaceTextureId,
        /* [in] */ Int32 childProcessId);

    static CARAPI_(String) GetSwitchValue(
        /* [in] */ const ArrayOf<String>* commandLine,
        /* [in] */ const String& switchKey);

    /**
     * This implementation is used to receive callbacks from the remote service.
     */
//    static CARAPI_(AutoPtr<IChildProcessCallback>) CreateCallback(
//        /* [in] */ const Int32 childProcessId,
//        /* [in] */ const Int32 callbackType);

    static CARAPI_(void) NativeOnChildProcessStarted(
        /* [in] */ Int64 clientContext,
        /* [in] */ Int32 pid);

    static CARAPI_(void) NativeEstablishSurfacePeer(
        /* [in] */ Int32 pid,
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 primaryID,
        /* [in] */ Int32 secondaryID);

    static CARAPI_(Boolean) NativeIsSingleProcess();

public:
    static const Int32 CALLBACK_FOR_UNKNOWN_PROCESS = 0;
    static const Int32 CALLBACK_FOR_GPU_PROCESS = 1;
    static const Int32 CALLBACK_FOR_RENDERER_PROCESS = 2;

    // The upper limit on the number of simultaneous sandboxed and privileged child service process
    // instances supported. Each limit must not exceed total number of SandboxedProcessServiceX
    // classes and PrivilegedProcessServiceX classes declared in this package and defined as
    // services in the embedding application's manifest file.
    // (See {@link ChildProcessService} for more details on defining the services.)
    /* package */ static const Int32 MAX_REGISTERED_SANDBOXED_SERVICES = 13;
    /* package */ static const Int32 MAX_REGISTERED_PRIVILEGED_SERVICES = 3;

private:
    static const String TAG;

    static const String SWITCH_PROCESS_TYPE;
    static const String SWITCH_PPAPI_BROKER_PROCESS;
    static const String SWITCH_RENDERER_PROCESS;
    static const String SWITCH_GPU_PROCESS;

    // Service class for child process. As the default value it uses SandboxedProcessService0 and
    // PrivilegedProcessService0.
    static const AutoPtr<ChildConnectionAllocator> sSandboxedChildConnectionAllocator;
    static const AutoPtr<ChildConnectionAllocator> sPrivilegedChildConnectionAllocator;

    static Boolean sConnectionAllocated;

    static Boolean sLinkerInitialized;
    static Int64 sLinkerLoadAddress;

    // Represents an invalid process handle; same as base/process/process.h kNullProcessHandle.
    static const Int32 NULL_PROCESS_HANDLE = 0;

    // Map from pid to ChildService connection.
    //static Map<Integer, ChildProcessConnection> sServiceMap;
    static AutoPtr<IMap> sServiceMap;

    // A pre-allocated and pre-bound connection ready for connection setup, or null.
    static AutoPtr<ChildProcessConnection> sSpareSandboxedConnection;

    // Manages oom bindings used to bind chind services.
    static AutoPtr<BindingManager> sBindingManager;

    // Map from surface id to Surface.
    //static Map<Integer, Surface> sViewSurfaceMap;
    static AutoPtr<IMap> sViewSurfaceMap;

    // Map from surface texture id to Surface.
    //static Map<Pair<Integer, Integer>, Surface> sSurfaceTextureSurfaceMap;
    static AutoPtr<IMap> sSurfaceTextureSurfaceMap;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CHILDPROCESSLAUNCHER_H__

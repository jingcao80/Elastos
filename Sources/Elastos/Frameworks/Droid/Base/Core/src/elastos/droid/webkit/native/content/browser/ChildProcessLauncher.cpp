
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/native/content/browser/ChildProcessLauncher.h"
#include "elastos/droid/webkit/native/content/api/ChildProcessLauncher_dec.h"
#include "elastos/droid/webkit/native/content/browser/BindingManagerImpl.h"
#include "elastos/droid/webkit/native/base/ThreadUtils.h"
#include "elastos/droid/webkit/native/base/TraceEvent.h"

using Elastos::Droid::Webkit::Base::ThreadUtils;
using Elastos::Droid::Webkit::Base::TraceEvent;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

//===============================================================
//        ChildProcessLauncher::ChildConnectionAllocator
//===============================================================

ChildProcessLauncher::ChildConnectionAllocator::ChildConnectionAllocator(
    /* [in] */ Boolean inSandbox)
    : mInSandbox(inSandbox)
{
    assert(0);
    Int32 numChildServices = inSandbox ?
            MAX_REGISTERED_SANDBOXED_SERVICES : MAX_REGISTERED_PRIVILEGED_SERVICES;
    // mChildProcessConnections = ArrayOf<ChildProcessConnection>::Alloc(numChildServices);
    // mFreeConnectionIndices = new ArrayList<Integer>(numChildServices);
    for (Int32 i = 0; i < numChildServices; i++) {
    //     mFreeConnectionIndices->Add(i);
    }
    // SetServiceClass(inSandbox ?
    //         SandboxedProcessService.class : PrivilegedProcessService.class);
}

void ChildProcessLauncher::ChildConnectionAllocator::SetServiceClass(
    /* [in] */ ChildProcessService* childClass)
{
    mChildClass = childClass;
}

AutoPtr<ChildProcessConnection> ChildProcessLauncher::ChildConnectionAllocator::Allocate(
    /* [in] */ IContext* context,
    /* [in] */ ChildProcessConnection::DeathCallback* deathCallback,
    /* [in] */ ChromiumLinkerParams* chromiumLinkerParams)
{
    assert(0);
#if 0
    Object::Autolock lock(mConnectionLock);

    if (mFreeConnectionIndices->IsEmpty()) {
//        Log.w(TAG, "Ran out of service.");
        return NULL;
    }

    Int32 slot = mFreeConnectionIndices.remove(0);
    assert((*mChildProcessConnections)[slot] == NULL);
    (*mChildProcessConnections)[slot] = new ChildProcessConnectionImpl(context, slot,
            mInSandbox, deathCallback, mChildClass, chromiumLinkerParams);
    return (*mChildProcessConnections)[slot];
#endif

    return NULL;
}

void ChildProcessLauncher::ChildConnectionAllocator::Free(
    /* [in] */ ChildProcessConnection* connection)
{
    assert(0);
#if 0
    Object::Autolock lock(mConnectionLock);

    Int32 slot = connection->GetServiceNumber();
    if (mChildProcessConnections[slot] != connection) {
        Int32 occupier = (*mChildProcessConnections)[slot] == NULL ?
                -1 : (*mChildProcessConnections)[slot]->GetServiceNumber();
//        Log.e(TAG, "Unable to find connection to free in slot: " + slot +
//                " already occupied by service: " + occupier);
        assert(FALSE);
    }
    else {
        (*mChildProcessConnections)[slot] = NULL;
        assert(!mFreeConnectionIndices->Contains(slot));
        mFreeConnectionIndices->Add(slot);
    }
#endif
}

/** @return the count of connections managed by the allocator */
//@VisibleForTesting
Int32 ChildProcessLauncher::ChildConnectionAllocator::AllocatedConnectionsCountForTesting()
{
    assert(0);
//    return mChildProcessConnections->GetLength() - mFreeConnectionIndices->Size();
    return 0;
}

//===============================================================
//           ChildProcessLauncher::InnerDeathCallback
//===============================================================

ChildProcessLauncher::InnerDeathCallback::InnerDeathCallback()
{
}

void ChildProcessLauncher::InnerDeathCallback::OnChildProcessDied(
    /* [in] */ ChildProcessConnection* connection)
{
    if (connection->GetPid() != 0) {
        Stop(connection->GetPid());
    }
    else {
        FreeConnection(connection);
    }
}

//===============================================================
//         ChildProcessLauncher::InnerConnectionCallback
//===============================================================

ChildProcessLauncher::InnerConnectionCallback::InnerConnectionCallback(
    /* [in] */ const ChildProcessConnection* connection,
    /* [in] */ const Int64 clientContext)
    : mConnection(connection)
    , mClientContext(clientContext)
{
}

void ChildProcessLauncher::InnerConnectionCallback::OnConnected(
    /* [in] */ Int32 pid)
{
    assert(0);
#if 0
//    Log.d(TAG, "on connect callback, pid=" + pid + " context=" + clientContext);
    if (pid != NULL_PROCESS_HANDLE) {
        sBindingManager->AddNewConnection(pid, mConnection);
        sServiceMap->Put(pid, mConnection);
    }

    // If the connection fails and pid == 0, the Java-side cleanup was already
    // handled by DeathCallback. We still have to call back to native for
    // cleanup there.
    if (mClientContext != 0) {  // Will be 0 in Java instrumentation tests.
        NativeOnChildProcessStarted(mClientContext, pid);
    }
#endif
}

//===============================================================
//                    ChildProcessLauncher
//===============================================================

const String ChildProcessLauncher::TAG("ChildProcessLauncher");

const Int32 ChildProcessLauncher::CALLBACK_FOR_UNKNOWN_PROCESS;
const Int32 ChildProcessLauncher::CALLBACK_FOR_GPU_PROCESS;
const Int32 ChildProcessLauncher::CALLBACK_FOR_RENDERER_PROCESS;

const String ChildProcessLauncher::SWITCH_PROCESS_TYPE("type");
const String ChildProcessLauncher::SWITCH_PPAPI_BROKER_PROCESS("ppapi-broker");
const String ChildProcessLauncher::SWITCH_RENDERER_PROCESS("renderer");
const String ChildProcessLauncher::SWITCH_GPU_PROCESS("gpu-process");

// The upper limit on the number of simultaneous sandboxed and privileged child service process
// instances supported. Each limit must not exceed total number of SandboxedProcessServiceX
// classes and PrivilegedProcessServiceX classes declared in this package and defined as
// services in the embedding application's manifest file.
// (See {@link ChildProcessService} for more details on defining the services.)
/* package */
const Int32 ChildProcessLauncher::MAX_REGISTERED_SANDBOXED_SERVICES;
/* package */
const Int32 ChildProcessLauncher::MAX_REGISTERED_PRIVILEGED_SERVICES;

// Service class for child process. As the default value it uses SandboxedProcessService0 and
// PrivilegedProcessService0.
const AutoPtr<ChildProcessLauncher::ChildConnectionAllocator> ChildProcessLauncher::sSandboxedChildConnectionAllocator =
        new ChildConnectionAllocator(TRUE);
const AutoPtr<ChildProcessLauncher::ChildConnectionAllocator> ChildProcessLauncher::sPrivilegedChildConnectionAllocator =
        new ChildProcessLauncher::ChildConnectionAllocator(FALSE);

Boolean ChildProcessLauncher::sConnectionAllocated = FALSE;

Boolean ChildProcessLauncher::sLinkerInitialized = FALSE;
Int64 ChildProcessLauncher::sLinkerLoadAddress = 0;

// Represents an invalid process handle; same as base/process/process.h kNullProcessHandle.
const Int32 ChildProcessLauncher::NULL_PROCESS_HANDLE;

// Map from pid to ChildService connection.
//Map<Integer, ChildProcessConnection> ChildProcessLauncher::sServiceMap =
//        new ConcurrentHashMap<Integer, ChildProcessConnection>();

// A pre-allocated and pre-bound connection ready for connection setup, or null.
AutoPtr<ChildProcessConnection> ChildProcessLauncher::sSpareSandboxedConnection;

// Manages oom bindings used to bind chind services.
AutoPtr<BindingManager> ChildProcessLauncher::sBindingManager = BindingManagerImpl::CreateBindingManager();

// Map from surface id to Surface.
//Map<Integer, Surface> ChildProcessLauncher::sViewSurfaceMap =
//        new ConcurrentHashMap<Integer, Surface>();

// Map from surface texture id to Surface.
//Map<Pair<Integer, Integer>, Surface> ChildProcessLauncher::sSurfaceTextureSurfaceMap =
//        new ConcurrentHashMap<Pair<Integer, Integer>, Surface>();

/**
 * Sets service class for sandboxed service and privileged service.
 */
void ChildProcessLauncher::SetChildProcessClass(
    /* [in] */ SandboxedProcessService* sandboxedServiceClass,
    /* [in] */ PrivilegedProcessService* privilegedServiceClass)
{
    // We should guarantee this is called before allocating connection.
    assert(!sConnectionAllocated);
    sSandboxedChildConnectionAllocator->SetServiceClass(sandboxedServiceClass);
    sPrivilegedChildConnectionAllocator->SetServiceClass(privilegedServiceClass);
}

AutoPtr<ChildProcessLauncher::ChildConnectionAllocator> ChildProcessLauncher::GetConnectionAllocator(
    /* [in] */ Boolean inSandbox)
{
    return inSandbox ?
            sSandboxedChildConnectionAllocator : sPrivilegedChildConnectionAllocator;
}

AutoPtr<ChildProcessConnection> ChildProcessLauncher::AllocateConnection(
    /* [in] */ IContext* context,
    /* [in] */ Boolean inSandbox,
    /* [in] */ ChromiumLinkerParams* chromiumLinkerParams)
{
    AutoPtr<ChildProcessConnection::DeathCallback> deathCallback = new ChildProcessLauncher::InnerDeathCallback();
    sConnectionAllocated = TRUE;
    return GetConnectionAllocator(inSandbox)->Allocate(context, deathCallback,
            chromiumLinkerParams);
}

AutoPtr<ChromiumLinkerParams> ChildProcessLauncher::GetLinkerParamsForNewConnection()
{
    assert(0);
#if 0
    if (!sLinkerInitialized) {
        if (Linker::IsUsed()) {
            sLinkerLoadAddress = Linker::GetBaseLoadAddress();
            if (sLinkerLoadAddress == 0) {
//                Log.i(TAG, "Shared RELRO support disabled!");
            }
        }
        sLinkerInitialized = TRUE;
    }

    if (sLinkerLoadAddress == 0) {
        return NULL;
    }

    // Always wait for the shared RELROs in service processes.
    const Boolean waitForSharedRelros = TRUE;
    return new ChromiumLinkerParams(sLinkerLoadAddress,
                            waitForSharedRelros,
                            Linker::GetTestRunnerClassName());
#endif

    return NULL;
}

AutoPtr<ChildProcessConnection> ChildProcessLauncher::AllocateBoundConnection(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<String>* commandLine,
    /* [in] */ Boolean inSandbox)
{
    AutoPtr<ChromiumLinkerParams> chromiumLinkerParams = GetLinkerParamsForNewConnection();
    AutoPtr<ChildProcessConnection> connection =
            AllocateConnection(context, inSandbox, chromiumLinkerParams);
    if (connection != NULL) {
        connection->Start(commandLine);
    }

    return connection;
}

void ChildProcessLauncher::FreeConnection(
     /* [in] */ ChildProcessConnection* connection)
{
    GetConnectionAllocator(connection->IsInSandbox())->Free(connection);
}

//@VisibleForTesting
void ChildProcessLauncher::SetBindingManagerForTesting(
    /* [in] */ BindingManager* manager)
{
    sBindingManager = manager;
}

/** @return true iff the child process is protected from out-of-memory killing */
//@CalledByNative
Boolean ChildProcessLauncher::IsOomProtected(
    /* [in] */ Int32 pid)
{
    return sBindingManager->IsOomProtected(pid);
}

//@CalledByNative
void ChildProcessLauncher::RegisterViewSurface(
    /* [in] */ Int32 surfaceId,
    /* [in] */ ISurface* surface)
{
    assert(0);
//    sViewSurfaceMap->Put(surfaceId, surface);
}

//@CalledByNative
void ChildProcessLauncher::UnregisterViewSurface(
    /* [in] */ Int32 surfaceId)
{
    assert(0);
//    sViewSurfaceMap->Remove(surfaceId);
}

//@CalledByNative
void ChildProcessLauncher::RegisterSurfaceTexture(
    /* [in] */ Int32 surfaceTextureId,
    /* [in] */ Int32 childProcessId,
    /* [in] */ ISurfaceTexture* surfaceTexture)
{
    assert(0);
#if 0
    Pair<Integer, Integer> key = new Pair<Integer, Integer>(surfaceTextureId, childProcessId);
    AutoPtr<ISurface> sur;
    CSurface::New((ISurface**)&sur);
    sSurfaceTextureSurfaceMap->Put(key, sur);
#endif
}

//@CalledByNative
void ChildProcessLauncher::UnregisterSurfaceTexture(
    /* [in] */ Int32 surfaceTextureId,
    /* [in] */ Int32 childProcessId)
{
    assert(0);
#if 0
    Pair<Integer, Integer> key = new Pair<Integer, Integer>(surfaceTextureId, childProcessId);
    sSurfaceTextureSurfaceMap->Remove(key);
#endif
}

/**
 * Sets the visibility of the child process when it changes or when it is determined for the
 * first time.
 */
//@CalledByNative
void ChildProcessLauncher::SetInForeground(
    /* [in] */ Int32 pid,
    /* [in] */ Boolean inForeground)
{
    sBindingManager->SetInForeground(pid, inForeground);
}

/**
 * Called when the embedding application is sent to background.
 */
void ChildProcessLauncher::OnSentToBackground()
{
    sBindingManager->OnSentToBackground();
}

/**
 * Called when the embedding application is brought to foreground.
 */
void ChildProcessLauncher::OnBroughtToForeground()
{
    sBindingManager->OnBroughtToForeground();
}

/**
 * Should be called early in startup so the work needed to spawn the child process can be done
 * in parallel to other startup work. Must not be called on the UI thread. Spare connection is
 * created in sandboxed child process.
 * @param context the application context used for the connection.
 */
void ChildProcessLauncher::WarmUp(
    /* [in] */ IContext* context)
{
    assert(0);
//    Object::Autolock lock(this);
    assert(!ThreadUtils::RunningOnUiThread());
    if (sSpareSandboxedConnection == NULL) {
        sSpareSandboxedConnection = AllocateBoundConnection(context, NULL, TRUE);
    }
}

String ChildProcessLauncher::GetSwitchValue(
    /* [in] */ const ArrayOf<String>* commandLine,
    /* [in] */ const String& switchKey)
{
    if (commandLine == NULL || switchKey == NULL) {
        return String(NULL);
    }

    // This format should be matched with the one defined in command_line.h.
    String switchKeyPrefix("--");
    switchKeyPrefix += switchKey;
    switchKeyPrefix += "=";
    Int32 length = commandLine->GetLength();
    String command;
    for (Int32 i = 0; i < length; --i) {
        command = (*commandLine)[i];
        if (command != NULL && command.StartWith(switchKeyPrefix)) {
            return command.Substring(switchKeyPrefix.GetLength());
        }
    }

    return String(NULL);
}

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
void ChildProcessLauncher::Start(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<String>* commandLine,
    /* [in] */ Int32 childProcessId,
    /* [in] */ ArrayOf<Int32>* fileIds,
    /* [in] */ ArrayOf<Int32>* fileFds,
    /* [in] */ ArrayOf<Boolean>* fileAutoClose,
    /* [in] */ Int64 clientContext)
{
    TraceEvent::Begin();
    assert(fileIds->GetLength() == fileFds->GetLength() && fileFds->GetLength() == fileAutoClose->GetLength());
    AutoPtr< ArrayOf<FileDescriptorInfo*> > filesToBeMapped = ArrayOf<FileDescriptorInfo*>::Alloc(fileFds->GetLength());
    for (Int32 i = 0; i < fileFds->GetLength(); i++) {
        (*filesToBeMapped)[i] =
                new FileDescriptorInfo((*fileIds)[i], (*fileFds)[i], (*fileAutoClose)[i]);
    }

    assert(clientContext != 0);

    Int32 callbackType = CALLBACK_FOR_UNKNOWN_PROCESS;
    Boolean inSandbox = TRUE;
    String processType = GetSwitchValue(commandLine, SWITCH_PROCESS_TYPE);
    if (SWITCH_RENDERER_PROCESS.Equals(processType)) {
        callbackType = CALLBACK_FOR_RENDERER_PROCESS;
    }
    else if (SWITCH_GPU_PROCESS.Equals(processType)) {
        callbackType = CALLBACK_FOR_GPU_PROCESS;
    }
    else if (SWITCH_PPAPI_BROKER_PROCESS.Equals(processType)) {
        inSandbox = FALSE;
    }

    AutoPtr<ChildProcessConnection> allocatedConnection;
    {
        assert(0);
//        Object::Autolock lock(this);
        if (inSandbox) {
            allocatedConnection = sSpareSandboxedConnection;
            sSpareSandboxedConnection = NULL;
        }
    }

    if (allocatedConnection == NULL) {
        assert(0);
//        allocatedConnection = AllocateBoundConnection(context, commandLine, inSandbox);
        if (allocatedConnection == NULL) {
            // Notify the native code so it can free the heap allocated callback.
            NativeOnChildProcessStarted(clientContext, 0);
//            Log.e(TAG, "Allocation of new service failed.");
            TraceEvent::End();
            return;
        }
    }

//    Log.d(TAG, "Setting up connection to process: slot=" +
//            allocatedConnection.getServiceNumber());
    assert(0);
    TriggerConnectionSetup(allocatedConnection, commandLine, childProcessId, filesToBeMapped,
            callbackType, clientContext);
    TraceEvent::End();
}

//@VisibleForTesting
void ChildProcessLauncher::TriggerConnectionSetup(
    /* [in] */ const ChildProcessConnection* connection,
    /* [in] */ ArrayOf<String>* commandLine,
    /* [in] */ Int32 childProcessId,
    /* [in] */ ArrayOf<FileDescriptorInfo*>* filesToBeMapped,
    /* [in] */ Int32 callbackType,
    /* [in] */ const Int64 clientContext)
{
    AutoPtr<ChildProcessConnection::ConnectionCallback> connectionCallback = new InnerConnectionCallback(connection, clientContext);

    // TODO(sievers): Revisit this as it doesn't correctly handle the utility process
    // assert callbackType != CALLBACK_FOR_UNKNOWN_PROCESS;

    assert(0);
    // connection->SetupConnection(commandLine,
    //                            filesToBeMapped,
    //                            CreateCallback(childProcessId, callbackType),
    //                            connectionCallback,
    //                            Linker::GetSharedRelros());
}

/**
 * Terminates a child process. This may be called from any thread.
 *
 * @param pid The pid (process handle) of the service connection obtained from {@link #start}.
 */
//@CalledByNative
void ChildProcessLauncher::Stop(
    /* [in] */ Int32 pid)
{
    assert(0);
#if 0
//    Log.d(TAG, "stopping child connection: pid=" + pid);
    AutoPtr<ChildProcessConnection> connection = sServiceMap->Remove(pid);
    if (connection == NULL) {
        LogPidWarning(pid, "Tried to stop non-existent connection");
        return;
    }
    sBindingManager->ClearConnection(pid);
    connection->Stop();
    FreeConnection(connection);
#endif
}

/**
 * This implementation is used to receive callbacks from the remote service.
 */
// AutoPtr<IChildProcessCallback> ChildProcessLauncher::CreateCallback(
//     /* [in] */ const Int32 childProcessId,
//     /* [in] */ const Int32 callbackType)
// {
//     return new IChildProcessCallback.Stub() {
//         /**
//          * This is called by the remote service regularly to tell us about new values. Note that
//          * IPC calls are dispatched through a thread pool running in each process, so the code
//          * executing here will NOT be running in our main thread -- so, to update the UI, we
//          * need to use a Handler.
//          */
//         @Override
//         public void establishSurfacePeer(
//                 int pid, Surface surface, int primaryID, int secondaryID) {
//             // Do not allow a malicious renderer to connect to a producer. This is only used
//             // from stream textures managed by the GPU process.
//             if (callbackType != CALLBACK_FOR_GPU_PROCESS) {
//                 Log.e(TAG, "Illegal callback for non-GPU process.");
//                 return;
//             }

//             nativeEstablishSurfacePeer(pid, surface, primaryID, secondaryID);
//         }

//         @Override
//         public SurfaceWrapper getViewSurface(int surfaceId) {
//             // Do not allow a malicious renderer to get to our view surface.
//             if (callbackType != CALLBACK_FOR_GPU_PROCESS) {
//                 Log.e(TAG, "Illegal callback for non-GPU process.");
//                 return null;
//             }

//             Surface surface = sViewSurfaceMap.get(surfaceId);
//             if (surface == null) {
//                 Log.e(TAG, "Invalid surfaceId.");
//                 return null;
//             }
//             assert surface.isValid();
//             return new SurfaceWrapper(surface);
//         }

//         @Override
//         public SurfaceWrapper getSurfaceTextureSurface(int primaryId, int secondaryId) {
//             if (callbackType != CALLBACK_FOR_RENDERER_PROCESS) {
//                 Log.e(TAG, "Illegal callback for non-renderer process.");
//                 return null;
//             }

//             if (secondaryId != childProcessId) {
//                 Log.e(TAG, "Illegal secondaryId for renderer process.");
//                 return null;
//             }

//             Pair<Integer, Integer> key = new Pair<Integer, Integer>(primaryId, secondaryId);
//             // Note: This removes the surface and passes the ownership to the caller.
//             Surface surface = sSurfaceTextureSurfaceMap.remove(key);
//             if (surface == null) {
//                 Log.e(TAG, "Invalid Id for surface texture.");
//                 return null;
//             }
//             assert surface.isValid();
//             return new SurfaceWrapper(surface);
//         }
//     };
// }

void ChildProcessLauncher::LogPidWarning(
    /* [in] */ Int32 pid,
    /* [in] */ const String& message)
{
    // This class is effectively a no-op in single process mode, so don't log warnings there.
    if (pid > 0 && !NativeIsSingleProcess()) {
//        Log.w(TAG, message + ", pid=" + pid);
    }
}

//@VisibleForTesting
AutoPtr<ChildProcessConnection> ChildProcessLauncher::AllocateBoundConnectionForTesting(
    /* [in] */ IContext* context)
{
    return AllocateBoundConnection(context, NULL, TRUE);
}

/** @return the count of sandboxed connections managed by the allocator */
//@VisibleForTesting
Int32 ChildProcessLauncher::AllocatedConnectionsCountForTesting()
{
    return sSandboxedChildConnectionAllocator->AllocatedConnectionsCountForTesting();
}

/** @return the count of services set up and working */
//@VisibleForTesting
Int32 ChildProcessLauncher::ConnectedServicesCountForTesting()
{
    assert(0);
//    return sServiceMap.size();
    return 0;
}

void ChildProcessLauncher::NativeOnChildProcessStarted(
    /* [in] */ Int64 clientContext,
    /* [in] */ Int32 pid)
{
    Elastos_ChildProcessLauncher_nativeOnChildProcessStarted(clientContext, pid);
}

void ChildProcessLauncher::NativeEstablishSurfacePeer(
    /* [in] */ Int32 pid,
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 primaryID,
    /* [in] */ Int32 secondaryID)
{
    Elastos_ChildProcessLauncher_nativeEstablishSurfacePeer(pid, TO_IINTERFACE(surface), primaryID, secondaryID);
}

Boolean ChildProcessLauncher::NativeIsSingleProcess()
{
    return Elastos_ChildProcessLauncher_nativeIsSingleProcess();
}

void ChildProcessLauncher::RegisterViewSurface(
    /* [in] */ Int32 surfaceId,
    /* [in] */ IInterface* surface)
{
    AutoPtr<ISurface> s = ISurface::Probe(surface);
    RegisterViewSurface(surfaceId, s);
}

void ChildProcessLauncher::RegisterSurfaceTexture(
    /* [in] */ Int32 surfaceTextureId,
    /* [in] */ Int32 childProcessId,
    /* [in] */ IInterface* surfaceTexture)
{
    AutoPtr<ISurfaceTexture> st = ISurfaceTexture::Probe(surfaceTexture);
    RegisterSurfaceTexture(surfaceTextureId, childProcessId, st);
}

void ChildProcessLauncher::Start(
    /* [in] */ IInterface* context,
    /* [in] */ ArrayOf<String>* commandLine,
    /* [in] */ Int32 childProcessId,
    /* [in] */ ArrayOf<Int32>* fileIds,
    /* [in] */ ArrayOf<Int32>* fileFds,
    /* [in] */ ArrayOf<Boolean>* fileAutoClose,
    /* [in] */ Int64 clientContext)
{
    AutoPtr<IContext> c = IContext::Probe(context);
    Start(c, commandLine, childProcessId, fileIds, fileFds, fileAutoClose, clientContext);
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

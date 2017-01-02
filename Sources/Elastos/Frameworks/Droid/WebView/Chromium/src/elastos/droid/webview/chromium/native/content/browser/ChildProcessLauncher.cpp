//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ChildProcessLauncher.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/ChildProcessLauncher_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/BindingManagerImpl.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ChildProcessConnectionImpl.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/base/TraceEvent.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::CSurface;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Base::TraceEvent;
using Elastos::Core::CoreUtils;
using Elastos::Core::AutoLock;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Droid::Utility::CPair;
using Elastos::Droid::Utility::IPair;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//===============================================================
//        ChildProcessLauncher::ChildConnectionAllocator
//===============================================================

ChildProcessLauncher::ChildConnectionAllocator::ChildConnectionAllocator(
    /* [in] */ Boolean inSandbox)
    : mInSandbox(inSandbox)
{
    Int32 numChildServices = inSandbox ?
            MAX_REGISTERED_SANDBOXED_SERVICES : MAX_REGISTERED_PRIVILEGED_SERVICES;
    mChildProcessConnections = ArrayOf<ChildProcessConnection*>::Alloc(numChildServices);
    //mFreeConnectionIndices = new ArrayList<Integer>(numChildServices);
    CArrayList::New((IList**)&mFreeConnectionIndices);
    for (Int32 i = 0; i < numChildServices; i++) {
        AutoPtr<IInteger32> iint = CoreUtils::Convert(i);
         mFreeConnectionIndices->Add(TO_IINTERFACE(iint));
    }
    //maybe here need to pass the class EID(car class must).
    //TODO SetServiceClass(inSandbox ? SandboxedProcessService.class : PrivilegedProcessService.class);
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
    AutoLock lock(mConnectionLock);

    Boolean isEmpty = FALSE;
    if (mFreeConnectionIndices->IsEmpty(&isEmpty), isEmpty) {
        Logger::W(TAG, "Ran out of service.");
        return NULL;
    }

    AutoPtr<IInterface> obj;
    mFreeConnectionIndices->Remove(0, (IInterface**)&obj);
    AutoPtr<IInteger32> iint = IInteger32::Probe(obj);
    Int32 slot;
    iint->GetValue(&slot);
    //assert((*mChildProcessConnections)[slot] == NULL);
    (*mChildProcessConnections)[slot] = new ChildProcessConnectionImpl(context, slot,
            mInSandbox, deathCallback, mChildClass, chromiumLinkerParams);
    return (*mChildProcessConnections)[slot];
}

void ChildProcessLauncher::ChildConnectionAllocator::Free(
    /* [in] */ ChildProcessConnection* connection)
{
    AutoLock lock(mConnectionLock);

    Int32 slot = connection->GetServiceNumber();
    if ((*mChildProcessConnections)[slot] != connection) {
        Int32 occupier = (*mChildProcessConnections)[slot] == NULL ?
                -1 : (*mChildProcessConnections)[slot]->GetServiceNumber();
        Logger::E(TAG, "Unable to find connection to free in slot: %d already occupied by service: %d", slot, occupier);
        assert(FALSE);
    }
    else {
        (*mChildProcessConnections)[slot] = NULL;
        //assert(!mFreeConnectionIndices->Contains(slot));
        AutoPtr<IInteger32> iint = CoreUtils::Convert(slot);
        mFreeConnectionIndices->Add(TO_IINTERFACE(iint));
    }
}

/** @return the count of connections managed by the allocator */
//@VisibleForTesting
Int32 ChildProcessLauncher::ChildConnectionAllocator::AllocatedConnectionsCountForTesting()
{
    Int32 size;
    mFreeConnectionIndices->GetSize(&size);
    return mChildProcessConnections->GetLength() - size;
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
    /* [in] */ ChildProcessConnection* connection,
    /* [in] */ Int64 clientContext)
    : mConnection(connection)
    , mClientContext(clientContext)
{
}

void ChildProcessLauncher::InnerConnectionCallback::OnConnected(
    /* [in] */ Int32 pid)
{
    Logger::D(TAG, "on connect callback, pid=%d context=%d", pid, mClientContext);
    if (pid != NULL_PROCESS_HANDLE) {
        sBindingManager->AddNewConnection(pid, mConnection);
        AutoPtr<IInteger32> iint = CoreUtils::Convert(pid);
        sServiceMap->Put(TO_IINTERFACE(iint), TO_IINTERFACE(mConnection));
    }

    // If the connection fails and pid == 0, the Java-side cleanup was already
    // handled by DeathCallback. We still have to call back to native for
    // cleanup there.
    if (mClientContext != 0) {  // Will be 0 in Java instrumentation tests.
        NativeOnChildProcessStarted(mClientContext, pid);
    }
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
// TODO:
const AutoPtr<ChildProcessLauncher::ChildConnectionAllocator> ChildProcessLauncher::sSandboxedChildConnectionAllocator =
        new ChildProcessLauncher::ChildConnectionAllocator(TRUE);
const AutoPtr<ChildProcessLauncher::ChildConnectionAllocator> ChildProcessLauncher::sPrivilegedChildConnectionAllocator =
        new ChildProcessLauncher::ChildConnectionAllocator(FALSE);

Boolean ChildProcessLauncher::sConnectionAllocated = FALSE;

Boolean ChildProcessLauncher::sLinkerInitialized = FALSE;
Int64 ChildProcessLauncher::sLinkerLoadAddress = 0;

// Represents an invalid process handle; same as base/process/process.h kNullProcessHandle.
const Int32 ChildProcessLauncher::NULL_PROCESS_HANDLE;

// Map from pid to ChildService connection.
static AutoPtr<IMap> InitServiceMap()
{
    AutoPtr<IMap> map;
    CConcurrentHashMap::New((IMap**)&map);
    return map;
}
AutoPtr<IMap> ChildProcessLauncher::sServiceMap = InitServiceMap();// <Integer, ChildProcessConnection>

// A pre-allocated and pre-bound connection ready for connection setup, or null.
AutoPtr<ChildProcessConnection> ChildProcessLauncher::sSpareSandboxedConnection;

// Manages oom bindings used to bind chind services.
// TODO
AutoPtr<BindingManager> ChildProcessLauncher::sBindingManager = BindingManagerImpl::CreateBindingManager();

// Map from surface id to Surface.
static AutoPtr<IMap> InitViewSurfaceMap()
{
    AutoPtr<IMap> map;
    CConcurrentHashMap::New((IMap**)&map);
    return map;
}
AutoPtr<IMap> ChildProcessLauncher::sViewSurfaceMap = InitViewSurfaceMap();//<Integer, Surface>

// Map from surface texture id to Surface.
static AutoPtr<IMap> InitSurfaceTextureSurfaceMap()
{
    AutoPtr<IMap> map;
    CConcurrentHashMap::New((IMap**)&map);
    return map;
}
AutoPtr<IMap> ChildProcessLauncher::sSurfaceTextureSurfaceMap = InitSurfaceTextureSurfaceMap();// <<Pair<Integer, Integer>, Surface>

Object ChildProcessLauncher::sLock;

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
    return inSandbox ? sSandboxedChildConnectionAllocator : sPrivilegedChildConnectionAllocator;
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
    if (!sLinkerInitialized) {
        if (FALSE/*Linker::IsUsed()*/) {//never use Linker in Android, see NativeLibraries.java::USE_LINKER
//            sLinkerLoadAddress = Linker::GetBaseLoadAddress();
//            if (sLinkerLoadAddress == 0) {
//                Log.i(TAG, "Shared RELRO support disabled!");
//            }
        }
        sLinkerInitialized = TRUE;
    }

    if (sLinkerLoadAddress == 0) {
        return NULL;
    }
    return NULL;
    //in android, will not execute here.

//    // Always wait for the shared RELROs in service processes.
//    const Boolean waitForSharedRelros = TRUE;
//    return new ChromiumLinkerParams(sLinkerLoadAddress,
//                            waitForSharedRelros,
//                            Linker::GetTestRunnerClassName());
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
    AutoPtr<IInteger32> iint = CoreUtils::Convert(surfaceId);
    sViewSurfaceMap->Put(TO_IINTERFACE(iint), TO_IINTERFACE(surface));
}

//@CalledByNative
void ChildProcessLauncher::UnregisterViewSurface(
    /* [in] */ Int32 surfaceId)
{
    AutoPtr<IInteger32> iint = CoreUtils::Convert(surfaceId);
    sViewSurfaceMap->Remove(TO_IINTERFACE(iint));
}

//@CalledByNative
void ChildProcessLauncher::RegisterSurfaceTexture(
    /* [in] */ Int32 surfaceTextureId,
    /* [in] */ Int32 childProcessId,
    /* [in] */ ISurfaceTexture* surfaceTexture)
{
    //Pair<Integer, Integer> key = new Pair<Integer, Integer>(surfaceTextureId, childProcessId);
    AutoPtr<IInteger32> istid = CoreUtils::Convert(surfaceTextureId);
    AutoPtr<IInteger32> cpid = CoreUtils::Convert(childProcessId);
    AutoPtr<IPair> key;
    CPair::New(TO_IINTERFACE(istid), TO_IINTERFACE(cpid), (IPair**)&key);
    AutoPtr<ISurface> sur;
    CSurface::New((ISurface**)&sur);
    sSurfaceTextureSurfaceMap->Put(TO_IINTERFACE(key), TO_IINTERFACE(sur));
}

//@CalledByNative
void ChildProcessLauncher::UnregisterSurfaceTexture(
    /* [in] */ Int32 surfaceTextureId,
    /* [in] */ Int32 childProcessId)
{
    AutoPtr<IInteger32> istid = CoreUtils::Convert(surfaceTextureId);
    AutoPtr<IInteger32> cpid = CoreUtils::Convert(childProcessId);
    AutoPtr<IPair> key;
    CPair::New(TO_IINTERFACE(istid), TO_IINTERFACE(cpid), (IPair**)&key);
    //Pair<Integer, Integer> key = new Pair<Integer, Integer>(surfaceTextureId, childProcessId);
    sSurfaceTextureSurfaceMap->Remove(TO_IINTERFACE(key));
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
    AutoLock lock(sLock);
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
    } else if (SWITCH_GPU_PROCESS.Equals(processType)) {
        callbackType = CALLBACK_FOR_GPU_PROCESS;
    } else if (SWITCH_PPAPI_BROKER_PROCESS.Equals(processType)) {
        inSandbox = FALSE;
    }

    AutoPtr<ChildProcessConnection> allocatedConnection;
    {
        AutoLock lock(sLock);
        if (inSandbox) {
            allocatedConnection = sSpareSandboxedConnection;
            sSpareSandboxedConnection = NULL;
        }
    }

    if (allocatedConnection == NULL) {
        allocatedConnection = AllocateBoundConnection(context, commandLine, inSandbox);
        if (allocatedConnection == NULL) {
            // Notify the native code so it can free the heap allocated callback.
            NativeOnChildProcessStarted(clientContext, 0);
            Logger::E(TAG, "Allocation of new service failed.");
            TraceEvent::End();
            return;
        }
    }

    Logger::D(TAG, "Setting up connection to process: slot=%d", allocatedConnection->GetServiceNumber());
    TriggerConnectionSetup(allocatedConnection, commandLine, childProcessId, filesToBeMapped,
            callbackType, clientContext);
    TraceEvent::End();
}

//@VisibleForTesting
void ChildProcessLauncher::TriggerConnectionSetup(
    /* [in] */ ChildProcessConnection* connection,
    /* [in] */ ArrayOf<String>* commandLine,
    /* [in] */ Int32 childProcessId,
    /* [in] */ ArrayOf<FileDescriptorInfo*>* filesToBeMapped,
    /* [in] */ Int32 callbackType,
    /* [in] */ const Int64 clientContext)
{
    AutoPtr<ChildProcessConnection::ConnectionCallback> connectionCallback = new InnerConnectionCallback(connection, clientContext);

    // TODO(sievers): Revisit this as it doesn't correctly handle the utility process
    // assert callbackType != CALLBACK_FOR_UNKNOWN_PROCESS;

    connection->SetupConnection(commandLine,
                               filesToBeMapped,
                               CreateCallback(childProcessId, callbackType),
                               connectionCallback,
                               NULL/*TODO Linker::GetSharedRelros()*/);//NULL because not used multi-process architecture
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
    Logger::D(TAG, "stopping child connection: pid=%d", pid);
    AutoPtr<IInteger32> iint = CoreUtils::Convert(pid);
    AutoPtr<IInterface> obj;
    sServiceMap->Remove(TO_IINTERFACE(iint), (IInterface**)&obj);
    AutoPtr<ChildProcessConnection> connection = (ChildProcessConnection*)(IObject::Probe(obj));
    if (connection == NULL) {
        LogPidWarning(pid, String("Tried to stop non-existent connection"));
        return;
    }
    sBindingManager->ClearConnection(pid);
    connection->Stop();
    FreeConnection(connection);
}

/**
 * This implementation is used to receive callbacks from the remote service.
 */
 AutoPtr<IInterface/*TODO IChildProcessCallback*/> ChildProcessLauncher::CreateCallback(
     /* [in] */ const Int32 childProcessId,
     /* [in] */ const Int32 callbackType)
 {
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
     return NULL;
}

void ChildProcessLauncher::LogPidWarning(
    /* [in] */ Int32 pid,
    /* [in] */ const String& message)
{
    // This class is effectively a no-op in single process mode, so don't log warnings there.
    if (pid > 0 && !NativeIsSingleProcess()) {
        Logger::W(TAG, message.string());
        Logger::W(TAG, "pid %d", pid);
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
    Int32 size;
    sServiceMap->GetSize(&size);
    return size;
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
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

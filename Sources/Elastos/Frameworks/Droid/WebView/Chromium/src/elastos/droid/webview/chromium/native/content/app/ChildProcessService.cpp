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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/content/app/ChildProcessService.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/ChildProcessService_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ChildProcessConnection.h"
#include "elastos/droid/os/Process.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Droid::Graphics::EIID_ISurfaceTexture;
using Elastos::Droid::Os::Process;
using Elastos::Droid::View::EIID_ISurface;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ChildProcessConnection;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace App {

//=====================================================================
//                ChildProcessService::InnerRunnable
//=====================================================================

ChildProcessService::InnerRunnable::InnerRunnable(
    /* [in] */ ChildProcessService* owner)
    : mOwner(owner)
{
}

ECode ChildProcessService::InnerRunnable::Run()
{
    // try {
        assert(0);

        // TODO
        // Boolean useLinker = Linker.isUsed();

        // if (useLinker) {
        //     {    AutoLock syncLock(mMainThread);
        //         while (!mIsBound) {
        //             mMainThread.wait();
        //         }
        //     }
        //     if (mLinkerParams != null) {
        //         if (mLinkerParams.mWaitForSharedRelro)
        //             Linker.initServiceProcess(mLinkerParams.mBaseLoadAddress);
        //         else
        //             Linker.disableSharedRelros();

        //         Linker.setTestRunnerClassName(mLinkerParams.mTestRunnerClassName);
        //     }
        // }
        // try {
        //     LibraryLoader.loadNow(getApplicationContext(), false);
        // } catch (ProcessInitException e) {
        //     Log.e(TAG, "Failed to load native library, exiting child process", e);
        //     System.exit(-1);
        // }
        // {    AutoLock syncLock(mMainThread);
        //     while (mCommandLineParams == null) {
        //         mMainThread.wait();
        //     }
        // }
        // LibraryLoader.initialize(mCommandLineParams);
        // {    AutoLock syncLock(mMainThread);
        //     mLibraryInitialized = true;
        //     mMainThread.notifyAll();
        //     while (mFileIds == null) {
        //         mMainThread.wait();
        //     }
        // }
        // assert mFileIds.size() == mFileFds.size();
        // int[] fileIds = new int[mFileIds.size()];
        // int[] fileFds = new int[mFileFds.size()];
        // for (int i = 0; i < mFileIds.size(); ++i) {
        //     fileIds[i] = mFileIds.get(i);
        //     fileFds[i] = mFileFds.get(i).detachFd();
        // }
        // ContentMain.initApplicationContext(sContext.get().getApplicationContext());
        // NativeInitChildProcess(sContext.get().getApplicationContext(),
        //         ChildProcessService.this, fileIds, fileFds,
        //         mCpuCount, mCpuFeatures);
        // ContentMain.start();
        // NativeExitChildProcess();
    // } catch (InterruptedException e) {
    //     Log.w(TAG, MAIN_THREAD_NAME + " startup failed: " + e);
    // } catch (ProcessInitException e) {
    //     Log.w(TAG, MAIN_THREAD_NAME + " startup failed: " + e);
    // }

    return E_NOT_IMPLEMENTED;
}

//=====================================================================
//                        ChildProcessService
//=====================================================================

const String ChildProcessService::MAIN_THREAD_NAME("ChildProcessMain");
const String ChildProcessService::TAG("ChildProcessService");
// TODO AtomicReference<Context> ChildProcessService::sContext = new AtomicReference<Context>(null);

ChildProcessService::ChildProcessService()
    : mCpuCount(0)
    , mCpuFeatures(0)
    , mLibraryInitialized(FALSE)
    , mIsBound(FALSE)
{
    assert(0);
    // TODO
    // // Binder object used by clients for this service.
    // private final IChildProcessService.Stub mBinder = new IChildProcessService.Stub() {
    //     // NOTE: Implement any IChildProcessService methods here.
    //     @Override
    //     public int setupConnection(Bundle args, IChildProcessCallback callback) {
    //         mCallback = callback;
    //         {    AutoLock syncLock(mMainThread);
    //             // Allow the command line to be set via bind() intent or setupConnection, but
    //             // the FD can only be transferred here.
    //             if (mCommandLineParams == null) {
    //                 mCommandLineParams = args.getStringArray(
    //                         ChildProcessConnection.EXTRA_COMMAND_LINE);
    //             }
    //             // We must have received the command line by now
    //             assert mCommandLineParams != null;
    //             mCpuCount = args.getInt(ChildProcessConnection.EXTRA_CPU_COUNT);
    //             mCpuFeatures = args.getLong(ChildProcessConnection.EXTRA_CPU_FEATURES);
    //             assert mCpuCount > 0;
    //             mFileIds = new ArrayList<Integer>();
    //             mFileFds = new ArrayList<ParcelFileDescriptor>();
    //             for (int i = 0;; i++) {
    //                 String fdName = ChildProcessConnection.EXTRA_FILES_PREFIX + i
    //                         + ChildProcessConnection.EXTRA_FILES_FD_SUFFIX;
    //                 ParcelFileDescriptor parcel = args.getParcelable(fdName);
    //                 if (parcel == null) {
    //                     // End of the file list.
    //                     break;
    //                 }
    //                 mFileFds.add(parcel);
    //                 String idName = ChildProcessConnection.EXTRA_FILES_PREFIX + i
    //                         + ChildProcessConnection.EXTRA_FILES_ID_SUFFIX;
    //                 mFileIds.add(args.getInt(idName));
    //             }
    //             Bundle sharedRelros = args.getBundle(Linker.EXTRA_LINKER_SHARED_RELROS);
    //             if (sharedRelros != null) {
    //                 Linker.useSharedRelros(sharedRelros);
    //                 sharedRelros = null;
    //             }
    //             mMainThread.notifyAll();
    //         }
    //         return Process.myPid();
    //     }

    //     @Override
    //     public void crashIntentionallyForTesting() {
    //         Process.killProcess(Process.myPid());
    //     }
    // };
}

AutoPtr<IContext> ChildProcessService::GetContext()
{
    assert(0);
    // TODO
    // return sContext.get();
    return NULL;
}

//@Override
ECode ChildProcessService::OnCreate()
{
    String logStr("Creating new ChildProcessService pid=");
    logStr += StringUtils::ToString(Process::MyPid());
    Slogger::I(TAG, logStr);
    assert(0);
    // TODO
    // if (sContext.get() != NULL) {
    //     Slogger::E(TAG, "ChildProcessService created again in process!");
    // }
    // sContext.set(this);
    // super.onCreate();

    AutoPtr<IRunnable> runnable = new InnerRunnable(this);

    assert(0);
    // TODO
    // CThread::New(runnable, MAIN_THREAD_NAME, (IThread**)&mMainThread);
    mMainThread->Start();

    return E_NOT_IMPLEMENTED;
}

//@Override
ECode ChildProcessService::OnDestroy()
{
    String logStr("Destroying ChildProcessService pid=");
    logStr += StringUtils::ToString(Process::MyPid());
    Slogger::I(TAG, logStr);
    assert(0);
    // TODO
    // super.onDestroy();
    if (mCommandLineParams == NULL) {
        // This process was destroyed before it even started. Nothing more to do.
        return NOERROR;
    }

    //{    AutoLock syncLock(mMainThread);
        AutoLock lock(mMainThreadLock);
        // try {
            while (!mLibraryInitialized) {
                // Avoid a potential race in calling through to native code before the library
                // has loaded.
                assert(0);
                // TODO
                // mMainThread->Wait();
            }
        // } catch (InterruptedException e) {
        //     // Ignore
        // }
    }
    // Try to shutdown the MainThread gracefully, but it might not
    // have chance to exit normally.
    NativeShutdownMainThread();

    return E_NOT_IMPLEMENTED;
}

//@Override
ECode ChildProcessService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);

    // We call stopSelf() to request that this service be stopped as soon as the client
    // unbinds. Otherwise the system may keep it around and available for a reconnect. The
    // child processes do not currently support reconnect; they must be initialized from
    // scratch every time.
    assert(0);
    // TODO
    // StopSelf();

    //{    AutoLock syncLock(mMainThread);
        AutoLock lock(mMainThreadLock);
        intent->GetStringArrayExtra(
                ChildProcessConnection::EXTRA_COMMAND_LINE,
                (ArrayOf<String>**)&mCommandLineParams);
        mLinkerParams = NULL;
        assert(0);
        // TODO
        // if (Linker.isUsed())
        //     mLinkerParams = new ChromiumLinkerParams(intent);
        mIsBound = TRUE;
        assert(0);
        // TODO
        // mMainThread->NotifyAll();
    }

    assert(0);
    // TODO
    // *binder = mBinder;
    REFCOUNT_ADD(*binder);

    return E_NOT_IMPLEMENTED;
}

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
void ChildProcessService::EstablishSurfaceTexturePeer(
    /* [in] */ Int32 pid,
    /* [in] */ IInterface* surfaceObject,
    /* [in] */ Int32 primaryID,
    /* [in] */ Int32 secondaryID)
{
    assert(0);
    // TODO
    // if (mCallback == NULL) {
    //     Slogger::E(TAG, "No callback interface has been provided.");
    //     return;
    // }

    AutoPtr<ISurface> surface;
    Boolean needRelease = FALSE;
    if (surfaceObject->Probe(EIID_ISurface)) {
        surface = ISurface::Probe(surfaceObject);
    }
    else if (surfaceObject->Probe(EIID_ISurfaceTexture)) {
        AutoPtr<ISurfaceTexture> surfaceTexture = ISurfaceTexture::Probe(surfaceObject);
        assert(0);
        // TODO
        // CSurface::New((SurfaceTexture) surfaceObject, (ISurface**)&surface);
        needRelease = TRUE;
    }
    else {
        String str;
        assert(0);
        // TODO
        // surfaceObject.ToString(&str);
        String logStr("Not a valid surfaceObject: ");
        logStr += str;
        Slogger::E(TAG, logStr);
        return;
    }

    // try {
        assert(0);
        // TODO
        // mCallback->EstablishSurfacePeer(pid, surface, primaryID, secondaryID);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "Unable to call establishSurfaceTexturePeer: " + e);
    //     return;
    // } finally {
        if (needRelease) {
            surface->Release();
        }
    // }

}

//@SuppressWarnings("unused")
//@CalledByNative
AutoPtr<ISurface> ChildProcessService::GetViewSurface(
    /* [in] */ Int32 surfaceId)
{
    assert(0);
    // TODO
    // if (mCallback == NULL) {
    //     Slogger::E(TAG, "No callback interface has been provided.");
    //     return NULL;
    // }

    // // try {
    //     return mCallback->GetViewSurface(surfaceId)->GetSurface();
    // // } catch (RemoteException e) {
    // //     Log.e(TAG, "Unable to call establishSurfaceTexturePeer: " + e);
    // //     return null;
    // // }

    return NULL;
}

//@SuppressWarnings("unused")
//@CalledByNative
AutoPtr<ISurface> ChildProcessService::GetSurfaceTextureSurface(
    /* [in] */ Int32 primaryId,
    /* [in] */ Int32 secondaryId)
{
    assert(0);
    // TODO
    // if (mCallback == NULL) {
    //     Slogger::E(TAG, "No callback interface has been provided.");
    //     return NULL;
    // }

    // // try {
    //     return mCallback->GetSurfaceTextureSurface(primaryId, secondaryId)->GetSurface();
    // // } catch (RemoteException e) {
    // //     Log.e(TAG, "Unable to call getSurfaceTextureSurface: " + e);
    // //     return null;
    // // }

    return NULL;
}

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
void ChildProcessService::NativeInitChildProcess(
    /* [in] */ IContext* applicationContext,
    /* [in] */ ChildProcessService* service,
    /* [in] */ ArrayOf<Int32>* extraFileIds,
    /* [in] */ ArrayOf<Int32>* extraFileFds,
    /* [in] */ Int32 cpuCount,
    /* [in] */ Int64 cpuFeatures)
{
    Elastos_ChildProcessService_nativeInitChildProcess(
            TO_IINTERFACE(applicationContext), TO_IINTERFACE(service), extraFileIds, extraFileFds, cpuCount, cpuFeatures);
}

/**
 * Force the child process to exit.
 */
void ChildProcessService::NativeExitChildProcess()
{
    Elastos_ChildProcessService_nativeExitChildProcess();
}

void ChildProcessService::NativeShutdownMainThread()
{
    Elastos_ChildProcessService_nativeShutdownMainThread(TO_IINTERFACE(this));
}

void ChildProcessService::EstablishSurfaceTexturePeer(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 pid,
    /* [in] */ IInterface* surfaceObject,
    /* [in] */ Int32 primaryID,
    /* [in] */ Int32 secondaryID)
{
    AutoPtr<ChildProcessService> mObj = (ChildProcessService*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ChildProcessService::EstablishSurfaceTexturePeer, mObj is NULL");
        return;
    }
    mObj->EstablishSurfaceTexturePeer(pid, surfaceObject, primaryID, secondaryID);
}

AutoPtr<IInterface> ChildProcessService::GetViewSurface(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 surfaceId)
{
    AutoPtr<ChildProcessService> mObj = (ChildProcessService*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ChildProcessService::GetViewSurface, mObj is NULL");
        return NULL;
    }
    return TO_IINTERFACE(mObj->GetViewSurface(surfaceId));
}

AutoPtr<IInterface> ChildProcessService::GetSurfaceTextureSurface(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 primaryId,
    /* [in] */ Int32 secondaryId)
{
    AutoPtr<ChildProcessService> mObj = (ChildProcessService*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ChildProcessService::GetSurfaceTextureSurface, mObj is NULL");
        return NULL;
    }
    return TO_IINTERFACE(mObj->GetSurfaceTextureSurface(primaryId, secondaryId));
}

} // namespace App
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

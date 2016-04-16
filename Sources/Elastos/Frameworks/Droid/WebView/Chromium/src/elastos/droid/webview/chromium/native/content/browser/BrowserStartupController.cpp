
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/BrowserStartupController.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/BrowserStartupController_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ResourceExtractor.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/DeviceUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/PepperPluginManager.h"
#include "elastos/droid/webkit/webview/chromium/native/content/app/ContentMain.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Content::App::ContentMain;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//==========================================================================
//          BrowserStartupController::PostStartupCompletedRunnable
//==========================================================================

BrowserStartupController::PostStartupCompletedRunnable::PostStartupCompletedRunnable(
    /* [in] */ BrowserStartupController* owner,
    /* [in] */ StartupCallback* callback)
    : mOwner(owner)
    , mCallback(callback)
{
}

CAR_INTERFACE_IMPL(BrowserStartupController::PostStartupCompletedRunnable, Runnable, IRunnable);

ECode BrowserStartupController::PostStartupCompletedRunnable::Run()
{
    if (mOwner->mStartupSuccess) {
        mCallback->OnSuccess(ALREADY_STARTED);
    }
    else {
        mCallback->OnFailure();
    }

    return NOERROR;
}

//==========================================================================
//        BrowserStartupController::EnqueueCallbackExecutionRunnable
//==========================================================================

BrowserStartupController::EnqueueCallbackExecutionRunnable::EnqueueCallbackExecutionRunnable(
    /* [in] */ BrowserStartupController* owner,
    /* [in] */ const Int32 startupFailure,
    /* [in] */ const Boolean alreadyStarted)
    : mOwner(owner)
    , mStartupFailure(startupFailure)
    , mAlreadyStarted(alreadyStarted)
{
}

CAR_INTERFACE_IMPL(BrowserStartupController::EnqueueCallbackExecutionRunnable, Runnable, IRunnable);

ECode BrowserStartupController::EnqueueCallbackExecutionRunnable::Run()
{
    mOwner->ExecuteEnqueuedCallbacks(mStartupFailure, mAlreadyStarted);
    return NOERROR;
}

//===============================================================
//                  BrowserStartupController
//===============================================================

const String BrowserStartupController::TAG("BrowserStartupController");

// Helper constants for {@link StartupCallback#onSuccess}.
const Boolean BrowserStartupController::ALREADY_STARTED = TRUE;
const Boolean BrowserStartupController::NOT_ALREADY_STARTED = FALSE;

// Helper constants for {@link #executeEnqueuedCallbacks(int, boolean)}.
//@VisibleForTesting
const Int32 BrowserStartupController::STARTUP_SUCCESS;
//@VisibleForTesting
const Int32 BrowserStartupController::STARTUP_FAILURE;

AutoPtr<BrowserStartupController> BrowserStartupController::sInstance;

Boolean BrowserStartupController::sBrowserMayStartAsynchronously = FALSE;

// Use single-process mode that runs the renderer on a separate thread in
// the main application.
const Int32 BrowserStartupController::MAX_RENDERERS_SINGLE_PROCESS;

// Cap on the maximum number of renderer processes that can be requested.
// This is currently set to account for:
//  13: The maximum number of sandboxed processes we have available
// - 1: The regular New Tab Page
// - 1: The incognito New Tab Page
// - 1: A regular incognito tab
// - 1: Safety buffer (http://crbug.com/251279)
const Int32 BrowserStartupController::MAX_RENDERERS_LIMIT;

BrowserStartupController::BrowserStartupController(
    /* [in] */ IContext* context)
    : mContext(context)
    , mHasStartedInitializingBrowserProcess(FALSE)
    , mStartupDone(FALSE)
    , mStartupSuccess(FALSE)
{
    CArrayList::New((IList**)&mAsyncStartupCallbacks);
}

void BrowserStartupController::SetAsynchronousStartup(
    /* [in] */ Boolean enable)
{
    sBrowserMayStartAsynchronously = enable;
}

//@VisibleForTesting
//@CalledByNative
Boolean BrowserStartupController::BrowserMayStartAsynchonously()
{
    return sBrowserMayStartAsynchronously;
}

//@VisibleForTesting
//@CalledByNative
void BrowserStartupController::BrowserStartupComplete(
    /* [in] */ Int32 result)
{
    if (sInstance != NULL) {
        sInstance->ExecuteEnqueuedCallbacks(result, NOT_ALREADY_STARTED);
    }
}

AutoPtr<BrowserStartupController> BrowserStartupController::Get(
    /* [in] */ IContext* context)
{
//    assert ThreadUtils.runningOnUiThread() : "Tried to start the browser on the wrong thread.";
    ThreadUtils::AssertOnUiThread();
    if (sInstance == NULL) {
        AutoPtr<IContext> ctx;
        context->GetApplicationContext((IContext**)&ctx);
        sInstance = new BrowserStartupController(ctx);
    }
    return sInstance;
}

//@VisibleForTesting
AutoPtr<BrowserStartupController> BrowserStartupController::OverrideInstanceForTest(
    /* [in] */ BrowserStartupController* controller)
{
    if (sInstance == NULL) {
        sInstance = controller;
    }
    return sInstance;
}

/**
 * Start the browser process asynchronously. This will set up a queue of UI thread tasks to
 * initialize the browser process.
 * <p/>
 * Note that this can only be called on the UI thread.
 *
 * @param callback the callback to be called when browser startup is complete.
 */
void BrowserStartupController::StartBrowserProcessesAsync(
    /* [in] */ StartupCallback* callback)
{
//    assert ThreadUtils.runningOnUiThread() : "Tried to start the browser on the wrong thread.";
    if (mStartupDone) {
        // Browser process initialization has already been completed, so we can immediately post
        // the callback.
        PostStartupCompleted(callback);
        return;
    }

    // Browser process has not been fully started yet, so we defer executing the callback.
    mAsyncStartupCallbacks->Add(TO_IINTERFACE(callback));

    if (!mHasStartedInitializingBrowserProcess) {
        // This is the first time we have been asked to start the browser process. We set the
        // flag that indicates that we have kicked off starting the browser process.
        mHasStartedInitializingBrowserProcess = TRUE;

        PrepareToStartBrowserProcess(MAX_RENDERERS_LIMIT);

        SetAsynchronousStartup(TRUE);
        if (ContentStart() > 0) {
            // Failed. The callbacks may not have run, so run them.
            EnqueueCallbackExecution(STARTUP_FAILURE, NOT_ALREADY_STARTED);
        }
    }
}

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
void BrowserStartupController::StartBrowserProcessesSync(
    /* [in] */ Int32 maxRenderers)
{
    // If already started skip to checking the result
    if (!mStartupDone) {
        if (!mHasStartedInitializingBrowserProcess) {
            PrepareToStartBrowserProcess(maxRenderers);
        }

        SetAsynchronousStartup(FALSE);
        if (ContentStart() > 0) {
            // Failed. The callbacks may not have run, so run them.
            EnqueueCallbackExecution(STARTUP_FAILURE, NOT_ALREADY_STARTED);
        }
    }

    // Startup should now be complete
    assert(mStartupDone);
    if (!mStartupSuccess) {
        //throw new ProcessInitException(LoaderErrors.LOADER_ERROR_NATIVE_STARTUP_FAILED);
        Logger::E(TAG, "ProcessInitException(LoaderErrors.LOADER_ERROR_NATIVE_STARTUP_FAILED)");
        assert(0);
    }
}

/**
 * Wrap ContentMain.start() for testing.
 */
//@VisibleForTesting
Int32 BrowserStartupController::ContentStart()
{
    return ContentMain::Start();
}

void BrowserStartupController::AddStartupCompletedObserver(
    /* [in] */ StartupCallback* callback)
{
    ThreadUtils::AssertOnUiThread();
    if (mStartupDone) {
        PostStartupCompleted(callback);
    }
    else {
        mAsyncStartupCallbacks->Add(TO_IINTERFACE(callback));
    }
}

void BrowserStartupController::ExecuteEnqueuedCallbacks(
    /* [in] */ Int32 startupResult,
    /* [in] */ Boolean alreadyStarted)
{
//    assert ThreadUtils.runningOnUiThread() : "Callback from browser startup from wrong thread.";
    mStartupDone = TRUE;
    mStartupSuccess = (startupResult <= 0);
    Int32 size;
    mAsyncStartupCallbacks->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mAsyncStartupCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<StartupCallback> asyncStartupCallback = (StartupCallback*)(IObject::Probe(obj));
        if (mStartupSuccess) {
            asyncStartupCallback->OnSuccess(alreadyStarted);
        }
        else {
            asyncStartupCallback->OnFailure();
        }
    }
    // We don't want to hold on to any objects after we do not need them anymore.
    mAsyncStartupCallbacks->Clear();
}

// Queue the callbacks to run. Since running the callbacks clears the list it is safe to call
// this more than once.
void BrowserStartupController::EnqueueCallbackExecution(
    /* [in] */ const Int32 startupFailure,
    /* [in] */ const Boolean alreadyStarted)
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    AutoPtr<IRunnable> runnable = new EnqueueCallbackExecutionRunnable(this, startupFailure, alreadyStarted);
    Boolean result;
    handler->Post(runnable, &result);
}

void BrowserStartupController::PostStartupCompleted(
    /* [in] */ StartupCallback* callback)
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    AutoPtr<IRunnable> runnable = new PostStartupCompletedRunnable(this, callback);
    Boolean result;
    handler->Post(runnable, &result);
}

//@VisibleForTesting
void BrowserStartupController::PrepareToStartBrowserProcess(
    /* [in] */ Int32 maxRendererProcesses)
{
    Logger::V(TAG, "Initializing chromium process, renderers=%d", maxRendererProcesses);

    // Normally Main.java will have kicked this off asynchronously for Chrome. But other
    // ContentView apps like tests also need them so we make sure we've extracted resources
    // here. We can still make it a little async (wait until the library is loaded).
    AutoPtr<ResourceExtractor> resourceExtractor = ResourceExtractor::Get(mContext);
    resourceExtractor->StartExtractingResources();

    // Normally Main.java will have already loaded the library asynchronously, we only need
    // to load it here if we arrived via another flow, e.g. bookmark access & sync setup.
    // do not need this in elastos
    //LibraryLoader::EnsureInitialized(mContext, TRUE);

    // TODO(yfriedman): Remove dependency on a command line flag for this.
    DeviceUtils::AddDeviceSpecificUserAgentSwitch(mContext);

    AutoPtr<IContext> appContext;
    mContext->GetApplicationContext((IContext**)&appContext);
    // Now we really need to have the resources ready.
    resourceExtractor->WaitForCompletion();

    NativeSetCommandLineFlags(maxRendererProcesses,
            NativeIsPluginEnabled() ? GetPlugins() : String(NULL));
    ContentMain::InitApplicationContext(appContext);
}

/**
 * Initialization needed for tests. Mainly used by content browsertests.
 */
void BrowserStartupController::InitChromiumBrowserProcessForTests()
{
    AutoPtr<ResourceExtractor> resourceExtractor = ResourceExtractor::Get(mContext);
    resourceExtractor->StartExtractingResources();
    resourceExtractor->WaitForCompletion();

    // Having a single renderer should be sufficient for tests. We can't have more than
    // MAX_RENDERERS_LIMIT.
    NativeSetCommandLineFlags(1 /* maxRenderers */, String(NULL));
}

String BrowserStartupController::GetPlugins()
{
    return PepperPluginManager::GetPlugins(mContext);
}

void BrowserStartupController::NativeSetCommandLineFlags(
    /* [in] */ Int32 maxRenderProcesses,
    /* [in] */ const String& pluginDescriptor)
{
    Elastos_BrowserStartupController_nativeSetCommandLineFlags(maxRenderProcesses, pluginDescriptor);
}

// Is this an official build of Chrome? Only native code knows for sure. Official build
// knowledge is needed very early in process startup.
Boolean BrowserStartupController::NativeIsOfficialBuild()
{
    return Elastos_BrowserStartupController_nativeIsOfficialBuild();
}

Boolean BrowserStartupController::NativeIsPluginEnabled()
{
    return Elastos_BrowserStartupController_nativeIsPluginEnabled();
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

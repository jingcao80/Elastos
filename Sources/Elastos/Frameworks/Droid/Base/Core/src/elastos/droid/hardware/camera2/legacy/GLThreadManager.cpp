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

#include "elastos/droid/hardware/camera2/legacy/GLThreadManager.h"
#include "elastos/droid/hardware/camera2/legacy/CRequestThreadManagerFpsCounter.h"
#include "elastos/droid/hardware/camera2/legacy/CSurfaceTextureRenderer.h"
#include "elastos/droid/hardware/camera2/legacy/CRequestHandlerThread.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/os/ConditionVariable.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Legacy::CRequestHandlerThread;
using Elastos::Droid::Hardware::Camera2::Legacy::CSurfaceTextureRenderer;
using Elastos::Droid::Hardware::Camera2::Legacy::CRequestThreadManagerFpsCounter;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraDeviceImplCameraDeviceCallbacks;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ConditionVariable;
using Elastos::Core::StringBuilder;
using Elastos::Core::IThread;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

GLThreadManager::ConfigureHolder::ConfigureHolder(
    /* [in] */ IConditionVariable* condition,
    /* [in] */ ICollection* surfaces,
    /* [in] */ ICaptureCollector* collector)
    : mCondition(condition)
    , mSurfaces(surfaces)
    , mCollector(collector)
{
}

CAR_INTERFACE_IMPL(GLThreadManager::MyHandlerCallback, Object, IHandlerCallback)

GLThreadManager::MyHandlerCallback::MyHandlerCallback(
    /* [in] */ GLThreadManager* host)
    : mHost(host)
    , mCleanup(FALSE)
    , mConfigured(FALSE)
    , mDroppingFrames(FALSE)
{}

ECode GLThreadManager::MyHandlerCallback::HandleMessage(
   /* [in] */ IMessage* msg,
   /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mCleanup) {
        *result = TRUE;
        return NOERROR;
    }
    //try {
    Int32 what;
    msg->GetWhat(&what);
    ECode ec = NOERROR;
    switch (what) {
        case GLThreadManager::MSG_NEW_CONFIGURATION:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ConfigureHolder> configure = (ConfigureHolder*)IObject::Probe(obj);
            ec = mHost->mTextureRenderer->CleanupEGLContext();
            if (FAILED(ec)) {
                Logger::E(mHost->TAG, "Received exception on GL render thread: %d", ec);
                mHost->mDeviceState->SetError(ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
                break;
            }
            ec = mHost->mTextureRenderer->ConfigureSurfaces(configure->mSurfaces);
            if (FAILED(ec)) {
                Logger::E(mHost->TAG, "Received exception on GL render thread: %d", ec);
                mHost->mDeviceState->SetError(ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
                break;
            }
            ec = Preconditions::CheckNotNull(configure->mCollector.Get());
            if (FAILED(ec)) {
                Logger::E(mHost->TAG, "Received exception on GL render thread: %d", ec);
                mHost->mDeviceState->SetError(ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
                break;
            }
            mHost->mCaptureCollector = configure->mCollector;
            ec = configure->mCondition->Open();
            if (FAILED(ec)) {
                Logger::E(mHost->TAG, "Received exception on GL render thread: %d", ec);
                mHost->mDeviceState->SetError(ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
                break;
            }
            mConfigured = TRUE;
            break;
        }
        case GLThreadManager::MSG_NEW_FRAME:
        {
            if (mDroppingFrames) {
                Logger::W(mHost->TAG, "Ignoring frame.");
                break;
            }
            if (DEBUG) {
                mHost->mPrevCounter->CountAndLog();
            }
            if (!mConfigured) {
                Logger::E(mHost->TAG, "Dropping frame, EGL context not configured!");
            }
            ec = mHost->mTextureRenderer->DrawIntoSurfaces(mHost->mCaptureCollector);
            if (FAILED(ec)) {
                Logger::E(mHost->TAG, "Received exception on GL render thread: %d", ec);
                mHost->mDeviceState->SetError(ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
                break;
            }
            break;
        }
        case GLThreadManager::MSG_CLEANUP:
            ec = mHost->mTextureRenderer->CleanupEGLContext();
            if (FAILED(ec)) {
                Logger::E(mHost->TAG, "Received exception on GL render thread: %d", ec);
                mHost->mDeviceState->SetError(ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
                break;
            }
            mCleanup = TRUE;
            mConfigured = FALSE;
            break;
        case GLThreadManager::MSG_DROP_FRAMES:
            mDroppingFrames = TRUE;
            break;
        case GLThreadManager::MSG_ALLOW_FRAMES:
            mDroppingFrames = FALSE;
            break;
        case IRequestHandlerThread::MSG_POKE_IDLE_HANDLER:
            // OK: Ignore message.
            break;
        default:
            Logger::E(mHost->TAG, "Unhandled message %d on GLThread.", what);
            break;
    }
    //} catch (Exception e) {
    if (FAILED(ec)) {
        Logger::E(mHost->TAG, "Received exception on GL render thread: %d", ec);
        mHost->mDeviceState->SetError(ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
    }
    //}

    *result = TRUE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(GLThreadManager, Object, IGLThreadManager)

const Boolean GLThreadManager::DEBUG = FALSE;//Log.isLoggable(LegacyCameraDevice.DEBUG_PROP, Log.DEBUG);

const Int32 GLThreadManager::MSG_NEW_CONFIGURATION;
const Int32 GLThreadManager::MSG_NEW_FRAME;
const Int32 GLThreadManager::MSG_CLEANUP;
const Int32 GLThreadManager::MSG_DROP_FRAMES;
const Int32 GLThreadManager::MSG_ALLOW_FRAMES;

GLThreadManager::GLThreadManager()
{
    CRequestThreadManagerFpsCounter::New(String("GL Preview Producer"),
            (IRequestThreadManagerFpsCounter**)&mPrevCounter);

    mGLHandlerCb = new MyHandlerCallback(this);
}

ECode GLThreadManager::constructor()
{
    return NOERROR;
}

ECode GLThreadManager::constructor(
    /* [in] */ Int32 cameraId,
    /* [in] */ Int32 facing,
    /* [in] */ ICameraDeviceState* state)
{
    CSurfaceTextureRenderer::New(facing, (ISurfaceTextureRenderer**)&mTextureRenderer);
    StringBuilder sb;
    sb += "CameraDeviceGLThread-";
    sb += cameraId;
    TAG = sb.ToString();
    CRequestHandlerThread::New(TAG, mGLHandlerCb, (IRequestHandlerThread**)&mGLHandlerThread);
    mDeviceState = state;
    return NOERROR;
}

ECode GLThreadManager::Start()
{
    return IThread::Probe(mGLHandlerThread)->Start();
}

ECode GLThreadManager::WaitUntilStarted()
{
    return mGLHandlerThread->WaitUntilStarted();
}

ECode GLThreadManager::Quit()
{
    AutoPtr<IHandler> handler;
    mGLHandlerThread->GetHandler((IHandler**)&handler);
    AutoPtr<IMessage> message;
    handler->ObtainMessage(MSG_CLEANUP, (IMessage**)&message);
    Boolean result;
    handler->SendMessageAtFrontOfQueue(message, &result);
    IHandlerThread::Probe(mGLHandlerThread)->QuitSafely(&result);
    //try {
    ECode ec = IThread::Probe(mGLHandlerThread)->Join();
    //} catch (InterruptedException e) {
    if (FAILED(ec)) {
        String name;
        IThread::Probe(mGLHandlerThread)->GetName(&name);
        Int64 id;
        IThread::Probe(mGLHandlerThread)->GetId(&id);
        Logger::E(TAG, "Thread %s (%d) interrupted while quitting.",
                name.string(), id);
    }
    //}
    return NOERROR;
}

ECode GLThreadManager::QueueNewFrame()
{
    AutoPtr<IHandler> handler;
    mGLHandlerThread->GetHandler((IHandler**)&handler);

    /**
     * Avoid queuing more than one new frame.  If we are not consuming faster than frames
     * are produced, drop frames rather than allowing the queue to back up.
     */
    Boolean result;
    handler->HasMessages(MSG_NEW_FRAME, &result);
    if (!result) {
        AutoPtr<IMessage> message;
        handler->ObtainMessage(MSG_NEW_FRAME, (IMessage**)&message);
        Boolean result;
        handler->SendMessage(message, &result);
    }
    else {
        Logger::E(TAG, "GLThread dropping frame.  Not consuming frames quickly enough!");
    }
    return NOERROR;
}

ECode GLThreadManager::SetConfigurationAndWait(
    /* [in] */ ICollection* surfaces,
    /* [in] */ ICaptureCollector* collector)
{
    FAIL_RETURN(Preconditions::CheckNotNull(collector, String("collector must not be null")))
    AutoPtr<IHandler> handler;
    mGLHandlerThread->GetHandler((IHandler**)&handler);

    AutoPtr<IConditionVariable> condition = new ConditionVariable(/*closed*/FALSE);
    AutoPtr<ConfigureHolder> configure = new ConfigureHolder(condition, surfaces, collector);

    AutoPtr<IMessage> m;
    handler->ObtainMessage(MSG_NEW_CONFIGURATION, /*arg1*/0, /*arg2*/0,
            TO_IINTERFACE(configure), (IMessage**)&m);
    Boolean result;
    handler->SendMessage(m, &result);

    // Block until configuration applied.
    return condition->Block();
}

ECode GLThreadManager::GetCurrentSurfaceTexture(
    /* [out] */ ISurfaceTexture** outsur)
{
    VALIDATE_NOT_NULL(outsur);

    return mTextureRenderer->GetSurfaceTexture(outsur);
}

ECode GLThreadManager::IgnoreNewFrames()
{
    AutoPtr<IHandler> handler;
    mGLHandlerThread->GetHandler((IHandler**)&handler);
    Boolean result;
    return handler->SendEmptyMessage(MSG_DROP_FRAMES, &result);
}

ECode GLThreadManager::WaitUntilIdle()
{
    return mGLHandlerThread->WaitUntilIdle();
}

ECode GLThreadManager::AllowNewFrames()
{
    AutoPtr<IHandler> handler;
    mGLHandlerThread->GetHandler((IHandler**)&handler);
    Boolean result;
    return handler->SendEmptyMessage(MSG_ALLOW_FRAMES, &result);
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

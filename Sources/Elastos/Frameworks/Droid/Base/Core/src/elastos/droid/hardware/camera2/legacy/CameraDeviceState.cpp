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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/hardware/camera2/legacy/CameraDeviceState.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraDeviceImplCameraDeviceCallbacks;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(CameraDeviceState, Object, ICameraDeviceState)

const String CameraDeviceState::TAG("CameraDeviceState");
const Boolean CameraDeviceState::DEBUG = FALSE;//Log.isLoggable(LegacyCameraDevice.DEBUG_PROP, Log.DEBUG);

const Int32 CameraDeviceState::STATE_ERROR = 0;
const Int32 CameraDeviceState::STATE_UNCONFIGURED = 1;
const Int32 CameraDeviceState::STATE_CONFIGURING = 2;
const Int32 CameraDeviceState::STATE_IDLE = 3;
const Int32 CameraDeviceState::STATE_CAPTURING = 4;

static AutoPtr<ArrayOf<String> > initArray()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(5);
    array->Set(0, String("ERROR"));
    array->Set(1, String("UNCONFIGURED"));
    array->Set(2, String("CONFIGURING"));
    array->Set(3, String("IDLE"));
    array->Set(4, String("CAPTURING"));
    return array;
}

const AutoPtr<ArrayOf<String> > CameraDeviceState::sStateNames = initArray();

CameraDeviceState::MyRunnableOnError::MyRunnableOnError(
    /* [in] */ Int32 captureError,
    /* [in] */ IRequestHolder* request,
    /* [in] */ CameraDeviceState* host)
    : mCaptureError(captureError)
    , mRequest(request)
    , mHost(host)
{}

ECode CameraDeviceState::MyRunnableOnError::Run()
{
    return mHost->mCurrentListener->OnError(mCaptureError, mRequest);
}

CameraDeviceState::MyRunnableOnCaptureResult::MyRunnableOnCaptureResult(
    /* [in] */ ICameraMetadataNative* result,
    /* [in] */ IRequestHolder* request,
    /* [in] */ CameraDeviceState* host)
    : mResult(result)
    , mRequest(request)
    , mHost(host)
{}

ECode CameraDeviceState::MyRunnableOnCaptureResult::Run()
{
    return mHost->mCurrentListener->OnCaptureResult(mResult, mRequest);
}

CameraDeviceState::MyRunnableOnBusy::MyRunnableOnBusy(
    /* [in] */ CameraDeviceState* host)
    : mHost(host)
{}

ECode CameraDeviceState::MyRunnableOnBusy::Run()
{
    return mHost->mCurrentListener->OnBusy();
}

CameraDeviceState::MyRunnableOnConfiguring::MyRunnableOnConfiguring(
    /* [in] */ CameraDeviceState* host)
    : mHost(host)
{}

ECode CameraDeviceState::MyRunnableOnConfiguring::Run()
{
    return mHost->mCurrentListener->OnConfiguring();
}

CameraDeviceState::MyRunnableOnIdle::MyRunnableOnIdle(
    /* [in] */ CameraDeviceState* host)
    : mHost(host)
{}

ECode CameraDeviceState::MyRunnableOnIdle::Run()
{
    return mHost->mCurrentListener->OnIdle();
}

CameraDeviceState::MyRunnableOnCaptureStarted::MyRunnableOnCaptureStarted(
    /* [in] */ IRequestHolder* request,
    /* [in] */ Int64 timestamp,
    /* [in] */ CameraDeviceState* host)
    : mRequest(request)
    , mTimestamp(timestamp)
    , mHost(host)
{}

ECode CameraDeviceState::MyRunnableOnCaptureStarted::Run()
{
    return mHost->mCurrentListener->OnCaptureStarted(mRequest, mTimestamp);
}

CameraDeviceState::CameraDeviceState()
    : mCurrentState(STATE_UNCONFIGURED)
    , mCurrentError(ICameraDeviceState::NO_CAPTURE_ERROR)
{
}

CARAPI CameraDeviceState::SetError(
    /* [in] */ Int32 error)
{
    {    AutoLock syncLock(this);
        mCurrentError = error;
        return DoStateTransition(STATE_ERROR);
    }
    return NOERROR;
}

CARAPI CameraDeviceState::SetConfiguring(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    {    AutoLock syncLock(this);
        DoStateTransition(STATE_CONFIGURING);
        *value = mCurrentError == NO_CAPTURE_ERROR;
        return NOERROR;
    }
    return NOERROR;
}

CARAPI CameraDeviceState::SetIdle(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    {    AutoLock syncLock(this);
        DoStateTransition(STATE_IDLE);
        *value = mCurrentError == NO_CAPTURE_ERROR;
        return NOERROR;
    }
    return NOERROR;
}

CARAPI CameraDeviceState::SetCaptureStart(
    /* [in] */ IRequestHolder* request,
    /* [in] */ Int64 timestamp,
    /* [in] */ Int32 captureError,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    {    AutoLock syncLock(this);
        mCurrentRequest = request;
        DoStateTransition(STATE_CAPTURING, timestamp, captureError);
        *value = mCurrentError == NO_CAPTURE_ERROR;
        return NOERROR;
    }
    return NOERROR;
}

CARAPI CameraDeviceState::SetCaptureResult(
    /* [in] */ IRequestHolder* request,
    /* [in] */ ICameraMetadataNative* result,
    /* [in] */ Int32 captureError,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    {
        AutoLock syncLock(this);
        if (mCurrentState != STATE_CAPTURING) {
            Logger::E(TAG, "Cannot receive result while in state: %d", mCurrentState);
            mCurrentError = ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE;
            DoStateTransition(STATE_ERROR);
            *value = mCurrentError == NO_CAPTURE_ERROR;
            return NOERROR;
        }

        if (mCurrentHandler != NULL && mCurrentListener != NULL) {
            if (captureError != ICameraDeviceState::NO_CAPTURE_ERROR) {
                AutoPtr<MyRunnableOnError> run = new MyRunnableOnError(captureError, request, this);
                Boolean result;
                mCurrentHandler->Post(run, &result);
            }
            else {
                AutoPtr<MyRunnableOnCaptureResult> run = new MyRunnableOnCaptureResult(result, request, this);
                Boolean result;
                mCurrentHandler->Post(run, &result);
            }
        }
        *value = mCurrentError == NO_CAPTURE_ERROR;
        return NOERROR;
    }
    return NOERROR;
}

CARAPI CameraDeviceState::SetCameraDeviceCallbacks(
    /* [in] */ IHandler* handler,
    /* [in] */ ICameraDeviceStateListener* listener)
{
    AutoLock syncLock(this);
    mCurrentHandler = handler;
    mCurrentListener = listener;
    return NOERROR;
}

CARAPI CameraDeviceState::DoStateTransition(
    /* [in] */ Int32 newState)
{
    return DoStateTransition(newState, /*timestamp*/0, ICameraDeviceState::NO_CAPTURE_ERROR);
}

CARAPI CameraDeviceState::DoStateTransition(
    /* [in] */ Int32 newState,
    /* [in] */ const Int64 timestamp,
    /* [in] */ const Int32 error)
{
    if (newState != mCurrentState) {
        String stateName("UNKNOWN");
        if (newState >= 0 && newState < sStateNames->GetLength()) {
            stateName = (*sStateNames)[newState];
        }
        Logger::I(TAG, "Legacy camera service transitioning to state %s", stateName.string());
    }

    // If we transitioned into a non-IDLE/non-ERROR state then mark the device as busy
    if(newState != STATE_ERROR && newState != STATE_IDLE) {
        if (mCurrentState != newState && mCurrentHandler != NULL &&
                mCurrentListener != NULL) {
            AutoPtr<MyRunnableOnBusy> run = new MyRunnableOnBusy(this);
            Boolean result;
            mCurrentHandler->Post(run, &result);
        }
    }

    switch(newState) {
        case STATE_ERROR:
        {
            if (mCurrentState != STATE_ERROR && mCurrentHandler != NULL &&
                    mCurrentListener != NULL) {
                AutoPtr<MyRunnableOnError> run = new MyRunnableOnError(mCurrentError, mCurrentRequest, this);
                Boolean result;
                mCurrentHandler->Post(run, &result);
            }
            mCurrentState = STATE_ERROR;
            break;
        }
        case STATE_CONFIGURING:
        {
            if (mCurrentState != STATE_UNCONFIGURED && mCurrentState != STATE_IDLE) {
                Logger::E(TAG, "Cannot call configure while in state: %d", mCurrentState);
                mCurrentError = ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE;
                DoStateTransition(STATE_ERROR);
                break;
            }
            if (mCurrentState != STATE_CONFIGURING && mCurrentHandler != NULL &&
                    mCurrentListener != NULL) {
                AutoPtr<MyRunnableOnConfiguring> run = new MyRunnableOnConfiguring(this);
                Boolean result;
                mCurrentHandler->Post(run, &result);
            }
            mCurrentState = STATE_CONFIGURING;
            break;
        }
        case STATE_IDLE:
        {
            if (mCurrentState == STATE_IDLE) {
                break;
            }

            if (mCurrentState != STATE_CONFIGURING && mCurrentState != STATE_CAPTURING) {
                Logger::E(TAG, "Cannot call idle while in state: %d", mCurrentState);
                mCurrentError = ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE;
                DoStateTransition(STATE_ERROR);
                break;
            }

            if (mCurrentState != STATE_IDLE && mCurrentHandler != NULL &&
                    mCurrentListener != NULL) {
                AutoPtr<MyRunnableOnIdle> run = new MyRunnableOnIdle(this);
                Boolean result;
                mCurrentHandler->Post(run, &result);
            }
            mCurrentState = STATE_IDLE;
            break;
        }
        case STATE_CAPTURING:
        {
            if (mCurrentState != STATE_IDLE && mCurrentState != STATE_CAPTURING) {
                Logger::E(TAG, "Cannot call capture while in state: %d", mCurrentState);
                mCurrentError = ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE;
                DoStateTransition(STATE_ERROR);
                break;
            }

            if (mCurrentHandler != NULL && mCurrentListener != NULL) {
                if (error != ICameraDeviceState::NO_CAPTURE_ERROR) {
                    AutoPtr<MyRunnableOnError> run = new MyRunnableOnError(error, mCurrentRequest, this);
                    Boolean result;
                    mCurrentHandler->Post(run, &result);
                }
                else {
                    AutoPtr<MyRunnableOnCaptureStarted> run = new MyRunnableOnCaptureStarted(
                            mCurrentRequest, timestamp, this);
                    Boolean result;
                    mCurrentHandler->Post(run,&result);
                }
            }
            mCurrentState = STATE_CAPTURING;
            break;
        }
        default:
            Logger::E(TAG, "Transition to unknown state: %d", newState);
            return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

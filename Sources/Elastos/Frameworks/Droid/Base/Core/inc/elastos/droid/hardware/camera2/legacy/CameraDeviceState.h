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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CAMERADEVICESTATE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CAMERADEVICESTATE_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class CameraDeviceState
    : public Object
    , public ICameraDeviceState
{
private:
    class MyRunnableOnError
        : public Runnable
    {
    public:
        MyRunnableOnError(
            /* [in] */ Int32 captureError,
            /* [in] */ IRequestHolder* request,
            /* [in] */ CameraDeviceState* host);

        CARAPI Run();

    private:
        Int32 mCaptureError;
        AutoPtr<IRequestHolder> mRequest;
        CameraDeviceState* mHost;
    };

    class MyRunnableOnCaptureResult
        : public Runnable
    {
    public:
        MyRunnableOnCaptureResult(
            /* [in] */ ICameraMetadataNative* result,
            /* [in] */ IRequestHolder* request,
            /* [in] */ CameraDeviceState* host);

        CARAPI Run();

    private:
        AutoPtr<ICameraMetadataNative> mResult;
        AutoPtr<IRequestHolder> mRequest;
        CameraDeviceState* mHost;
    };

    class MyRunnableOnBusy
        : public Runnable
    {
    public:
        MyRunnableOnBusy(
            /* [in] */ CameraDeviceState* host);

        CARAPI Run();

    private:
        CameraDeviceState* mHost;
    };

    class MyRunnableOnConfiguring
        : public Runnable
    {
    public:
        MyRunnableOnConfiguring(
            /* [in] */ CameraDeviceState* host);

        CARAPI Run();

    private:
        CameraDeviceState* mHost;
    };

    class MyRunnableOnIdle
        : public Runnable
    {
    public:
        MyRunnableOnIdle(
            /* [in] */ CameraDeviceState* host);

        CARAPI Run();

    private:
        CameraDeviceState* mHost;
    };

    class MyRunnableOnCaptureStarted
        : public Runnable
    {
    public:
        MyRunnableOnCaptureStarted(
            /* [in] */ IRequestHolder* request,
            /* [in] */ Int64 timestamp,
            /* [in] */ CameraDeviceState* host);

        CARAPI Run();

    private:
        AutoPtr<IRequestHolder> mRequest;
        Int64 mTimestamp;
        CameraDeviceState* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CameraDeviceState();

    virtual ~CameraDeviceState() {}

    /**
     * Transition to the {@code ERROR} state.
     *
     * <p>
     * The device cannot exit the {@code ERROR} state.  If the device was not already in the
     * {@code ERROR} state, {@link CameraDeviceStateListener#onError(int, RequestHolder)} will be
     * called.
     * </p>
     *
     * @param error the error to set.  Should be one of the error codes defined in
     *      {@link CameraDeviceImpl.CameraDeviceCallbacks}.
     */
    CARAPI SetError(
        /* [in] */ Int32 error);

    /**
     * Transition to the {@code CONFIGURING} state, or {@code ERROR} if in an invalid state.
     *
     * <p>
     * If the device was not already in the {@code CONFIGURING} state,
     * {@link CameraDeviceStateListener#onConfiguring()} will be called.
     * </p>
     *
     * @return {@code false} if an error has occurred.
     */
    CARAPI SetConfiguring(
        /* [out] */ Boolean* value);

    /**
     * Transition to the {@code IDLE} state, or {@code ERROR} if in an invalid state.
     *
     * <p>
     * If the device was not already in the {@code IDLE} state,
     * {@link CameraDeviceStateListener#onIdle()} will be called.
     * </p>
     *
     * @return {@code false} if an error has occurred.
     */
    CARAPI SetIdle(
        /* [out] */ Boolean* value);

    /**
     * Transition to the {@code CAPTURING} state, or {@code ERROR} if in an invalid state.
     *
     * <p>
     * If the device was not already in the {@code CAPTURING} state,
     * {@link CameraDeviceStateListener#onCaptureStarted(RequestHolder)} will be called.
     * </p>
     *
     * @param request A {@link RequestHolder} containing the request for the current capture.
     * @param timestamp The timestamp of the capture start in nanoseconds.
     * @param captureError Report a recoverable error for a single request using a valid
     *                     error code for {@code ICameraDeviceCallbacks}, or
     *                     {@link #NO_CAPTURE_ERROR}
     * @return {@code false} if an error has occurred.
     */
    CARAPI SetCaptureStart(
        /* [in] */ IRequestHolder* request,
        /* [in] */ Int64 timestamp,
        /* [in] */ Int32 captureError,
        /* [out] */ Boolean* value);

    /**
     * Set the result for a capture.
     *
     * <p>
     * If the device was in the {@code CAPTURING} state,
     * {@link CameraDeviceStateListener#onCaptureResult(CameraMetadataNative, RequestHolder)} will
     * be called with the given result, otherwise this will result in the device transitioning to
     * the {@code ERROR} state,
     * </p>
     *
     * @param request The {@link RequestHolder} request that created this result.
     * @param result The {@link CameraMetadataNative} result to set.
     * @param captureError Report a recoverable error for a single buffer or result using a valid
     *                     error code for {@code ICameraDeviceCallbacks}, or
     *                     {@link #NO_CAPTURE_ERROR}.
     * @return {@code false} if an error has occurred.
     */
    CARAPI SetCaptureResult(
        /* [in] */ IRequestHolder* request,
        /* [in] */ ICameraMetadataNative* result,
        /* [in] */ Int32 captureError,
        /* [out] */ Boolean* value);

    /**
     * Set the listener for state transition callbacks.
     *
     * @param handler handler on which to call the callbacks.
     * @param listener the {@link CameraDeviceStateListener} callbacks to call.
     */
    CARAPI SetCameraDeviceCallbacks(
        /* [in] */ IHandler* handler,
        /* [in] */ ICameraDeviceStateListener* listener);

private:
    CARAPI DoStateTransition(
        /* [in] */ Int32 newState);

    CARAPI DoStateTransition(
        /* [in] */ Int32 newState,
        /* [in] */ const Int64 timestamp,
        /* [in] */ const Int32 error);

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const Int32 STATE_ERROR;
    static const Int32 STATE_UNCONFIGURED;
    static const Int32 STATE_CONFIGURING;
    static const Int32 STATE_IDLE;
    static const Int32 STATE_CAPTURING;

    static const AutoPtr<ArrayOf<String> > sStateNames;

    Int32 mCurrentState;
    Int32 mCurrentError;

    AutoPtr<IRequestHolder> mCurrentRequest;

    AutoPtr<IHandler> mCurrentHandler;
    AutoPtr<ICameraDeviceStateListener> mCurrentListener;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CAMERADEVICESTATE_H__

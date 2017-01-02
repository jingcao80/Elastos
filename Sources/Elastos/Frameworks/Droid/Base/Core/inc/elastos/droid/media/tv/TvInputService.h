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

#ifndef __ELASTOS_DROID_MEDIA_TV_TVINPUTSERVICE_H__
#define __ELASTOS_DROID_MEDIA_TV_TVINPUTSERVICE_H__

#include "elastos/droid/app/Service.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::View::IInputChannel;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/**
 * The TvInputService class represents a TV input or source such as HDMI or built-in tuner which
 * provides pass-through video or broadcast TV programs.
 * <p>
 * Applications will not normally use this service themselves, instead relying on the standard
 * interaction provided by {@link TvView}. Those implementing TV input services should normally do
 * so by deriving from this class and providing their own session implementation based on
 * {@link TvInputService.Session}. All TV input services must require that clients hold the
 * {@link android.Manifest.permission#BIND_TV_INPUT} in order to interact with the service; if this
 * permission is not specified in the manifest, the system will refuse to bind to that TV input
 * service.
 * </p>
 */
class TvInputService
    : public Elastos::Droid::App::Service
    , public ITvInputService
{
private:
    class TvInputServiceStub
        : public Object
        , public IITvInputService
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        TvInputServiceStub(
            /* [in] */ TvInputService* host,
            /* [in] */ IIntent* intent)
            : mHost(host)
            , mIntent(intent)
        {}

        CARAPI RegisterCallback(
            /* [in] */ IITvInputServiceCallback * cb);

        CARAPI UnregisterCallback(
            /* [in] */ IITvInputServiceCallback * cb);

        CARAPI CreateSession(
            /* [in] */ IInputChannel * channel,
            /* [in] */ IITvInputSessionCallback * cb,
            /* [in] */ const String& inputId);

        // For hardware TvInputService
        CARAPI NotifyHardwareAdded(
            /* [in] */ ITvInputHardwareInfo * hardwareInfo);

        CARAPI NotifyHardwareRemoved(
            /* [in] */ ITvInputHardwareInfo * hardwareInfo);

        CARAPI NotifyHdmiDeviceAdded(
            /* [in] */ IHdmiDeviceInfo * deviceInfo);

        CARAPI NotifyHdmiDeviceRemoved(
            /* [in] */ IHdmiDeviceInfo * deviceInfo);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        TvInputService* mHost;
        AutoPtr<IIntent> mIntent;
    };

public:
    class ServiceHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("TvInputService::ServiceHandler")

        ServiceHandler(
            /* [in] */ TvInputService* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(void) BroadcastAddHardwareTvInput(
            /* [in] */ Int32 deviceId,
            /* [in] */ ITvInputInfo* inputInfo);

        CARAPI_(void) BroadcastAddHdmiTvInput(
            /* [in] */ Int32 id,
            /* [in] */ ITvInputInfo* inputInfo);

        CARAPI_(void) BroadcastRemoveTvInput(
            /* [in] */ const String& inputId);

    public:
        const static Int32 DO_CREATE_SESSION;
        const static Int32 DO_NOTIFY_SESSION_CREATED;
        const static Int32 DO_ADD_HARDWARE_TV_INPUT;
        const static Int32 DO_REMOVE_HARDWARE_TV_INPUT;
        const static Int32 DO_ADD_HDMI_TV_INPUT;
        const static Int32 DO_REMOVE_HDMI_TV_INPUT;

    private:
        TvInputService* mHost;
    };

public:
    friend class TvInputServiceSession;
    friend class TvInputServiceHardwareSession;

    TvInputService();

    virtual ~TvInputService();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Get the number of callbacks that are registered.
     * @hide
     */
    CARAPI GetRegisteredCallbackCount(
        /* [out] */ Int32* result);

    /**
     * Returns a concrete implementation of {@link Session}.
     * <p>
     * May return {@code null} if this TV input service fails to create a session for some reason.
     * If TV input represents an external device connected to a hardware TV input,
     * {@link HardwareSession} should be returned.
     * </p>
     * @param inputId The ID of the TV input associated with the session.
     */
    CARAPI OnCreateSession(
        /* [in] */ const String& inputId,
        /* [out] */ ITvInputServiceSession** result);

    /**
     * Returns a new {@link TvInputInfo} object if this service is responsible for
     * {@code hardwareInfo}; otherwise, return {@code null}. Override to modify default behavior of
     * ignoring all hardware input.
     *
     * @param hardwareInfo {@link TvInputHardwareInfo} object just added.
     * @hide
     */
    CARAPI OnHardwareAdded(
        /* [in] */ ITvInputHardwareInfo* hardwareInfo,
        /* [out] */ ITvInputInfo** result);

    /**
     * Returns the input ID for {@code deviceId} if it is handled by this service;
     * otherwise, return {@code null}. Override to modify default behavior of ignoring all hardware
     * input.
     *
     * @param hardwareInfo {@link TvInputHardwareInfo} object just removed.
     * @hide
     */
    CARAPI OnHardwareRemoved(
        /* [in] */ ITvInputHardwareInfo* hardwareInfo,
        /* [out] */ String* result);

    /**
     * Returns a new {@link TvInputInfo} object if this service is responsible for
     * {@code deviceInfo}; otherwise, return {@code null}. Override to modify default behavior of
     * ignoring all HDMI logical input device.
     *
     * @param deviceInfo {@link HdmiDeviceInfo} object just added.
     * @hide
     */
    CARAPI OnHdmiDeviceAdded(
        /* [in] */ IHdmiDeviceInfo* deviceInfo,
        /* [out] */ ITvInputInfo** result);

    /**
     * Returns the input ID for {@code deviceInfo} if it is handled by this service; otherwise,
     * return {@code null}. Override to modify default behavior of ignoring all HDMI logical input
     * device.
     *
     * @param deviceInfo {@link HdmiDeviceInfo} object just removed.
     * @hide
     */
    CARAPI OnHdmiDeviceRemoved(
        /* [in] */ IHdmiDeviceInfo* deviceInfo,
        /* [out] */ String* result);

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    /** @hide */
    static CARAPI IsNavigationKey(
        /* [in] */ Int32 keyCode,
        /* [out] */ Boolean* result);

private:
    CARAPI_(Boolean) IsPassthroughInput(
        /* [in] */ const String& inputId);

private:
    static Boolean DEBUG;
    static String TAG;

    /**
     * Handler instance to handle request from TV Input Manager Service. Should be run in the main
     * looper to be synchronously run with {@code Session.mHandler}.
     */
    AutoPtr<ServiceHandler> mServiceHandler;
    AutoPtr<IRemoteCallbackList> mCallbacks;

    AutoPtr<ITvInputManager> mTvInputManager;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_TVINPUTSERVICE_H__

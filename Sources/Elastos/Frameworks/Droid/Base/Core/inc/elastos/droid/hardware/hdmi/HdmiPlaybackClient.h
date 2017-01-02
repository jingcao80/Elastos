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

#ifndef __ELASTOS_DROID_HARDWARE_HDMI_HDMIPLAYBACKCLIENT_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_HDMIPLAYBACKCLIENT_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/hardware/hdmi/HdmiClient.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

class HdmiPlaybackClient
    : public HdmiClient
    , public IHdmiPlaybackClient
{
private:
    class MyCallback
        : public Object
        , public IIHdmiControlCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MyCallback(
            /* [in] */ IHdmiPlaybackClientOneTouchPlayCallback* _callback);

        //@Override
        CARAPI OnComplete(
            /* [in] */ Int32 result);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IHdmiPlaybackClientOneTouchPlayCallback> mCallback;
    };

    class MyCallback2
        : public Object
        , public IIHdmiControlCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MyCallback2(
            /* [in] */ IHdmiPlaybackClientDisplayStatusCallback* _callback);

        //@Override
        CARAPI OnComplete(
            /* [in] */ Int32 result);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IHdmiPlaybackClientDisplayStatusCallback> mCallback;
    };

public:
    CAR_INTERFACE_DECL()

    HdmiPlaybackClient();

    virtual ~HdmiPlaybackClient() {}

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IIHdmiControlService* service);

    /**
     * Perform the feature 'one touch play' from playback device to turn on display
     * and switch the input.
     *
     * @param callback {@link OneTouchPlayCallback} object to get informed
     *         of the result
     */
    CARAPI OneTouchPlay(
        /* [in] */ IHdmiPlaybackClientOneTouchPlayCallback* _callback);

    //@Override
    CARAPI GetDeviceType(
        /* [out] */ Int32* type);

    /**
     * Get the status of display device connected through HDMI bus.
     *
     * @param callback {@link DisplayStatusCallback} object to get informed
     *         of the result
     */
    CARAPI QueryDisplayStatus(
        /* [in] */ IHdmiPlaybackClientDisplayStatusCallback* _callback);

private:
    CARAPI_(AutoPtr<IIHdmiControlCallback>) GetCallbackWrapper(
        /* [in] */ IHdmiPlaybackClientOneTouchPlayCallback* callback);

    CARAPI_(AutoPtr<IIHdmiControlCallback>) GetCallbackWrapper(
        /* [in] */ IHdmiPlaybackClientDisplayStatusCallback* callback);

private:
    static const String TAG;
};

} //Hdmi
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_HDMI_HDMIPLAYBACKCLIENT_H__

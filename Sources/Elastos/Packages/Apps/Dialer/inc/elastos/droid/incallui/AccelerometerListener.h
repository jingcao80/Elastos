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

#ifndef __ELASTOS_DROID_INCALLUI_ACCELEROMETERLISTENER_H__
#define __ELASTOS_DROID_INCALLUI_ACCELEROMETERLISTENER_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Hardware::ISensorEventListener;
using Elastos::Droid::Hardware::ISensorEvent;
using Elastos::Droid::Os::Handler;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class ProximitySensor;

/**
 * This class is used to listen to the accelerometer to monitor the
 * orientation of the phone. The client of this class is notified when
 * the orientation changes between horizontal and vertical.
 */
class AccelerometerListener
    : public Object
{
public:
    class MySensorEventListener
        : public Object
        , public ISensorEventListener
    {
    public:
        MySensorEventListener(
            /* [in] */ AccelerometerListener* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnSensorChanged(
            /* [in] */ ISensorEvent* event);

        CARAPI OnAccuracyChanged(
            /* [in] */ ISensor* sensor,
            /* [in] */ Int32 accuracy);

    private:
        AccelerometerListener* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ AccelerometerListener* host)
            : mHost(host)
        {
            Handler::constructor();
        }

        TO_STRING_IMPL("AccelerometerListener::MyHandler")

        CARAPI HandlerMessage(
            /* [in] */ IMessage* msg);

    private:
        AccelerometerListener* mHost;
    };

public:
    AccelerometerListener(
        /* [in] */ IContext* context,
        /* [in] */ IOrientationListener* listener);

    CARAPI_(void) Enable(
        /* [in] */ Boolean enable);

private:
    CARAPI_(void) SetOrientation(
        /* [in] */ Int32 orientation);

    CARAPI_(void) OnSensorEvent(
        /* [in] */ Double x,
        /* [in] */ Double y,
        /* [in] */ Double z);

public:
    AutoPtr<ISensorEventListener> mSensorListener;
    AutoPtr<IHandler> mHandler;

private:
    static const String TAG;
    static const Boolean DEBUG = TRUE;
    static const Boolean VDEBUG = FALSE;

    // Device orientation
    static const Int32 ORIENTATION_UNKNOWN = 0;
    static const Int32 ORIENTATION_VERTICAL = 1;
    static const Int32 ORIENTATION_HORIZONTAL = 2;

    static const Int32 ORIENTATION_CHANGED = 1234;

    static const Int32 VERTICAL_DEBOUNCE = 100;
    static const Int32 HORIZONTAL_DEBOUNCE = 500;
    static const Double VERTICAL_ANGLE = 50.0;

    AutoPtr<ISensorManager> mSensorManager;
    AutoPtr<ISensor> mSensor;

    // mOrientation is the orientation value most recently reported to the client.
    Int32 mOrientation;

    // mPendingOrientation is the latest orientation computed based on the sensor value.
    // This is sent to the client after a rebounce delay, at which point it is copied to
    // mOrientation.
    Int32 mPendingOrientation;

    AutoPtr<IOrientationListener> mListener;

    friend class MySensorEventListener;
    friend class MyHandler;
    friend class ProximitySensor;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_ACCELEROMETERLISTENER_H__

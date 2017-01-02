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

#ifndef  __ELASTOS_DROID_PHONE_ACCELEROMETERLISRENER_H__
#define  __ELASTOS_DROID_PHONE_ACCELEROMETERLISRENER_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/content/AsyncQueryHandler.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Hardware.h"

using Elastos::Droid::Content::AsyncQueryHandler;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorEvent;
using Elastos::Droid::Hardware::ISensorEventListener;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Os::Handler;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * This class is used to listen to the accelerometer to monitor the
 * orientation of the phone. The client of this class is notified when
 * the orientation changes between horizontal and vertical.
 */
class AccelerometerListener
    : public Object
    , public IAccelerometerListener
{
private:
    class MySensorEventListener
        : public Object
        , public ISensorEventListener
    {
    public:
        TO_STRING_IMPL("AccelerometerListener::MySensorEventListener")

        CAR_INTERFACE_DECL()

        MySensorEventListener(
            /* [in] */ AccelerometerListener* host)
            : mHost(host)
        {}

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
        TO_STRING_IMPL("AccelerometerListener::MyHandler")

        MyHandler(
            /* [in] */ AccelerometerListener* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AccelerometerListener* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("AccelerometerListener")

    AccelerometerListener(
        /* [in] */ IContext* context,
        /* [in] */ IAccelerometerListenerOrientationListener* listener);

    CARAPI Enable(
        /* [in] */ Boolean enable);

private:
    CARAPI_(void) SetOrientation(
        /* [in] */ Int32 orientation);

    CARAPI_(void) OnSensorEvent(
        /* [in] */ Double x,
        /* [in] */ Double y,
        /* [in] */ Double z);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean VDEBUG;

    AutoPtr<ISensorManager> mSensorManager;
    AutoPtr<ISensor> mSensor;

    // mOrientation is the orientation value most recently reported to the client.
    Int32 mOrientation;

    // mPendingOrientation is the latest orientation computed based on the sensor value.
    // This is sent to the client after a rebounce delay, at which point it is copied to
    // mOrientation.
    Int32 mPendingOrientation;

    AutoPtr<IAccelerometerListenerOrientationListener> mListener;

    AutoPtr<ISensorEventListener> mSensorListener;
    AutoPtr<Handler> mHandler;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_ACCELEROMETERLISRENER_H__

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

#include "elastos/droid/incallui/AccelerometerListener.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::EIID_ISensorEventListener;
using Elastos::Droid::Os::IMessage;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//================================================================
// AccelerometerListener::MySensorEventListener
//================================================================
CAR_INTERFACE_IMPL(AccelerometerListener::MySensorEventListener, Object, ISensorEventListener);

ECode AccelerometerListener::MySensorEventListener::OnSensorChanged(
    /* [in] */ ISensorEvent* event)
{
    AutoPtr<ArrayOf<Float> > values;
    event->GetValues((ArrayOf<Float>**)&values);
    mHost->OnSensorEvent((Double)(*values)[0], (Double)(*values)[1], (Double)(*values)[2]);
    return NOERROR;
}

ECode AccelerometerListener::MySensorEventListener::OnAccuracyChanged(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 accuracy)
{
    return NOERROR;
}


//================================================================
// AccelerometerListener::MyHandler
//================================================================
ECode AccelerometerListener::MyHandler::HandlerMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case ORIENTATION_CHANGED: {
            AutoLock lock(this);
            mHost->mOrientation = mHost->mPendingOrientation;
            if (DEBUG) {
                Logger::D(TAG, "orientation: %s",
                        (mHost->mOrientation == ORIENTATION_HORIZONTAL ? "horizontal"
                                : (mHost->mOrientation == ORIENTATION_VERTICAL ? "vertical" : "unknown")));
            }
            mHost->mListener->OrientationChanged(mHost->mOrientation);
            break;
        }
    }
    return NOERROR;
}


//================================================================
// AccelerometerListener
//================================================================
const String AccelerometerListener::TAG("AccelerometerListener");
const Boolean AccelerometerListener::DEBUG;
const Boolean AccelerometerListener::VDEBUG;
const Int32 AccelerometerListener::ORIENTATION_UNKNOWN;
const Int32 AccelerometerListener::ORIENTATION_VERTICAL;
const Int32 AccelerometerListener::ORIENTATION_HORIZONTAL;
const Int32 AccelerometerListener::ORIENTATION_CHANGED;
const Int32 AccelerometerListener::VERTICAL_DEBOUNCE;
const Int32 AccelerometerListener::HORIZONTAL_DEBOUNCE;
const Double AccelerometerListener::VERTICAL_ANGLE;

AccelerometerListener::AccelerometerListener(
    /* [in] */ IContext* context,
    /* [in] */ IOrientationListener* listener)
    : mOrientation(0)
    , mPendingOrientation(0)
    , mListener(listener)
{
    mSensorListener = (ISensorEventListener*)new MySensorEventListener(this);
    mHandler = (IHandler*)new MyHandler(this);

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::SENSOR_SERVICE, (IInterface**)&service);
    mSensorManager = ISensorManager::Probe(service);
    mSensorManager->GetDefaultSensor(ISensor::TYPE_ACCELEROMETER, (ISensor**)&mSensor);
}

void AccelerometerListener::Enable(
    /* [in] */ Boolean enable)
{
    if (DEBUG) Logger::D(TAG, "enable(%d)", enable);
    AutoLock lock(this);
    if (enable) {
        mOrientation = ORIENTATION_UNKNOWN;
        mPendingOrientation = ORIENTATION_UNKNOWN;
        Boolean result;
        mSensorManager->RegisterListener(mSensorListener, mSensor,
                ISensorManager::SENSOR_DELAY_NORMAL, &result);
    }
    else {
        mSensorManager->UnregisterListener(mSensorListener);
        mHandler->RemoveMessages(ORIENTATION_CHANGED);
    }
}

void AccelerometerListener::SetOrientation(
    /* [in] */ Int32 orientation)
{
    AutoLock lock(this);
    if (mPendingOrientation == orientation) {
        // Pending orientation has not changed, so do nothing.
        return;
    }

    // Cancel any pending messages.
    // We will either start a new timer or cancel alltogether
    // if the orientation has not changed.
    mHandler->RemoveMessages(ORIENTATION_CHANGED);

    if (mOrientation != orientation) {
        // Set timer to send an event if the orientation has changed since its
        // previously reported value.
        mPendingOrientation = orientation;
        AutoPtr<IMessage> m;
        mHandler->ObtainMessage(ORIENTATION_CHANGED, (IMessage**)&m);
        // set delay to our debounce timeout
        Int32 delay = (orientation == ORIENTATION_VERTICAL ? VERTICAL_DEBOUNCE : HORIZONTAL_DEBOUNCE);
        Boolean result;
        mHandler->SendMessageDelayed(m, delay, &result);
    }
    else {
        // no message is pending
        mPendingOrientation = ORIENTATION_UNKNOWN;
    }
}

void AccelerometerListener::OnSensorEvent(
    /* [in] */ Double x,
    /* [in] */ Double y,
    /* [in] */ Double z)
{
    if (VDEBUG) Logger::D(TAG, "onSensorEvent(%f, %f, %f)", x, y, z);

    // If some values are exactly zero, then likely the sensor is not powered up yet.
    // ignore these events to avoid false horizontal positives.
    if (x == 0.0 || y == 0.0 || z == 0.0) return;

    // magnitude of the acceleration vector projected onto XY plane
    Double xy = Elastos::Core::Math::Sqrt(x*x + y*y);
    // compute the vertical angle
    Double angle = Elastos::Core::Math::Atan2(xy, z);
    // convert to degrees
    angle = angle * 180.0 / Elastos::Core::Math::PI;
    Int32 orientation = (angle >  VERTICAL_ANGLE ? ORIENTATION_VERTICAL : ORIENTATION_HORIZONTAL);
    if (VDEBUG) Logger::D(TAG, "angle: %f orientation: %d", angle, orientation);
    SetOrientation(orientation);
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
